#ifndef GBA_DMA_H
#define GBA_DMA_H

#include "gba_types.h"
#include "gba_reg.h"

/* =========================================================
 * DMA Transfer Helpers
 * DMA3 is the general-purpose transfer channel
 * DMA1/DMA2 are used for audio FIFO
 * ========================================================= */

/* General 16-bit DMA copy (DMA3) */
INLINE void dma3_copy16(void* dst, const void* src, u32 count) {
    REG_DMA3CNT_H = 0;
    REG_DMA3SAD   = (u32)src;
    REG_DMA3DAD   = (u32)dst;
    REG_DMA3CNT_L = (u16)count;
    REG_DMA3CNT_H = DMA_ENABLE | DMA_16BIT;
}

/* General 32-bit DMA copy (DMA3) */
INLINE void dma3_copy32(void* dst, const void* src, u32 count) {
    REG_DMA3CNT_H = 0;
    REG_DMA3SAD   = (u32)src;
    REG_DMA3DAD   = (u32)dst;
    REG_DMA3CNT_L = (u16)count;
    REG_DMA3CNT_H = DMA_ENABLE | DMA_32BIT;
}

/* DMA fill: repeating 32-bit value */
INLINE void dma3_fill32(void* dst, u32 val, u32 count) {
    REG_DMA3CNT_H = 0;
    REG_DMA3SAD   = (u32)&val;
    REG_DMA3DAD   = (u32)dst;
    REG_DMA3CNT_L = (u16)count;
    REG_DMA3CNT_H = DMA_ENABLE | DMA_32BIT | DMA_SRC_FIXED;
}

/* Copy data into VRAM tile memory */
INLINE void vram_copy_tiles(u32 charblock, u32 tile_idx,
                             const void* src, u32 tile_count) {
    u32* dst = (u32*)(MEM_VRAM + (charblock * 0x4000) + (tile_idx * 32));
    dma3_copy32(dst, src, tile_count * 8);   /* 8 u32 per 8x8 4bpp tile */
}

/* Copy a screenblock (2KB = 1024 u16 entries) */
INLINE void vram_copy_map(u32 screenblock, const u16* src) {
    u16* dst = (u16*)(MEM_VRAM + (screenblock * 0x800));
    dma3_copy16(dst, src, 1024);
}

/* Setup DMA1 for audio FIFO A (repeating at timer0) */
INLINE void dma1_audio_fifo_a(const void* src) {
    REG_DMA1CNT_H = 0;
    REG_DMA1SAD   = (u32)src;
    REG_DMA1DAD   = (u32)&REG_FIFO_A;
    REG_DMA1CNT_L = 0;   /* FIFO mode: count ignored */
    REG_DMA1CNT_H = DMA_ENABLE | DMA_32BIT | DMA_REPEAT |
                    DMA_DST_FIXED | DMA_AT_SPECIAL;
}

/* Setup DMA2 for audio FIFO B (repeating at timer1) */
INLINE void dma2_audio_fifo_b(const void* src) {
    REG_DMA2CNT_H = 0;
    REG_DMA2SAD   = (u32)src;
    REG_DMA2DAD   = (u32)&REG_FIFO_B;
    REG_DMA2CNT_L = 0;
    REG_DMA2CNT_H = DMA_ENABLE | DMA_32BIT | DMA_REPEAT |
                    DMA_DST_FIXED | DMA_AT_SPECIAL;
}

/* Stop DMA channels */
INLINE void dma1_stop(void) { REG_DMA1CNT_H = 0; }
INLINE void dma2_stop(void) { REG_DMA2CNT_H = 0; }
INLINE void dma3_stop(void) { REG_DMA3CNT_H = 0; }

#endif /* GBA_DMA_H */
