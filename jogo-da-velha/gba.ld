ENTRY(_start)

MEMORY {
    rom  (rx)  : ORIGIN = 0x08000000, LENGTH = 32M
    iwram(rwx) : ORIGIN = 0x03000000, LENGTH = 32K
    ewram(rwx) : ORIGIN = 0x02000000, LENGTH = 256K
}

SECTIONS {
    .text : {
        *(.text.start)
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
    } > rom

    .data : {
        *(.data*)
        . = ALIGN(4);
    } > iwram

    .bss : {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > iwram
}
