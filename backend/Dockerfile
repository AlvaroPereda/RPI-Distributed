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
    && cmake --build . --config Release --target llama-server -j$(nproc)

RUN strip -s bin/llama-server

FROM ubuntu:22.04 AS builder-orchestator

RUN apt-get update && apt-get install -y g++

WORKDIR /app

COPY ./orchestator .

RUN g++ -O3 -static-libgcc -static-libstdc++ orchestator.cpp -o orchestator -lpthread


FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y \
    libgomp1 \
    curl \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /app/llama.cpp/build/bin/llama-server /llama-server
COPY --from=builder-orchestator /app/orchestator .

EXPOSE 8080

ENTRYPOINT ["./orchestator"]