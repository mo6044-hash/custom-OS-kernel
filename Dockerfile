# Dockerfile for building the OS kernel
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    nasm \
    qemu-system-x86 \
    grub-pc-bin \
    xorriso \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /workspace

# Default command
CMD ["/bin/bash"]
