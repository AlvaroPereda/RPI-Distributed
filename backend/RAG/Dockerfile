FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libsqlite3-dev \
    && rm -rf /var/lib/apt/lists/*

RUN git clone --depth 1 https://github.com/ggml-org/llama.cpp.git

WORKDIR /llama.cpp

COPY main.cpp         ./examples/rpc-embedding/main.cpp
COPY src/             ./examples/rpc-embedding/src/
COPY include/         ./examples/rpc-embedding/include/
COPY CMakeLists.txt   ./examples/rpc-embedding/CMakeLists.txt
COPY llama.cpp/CMakeLists.txt ./examples/CMakeLists.txt
COPY rag.db .

RUN cmake -S . -B build -DLLAMA_BUILD_BORINGSSL=ON && \
    cmake --build build -j$(nproc) --config Release -t main && \
    mv build/bin/main . && \
    strip -s main

ENTRYPOINT ["./main"]