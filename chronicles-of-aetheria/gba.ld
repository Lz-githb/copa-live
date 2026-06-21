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
    /* ROM: header + code + rodata */
    .text 0x08000000 : {
        KEEP(*(.crt0))
        *(.text .text.*)
        *(.rodata .rodata.*)
        . = ALIGN(4);
        _text_end = .;
    } > ROM

    /* EWRAM: initialized data (VMA=EWRAM, LMA=ROM after .text) */
    .data 0x02000000 : AT(_text_end) {
        _data_vma = .;
        *(.data .data.*)
        . = ALIGN(4);
        _data_vma_end = .;
    } > EWRAM
    _data_lma  = LOADADDR(.data);
    _data_size = _data_vma_end - _data_vma;

    /* EWRAM: zero-init BSS */
    .bss (NOLOAD) : {
        _bss_start = .;
        *(.bss .bss.*)
        *(.sbss .sbss.*)
        *(COMMON)
        . = ALIGN(4);
        _bss_end = .;
    } > EWRAM

    /* IWRAM: time-critical code (copied from ROM at boot) */
    .iwram 0x03000000 : AT(_text_end + _data_size) {
        _iwram_start = .;
        *(.iwram)          /* only exact .iwram sections (code), not .iwram.bss */
        . = ALIGN(4);
        _iwram_end = .;
    } > IWRAM
    _data_end = _text_end + _data_size;   /* LMA base for IWRAM copy */

    /* IWRAM BSS: uninit data in IWRAM (zeroed at boot, NOT copied from ROM) */
    .iwram.bss (NOLOAD) : {
        _iwram_bss_start = .;
        *(.iwram.bss .iwram.bss.*)
        . = ALIGN(4);
        _iwram_bss_end = .;
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
