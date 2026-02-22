#ifndef RPC_EMBEDDING_HPP
#define RPC_EMBEDDING_HPP

#include <string>
#include <vector>

struct RagResult {
    std::string document_name;
    std::string content;
    float score;
};

std::vector<RagResult> generate_embeddings(const std::string &prompt);
void generate_embeddings(const std::string &filename, const std::string &prompt);

#endif