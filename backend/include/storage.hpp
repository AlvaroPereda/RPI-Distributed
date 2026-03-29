#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <vector>
#include <string>

struct RetrievedChunk {
    std::string document_name;
    int chunk_index;
    std::string content;
    double distance;
};

void index_document(const std::string &document_name, const std::vector<std::string> &prompts, const std::vector<std::vector<float>> &embeddings);
std::vector<std::string> get_documents();
void delete_document(const std::string& document_name);
std::vector<RetrievedChunk> search_similar(const std::vector<float>& query);

#endif
