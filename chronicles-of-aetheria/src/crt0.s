@ GBA crt0 — standalone startup without devkitPro
@ Sets up stacks, clears BSS, copies IWRAM, then calls main()

    .section .crt0, "ax", %progbits
    .global _start
    .arm

_start:
    @ --- GBA ROM header (192 bytes) ---
    b       _gba_init           @ branch past header
    .fill   156, 1, 0           @ Nintendo logo placeholder (gbafix patches this)
    .ascii  "AETHERIA"          @ Game title (12 bytes, padded)
    .fill   4, 1, 0
    .ascii  "CAEG"              @ Game code
    .ascii  "GB"                @ Maker code
    .byte   0x96                @ Fixed value
    .byte   0x00                @ Unit code
    .byte   0x00                @ Device type
    .fill   7, 1, 0             @ Reserved
    .byte   0x00                @ Software version
    .byte   0x00                @ Complement check (gbafix patches)
    .fill   2, 1, 0             @ Reserved

_gba_init:
    @ Disable interrupts
    mov     r0, #0x04000000
    mov     r1, #0
    str     r1, [r0, #0x208]    @ REG_IME = 0

    @ Set up IRQ stack (mode 0x12)
    msr     cpsr_c, #0xD2
    ldr     sp, =_sp_irq

    @ Set up SYS/USR stack (mode 0x1F)
    msr     cpsr_c, #0xDF
    ldr     sp, =_sp_usr

    @ Clear BSS
    ldr     r0, =_bss_start
    ldr     r1, =_bss_end
    mov     r2, #0
_clear_bss:
    cmp     r0, r1
    strlt   r2, [r0], #4
    blt     _clear_bss

    @ Copy IWRAM section from ROM LMA to IWRAM VMA
    ldr     r0, =_iwram_start
    ldr     r1, =_iwram_end
    ldr     r2, =_data_end      @ LMA of IWRAM in ROM
_copy_iwram:
    cmp     r0, r1
    ldrlt   r3, [r2], #4
    strlt   r3, [r0], #4
    blt     _copy_iwram

    @ Call main()
    bl      main

_halt:
    b       _halt

@ ---- Minimal C runtime builtins (ARM mode) ---------------
    .section .text.memset, "ax", %progbits
    .global memset
    .arm
memset:
    @ r0=dest, r1=val, r2=count  returns r0
    stmfd   sp!, {r0, r4, lr}
    mov     r3, r0
    cmp     r2, #0
    beq     2f
1:  strb    r1, [r3], #1
    subs    r2, r2, #1
    bne     1b
2:  ldmfd   sp!, {r0, r4, pc}

    .section .text.memcpy, "ax", %progbits
    .global memcpy
    .arm
memcpy:
    @ r0=dest, r1=src, r2=count  returns r0
    stmfd   sp!, {r0, r4, lr}
    mov     r3, r0
    cmp     r2, #0
    beq     2f
1:  ldrb    r4, [r1], #1
    strb    r4, [r3], #1
    subs    r2, r2, #1
    bne     1b
2:  ldmfd   sp!, {r0, r4, pc}

    .section .text.memmove, "ax", %progbits
    .global memmove
    .arm
memmove:
    @ r0=dest, r1=src, r2=count  returns r0
    stmfd   sp!, {r0, r4, lr}
    cmp     r0, r1
    bls     _memmove_fwd
    add     r3, r0, r2
    add     r1, r1, r2
    cmp     r2, #0
    beq     2f
1:  ldrb    r4, [r1, #-1]!
    strb    r4, [r3, #-1]!
    subs    r2, r2, #1
    bne     1b
    b       2f
_memmove_fwd:
    mov     r3, r0
    cmp     r2, #0
    beq     2f
3:  ldrb    r4, [r1], #1
    strb    r4, [r3], #1
    subs    r2, r2, #1
    bne     3b
2:  ldmfd   sp!, {r0, r4, pc}
