#!/bin/bash

# arg 1 = .c to compile
# arg 2 = .o to compile the .c into
# arg 3 = linker script (.ld file) name
# arg 4 = .elf to link .o into
# arg 5 = .bin to copy .elf into

echo "compiling $1 .c file to $2 .o file"
arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -c -o $2 $1
echo "Waiting for compile"
wait
echo "compiled. Running linker $3. links $2 to $4"
arm-none-eabi-ld -T $3 -o $4 $2
wait
echo "linked. converting .elf to .bin. $4 -> $5"
arm-none-eabi-objcopy -O binary $4 $5
wait
echo "Done"


# stm32flash -w blink.bin -v /dev/ttyUSB0