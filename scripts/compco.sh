#!/usr/bin/env bash
set -euo pipefail

# ARG 1 = source directory (.c files)
# ARG 2 = build directory (where the .o files will go)

# Absolute path to this script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# Project root = parent of bash_scripts
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"


# Usage check
if [[ $# -ne 2 ]]; then
    echo "Usage: $0 <source-directory> <object-directory>"
    exit 1
fi

SRCDIR="$PROJECT_ROOT/$1"
OBJDIR="$PROJECT_ROOT/$2"

SRCDIR="$(realpath "$SRCDIR")"
OBJDIR="$(realpath "$OBJDIR")"

COMPILE=arm-none-eabi-gcc

CFLAGS=(
    -mcpu=cortex-m3
    -mthumb
    -ffreestanding
    -Wall
    -Wextra
)

INCLUDES=(
    -I"$PROJECT_ROOT/include"
    -I"$PROJECT_ROOT/CMSIS/core/include"
    -I"$PROJECT_ROOT/CMSIS/device/ST/STM32F1xx/include"
)

mapfile -t CFILES < <(find "$SRCDIR" -name '*.c')

if (( ${#CFILES[@]} == 0 )); then
    echo "Error: no .c files found in $SRCDIR"
    exit 1
fi

echo "Compiling:"

for cfile in "${CFILES[@]}"; do
    # Path relative to source root
    relpath="${cfile#$SRCDIR/}"

    # Change extension and prepend object directory
    ofile="$OBJDIR/${relpath%.c}.o"

    # Ensure output directory exists
    mkdir -p "$(dirname "$ofile")"

    echo "  $relpath -> ${ofile#$OBJDIR/}"
    "$COMPILE" \
    "${CFLAGS[@]}" \
    "${INCLUDES[@]}" \
    -c "$cfile" -o "$ofile"
done

cd ..
cd build

echo "Done"

# stm32flash -w firmware.bin -v /dev/ttyUSB0