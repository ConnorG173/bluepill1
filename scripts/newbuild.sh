#!/usr/bin/env bash
set -euo pipefail


# ARG 1 = Directory of .o files

# ------------------------------------------------------------
# Toolchain
# ------------------------------------------------------------
LD="arm-none-eabi-ld"
OBJCOPY="arm-none-eabi-objcopy"

# ------------------------------------------------------------
# Resolve script directory (linker script lives here)
# ------------------------------------------------------------
#Absolute Path to Script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LINKER_SCRIPT="$SCRIPT_DIR/stm32f103.ld"

# Project root = parent of bash_scripts
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# ------------------------------------------------------------
# Input validation
# ------------------------------------------------------------
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <object_directory>" >&2
    exit 1
fi

OBJDIR="$PROJECT_ROOT/$1"

if [ ! -d "$OBJDIR" ]; then
    echo "Error: object directory does not exist: $OBJDIR" >&2
    exit 1
fi

if [ ! -f "$LINKER_SCRIPT" ]; then
    echo "Error: linker script not found: $LINKER_SCRIPT" >&2
    exit 1
fi

# ------------------------------------------------------------
# Collect object files
# ------------------------------------------------------------
OBJECTS=("$OBJDIR"/*.o)

if [ ! -e "${OBJECTS[0]}" ]; then
    echo "Error: no .o files found in $OBJDIR" >&2
    exit 1
fi

# ------------------------------------------------------------
# Output files (placed in object directory)
# ------------------------------------------------------------
ELF="$OBJDIR/firmware.elf"
BIN="$OBJDIR/firmware.bin"
MAP="$OBJDIR/firmware.map"

# ------------------------------------------------------------
# Info
# ------------------------------------------------------------
echo "Linking firmware"
echo "  Object directory : $OBJDIR"
echo "  Linker script    : $LINKER_SCRIPT"
echo "  Output ELF       : $ELF"
echo "  Output BIN       : $BIN"
echo "  Output MAP       : $MAP"
echo

# ------------------------------------------------------------
# Link
# ------------------------------------------------------------
"$LD" \
    -T "$LINKER_SCRIPT" \
    --gc-sections \
    -Map="$MAP" \
    "${OBJECTS[@]}" \
    -o "$ELF"

# ------------------------------------------------------------
# Convert ELF -> BIN
# ------------------------------------------------------------
"$OBJCOPY" -O binary "$ELF" "$BIN"

# ------------------------------------------------------------
# Done
# ------------------------------------------------------------
echo "Build complete."
