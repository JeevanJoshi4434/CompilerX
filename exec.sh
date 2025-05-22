#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <filename.cx>"
    exit 1
fi

FILENAME=$(basename "$1" .cx)

if [ ! -f "$1" ] || [[ "$1" != *.cx ]]; then
    echo "Error: File $1 does not exist or is not a .cx file"
    exit 1
fi

echo "=== Compiling $1 using cmpx ==="
./cmpx "$1"

if [ $? -ne 0 ]; then
    echo "Error: Compilation failed"
    exit 1
fi

ASM_FILE="${FILENAME}.asm"

if [ ! -f "$ASM_FILE" ]; then
    echo "Error: Assembly file $ASM_FILE was not generated"
    exit 1
fi

echo "=== Assembling $ASM_FILE ==="
nasm -f elf64 "$ASM_FILE" -o "${FILENAME}.o"

if [ $? -ne 0 ]; then
    echo "Error: Assembly failed"
    exit 1
fi

echo "=== Linking ${FILENAME}.o ==="
ld -o "${FILENAME}" "${FILENAME}.o"

if [ $? -ne 0 ]; then
    echo "Error: Linking failed"
    exit 1
fi

echo "=== Running ${FILENAME} ==="
./"${FILENAME}"

echo "=== Execution complete ==="

echo "=== Cleaning up temporary files ==="
rm -f "${FILENAME}.o" "${FILENAME}.asm" "${FILENAME}"
echo "=== Cleanup complete ==="