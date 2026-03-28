FROM debian:bookworm-slim AS builder 

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

RUN git clone --depth 1 https://github.com/ggml-org/llama.cpp.git

WORKDIR /app/llama.cpp/build

RUN cmake .. \
    -DGGML_RPC=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=OFF \
    -DLLAMA_BUILD_BORINGSSL=ON \
    -DLLAMA_BUILD_LIBRESSL=ON \
    -DGGML_NATIVE=OFF \
    && cmake --build . --config Release --target llama-server -j$(nproc)

RUN strip -s bin/llama-server


FROM debian:bookworm-slim AS builder-orchestrator

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libsqlite3-dev \
    libssh-dev \
    && rm -rf /var/lib/apt/lists/*

RUN git clone --depth 1 https://github.com/ggml-org/llama.cpp.git

WORKDIR /llama.cpp

COPY orchestrator.cpp         ./examples/rpc-embedding/orchestrator.cpp
COPY src/             ./examples/rpc-embedding/src/
COPY include/         ./examples/rpc-embedding/include/
COPY CMakeLists.txt   ./examples/rpc-embedding/CMakeLists.txt
COPY llama.cpp/CMakeLists.txt ./examples/CMakeLists.txt

RUN cmake -S . -B build -DLLAMA_BUILD_BORINGSSL=ON -DBUILD_SHARED_LIBS=OFF && \
    cmake --build build -j$(nproc) --config Release -t orchestrator && \
    mv build/bin/orchestrator . && \
    strip -s orchestrator

FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y \
    libgomp1 \
    curl \
    libssh-4 \
    libsqlite3-0 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /app/llama.cpp/build/bin/llama-server /llama-server
COPY --from=builder-orchestrator /llama.cpp/orchestrator .
COPY rag.db .

EXPOSE 8080

ENTRYPOINT ["./orchestrator"]