#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <vector>
#include <string>
#include <sqlite3.h>

struct RetrievedChunk {
    std::string document_name;
    int chunk_index;
    std::string content;
    double distance;
};

class Storage
{
    private:
        sqlite3* db = nullptr;
        void init_schema();
        void insert_chunk(const std::string& document_name, const int chunk_index, const std::string& content, const std::vector<float>& embedding);
    public:
        Storage();
        ~Storage();
        void index_document(const std::string& document_name, const std::vector<std::string>& prompts, const std::vector<std::vector<float>>& embeddings);
        void delete_document(const std::string& document_name);
        std::vector<RetrievedChunk> search_similar(const std::vector<float>& query);
        std::vector<std::string> get_documents();
};

#endif
