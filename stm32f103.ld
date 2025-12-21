ENTRY(Reset_Handler)

MEMORY {
    flash (RX) : ORIGIN = 0x08000000, LENGTH = 64K
    sram  (RW) : ORIGIN = 0x20000000, LENGTH = 20K
}

SECTIONS
{
    . = ORIGIN(flash);

    .vectors ALIGN(128) :
    {
        KEEP(*(.vectors))
    } > flash

    .text :
    {
        _stext = .;
        *(.text*)
        *(.rodata*)
        _etext = .;
    } > flash

    .data : AT (_etext)
    {
        _sdata = .;
        *(.data*)
        _edata = .;
    } > sram

    _sidata = LOADADDR(.data);

    .bss :
    {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > sram

    ASSERT(ORIGIN(flash) == 0x08000000, "FLASH ORIGIN WRONG")
    _estack = ORIGIN(sram) + LENGTH(sram);
}
