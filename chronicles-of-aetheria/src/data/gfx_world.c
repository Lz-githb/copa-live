#include "../../include/data/gfx_world.h"
#include "../../include/gba/gba_types.h"
#include "../../include/engine/tile.h"

/* External tileset data (from tileset_overworld.c / tileset_dungeon.c) */
extern const u32 g_tileset_overworld[32][8];
extern const u32 g_tileset_dungeon[24][8];
u16 tileset_overworld_count(void);
u16 tileset_dungeon_count(void);

/* External sprite loader declarations (defined in their respective .c files) */
extern void gfx_chars_load(void);
extern void gfx_npcs_load(void);
extern void gfx_monsters_load(void);

/* =========================================================
 * BG Palette slot 9 — Overworld
 *
 * 15-bit BGR555:  0bBBBBBGGGGGRRRRR
 *
 * Index  Meaning
 *  0     transparent
 *  1     dark forest green   (#1A5C1A -> R=8,G=22,B=8  -> 0x2C88)
 *  2     grass green         (#3CA83C -> R=15,G=21,B=15 -> 0x3DEF)  -- approx
 *  3     light path tan      (#C8A878 -> R=25,G=21,B=12 -> 0x3339)
 *  4     water blue          (#3878C8 -> R=7,G=15,B=25  -> 0x79E7... approx)
 *  5     bark brown          (#7C4C28 -> R=15,G=9,B=5   -> 0x14CF)
 *  6     stone grey          (#909090 -> R=18,G=18,B=18 -> 0x4A52)
 *  7     yellow flower       (#F8E820 -> R=31,G=29,B=4  -> 0x13BF)
 *  8     fence grey          (#B0B0B0 -> R=22,G=22,B=22 -> 0x5AD6)
 *  9     white               (#F8F8F8 -> R=31,G=31,B=31 -> 0x7FFF)
 * 10     deep blue water     (#1840A0 -> R=3,G=8,B=20   -> 0x5043)
 * 11     sand                (#E8D080 -> R=29,G=26,B=8  -> 0x21BD)
 * 12     snow                (#E8F0F8 -> R=29,G=30,B=31 -> 0x7F7D)
 * 13     red                 (#C82020 -> R=25,G=4,B=4   -> 0x0899)
 * 14     orange              (#E87820 -> R=29,G=15,B=4  -> 0x21FD... approx)
 * 15     bright green        (#40F840 -> R=8,G=31,B=8   -> 0x27E8)
 * ========================================================= */
static const COLOR overworld_pal[16] = {
    0x0000, /* 0  transparent          */
    0x2C88, /* 1  dark forest green    */
    0x3DEF, /* 2  grass green          */
    0x3339, /* 3  light path tan       */
    0x79E7, /* 4  water blue           */
    0x14CF, /* 5  bark brown           */
    0x4A52, /* 6  stone grey           */
    0x13BF, /* 7  yellow flower        */
    0x5AD6, /* 8  fence grey           */
    0x7FFF, /* 9  white                */
    0x5043, /* 10 deep blue water      */
    0x21BD, /* 11 sand                 */
    0x7F7D, /* 12 snow                 */
    0x0899, /* 13 red                  */
    0x21FD, /* 14 orange               */
    0x27E8  /* 15 bright green         */
};

/* =========================================================
 * BG Palette slot 10 — Dungeon
 *
 * Index  Meaning
 *  0     transparent
 *  1     black               -> 0x0000
 *  2     dark stone          (#303030 -> R=6,G=6,B=6   -> 0x0C63)
 *  3     stone grey          (#606060 -> R=12,G=12,B=12 -> 0x318C)
 *  4     light stone         (#909090 -> R=18,G=18,B=18 -> 0x4A52... same as overworld 6)
 *  5     dirt brown          (#7C5030 -> R=15,G=10,B=6  -> 0x194F)
 *  6     moss green          (#408030 -> R=8,G=16,B=6   -> 0x1808... approx)
 *  7     torch orange        (#E87820 -> R=29,G=15,B=4  -> 0x21FD)
 *  8     torch yellow        (#F8E820 -> R=31,G=29,B=4  -> 0x13BF) -- same as overworld 7
 *  9     magic blue          (#2040E8 -> R=4,G=8,B=29   -> 0x7484)
 * 10     lava red            (#E84020 -> R=29,G=8,B=4   -> 0x211D)
 * 11     dark red            (#801010 -> R=16,G=2,B=2   -> 0x0450)
 * 12     purple              (#6040A0 -> R=12,G=8,B=20  -> 0x510C)
 * 13     bone white          (#E8E0C8 -> R=29,G=28,B=25 -> 0x659D)
 * 14     dark brown          (#402010 -> R=8,G=4,B=2    -> 0x0888) -- approx
 * 15     gold                (#F8C820 -> R=31,G=25,B=4  -> 0x133F)
 * ========================================================= */
static const COLOR dungeon_pal[16] = {
    0x0000, /* 0  transparent          */
    0x0000, /* 1  black                */
    0x0C63, /* 2  dark stone           */
    0x318C, /* 3  stone grey           */
    0x4A52, /* 4  light stone          */
    0x194F, /* 5  dirt brown           */
    0x1808, /* 6  moss green           */
    0x21FD, /* 7  torch orange         */
    0x13BF, /* 8  torch yellow         */
    0x7484, /* 9  magic blue           */
    0x211D, /* 10 lava red             */
    0x0450, /* 11 dark red             */
    0x510C, /* 12 purple               */
    0x659D, /* 13 bone white           */
    0x0888, /* 14 dark brown           */
    0x133F  /* 15 gold                 */
};

/* ---------------------------------------------------------
 * gfx_world_load_overworld
 * Upload overworld palette to BG slot 9 and tileset to
 * charblock 1 starting at tile 0. tile_count = 32.
 * --------------------------------------------------------- */
void gfx_world_load_overworld(void) {
    tile_load_palette(9, overworld_pal);
    tile_load_tileset(1, 0, (const u32*)g_tileset_overworld, 32);
}

/* ---------------------------------------------------------
 * gfx_world_load_dungeon
 * Upload dungeon palette to BG slot 10 and tileset to
 * charblock 1 starting at tile 0. tile_count = 24.
 * --------------------------------------------------------- */
void gfx_world_load_dungeon(void) {
    tile_load_palette(10, dungeon_pal);
    tile_load_tileset(1, 0, (const u32*)g_tileset_dungeon, 24);
}

/* ---------------------------------------------------------
 * gfx_world_load
 * Called once on game start; loads the overworld by default.
 * --------------------------------------------------------- */
void gfx_world_load(void) {
    gfx_world_load_overworld();
}

/* ---------------------------------------------------------
 * gfx_sprites_load_all
 * Called once at startup to upload all OBJ sprite sheets.
 * --------------------------------------------------------- */
void gfx_sprites_load_all(void) {
    gfx_chars_load();
    gfx_npcs_load();
    gfx_monsters_load();
}
