ARG LLAMA_TAG=b8565

FROM debian:bookworm-slim AS builder
ARG LLAMA_TAG
ARG TARGETPLATFORM

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

RUN git clone --depth 1 --branch ${LLAMA_TAG} https://github.com/ggml-org/llama.cpp.git

RUN --mount=type=cache,id=rpc-server-build-${TARGETPLATFORM},target=/build \
    cmake -S /llama.cpp -B /build \
        -DGGML_RPC=ON \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_SHARED_LIBS=OFF \
        -DLLAMA_BUILD_BORINGSSL=ON \
        -DGGML_NATIVE=OFF \
    && cmake --build /build -j$(nproc) --config Release -t rpc-server \
    && cp /build/bin/rpc-server /rpc-server-bin \
    && strip -s /rpc-server-bin

FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y \
    libgomp1 \
    curl \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /rpc-server-bin /rpc-server

EXPOSE 50051

ENTRYPOINT [ "./rpc-server", "-p", "50051", "-H", "0.0.0.0" ]
