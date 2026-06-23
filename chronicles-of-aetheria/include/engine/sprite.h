#ifndef ENGINE_SPRITE_H
#define ENGINE_SPRITE_H

#include "../gba/gba_types.h"
#include "../gba/gba_reg.h"

/* =========================================================
 * Sprite System (OAM Manager)
 *
 * GBA supports 128 hardware OBJ sprites. We double-buffer
 * OAM in IWRAM and DMA it to hardware on VBlank. This
 * prevents tearing and lets us build the list freely.
 *
 * Sprites use a handle-based API so internal indices can
 * be compacted without invalidating caller references.
 * ========================================================= */

#define SPRITE_COUNT      128
#define SPRITE_TILE_BASE  512    /* OBJ tiles start at tile 512 in VRAM */
#define INVALID_SPRITE    0xFF

/* Sizes: (shape << 2 | size) encodes attr0/attr1 shape+size bits */
typedef enum {
    SPR_SIZE_8x8   = 0,
    SPR_SIZE_16x16 = 1,
    SPR_SIZE_32x32 = 2,
    SPR_SIZE_64x64 = 3,
    SPR_SIZE_16x8  = 4,
    SPR_SIZE_32x8  = 5,
    SPR_SIZE_32x16 = 6,
    SPR_SIZE_64x32 = 7,
    SPR_SIZE_8x16  = 8,
    SPR_SIZE_8x32  = 9,
    SPR_SIZE_16x32 = 10,
    SPR_SIZE_32x64 = 11,
} SpriteSize;

typedef struct {
    s16 x, y;          /* screen position */
    u16 tile;          /* base tile index in OBJ VRAM */
    u8  palette;       /* palette slot 0-15 */
    u8  priority;      /* draw priority 0-3 (0=front) */
    SpriteSize size;
    BOOL hflip;
    BOOL vflip;
    BOOL hidden;
    u8   anim_frame;   /* current animation frame */
    u8   anim_timer;   /* frames until next anim frame */
} Sprite;

typedef u8 SpriteHandle;

/* ---- Sprite system API ----------------------------------- */

void sprite_init(void);

/* Allocate a sprite slot; returns INVALID_SPRITE if full */
SpriteHandle sprite_alloc(void);

/* Free a slot */
void sprite_free(SpriteHandle h);

/* Set position */
void sprite_set_pos(SpriteHandle h, s16 x, s16 y);

/* Set tile, palette, size */
void sprite_set_gfx(SpriteHandle h, u16 tile, u8 palette, SpriteSize size);

/* Set flip flags */
void sprite_set_flip(SpriteHandle h, BOOL hflip, BOOL vflip);

/* Show/hide without freeing */
void sprite_set_hidden(SpriteHandle h, BOOL hidden);

/* Set draw priority (0 = above all BGs) */
void sprite_set_priority(SpriteHandle h, u8 prio);

/* Load n tiles of 4bpp OBJ tile data into VRAM at tile_idx */
void sprite_load_tiles(u16 tile_idx, const u32* data, u32 tile_count);

/* Load a 16-color palette into OBJ palette slot */
void sprite_load_palette(u8 slot, const COLOR* pal);

/* Write shadow OAM to hardware (call during VBlank) */
void sprite_flush(void);

/* Hide all sprites (clear OAM) */
void sprite_clear_all(void);

/* Pointer to internal sprite data (for direct mutation) */
Sprite* sprite_get(SpriteHandle h);

#endif /* ENGINE_SPRITE_H */
