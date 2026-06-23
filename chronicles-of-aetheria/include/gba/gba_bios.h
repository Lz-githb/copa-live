#ifndef GBA_BIOS_H
#define GBA_BIOS_H

#include "gba_types.h"

/* =========================================================
 * GBA BIOS Software Interrupts
 * Call via ARM swi instruction; faster than software impl
 * ========================================================= */

/* VBlank wait — busy-wait (avoids BIOS SWI interaction issues) */
static inline void VBlankIntrWait(void) {
    volatile u16* dispstat = (volatile u16*)0x04000004;
    while (!(*dispstat & 1));
    while (*dispstat & 1);
}

/* Halt — suspend CPU until any interrupt */
static inline void Halt(void) {
    __asm__ volatile ("swi 0x02" ::: "r0", "r1", "r2", "r3");
}

/* Stop — deep sleep; wakeup on keypad or cartridge interrupt */
static inline void Stop(void) {
    __asm__ volatile ("swi 0x03" ::: "r0", "r1", "r2", "r3");
}

/* CPU fast-fill: fills dst with 32-bit value, len in 32-bit units */
static inline void CpuFastSet(const void* src, void* dst, u32 len_mode) {
    register const void* r0 __asm__("r0") = src;
    register void*       r1 __asm__("r1") = dst;
    register u32         r2 __asm__("r2") = len_mode;
    __asm__ volatile ("swi 0x0C" :: "r"(r0), "r"(r1), "r"(r2) : "r3", "memory");
}

/* CPU set: general copy/fill */
static inline void CpuSet(const void* src, void* dst, u32 len_mode) {
    register const void* r0 __asm__("r0") = src;
    register void*       r1 __asm__("r1") = dst;
    register u32         r2 __asm__("r2") = len_mode;
    __asm__ volatile ("swi 0x0B" :: "r"(r0), "r"(r1), "r"(r2) : "r3", "memory");
}

/* CpuFastSet flags */
#define CPUFSSET_COPY 0x00000000
#define CPUFSSET_FILL 0x01000000

/* Sqrt (BIOS) */
static inline u32 Sqrt(u32 x) {
    register u32 r0 __asm__("r0") = x;
    __asm__ volatile ("swi 0x08" : "+r"(r0) :: "r1", "r2", "r3");
    return r0;
}

/* ArcTan2 */
static inline s16 ArcTan2(s16 x, s16 y) {
    register s32 r0 __asm__("r0") = x;
    register s32 r1 __asm__("r1") = y;
    __asm__ volatile ("swi 0x0A" : "+r"(r0) : "r"(r1) : "r2", "r3");
    return (s16)r0;
}

/* ---- Fast memory helpers ---------------------------------- */
/* Fill 32-bit words using plain C (safe, no BIOS alignment requirements) */
INLINE void mem_fill32(void* dst, u32 val, u32 n) {
    u32* p = (u32*)dst;
    while (n--) *p++ = val;
}

/* Copy 32-bit words */
INLINE void mem_copy32(void* dst, const void* src, u32 n) {
    const u32* s = (const u32*)src;
    u32*       d = (u32*)dst;
    while (n--) *d++ = *s++;
}

/* Zero a region (handles any size, any alignment) */
INLINE void mem_zero(void* dst, u32 bytes) {
    u32  words = bytes >> 2;
    u32  tail  = bytes & 3;
    u32* p     = (u32*)dst;
    while (words--) *p++ = 0;
    if (tail) {
        u8* q = (u8*)p;
        while (tail--) *q++ = 0;
    }
}

#endif /* GBA_BIOS_H */
