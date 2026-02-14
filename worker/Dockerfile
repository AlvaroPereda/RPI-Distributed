FROM debian:bookworm-slim AS builder 

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    && rm -rf  /var/lib/apt/lists/*

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
    && cmake --build . --config Release --target rpc-server -j$(nproc)

RUN strip -s bin/rpc-server

FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y \
    libgomp1 \
    curl \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /app/llama.cpp/build/bin/rpc-server /rpc-server

EXPOSE 50051

ENTRYPOINT [ "./rpc-server", "-p", "50051", "-H", "0.0.0.0" ]
