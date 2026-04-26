FROM ubuntu:22.04

LABEL maintainer="RexCoin Developers"
LABEL description="RexCoin (RXC) - Rex Network Node"

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libssl-dev \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /rexcoin

# Copy source
COPY . .

# Build
RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build . -j$(nproc)

# Expose ports
# 8333 = P2P Network
# 8332 = JSON-RPC
# 8080 = REST API
# 8765 = WebSocket
EXPOSE 8333 8332 8080 8765

# Create data directory
RUN mkdir -p /root/.rexcoin

# Run node
CMD ["./build/rexcoin"]
