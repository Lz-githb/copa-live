ENTRY(_start)

MEMORY {
  rom  (rx)  : ORIGIN = 0x08000000, LENGTH = 32M
  iwram (rwx): ORIGIN = 0x03000000, LENGTH = 32K
  ewram (rw) : ORIGIN = 0x02000000, LENGTH = 256K
}

SECTIONS {
  .text : {
    KEEP(*(.text.header))
    *(.text*)
    *(.rodata*)
  } > rom

  .bss : {
    __bss_start = .;
    *(.bss*)
    *(COMMON)
    __bss_end = .;
  } > iwram

  .data : {
    *(.data*)
  } > iwram AT > rom
}
