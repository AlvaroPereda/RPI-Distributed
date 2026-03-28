#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <vector>
#include <string>

struct ChunkRow {
    std::string document_name;
    int chunk_index;
    std::string content;
    std::vector<float> embedding;
};

void index_document(const std::string &document_name, const std::vector<std::string> &prompts, const std::vector<std::vector<float>> &embeddings);
std::vector<ChunkRow> get_all_chunks();

#endif
