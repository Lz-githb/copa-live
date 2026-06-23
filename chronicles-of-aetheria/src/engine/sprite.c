#include "../../include/engine/sprite.h"
#include "../../include/gba/gba_bios.h"
#include "../../include/gba/gba_dma.h"

/* =========================================================
 * Sprite System Implementation
 *
 * OAM is double-buffered:
 *   s_oam_shadow[]  — written by game logic each frame
 *   MEM_OAM         — pushed from shadow during VBlank
 * ========================================================= */

/* Shadow OAM in IWRAM for fast access during VBlank copy */
static OBJ_ATTR IWRAM_DATA s_oam_shadow[SPRITE_COUNT];

/* Sprite metadata pool */
static Sprite   s_sprites[SPRITE_COUNT];
static BOOL     s_used[SPRITE_COUNT];

/* ---- Helper: encode OBJ_ATTR from Sprite ---------------- */

/* Map SpriteSize enum to (shape, size) pairs for attr0/attr1 */
static const u8 s_shape_lut[12] = {
    /* SQUARE */  0, 0, 0, 0,
    /* WIDE   */  1, 1, 1, 1,
    /* TALL   */  2, 2, 2, 2,
};
static const u8 s_size_lut[12] = {
    0, 1, 2, 3,  /* square 8/16/32/64 */
    0, 1, 2, 3,  /* wide */
    0, 1, 2, 3,  /* tall */
};

static void _build_oam(u32 idx) {
    const Sprite* sp = &s_sprites[idx];
    OBJ_ATTR* oa = &s_oam_shadow[idx];

    if (!s_used[idx] || sp->hidden) {
        /* Hide: set attr0 bit 9 (OBJ_HIDE) with ROT_SCALE=0 */
        oa->attr0 = OBJ_HIDE;
        oa->attr1 = 0;
        oa->attr2 = 0;
        return;
    }

    u8 shape = s_shape_lut[sp->size];
    u8 sz    = s_size_lut[sp->size];

    oa->attr0 = OBJ_Y(sp->y) |
                (shape << 14) |
                (sp->hidden ? OBJ_HIDE : 0);

    oa->attr1 = OBJ_X(sp->x) |
                (sz << 14) |
                (sp->hflip ? OBJ_HFLIP : 0) |
                (sp->vflip ? OBJ_VFLIP : 0);

    oa->attr2 = OBJ_TID(sp->tile) |
                OBJ_PRIO(sp->priority) |
                OBJ_PAL(sp->palette);
}

/* ---- Public API ----------------------------------------- */

void sprite_init(void) {
    u32 i;
    for (i = 0; i < SPRITE_COUNT; i++) {
        s_used[i]  = FALSE;
        s_sprites[i].hidden = TRUE;
    }
    sprite_clear_all();
}

SpriteHandle sprite_alloc(void) {
    u32 i;
    for (i = 0; i < SPRITE_COUNT; i++) {
        if (!s_used[i]) {
            s_used[i] = TRUE;
            s_sprites[i].hidden   = FALSE;
            s_sprites[i].x        = 0;
            s_sprites[i].y        = 0;
            s_sprites[i].tile     = 0;
            s_sprites[i].palette  = 0;
            s_sprites[i].priority = 0;
            s_sprites[i].size     = SPR_SIZE_8x8;
            s_sprites[i].hflip    = FALSE;
            s_sprites[i].vflip    = FALSE;
            s_sprites[i].anim_frame = 0;
            s_sprites[i].anim_timer = 0;
            return (SpriteHandle)i;
        }
    }
    return INVALID_SPRITE;
}

void sprite_free(SpriteHandle h) {
    if (h >= SPRITE_COUNT) return;
    s_used[h] = FALSE;
    s_sprites[h].hidden = TRUE;
    _build_oam(h);
}

void sprite_set_pos(SpriteHandle h, s16 x, s16 y) {
    if (h >= SPRITE_COUNT || !s_used[h]) return;
    s_sprites[h].x = x;
    s_sprites[h].y = y;
    _build_oam(h);
}

void sprite_set_gfx(SpriteHandle h, u16 tile, u8 palette, SpriteSize size) {
    if (h >= SPRITE_COUNT || !s_used[h]) return;
    s_sprites[h].tile    = tile;
    s_sprites[h].palette = palette;
    s_sprites[h].size    = size;
    _build_oam(h);
}

void sprite_set_flip(SpriteHandle h, BOOL hflip, BOOL vflip) {
    if (h >= SPRITE_COUNT || !s_used[h]) return;
    s_sprites[h].hflip = hflip;
    s_sprites[h].vflip = vflip;
    _build_oam(h);
}

void sprite_set_hidden(SpriteHandle h, BOOL hidden) {
    if (h >= SPRITE_COUNT || !s_used[h]) return;
    s_sprites[h].hidden = hidden;
    _build_oam(h);
}

void sprite_set_priority(SpriteHandle h, u8 prio) {
    if (h >= SPRITE_COUNT || !s_used[h]) return;
    s_sprites[h].priority = prio & 3;
    _build_oam(h);
}

void sprite_load_tiles(u16 tile_idx, const u32* data, u32 tile_count) {
    /* OBJ VRAM starts at 0x06010000; each tile = 32 bytes (4bpp 8x8) */
    u32* dst = (u32*)(MEM_VRAM + 0x10000 + (u32)tile_idx * 32);
    dma3_copy32(dst, data, tile_count * 8);   /* 8 u32 per tile */
}

void sprite_load_palette(u8 slot, const COLOR* pal) {
    /* OBJ palette: 0x05000200, 16 colors per slot */
    COLOR* dst = PAL_OBJ_MEM + (slot * 16);
    dma3_copy16(dst, pal, 16);
}

void sprite_flush(void) {
    /* DMA shadow OAM to hardware OAM during VBlank */
    dma3_copy32(OAM_MEM, s_oam_shadow,
                SPRITE_COUNT * sizeof(OBJ_ATTR) / 4);
}

void sprite_clear_all(void) {
    u32 i;
    for (i = 0; i < SPRITE_COUNT; i++) {
        s_oam_shadow[i].attr0 = OBJ_HIDE;
        s_oam_shadow[i].attr1 = 0;
        s_oam_shadow[i].attr2 = 0;
        s_oam_shadow[i].fill  = 0;
    }
    sprite_flush();
}

Sprite* sprite_get(SpriteHandle h) {
    if (h >= SPRITE_COUNT) return NULL;
    return &s_sprites[h];
}
