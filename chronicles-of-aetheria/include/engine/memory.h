#ifndef ENGINE_MEMORY_H
#define ENGINE_MEMORY_H

#include "../gba/gba_types.h"

/* =========================================================
 * Memory Manager
 *
 * GBA has no OS allocator. We use a two-pool system:
 *   - EWRAM frame pool: reset each frame (temporary allocs)
 *   - EWRAM persistent pool: lives until explicitly freed
 *
 * No pointer arithmetic beyond these pools — malloc/free
 * are too slow and fragment memory on a 16MHz CPU.
 * ========================================================= */

/* Pool sizes */
#define MEM_FRAME_POOL_SIZE   (16 * 1024)    /* 16KB frame temp */
#define MEM_PERSIST_POOL_SIZE (64 * 1024)    /* 64KB persistent */
#define MEM_STACK_MAX         32             /* max alloc markers */

typedef struct {
    u8* base;
    u8* top;
    u32 capacity;
    u32 used;
} MemPool;

typedef struct {
    u32 markers[MEM_STACK_MAX];
    u32 sp;
} MemStack;

/* Initialize both pools (called once at startup) */
void mem_init(void);

/* --- Frame pool: reset every frame ----------------------- */
/* Allocate from frame pool; aligned to 4 bytes */
void* mem_frame_alloc(u32 size);

/* Reset frame pool (call at end of each frame) */
void mem_frame_reset(void);

/* --- Persistent pool: scoped with push/pop stack --------- */
/* Push a marker (save current position) */
void  mem_push(void);

/* Allocate from persistent pool */
void* mem_alloc(u32 size);

/* Pop to last marker (free everything allocated since push) */
void  mem_pop(void);

/* How much persistent pool is used */
u32 mem_used(void);
u32 mem_remaining(void);

/* --- Utility -------------------------------------------- */
/* Copy n bytes (must be multiple of 4 for DMA copy) */
void mem_copy(void* dst, const void* src, u32 bytes);

/* Zero n bytes */
void mem_set_zero(void* dst, u32 bytes);

/* Fill n bytes with a value */
void mem_set(void* dst, u8 val, u32 bytes);

#endif /* ENGINE_MEMORY_H */
