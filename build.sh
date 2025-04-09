#!/bin/bash

set -e

IMAGE_NAME=mangos-buildenv
DOCKERFILE_DIR=buildenv
ISO_FILE=dist/x86_64/MangOS.iso

echo "[*] Building"

make iso

echo "[*] Running '$ISO_FILE' in QEMU"
qemu-system-x86_64 -cdrom "$ISO_FILE" -serial stdio --no-reboot