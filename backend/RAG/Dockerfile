FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libsqlite3-dev \
    && rm -rf /var/lib/apt/lists/*

RUN git clone --depth 1 https://github.com/ggml-org/llama.cpp.git

WORKDIR /llama.cpp

COPY src/ ./examples/rpc-embedding
COPY llama.cpp/CMakeLists.txt ./examples/CMakeLists.txt

WORKDIR /llama.cpp/build

RUN cmake .. && make -j$(nproc) main

ENTRYPOINT ["./bin/main"]