#ifndef GBA_BIOS_H
#define GBA_BIOS_H

#include "gba_types.h"

/* =========================================================
 * GBA BIOS Software Interrupts
 * Call via ARM swi instruction; faster than software impl
 * ========================================================= */

/* VBlank wait — suspends CPU until next VBlank */
static inline void VBlankIntrWait(void) {
    __asm__ volatile ("swi 0x05" ::: "r0", "r1", "r2", "r3");
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
/* Fill 32-bit words; n = number of 32-bit words */
INLINE void mem_fill32(void* dst, u32 val, u32 n) {
    u32 buf[1] = { val };
    CpuFastSet(buf, dst, CPUFSSET_FILL | n);
}

/* Copy 32-bit words; n = number of 32-bit words */
INLINE void mem_copy32(void* dst, const void* src, u32 n) {
    CpuFastSet(src, dst, CPUFSSET_COPY | n);
}

/* Zero a region */
INLINE void mem_zero(void* dst, u32 bytes) {
    mem_fill32(dst, 0, bytes >> 2);
}

#endif /* GBA_BIOS_H */
