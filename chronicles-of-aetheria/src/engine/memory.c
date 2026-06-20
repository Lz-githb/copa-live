#include "../../include/engine/memory.h"
#include "../../include/gba/gba_bios.h"
#include "../../include/gba/gba_reg.h"

/* =========================================================
 * Memory Manager Implementation
 * ========================================================= */

/* Place pools in EWRAM to keep IWRAM for hot code */
static u8 EWRAM_BSS s_frame_pool[MEM_FRAME_POOL_SIZE]   ALIGN4;
static u8 EWRAM_BSS s_persist_pool[MEM_PERSIST_POOL_SIZE] ALIGN4;

static MemPool s_frame;
static MemPool s_persist;
static MemStack s_stack;

void mem_init(void) {
    s_frame.base     = s_frame_pool;
    s_frame.top      = s_frame_pool;
    s_frame.capacity = MEM_FRAME_POOL_SIZE;
    s_frame.used     = 0;

    s_persist.base     = s_persist_pool;
    s_persist.top      = s_persist_pool;
    s_persist.capacity = MEM_PERSIST_POOL_SIZE;
    s_persist.used     = 0;

    s_stack.sp = 0;

    mem_zero(s_frame_pool,   MEM_FRAME_POOL_SIZE);
    mem_zero(s_persist_pool, MEM_PERSIST_POOL_SIZE);
}

/* --- Frame pool ------------------------------------------ */

void* mem_frame_alloc(u32 size) {
    /* Round up to 4-byte alignment */
    size = (size + 3) & ~3u;

    if (s_frame.used + size > s_frame.capacity) {
        /* Frame pool overflow: this is a bug, return NULL
         * In debug builds you could trigger a hard-fault here */
        return NULL;
    }

    void* ptr    = s_frame.top;
    s_frame.top += size;
    s_frame.used += size;
    return ptr;
}

void mem_frame_reset(void) {
    s_frame.top  = s_frame.base;
    s_frame.used = 0;
}

/* --- Persistent pool ------------------------------------- */

void mem_push(void) {
    if (s_stack.sp < MEM_STACK_MAX) {
        s_stack.markers[s_stack.sp++] = s_persist.used;
    }
}

void* mem_alloc(u32 size) {
    size = (size + 3) & ~3u;

    if (s_persist.used + size > s_persist.capacity) {
        return NULL;
    }

    void* ptr      = s_persist.top;
    s_persist.top += size;
    s_persist.used += size;
    return ptr;
}

void mem_pop(void) {
    if (s_stack.sp == 0) return;
    u32 marker    = s_stack.markers[--s_stack.sp];
    s_persist.top  = s_persist.base + marker;
    s_persist.used = marker;
}

u32 mem_used(void)      { return s_persist.used; }
u32 mem_remaining(void) { return s_persist.capacity - s_persist.used; }

/* --- Utility -------------------------------------------- */

void mem_copy(void* dst, const void* src, u32 bytes) {
    /* Use BIOS CpuFastSet for speed (requires 4-byte alignment) */
    u32 words = bytes >> 2;
    if (words > 0) {
        mem_copy32(dst, src, words);
    }
    /* Handle trailing bytes (slow path, rare) */
    u32 tail = bytes & 3;
    if (tail) {
        u8* d = (u8*)dst + (words << 2);
        const u8* s = (const u8*)src + (words << 2);
        while (tail--) *d++ = *s++;
    }
}

void mem_set_zero(void* dst, u32 bytes) {
    mem_zero(dst, bytes);
}

void mem_set(void* dst, u8 val, u32 bytes) {
    /* For non-zero fill we can't use BIOS fast-fill easily,
     * so we build a 32-bit pattern and use DMA-style copy */
    u32 pattern = (u32)val | ((u32)val << 8) |
                  ((u32)val << 16) | ((u32)val << 24);
    u8* d = (u8*)dst;
    /* Align to 4 bytes first */
    while (((u32)d & 3) && bytes) { *d++ = val; bytes--; }
    /* Fill 32-bit words */
    u32* d32 = (u32*)d;
    u32 words = bytes >> 2;
    while (words--) *d32++ = pattern;
    /* Trailing bytes */
    d = (u8*)d32;
    bytes &= 3;
    while (bytes--) *d++ = val;
}
