@ GBA CRT0 - inicialização mínima
.section .text.start
.global _start
.arm

_start:
    @ Cabeçalho GBA (obrigatório)
    b       _gba_start          @ branch para código
    .fill   156, 1, 0           @ logo e campos do cabeçalho (preenchidos depois)

_gba_start:
    @ Inicializa stack no topo da IWRAM
    mov     r0, #0x03000000
    add     sp, r0, #0x8000

    @ Limpa BSS
    ldr     r0, =__bss_start__
    ldr     r1, =__bss_end__
    mov     r2, #0
bss_loop:
    cmp     r0, r1
    strlt   r2, [r0], #4
    blt     bss_loop

    @ Chama main
    bl      main

inf_loop:
    b       inf_loop

.global __bss_start__
.global __bss_end__
