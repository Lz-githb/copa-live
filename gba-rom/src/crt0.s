@ GBA ROM header + startup
    .section .text.header
    .arm
    .global _start

_start:
    b       _rom_start          @ branch to code (skips header data)

    @ Nintendo logo (must be present and correct for real HW boot, emulators usually skip)
    .fill   156, 1, 0x00

    @ Game title (12 bytes, uppercase ASCII, padded with 0)
    .ascii  "GBA10GAMES  "

    @ Game code (4 bytes)
    .ascii  "AGBJ"

    @ Maker code (2 bytes)
    .ascii  "01"

    @ Fixed value
    .byte   0x96

    @ Unit code, device type
    .byte   0x00, 0x00

    @ Reserved (7 bytes)
    .fill   7, 1, 0x00

    @ Software version
    .byte   0x00

    @ Complement check (will be patched by gbafix)
    .byte   0x00

    @ Reserved (2 bytes)
    .fill   2, 1, 0x00

_rom_start:
    @ Set up stack pointer in IWRAM
    ldr     sp, =0x03008000

    @ Clear BSS
    ldr     r0, =__bss_start
    ldr     r1, =__bss_end
    mov     r2, #0
.Lbss_clear:
    cmp     r0, r1
    strlt   r2, [r0], #4
    blt     .Lbss_clear

    @ Jump to main
    bl      main
    b       .
