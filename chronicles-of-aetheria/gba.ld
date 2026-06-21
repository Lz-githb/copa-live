/* GBA Linker Script — standalone (no devkitPro) */

OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY {
    ROM  (rx)  : ORIGIN = 0x08000000, LENGTH = 32M
    EWRAM (rwx): ORIGIN = 0x02000000, LENGTH = 256K
    IWRAM (rwx): ORIGIN = 0x03000000, LENGTH = 32K
}

SECTIONS {
    /* ROM: header + code + rodata + initialized data */
    .text 0x08000000 : {
        KEEP(*(.crt0))
        *(.text .text.*)
        *(.rodata .rodata.*)
        *(.data .data.*)
        . = ALIGN(4);
        _data_end = .;
    } > ROM

    /* EWRAM: zero-init BSS */
    .bss (NOLOAD) : {
        _bss_start = .;
        *(.bss .bss.*)
        *(.sbss .sbss.*)
        *(COMMON)
        . = ALIGN(4);
        _bss_end = .;
    } > EWRAM

    /* IWRAM: time-critical code tagged IWRAM_CODE */
    .iwram 0x03000000 : AT(_data_end) {
        _iwram_start = .;
        *(.iwram .iwram.*)
        *(.iwram.bss .iwram.bss.*)
        . = ALIGN(4);
        _iwram_end = .;
    } > IWRAM

    _sp_irq  = ORIGIN(IWRAM) + LENGTH(IWRAM) - 0x060;
    _sp_usr  = ORIGIN(IWRAM) + LENGTH(IWRAM) - 0x100;

    /DISCARD/ : {
        *(.ARM.exidx*)
        *(.ARM.attributes)
        *(.comment)
        *(.debug*)
    }
}
