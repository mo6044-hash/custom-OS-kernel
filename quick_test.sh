#!/bin/bash
echo "=== Custom OS Kernel Test Script ==="
echo ""

# Check dependencies
echo "Checking dependencies..."
MISSING=0

if ! command -v gcc &> /dev/null; then
    echo "❌ GCC not found"
    MISSING=1
else
    echo "✅ GCC found: $(gcc --version | head -1)"
fi

if ! command -v nasm &> /dev/null; then
    echo "❌ NASM not found - Install with: brew install nasm"
    MISSING=1
else
    echo "✅ NASM found: $(nasm -v)"
fi

if ! command -v qemu-system-i386 &> /dev/null; then
    echo "❌ QEMU not found - Install with: brew install qemu"
    MISSING=1
else
    echo "✅ QEMU found"
fi

if [ $MISSING -eq 1 ]; then
    echo ""
    echo "Please install missing dependencies first!"
    echo "On macOS: brew install nasm qemu"
    exit 1
fi

echo ""
echo "Building kernel..."
make clean
make

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Build successful!"
    echo ""
    echo "Starting QEMU..."
    echo "You should see the kernel boot and a shell prompt (>)."
    echo "Try commands like: help, ls, create test.txt 1024"
    echo ""
    make run
else
    echo ""
    echo "❌ Build failed!"
    exit 1
fi
