FROM ghcr.io/philips-software/amp-devcontainer-cpp:latest

# Install debugging tools
RUN apt-get update && apt-get install -y \
    gdb-multiarch \
    iproute2 \
    jq \
    curl \
    wget \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /workspace

# Copy the project files into the container
COPY . .

# Make scripts executable and run initial setup
RUN chmod +x scripts/*.sh && \
    ./scripts/get-wsl-ip.sh

CMD ["bash"]