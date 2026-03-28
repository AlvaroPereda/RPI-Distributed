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

RUN --mount=type=cache,id=llama-server-build-${TARGETPLATFORM},target=/build \
    cmake -S /llama.cpp -B /build \
        -DGGML_RPC=ON \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_SHARED_LIBS=OFF \
        -DLLAMA_BUILD_BORINGSSL=ON \
        -DGGML_NATIVE=OFF \
    && cmake --build /build -j$(nproc) --config Release -t llama-server \
    && cp /build/bin/llama-server /llama-server-bin \
    && strip -s /llama-server-bin


FROM debian:bookworm-slim AS builder-orchestrator
ARG LLAMA_TAG
ARG TARGETPLATFORM

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libsqlite3-dev \
    libssh-dev \
    && rm -rf /var/lib/apt/lists/*

RUN git clone --depth 1 --branch ${LLAMA_TAG} https://github.com/ggml-org/llama.cpp.git

COPY orchestrator.cpp         /llama.cpp/examples/rpc-embedding/orchestrator.cpp
COPY src/                     /llama.cpp/examples/rpc-embedding/src/
COPY include/                 /llama.cpp/examples/rpc-embedding/include/
COPY CMakeLists.txt           /llama.cpp/examples/rpc-embedding/CMakeLists.txt
COPY llama.cpp/CMakeLists.txt /llama.cpp/examples/CMakeLists.txt

RUN --mount=type=cache,id=orchestrator-build-${TARGETPLATFORM},target=/build \
    cmake -S /llama.cpp -B /build \
        -DLLAMA_BUILD_BORINGSSL=ON \
        -DBUILD_SHARED_LIBS=OFF \
    && cmake --build /build -j$(nproc) --config Release -t orchestrator \
    && cp /build/bin/orchestrator /orchestrator-bin \
    && strip -s /orchestrator-bin

FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y \
    libgomp1 \
    curl \
    libssh-4 \
    libsqlite3-0 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder              /llama-server-bin  ./llama-server
COPY --from=builder-orchestrator /orchestrator-bin  ./orchestrator
RUN mkdir rag

EXPOSE 8080

ENTRYPOINT ["./orchestrator"]