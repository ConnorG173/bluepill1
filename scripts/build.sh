#!/usr/bin/env bash
set -e

# Must place build.sh in same dir as .o files!

# Toolchain
LD=arm-none-eabi-ld
OBJCOPY=arm-none-eabi-objcopy

# Files
LINKER_SCRIPT=stm32f103.ld
ELF=firmware.elf
BIN=firmware.bin

# Collect all object files
OBJECTS=$(ls *.o 2>/dev/null || true)

if [ -z "$OBJECTS" ]; then
    echo "Error: no object files found"
    exit 1
fi

echo "Linking:"
echo "  $OBJECTS"

# Link
$LD -T $LINKER_SCRIPT $OBJECTS -Map=firmware.map -o $ELF

# Convert to binary
$OBJCOPY -O binary $ELF $BIN

echo "Build complete:"
echo "  $ELF"
echo "  $BIN"

# stm32flash -w firmware.bin -v /dev/ttyUSB0
