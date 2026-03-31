#include <iostream>
#include <vector>
#include <sqlite3.h>
#include <cmath>
#include <algorithm>
#include <cstring> // Para memcpy
#include <fstream>
#include <sstream>

#include "storage.hpp"

extern "C" {
    #define SQLITE_CORE 1
    #include "sqlite-vec.h"
}

const size_t EMBEDDING_DIM = 768;
const size_t TOP_K = 3;
static const char* DB_PATH = "rag/rag.db";

struct Resultado {
    int id;
    std::string texto;
    float score;
};

static void init_db(sqlite3* db) {
    char* errMsg = nullptr;

    const char* sql_metadata = 
        "CREATE TABLE IF NOT EXISTS document_chunks ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "document_name TEXT NOT NULL, "
        "chunk_index INTEGER NOT NULL, "
        "content TEXT NOT NULL);";

    std::string sql_vec =
        "CREATE VIRTUAL TABLE IF NOT EXISTS document_chunks_vec USING vec0(embedding float[" + std::to_string(EMBEDDING_DIM) + "]);";

    if (sqlite3_exec(db, sql_metadata, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Error creating metadata table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return;
    }

    if (sqlite3_exec(db, sql_vec.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Error creating vector table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return;
    }
}

static void insert(sqlite3* db, const std::string &document_name, const int &chunk_index, const std::string &content, const std::vector<float> &embedding) {
    sqlite3_stmt* stmt;

    const char* sql_metadata = "INSERT INTO document_chunks (document_name, chunk_index, content) VALUES (?, ?, ?);";
    
    sqlite3_prepare_v2(db, sql_metadata, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, document_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, chunk_index);
    sqlite3_bind_text(stmt, 3, content.c_str(), -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    sqlite3_int64 rowid = sqlite3_last_insert_rowid(db);

    const char* sql_vec = "INSERT INTO document_chunks_vec (rowid, embedding) VALUES (?, ?);";

    sqlite3_prepare_v2(db, sql_vec, -1, &stmt, nullptr);
    sqlite3_bind_int64(stmt, 1, rowid);
    sqlite3_bind_blob(stmt, 2, embedding.data(), embedding.size() * sizeof(float), SQLITE_STATIC); // Se guardan como bytes
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

std::vector<RetrievedChunk> search_similar(const std::vector<float>& query) {
    sqlite3* db;
    sqlite3_auto_extension((void(*)(void))sqlite3_vec_init);
    sqlite3_open(DB_PATH, &db);

    sqlite3_stmt* stmt;
    const char* sql =
        "SELECT c.document_name, c.chunk_index, c.content, v.distance "
        "FROM document_chunks_vec v "
        "JOIN document_chunks c ON c.id = v.rowid "
        "WHERE v.embedding MATCH ? "
        "AND k = ?  "
        "ORDER BY v.distance;";

    std::vector<RetrievedChunk> results;

    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_blob(stmt, 1, query.data(), query.size() * sizeof(float), SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, TOP_K);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        RetrievedChunk r;
        r.document_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        r.chunk_index   = sqlite3_column_int(stmt, 1);
        r.content       = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        r.distance      = sqlite3_column_double(stmt, 3);
        results.push_back(r);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return results;
}

std::vector<std::string> get_documents() {
    sqlite3* db;
    sqlite3_open(DB_PATH, &db);

    sqlite3_stmt* stmt;
    const char* sql = "SELECT DISTINCT document_name FROM document_chunks ORDER BY document_name;";
    std::vector<std::string> documents;

    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    while (sqlite3_step(stmt) == SQLITE_ROW)
        documents.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return documents;
}

void delete_document(const std::string& document_name) {
    sqlite3* db;
    sqlite3_open(DB_PATH, &db);

    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM document_chunks WHERE document_name = ?;";

    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, document_name.c_str(), -1, SQLITE_STATIC); // Se evita inyeccion SQL 
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void index_document(const std::string &document_name, const std::vector<std::string> &prompts, const std::vector<std::vector<float>> &embeddings) {
    sqlite3* db;

    sqlite3_auto_extension((void(*)(void))sqlite3_vec_init);

    sqlite3_open(DB_PATH, &db);
    init_db(db);

    for (size_t i = 0; i < embeddings.size(); ++i) {
        std::cout << "Indexando chunk " << i << " del documento " << document_name << std::endl;
        insert(db, document_name, i, prompts[i], embeddings[i]);
    }
    
    sqlite3_close(db);
}
