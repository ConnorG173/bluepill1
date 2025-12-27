#!/usr/bin/env bash

#ARG 1 = directory where .bin is located
#ARG 2 = name of .bin to flash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# Project root = parent of bash_scripts
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

FLASH_DIR="$PROJECT_ROOT/$1/$2"

stm32flash -w $FLASH_DIR -v /dev/ttyUSB0