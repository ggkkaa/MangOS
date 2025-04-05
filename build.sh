#!/bin/bash

set -e

IMAGE_NAME=mangos-buildenv
DOCKERFILE_DIR=buildenv
ISO_FILE=dist/x86_64/MangOS.iso

echo "[*] If the script freezes here, don't worry."

if ! docker info > /dev/null 2>&1; then
    echo "[!]   Docker does not seem to be running."
    echo "      Please start the Docker Engine and try again."
    exit 1
fi

if ! docker image inspect "$IMAGE_NAME" > /dev/null 2>&1; then
    echo "[*] Docker image not found. Building..."
    docker build "$DOCKERFILE_DIR" -t "$IMAGE_NAME"
fi

echo "Building ISO image..."

docker run --rm -it \
    -v "$PWD":/project \
    -w /project \
    "$IMAGE_NAME" \
    make iso

echo "[*] Running '$ISO_FILE' in QEMU"
qemu-system-x86_64 -cdrom "$ISO_FILE" -serial stdio -m 1024 --no-reboot