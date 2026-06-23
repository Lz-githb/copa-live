#include "../../include/engine/tile.h"
#include "../../include/gba/gba_bios.h"
#include "../../include/gba/gba_dma.h"
#include "../../include/gba/gba_reg.h"

/* =========================================================
 * Tile & Background System Implementation
 * ========================================================= */

static BgLayer s_bg[BG_LAYER_COUNT];

/* BG control register array for indexed access */
static vu16* const BG_CNT_REGS[4] = {
    &REG_BG0CNT, &REG_BG1CNT, &REG_BG2CNT, &REG_BG3CNT
};

/* BG scroll register pairs (H then V, interleaved in IO) */
static vu16* const BG_HOFS_REGS[4] = {
    &REG_BG0HOFS, &REG_BG1HOFS, &REG_BG2HOFS, &REG_BG3HOFS
};
static vu16* const BG_VOFS_REGS[4] = {
    &REG_BG0VOFS, &REG_BG1VOFS, &REG_BG2VOFS, &REG_BG3VOFS
};

void tile_init(void) {
    u32 i;
    for (i = 0; i < BG_LAYER_COUNT; i++) {
        s_bg[i].hofs    = 0;
        s_bg[i].vofs    = 0;
        s_bg[i].enabled = FALSE;
    }
}

void tile_bg_setup(u8 layer, u8 charblock, u8 screenblock,
                   u8 priority, u8 palette_mode, u8 size) {
    if (layer >= BG_LAYER_COUNT) return;

    s_bg[layer].charblock    = charblock;
    s_bg[layer].screenblock  = screenblock;
    s_bg[layer].priority     = priority;
    s_bg[layer].palette_mode = palette_mode;

    *BG_CNT_REGS[layer] = BG_CNT(priority, charblock,
                                  0,             /* no mosaic */
                                  palette_mode,
                                  screenblock,
                                  0,             /* no wrap */
                                  size);
}

void tile_bg_enable(u8 layer, BOOL enable) {
    if (layer >= BG_LAYER_COUNT) return;
    s_bg[layer].enabled = enable;

    if (enable) {
        REG_DISPCNT |= (DCNT_BG0 << layer);
    } else {
        REG_DISPCNT &= ~(DCNT_BG0 << layer);
    }
}

void tile_bg_scroll(u8 layer, u16 hofs, u16 vofs) {
    if (layer >= BG_LAYER_COUNT) return;
    s_bg[layer].hofs = hofs;
    s_bg[layer].vofs = vofs;
    *BG_HOFS_REGS[layer] = hofs;
    *BG_VOFS_REGS[layer] = vofs;
}

void tile_load_tileset(u8 charblock, u16 tile_idx,
                       const u32* gfx, u32 tile_count) {
    /* Each charblock = 0x4000 bytes; each 4bpp tile = 32 bytes */
    u32* dst = (u32*)(MEM_VRAM + ((u32)charblock * 0x4000) +
                      (u32)tile_idx * TILE_BYTES_4BPP);
    dma3_copy32(dst, gfx, tile_count * 8);
}

void tile_load_palette(u8 slot, const COLOR* pal) {
    /* BG palette: 0x05000000, 16 colors per slot */
    COLOR* dst = PAL_BG_MEM + (slot * 16);
    dma3_copy16(dst, pal, 16);
}

void tile_upload_map(u8 screenblock, const Tilemap* map) {
    u16* dst = (u16*)(MEM_VRAM + ((u32)screenblock * 0x800));
    u32 r, c;
    for (r = 0; r < map->height && r < MAP_ROWS; r++) {
        for (c = 0; c < map->width && c < MAP_COLS; c++) {
            dst[r * MAP_COLS + c] = SE_TID(map->cells[r][c].tile_id);
        }
    }
}

void tile_set_cell(u8 screenblock, u8 col, u8 row, u16 entry) {
    u16* map = (u16*)(MEM_VRAM + ((u32)screenblock * 0x800));
    map[(u32)row * MAP_COLS + col] = entry;
}

u8 tile_get_flags(const Tilemap* map, u8 col, u8 row) {
    if (!map || col >= map->width || row >= map->height) return 0;
    return map->cells[row][col].flags;
}

BOOL tile_is_solid(const Tilemap* map, u8 col, u8 row) {
    return (tile_get_flags(map, col, row) & TILE_FLAG_SOLID) != 0;
}
