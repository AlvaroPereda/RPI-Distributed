#include <iostream>
#include <vector>
#include <sqlite3.h>
#include <cmath>
#include <algorithm>
#include <cstring> // Para memcpy
#include <fstream>
#include <sstream>

#include "storage.hpp"

const size_t EMBEDDING_DIM = 768;
static const char* DB_PATH = "rag/rag.db";

struct Resultado {
    int id;
    std::string texto;
    float score;
};

void init_db(sqlite3* db) {
    const char* sql = 
        "CREATE TABLE IF NOT EXISTS document_chunks ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "document_name TEXT NOT NULL, "
        "chunk_index INTEGER NOT NULL, "
        "content TEXT NOT NULL, "
        "embedding BLOB NOT NULL);"; // BLOB guarda los bytes crudos del vector
    
    char* errMsg = 0;
    sqlite3_exec(db, sql, 0, 0, &errMsg);
}

void insert(sqlite3* db, const std::string &document_name, const int &chunk_index, const std::string &content, const std::vector<float> &embedding) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO document_chunks (document_name, chunk_index, content, embedding) VALUES (?, ?, ?, ?);";
    
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    
    // Bind Texto
    sqlite3_bind_text(stmt, 1, document_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, chunk_index);
    sqlite3_bind_text(stmt, 3, content.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 4, embedding.data(), embedding.size() * sizeof(float), SQLITE_STATIC); // Se guardan como bytes
    
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

std::vector<ChunkRow> get_all_chunks() {
    sqlite3* db;
    sqlite3_open(DB_PATH, &db);

    sqlite3_stmt* stmt;
    const char* sql = "SELECT document_name, chunk_index, content, embedding FROM document_chunks;";
    std::vector<ChunkRow> chunks;

    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        ChunkRow row;
        row.document_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        row.chunk_index = sqlite3_column_int(stmt, 1);
        row.content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        const float* blob = reinterpret_cast<const float*>(sqlite3_column_blob(stmt, 3));
        int num_floats = sqlite3_column_bytes(stmt, 3) / sizeof(float);
        row.embedding = std::vector<float>(blob, blob + num_floats);

        chunks.push_back(row);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return chunks;
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
    sqlite3_open(DB_PATH, &db);
    init_db(db);

    for (size_t i = 0; i < embeddings.size(); ++i) {
        std::cout << "Indexando chunk " << i << " del documento " << document_name << std::endl;
        insert(db, document_name, i, prompts[i], embeddings[i]);
    }
    
    sqlite3_close(db);
}
