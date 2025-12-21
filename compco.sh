#!/usr/bin/env bash
# arg 1 = .c to compile
# arg 2 = .o to compile the .c into

echo "compiling $1 .c file to $2 .o file"
arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -c -o $2 $1
wait 
echo "done"