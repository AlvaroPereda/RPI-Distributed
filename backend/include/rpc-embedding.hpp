#ifndef RPC_EMBEDDING_HPP
#define RPC_EMBEDDING_HPP

#include <string>
#include <vector>
#include "storage.hpp"

std::vector<RetrievedChunk> generate_embeddings(Storage& storage, const std::string &prompt);
void generate_embeddings(Storage& storage, const std::string &filename, const std::string &prompt);

#endif
