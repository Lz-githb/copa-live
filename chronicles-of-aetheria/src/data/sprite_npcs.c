/*
 * sprite_npcs.c
 * Chronicles of Aetheria - GBA RPG
 *
 * NPC sprite tile data and palettes.
 * 8 NPC types × 1 frame × 4 tiles = 32 tiles (OBJ tiles 128–159).
 *
 * NPC types (all face DOWN / south; use HFLIP attribute for other dirs):
 *   NPC 0 (tile 128, +0):  Villager male   — blue shirt, brown pants, black hair
 *   NPC 1 (tile 132, +4):  Villager female — red dress, brown hair
 *   NPC 2 (tile 136, +8):  Merchant        — green coat, cap, white hair
 *   NPC 3 (tile 140, +12): Guard           — armor, helmet, red plume
 *   NPC 4 (tile 144, +16): Elder           — white robe, long white beard, staff
 *   NPC 5 (tile 148, +20): Child           — simple clothes, small stature
 *   NPC 6 (tile 152, +24): Innkeeper       — apron, dark hair, friendly face
 *   NPC 7 (tile 156, +28): Mage NPC        — blue robe, pointed hat, staff
 *
 * Palettes:
 *   Slot 4 — warm civilian palette (NPC 0,1,2,5,6)
 *   Slot 5 — cool guard/mage palette (NPC 3,4,7)
 *
 * Tile frame layout (identical to player characters):
 *   tile[npc*4+0]  tile[npc*4+1]   <- top half  (head + hair)
 *   tile[npc*4+2]  tile[npc*4+3]   <- bottom half (body + legs)
 *
 * Row encoding helper:
 *   ROW(p0,p1,p2,p3,p4,p5,p6,p7)
 *   p0 = leftmost pixel, value = 4-bit palette index
 */

#include "../../include/gba/gba_types.h"
#include "../../include/engine/sprite.h"

/* Forward declarations */
void sprite_load_tiles(u16 start_tile, const u32 *data, u32 count);
void sprite_load_palette(u8 slot, const COLOR *pal);

#define ROW(p0,p1,p2,p3,p4,p5,p6,p7) \
    ((u32)((p0) | ((p1)<<4) | ((p2)<<8) | ((p3)<<12) | \
           ((p4)<<16) | ((p5)<<20) | ((p6)<<24) | ((p7)<<28)))

/* =========================================================
 * NPC PALETTES
 *
 * Palette 4: warm civilian tones
 *   0  transparent
 *   1  skin
 *   2  black/dark hair
 *   3  blue shirt
 *   4  brown pants
 *   5  brown boots
 *   6  red dress/accent
 *   7  dark outline
 *   8  white/highlight
 *   9  dark eyes
 *  10  green coat
 *  11  apron white
 *  12  tan/wheat
 *  13  gold
 *  14  mid brown
 *  15  light skin shadow
 *
 * Palette 5: cool guard/mage tones
 *   0  transparent
 *   1  skin
 *   2  steel grey helmet
 *   3  royal blue
 *   4  dark iron/chainmail
 *   5  boots dark
 *   6  red plume
 *   7  black outline
 *   8  white/silver highlight
 *   9  dark eyes
 *  10  gold accent
 *  11  purple robe
 *  12  mage hat dark blue
 *  13  light grey
 *  14  sky blue
 *  15  deep red
 * ========================================================= */

const COLOR g_npc_palettes[2][16] = {
    /* --- Palette 4: warm civilians --- */
    {
        RGB15( 0, 0, 0),   /*  0 transparent */
        RGB15(28,20,14),   /*  1 skin */
        RGB15( 4, 2, 2),   /*  2 dark hair */
        RGB15( 4,10,26),   /*  3 blue shirt */
        RGB15(14, 8, 2),   /*  4 brown pants */
        RGB15( 8, 5, 2),   /*  5 brown boots */
        RGB15(26, 4, 4),   /*  6 red dress */
        RGB15( 2, 2, 2),   /*  7 dark outline */
        RGB15(31,31,31),   /*  8 white */
        RGB15( 4, 4,10),   /*  9 dark eyes */
        RGB15( 4,18, 4),   /* 10 green coat */
        RGB15(30,30,28),   /* 11 apron */
        RGB15(24,18, 8),   /* 12 tan */
        RGB15(26,22, 2),   /* 13 gold */
        RGB15(18,12, 6),   /* 14 mid brown */
        RGB15(30,24,20),   /* 15 light skin shadow */
    },
    /* --- Palette 5: cool guard/mage --- */
    {
        RGB15( 0, 0, 0),   /*  0 transparent */
        RGB15(28,20,14),   /*  1 skin */
        RGB15(18,18,20),   /*  2 steel grey */
        RGB15( 4,10,26),   /*  3 royal blue */
        RGB15( 8, 8,10),   /*  4 dark iron */
        RGB15( 4, 4, 6),   /*  5 dark boots */
        RGB15(26, 4, 4),   /*  6 red plume */
        RGB15( 2, 2, 2),   /*  7 black outline */
        RGB15(31,31,31),   /*  8 silver/white */
        RGB15( 4, 4,10),   /*  9 dark eyes */
        RGB15(26,22, 2),   /* 10 gold accent */
        RGB15(18, 4,26),   /* 11 purple robe */
        RGB15( 2, 4,20),   /* 12 dark blue hat */
        RGB15(22,22,24),   /* 13 light grey */
        RGB15(10,18,28),   /* 14 sky blue */
        RGB15(20, 2, 2),   /* 15 deep red */
    },
};

/* =========================================================
 * NPC TILE DATA
 * 32 tiles total, OBJ tile indices 128-159
 * (loaded via sprite_load_tiles(128, ..., 32))
 * ========================================================= */

const u32 g_npc_tiles[32][8] = {

/* ===================================================
 * NPC 0: Villager Male
 * Blue shirt, brown pants, black hair, palette 4
 * =================================================== */

/* tile 0 (+128): top-left (head left) */
{ ROW(0,0,7,7,7,7,0,0),
  ROW(0,7,2,2,2,2,7,0),
  ROW(7,2,2,2,2,2,2,7),
  ROW(7,1,1,9,1,1,1,7),
  ROW(7,1,1,1,1,1,1,7),
  ROW(7,1,7,1,1,7,1,7),
  ROW(0,7,1,1,1,1,7,0),
  ROW(0,0,7,7,7,7,0,0) },
/* tile 1 (+129): top-right (head right) */
{ ROW(0,0,7,7,7,7,0,0),
  ROW(0,7,2,2,2,2,7,0),
  ROW(7,2,2,2,2,2,2,7),
  ROW(7,1,1,1,9,1,1,7),
  ROW(7,1,1,1,1,1,1,7),
  ROW(7,1,7,1,1,7,1,7),
  ROW(0,7,1,1,1,1,7,0),
  ROW(0,0,7,7,7,7,0,0) },
/* tile 2 (+130): bottom-left (body) */
{ ROW(0,0,7,3,3,3,7,0),
  ROW(0,7,3,3,3,3,3,7),
  ROW(7,3,3,3,3,3,3,7),
  ROW(7,3,3,3,3,3,3,7),
  ROW(7,4,4,4,4,4,4,7),
  ROW(7,4,4,4,4,4,4,7),
  ROW(0,7,5,5,0,0,7,0),
  ROW(0,0,5,5,0,0,0,0) },
/* tile 3 (+131): bottom-right */
{ ROW(0,7,3,3,3,7,0,0),
  ROW(7,3,3,3,3,3,7,0),
  ROW(7,3,3,3,3,3,3,7),
  ROW(7,3,3,3,3,3,3,7),
  ROW(7,4,4,4,4,4,4,7),
  ROW(7,4,4,4,4,4,4,7),
  ROW(0,7,0,0,5,5,7,0),
  ROW(0,0,0,0,5,5,0,0) },

/* ===================================================
 * NPC 1: Villager Female
 * Red dress, brown hair, palette 4
 * =================================================== */

/* tile 4 (+132): top-left */
{ ROW(0,0,7,7,7,7,0,0),
  ROW(0,7,14,14,14,14,7,0),
  ROW(7,14,14,14,14,14,14,7),
  ROW(7,1,1,9,1,1,1,7),
  ROW(7,1,1,1,1,1,1,7),
  ROW(7,1,7,1,1,7,1,7),
  ROW(0,7,1,1,1,1,7,0),
  ROW(0,0,7,7,7,7,0,0) },
/* tile 5 (+133): top-right */
{ ROW(0,0,7,7,7,7,0,0),
  ROW(0,7,14,14,14,14,7,0),
  ROW(7,14,14,14,14,14,14,7),
  ROW(7,1,1,1,9,1,1,7),
  ROW(7,1,1,1,1,1,1,7),
  ROW(7,1,7,1,1,7,1,7),
  ROW(0,7,1,1,1,1,7,0),
  ROW(0,0,7,7,7,7,0,0) },
/* tile 6 (+134): bottom-left (red dress, wider) */
{ ROW(0,0,7,6,6,6,7,0),
  ROW(0,7,6,6,6,6,6,7),
  ROW(7,6,6,6,6,6,6,7),
  ROW(7,6,6,6,6,6,6,7),
  ROW(7,6,6,6,6,6,6,7),
  ROW(7,6,6,6,6,6,6,7),
  ROW(0,7,5,5,5,5,7,0),
  ROW(0,0,5,5,5,5,0,0) },
/* tile 7 (+135): bottom-right */
{ ROW(0,7,6,6,6,7,0,0),
  ROW(7,6,6,6,6,6,7,0),
  ROW(7,6,6,6,6,6,6,7),
  ROW(7,6,6,6,6,6,6,7),
  ROW(7,6,6,6,6,6,6,7),
  ROW(7,6,6,6,6,6,6,7),
  ROW(0,7,5,5,5,5,7,0),
  ROW(0,0,5,5,5,5,0,0) },

/* ===================================================
 * NPC 2: Merchant
 * Green coat, cap, white hair, palette 4
 * =================================================== */

/* tile 8 (+136): top-left */
{ ROW(0,7,10,10,10,10,7,0),
  ROW(7,10,10,10,10,10,10,7),
  ROW(0,7,8,8,8,8,7,0),
  ROW(0,7,1,9,1,1,7,0),
  ROW(0,7,1,1,1,1,7,0),
  ROW(0,7,1,7,1,7,0,0),
  ROW(0,0,7,1,1,7,0,0),
  ROW(0,0,0,7,7,0,0,0) },
/* tile 9 (+137): top-right */
{ ROW(0,7,10,10,10,10,7,0),
  ROW(7,10,10,10,10,10,10,7),
  ROW(0,7,8,8,8,8,7,0),
  ROW(0,7,1,1,9,1,7,0),
  ROW(0,7,1,1,1,1,7,0),
  ROW(0,0,7,1,7,1,7,0),
  ROW(0,0,0,7,1,1,7,0),
  ROW(0,0,0,0,7,7,0,0) },
/* tile 10 (+138): bottom-left */
{ ROW(0,0,7,10,10,7,0,0),
  ROW(0,7,10,10,10,10,7,0),
  ROW(7,10,10,13,13,10,10,7),
  ROW(7,10,10,10,10,10,10,7),
  ROW(7,4,4,4,4,4,4,7),
  ROW(7,4,4,4,4,4,4,7),
  ROW(0,7,5,5,0,0,7,0),
  ROW(0,0,5,5,0,0,0,0) },
/* tile 11 (+139): bottom-right */
{ ROW(0,0,7,10,10,7,0,0),
  ROW(0,7,10,10,10,10,7,0),
  ROW(7,10,10,13,13,10,10,7),
  ROW(7,10,10,10,10,10,10,7),
  ROW(7,4,4,4,4,4,4,7),
  ROW(7,4,4,4,4,4,4,7),
  ROW(0,7,0,0,5,5,7,0),
  ROW(0,0,0,0,5,5,0,0) },

/* ===================================================
 * NPC 3: Guard
 * Steel armor, helmet, red plume, palette 5
 * =================================================== */

/* tile 12 (+140): top-left */
{ ROW(0,0,7,6,6,7,0,0),
  ROW(0,7,6,6,6,6,7,0),
  ROW(7,2,2,2,2,2,2,7),
  ROW(7,2,1,9,1,1,2,7),
  ROW(7,2,1,1,1,1,2,7),
  ROW(7,2,2,2,2,2,2,7),
  ROW(0,7,2,2,2,2,7,0),
  ROW(0,0,7,7,7,7,0,0) },
/* tile 13 (+141): top-right */
{ ROW(0,0,7,6,6,7,0,0),
  ROW(0,7,6,6,6,6,7,0),
  ROW(7,2,2,2,2,2,2,7),
  ROW(7,2,1,1,9,1,2,7),
  ROW(7,2,1,1,1,1,2,7),
  ROW(7,2,2,2,2,2,2,7),
  ROW(0,7,2,2,2,2,7,0),
  ROW(0,0,7,7,7,7,0,0) },
/* tile 14 (+142): bottom-left (armor body) */
{ ROW(0,0,7,4,4,4,7,0),
  ROW(0,7,4,4,4,4,4,7),
  ROW(7,4,4,10,10,4,4,7),
  ROW(7,4,4,4,4,4,4,7),
  ROW(7,4,4,4,4,4,4,7),
  ROW(7,4,4,4,4,4,4,7),
  ROW(0,7,5,5,0,0,7,0),
  ROW(0,0,5,5,0,0,0,0) },
/* tile 15 (+143): bottom-right */
{ ROW(0,7,4,4,4,7,0,0),
  ROW(7,4,4,4,4,4,7,0),
  ROW(7,4,4,10,10,4,4,7),
  ROW(7,4,4,4,4,4,4,7),
  ROW(7,4,4,4,4,4,4,7),
  ROW(7,4,4,4,4,4,4,7),
  ROW(0,7,0,0,5,5,7,0),
  ROW(0,0,0,0,5,5,0,0) },

/* ===================================================
 * NPC 4: Elder
 * White robe, long white beard, staff, palette 5
 * =================================================== */

/* tile 16 (+144): top-left */
{ ROW(0,0,7,7,7,7,0,0),
  ROW(0,7,8,8,8,8,7,0),
  ROW(7,8,8,8,8,8,8,7),
  ROW(7,1,1,9,1,1,8,7),
  ROW(7,8,1,1,1,8,8,7),
  ROW(7,8,8,1,8,8,8,7),
  ROW(0,7,8,8,8,8,7,0),
  ROW(0,0,7,7,7,7,0,0) },
/* tile 17 (+145): top-right */
{ ROW(0,0,7,7,7,7,0,0),
  ROW(0,7,8,8,8,8,7,0),
  ROW(7,8,8,8,8,8,8,7),
  ROW(7,1,1,1,9,1,8,7),
  ROW(7,8,8,1,1,8,8,7),
  ROW(7,8,8,8,1,8,8,7),
  ROW(0,7,8,8,8,8,7,0),
  ROW(0,0,7,7,7,7,0,0) },
/* tile 18 (+146): bottom-left (white robe, staff) */
{ ROW(6,7,8,8,8,8,7,0),
  ROW(6,8,8,8,8,8,8,7),
  ROW(6,8,8,8,8,8,8,7),
  ROW(6,8,8,8,8,8,8,7),
  ROW(6,8,8,8,8,8,8,7),
  ROW(6,8,8,8,8,8,8,7),
  ROW(6,7,5,5,5,5,7,0),
  ROW(6,0,5,5,5,5,0,0) },
/* tile 19 (+147): bottom-right */
{ ROW(0,7,8,8,8,7,0,0),
  ROW(7,8,8,8,8,8,7,0),
  ROW(7,8,8,8,8,8,8,7),
  ROW(7,8,8,8,8,8,8,7),
  ROW(7,8,8,8,8,8,8,7),
  ROW(7,8,8,8,8,8,8,7),
  ROW(0,7,5,5,5,5,7,0),
  ROW(0,0,5,5,5,5,0,0) },

/* ===================================================
 * NPC 5: Child
 * Simple clothes, small stature (figure in lower 8x12 of 16x16)
 * Uses palette 4 (warm)
 * =================================================== */

/* tile 20 (+148): top-left (mostly empty, top of small head) */
{ ROW(0,0,0,0,0,0,0,0),
  ROW(0,0,0,0,0,0,0,0),
  ROW(0,0,7,7,7,0,0,0),
  ROW(0,7,2,2,2,7,0,0),
  ROW(7,2,2,2,2,2,7,0),
  ROW(7,1,9,1,1,1,7,0),
  ROW(0,7,1,1,1,7,0,0),
  ROW(0,0,7,7,7,0,0,0) },
/* tile 21 (+149): top-right */
{ ROW(0,0,0,0,0,0,0,0),
  ROW(0,0,0,0,0,0,0,0),
  ROW(0,0,0,7,7,7,0,0),
  ROW(0,0,7,2,2,2,7,0),
  ROW(0,7,2,2,2,2,2,7),
  ROW(0,7,1,1,1,9,1,7),
  ROW(0,0,7,1,1,1,7,0),
  ROW(0,0,0,7,7,7,0,0) },
/* tile 22 (+150): bottom-left (body, short legs) */
{ ROW(0,0,7,3,3,7,0,0),
  ROW(0,7,3,3,3,3,7,0),
  ROW(7,3,3,3,3,3,3,7),
  ROW(7,3,3,3,3,3,3,7),
  ROW(0,7,4,4,4,7,0,0),
  ROW(0,7,4,4,4,7,0,0),
  ROW(0,7,5,5,0,7,0,0),
  ROW(0,0,5,5,0,0,0,0) },
/* tile 23 (+151): bottom-right */
{ ROW(0,0,7,3,3,7,0,0),
  ROW(0,7,3,3,3,3,7,0),
  ROW(7,3,3,3,3,3,3,7),
  ROW(7,3,3,3,3,3,3,7),
  ROW(0,7,4,4,4,7,0,0),
  ROW(0,7,4,4,4,7,0,0),
  ROW(0,7,0,5,5,7,0,0),
  ROW(0,0,0,5,5,0,0,0) },

/* ===================================================
 * NPC 6: Innkeeper
 * Apron, dark hair, friendly face, palette 4
 * =================================================== */

/* tile 24 (+152): top-left */
{ ROW(0,0,7,7,7,7,0,0),
  ROW(0,7,2,2,2,2,7,0),
  ROW(7,2,2,2,2,2,2,7),
  ROW(7,1,1,9,1,1,1,7),
  ROW(7,1,1,1,1,1,1,7),
  ROW(7,1,7,1,1,7,1,7),
  ROW(0,7,1,15,1,1,7,0),
  ROW(0,0,7,7,7,7,0,0) },
/* tile 25 (+153): top-right */
{ ROW(0,0,7,7,7,7,0,0),
  ROW(0,7,2,2,2,2,7,0),
  ROW(7,2,2,2,2,2,2,7),
  ROW(7,1,1,1,9,1,1,7),
  ROW(7,1,1,1,1,1,1,7),
  ROW(7,1,7,1,1,7,1,7),
  ROW(0,7,1,1,15,1,7,0),
  ROW(0,0,7,7,7,7,0,0) },
/* tile 26 (+154): bottom-left (body with apron) */
{ ROW(0,0,7,6,6,6,7,0),
  ROW(0,7,6,11,11,6,6,7),
  ROW(7,6,6,11,11,6,6,7),
  ROW(7,6,6,11,11,6,6,7),
  ROW(7,6,6,11,11,6,6,7),
  ROW(7,6,6,6,6,6,6,7),
  ROW(0,7,5,5,0,0,7,0),
  ROW(0,0,5,5,0,0,0,0) },
/* tile 27 (+155): bottom-right */
{ ROW(0,7,6,6,6,7,0,0),
  ROW(7,6,6,11,11,6,7,0),
  ROW(7,6,6,11,11,6,6,7),
  ROW(7,6,6,11,11,6,6,7),
  ROW(7,6,6,11,11,6,6,7),
  ROW(7,6,6,6,6,6,6,7),
  ROW(0,7,0,0,5,5,7,0),
  ROW(0,0,0,0,5,5,0,0) },

/* ===================================================
 * NPC 7: Mage NPC
 * Blue robe, pointed hat, staff, palette 5
 * =================================================== */

/* tile 28 (+156): top-left (pointed hat) */
{ ROW(0,0,0,12,0,0,0,0),
  ROW(0,0,7,12,12,7,0,0),
  ROW(0,7,12,12,12,12,7,0),
  ROW(7,12,12,12,12,12,12,7),
  ROW(0,7,1,9,1,1,7,0),
  ROW(0,7,1,1,1,1,7,0),
  ROW(0,7,1,7,1,7,0,0),
  ROW(0,0,7,1,1,7,0,0) },
/* tile 29 (+157): top-right */
{ ROW(0,0,0,0,12,0,0,0),
  ROW(0,0,7,12,12,7,0,0),
  ROW(0,7,12,12,12,12,7,0),
  ROW(7,12,12,12,12,12,12,7),
  ROW(0,7,1,1,9,1,7,0),
  ROW(0,7,1,1,1,1,7,0),
  ROW(0,0,7,1,7,1,7,0),
  ROW(0,0,0,7,1,1,7,0) },
/* tile 30 (+158): bottom-left (blue robe, staff) */
{ ROW(6,7,3,3,3,3,7,0),
  ROW(6,3,3,3,3,3,3,7),
  ROW(6,3,3,10,10,3,3,7),
  ROW(6,3,3,3,3,3,3,7),
  ROW(6,3,3,3,3,3,3,7),
  ROW(6,3,3,3,3,3,3,7),
  ROW(6,7,5,5,5,5,7,0),
  ROW(6,0,5,5,5,5,0,0) },
/* tile 31 (+159): bottom-right */
{ ROW(0,7,3,3,3,7,0,0),
  ROW(7,3,3,3,3,3,7,0),
  ROW(7,3,3,10,10,3,3,7),
  ROW(7,3,3,3,3,3,3,7),
  ROW(7,3,3,3,3,3,3,7),
  ROW(7,3,3,3,3,3,3,7),
  ROW(0,7,5,5,5,5,7,0),
  ROW(0,0,5,5,5,5,0,0) },

}; /* end g_npc_tiles */

/* =========================================================
 * PUBLIC API
 * ========================================================= */

/* Load all NPC gfx into OBJ VRAM (tiles 128-159, palette slots 4-5). */
void gfx_npcs_load(void) {
    sprite_load_palette(4, g_npc_palettes[0]);
    sprite_load_palette(5, g_npc_palettes[1]);
    sprite_load_tiles(128, (const u32 *)g_npc_tiles, 32);
}
