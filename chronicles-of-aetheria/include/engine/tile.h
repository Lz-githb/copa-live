#ifndef ENGINE_TILE_H
#define ENGINE_TILE_H

#include "../gba/gba_types.h"
#include "../gba/gba_reg.h"

/* =========================================================
 * Tile & Background System
 *
 * GBA has 4 BG layers, each backed by:
 *   - A charblock (tile graphics, 16KB each, 0-3)
 *   - A screenblock (tile map, 2KB each, 0-31)
 *
 * Layout for Chronicles:
 *   BG0 (priority 0) — HUD / UI overlay
 *   BG1 (priority 1) — foreground layer (trees, roofs)
 *   BG2 (priority 2) — main overworld/dungeon tiles
 *   BG3 (priority 3) — background / sky layer
 *
 * Charblock assignment:
 *   CB0 (0x06000000) — BG0+BG1 tiles (16KB)
 *   CB1 (0x06004000) — BG2+BG3 tiles (16KB)
 *   CB2 (0x06008000) — more BG tiles if needed
 *   CB3 (0x0600C000) — reserved / BG extended
 *   OBJ (0x06010000) — sprite tiles
 *
 * Map size: 32x32 tiles = 256x256 pixels per screenblock
 * ========================================================= */

#define BG_LAYER_COUNT      4
#define TILE_SIZE           8       /* pixels per tile */
#define TILE_BYTES_4BPP     32      /* bytes per 4bpp 8x8 tile */

/* Map geometry */
#define MAP_COLS            32
#define MAP_ROWS            32
#define MAP_TILE_COUNT      (MAP_COLS * MAP_ROWS)

/* BG layer indices */
#define BG_LAYER_UI         0
#define BG_LAYER_FG         1
#define BG_LAYER_WORLD      2
#define BG_LAYER_BG         3

typedef struct {
    u8  charblock;      /* which charblock for tiles */
    u8  screenblock;    /* which screenblock for map */
    u8  priority;
    u8  palette_mode;   /* 0=4bpp/16pal, 1=8bpp/1pal */
    u16 hofs;           /* horizontal scroll */
    u16 vofs;           /* vertical scroll */
    BOOL enabled;
} BgLayer;

typedef struct {
    u16 tile_id;   /* map tile index into tileset */
    u8  flags;     /* collision, event trigger, etc. */
    u8  meta;      /* extra data (light level, etc.) */
} MapCell;

/* Collision flags embedded in tile metadata */
#define TILE_FLAG_SOLID     0x01
#define TILE_FLAG_WATER     0x02
#define TILE_FLAG_EVENT     0x04
#define TILE_FLAG_WARP      0x08
#define TILE_FLAG_LADDER    0x10

/* ---- Tilemap (one room / map area) ----------------------- */
#define MAP_MAX_W    32   /* tiles */
#define MAP_MAX_H    32   /* tiles */

typedef struct {
    u16      width;
    u16      height;
    u16      tileset_id;   /* which tileset to use */
    MapCell  cells[MAP_MAX_H][MAP_MAX_W];
} Tilemap;

/* ---- Tile system API ------------------------------------- */

void tile_init(void);

/* Configure and enable a BG layer */
void tile_bg_setup(u8 layer, u8 charblock, u8 screenblock,
                   u8 priority, u8 palette_mode, u8 size);

/* Enable/disable a layer */
void tile_bg_enable(u8 layer, BOOL enable);

/* Set scroll offsets (in pixels) */
void tile_bg_scroll(u8 layer, u16 hofs, u16 vofs);

/* Load tile graphics into a charblock */
void tile_load_tileset(u8 charblock, u16 tile_idx,
                       const u32* gfx, u32 tile_count);

/* Load a 16-color palette into BG palette slot */
void tile_load_palette(u8 slot, const COLOR* pal);

/* Upload a Tilemap to a screenblock */
void tile_upload_map(u8 screenblock, const Tilemap* map);

/* Write a single map entry */
void tile_set_cell(u8 screenblock, u8 col, u8 row, u16 entry);

/* Read map cell flags from a Tilemap */
u8 tile_get_flags(const Tilemap* map, u8 col, u8 row);
BOOL tile_is_solid(const Tilemap* map, u8 col, u8 row);

/* Convert pixel coordinates to tile coordinates */
INLINE u8 pixel_to_tile(s16 px) { return (u8)(px >> 3); }

#endif /* ENGINE_TILE_H */
