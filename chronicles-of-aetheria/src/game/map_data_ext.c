#include "../../include/game/world.h"
#include "../../include/game/interaction.h"
#include "../../include/gba/gba_types.h"

/* =========================================================
 * Map Data Extension — Maps 2-21
 *
 * Map ID registry:
 *   0  TOWN_START        (map_data.c)
 *   1  HOUSE_01          (map_data.c)
 *   2  ROUTE_01          outdoor route, tall grass
 *   3  DUNGEON_01        Forest Shrine
 *   4  TOWN_RIVERSIDE    town with river
 *   5  INN_RIVERSIDE     inn interior
 *   6  ROUTE_02          mountain pass
 *   7  DUNGEON_02        Cave of Trials
 *   8  CASTLE_AETHERIA   castle exterior
 *   9  CASTLE_THRONE     throne room
 *  10  TOWN_HARBOR       port town
 *  11  DUNGEON_03        Ancient Crypt
 *  12  ROUTE_03          forest path
 *  13  VILLAGE_FOREST    small forest village
 *  14  DUNGEON_04        Volcano Cavern
 *  15  TOWN_SNOWPEAK     snow town
 *  16  DUNGEON_05        Ice Cave
 *  17  SECRET_ISLAND_01  hidden island
 *  18  SHADOW_REALM      dark dimension
 *  19  DUNGEON_BOSS_LAIR final dungeon
 *  20  CASTLE_DARK       dark lord's castle
 *  21  HOUSE_SHOP        generic shop
 * ========================================================= */

/* Tile legend (shared across outdoor maps):
 *  0 = grass/floor (walkable)
 *  1 = tree/wall   (solid)
 *  2 = path/stone  (walkable)
 *  3 = water       (solid, TF_WATER)
 *  4 = house wall  (solid)
 *  5 = door/warp   (TF_WARP)
 *  6 = flower/deco (walkable)
 *  7 = fence       (solid)
 *  8 = sign/obj    (TF_EVENT)
 *  9 = tall grass  (TF_TALL_GRASS) */

/* Flag helpers */
#define F0  0                          /* walkable, nothing */
#define FS  TF_SOLID                   /* solid */
#define FW  TF_WATER                   /* water */
#define FP  TF_WARP                    /* warp tile */
#define FG  TF_TALL_GRASS              /* encounter zone */
#define FE  TF_EVENT                   /* interactive event */
#define FSW (TF_SOLID|TF_WATER)        /* solid water */

/* =========================================================
 * MAP 2 — ROUTE_01  (24 x 32)
 * ========================================================= */
static const u16 map2_tiles[24 * 32] = {
    /* Row 0  */ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    /* Row 1  */ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    /* Row 2  */ 1,0,0,9,9,9,0,0,0,9,9,9,9,0,0,0,9,9,9,0,0,0,0,1,
    /* Row 3  */ 1,0,9,9,9,9,9,0,0,9,9,9,9,0,0,0,9,9,9,0,0,0,0,1,
    /* Row 4  */ 1,0,9,9,0,0,0,0,0,0,0,0,0,0,0,0,0,9,9,0,0,0,0,1,
    /* Row 5  */ 1,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,1,
    /* Row 6  */ 1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,1,
    /* Row 7  */ 1,0,0,2,0,9,9,9,0,0,0,0,0,9,9,9,0,2,0,0,0,0,0,1,
    /* Row 8  */ 1,0,0,2,0,9,9,9,0,0,0,0,0,9,9,9,0,2,0,0,0,0,0,1,
    /* Row 9  */ 1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,1,
    /* Row 10 */ 1,0,0,2,0,0,0,0,0,0,8,0,0,0,0,0,0,2,0,0,0,0,0,1,
    /* Row 11 */ 1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,1,
    /* Row 12 */ 1,0,0,2,0,9,9,9,9,0,0,0,9,9,9,9,0,2,0,0,0,0,0,1,
    /* Row 13 */ 1,0,0,2,0,9,9,9,9,0,0,0,9,9,9,9,0,2,0,0,0,0,0,1,
    /* Row 14 */ 1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,1,
    /* Row 15 */ 1,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,1,
    /* Row 16 */ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    /* Row 17 */ 1,0,9,9,9,9,0,0,0,0,0,0,0,0,9,9,9,9,0,0,0,0,0,1,
    /* Row 18 */ 1,0,9,9,9,9,0,0,0,0,0,0,0,0,9,9,9,9,0,0,0,0,0,1,
    /* Row 19 */ 1,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,1,
    /* Row 20 */ 1,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,1,
    /* Row 21 */ 1,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,1,
    /* Row 22 */ 1,0,0,9,9,0,0,0,0,2,0,0,2,0,0,0,9,9,0,0,0,0,0,1,
    /* Row 23 */ 1,0,0,9,9,0,0,0,0,2,0,0,2,0,0,0,9,9,0,0,0,0,0,1,
    /* Row 24 */ 1,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,1,
    /* Row 25 */ 1,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,1,
    /* Row 26 */ 1,0,0,0,0,0,9,9,0,2,0,0,2,0,9,9,0,0,0,0,0,0,0,1,
    /* Row 27 */ 1,0,0,0,0,0,9,9,0,2,0,0,2,0,9,9,0,0,0,0,0,0,0,1,
    /* Row 28 */ 1,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,1,
    /* Row 29 */ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    /* Row 30 */ 1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,1,
    /* Row 31 */ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};
static const u8 map2_flags[24 * 32] = {
    FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,FG,FG,FG,F0,F0,F0,FG,FG,FG,FG,F0,F0,F0,FG,FG,FG,F0,F0,F0,F0,FS,
    FS,F0,FG,FG,FG,FG,FG,F0,F0,FG,FG,FG,FG,F0,F0,F0,FG,FG,FG,F0,F0,F0,F0,FS,
    FS,F0,FG,FG,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FG,FG,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,FG,FG,FG,F0,F0,F0,F0,F0,FG,FG,FG,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,FG,FG,FG,F0,F0,F0,F0,F0,FG,FG,FG,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,FE,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,FG,FG,FG,FG,F0,F0,F0,FG,FG,FG,FG,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,FG,FG,FG,FG,F0,F0,F0,FG,FG,FG,FG,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,FG,FG,FG,FG,F0,F0,F0,F0,F0,F0,F0,F0,FG,FG,FG,FG,F0,F0,F0,F0,F0,FS,
    FS,F0,FG,FG,FG,FG,F0,F0,F0,F0,F0,F0,F0,F0,FG,FG,FG,FG,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,FG,FG,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FG,FG,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,FG,FG,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FG,FG,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,FG,FG,F0,F0,F0,F0,F0,F0,FG,FG,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,FG,FG,F0,F0,F0,F0,F0,F0,FG,FG,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FS,
    FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,
};
static const WarpEntry map2_warps[] = {
    { 1,  30, 4,  0, WARP_TRANSITION }, /* south → TOWN_RIVERSIDE */
    { 9,  19, 3,  0, WARP_CAVE       }, /* dungeon entrance */
};
static const SpawnPoint map2_spawns[] = {
    { 11, 1, 2 }, /* from north (TOWN_START) */
    { 10, 29, 0 }, /* from south */
};
static const NpcTemplate map2_npcs[] = {
    { 2, 10, 10, 0, NPC_MOVE_WANDER, 3, {{0,0}}, 0, 1, 0, TRUE },
};

/* =========================================================
 * MAP 3 — DUNGEON_01 Forest Shrine (20x20)
 * ========================================================= */
static const u16 map3_tiles[20 * 20] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,1,
    1,2,0,0,0,0,0,0,2,1,1,2,0,0,0,0,0,0,2,1,
    1,2,0,0,0,0,0,0,2,1,1,2,0,0,0,0,0,0,2,1,
    1,2,0,0,1,1,0,0,2,2,2,2,0,0,1,1,0,0,2,1,
    1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1,
    1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1,
    1,2,2,2,0,0,2,2,2,2,2,2,2,2,0,0,2,2,2,1,
    1,1,1,2,0,0,2,1,1,1,1,1,1,2,0,0,2,1,1,1,
    1,1,1,2,0,0,2,1,0,0,0,0,1,2,0,0,2,1,1,1,
    1,1,1,2,2,2,2,1,0,8,0,0,1,2,2,2,2,1,1,1,
    1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,2,2,2,2,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,2,0,0,2,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,2,0,0,2,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,2,0,0,2,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,2,0,0,2,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,5,5,5,5,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};
static const u8 map3_flags[20 * 20] = {
    FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,FS,FS,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,FS,FS,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,FS,FS,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,FS,FS,F0,F0,F0,F0,F0,F0,F0,F0,FS,FS,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,FS,FS,F0,F0,F0,F0,FS,FS,FS,FS,FS,FS,F0,F0,F0,F0,FS,FS,FS,
    FS,FS,FS,F0,F0,F0,F0,FS,F0,F0,F0,F0,FS,F0,F0,F0,F0,FS,FS,FS,
    FS,FS,FS,F0,F0,F0,F0,FS,F0,FE,F0,F0,FS,F0,F0,F0,F0,FS,FS,FS,
    FS,FS,FS,FS,FS,FS,FS,FS,F0,F0,F0,F0,FS,FS,FS,FS,FS,FS,FS,FS,
    FS,FS,FS,FS,FS,FS,FS,FS,F0,F0,F0,F0,FS,FS,FS,FS,FS,FS,FS,FS,
    FS,FS,FS,FS,FS,FS,FS,FS,F0,F0,F0,F0,FS,FS,FS,FS,FS,FS,FS,FS,
    FS,FS,FS,FS,FS,FS,FS,FS,F0,F0,F0,F0,FS,FS,FS,FS,FS,FS,FS,FS,
    FS,FS,FS,FS,FS,FS,FS,FS,F0,F0,F0,F0,FS,FS,FS,FS,FS,FS,FS,FS,
    FS,FS,FS,FS,FS,FS,FS,FS,F0,F0,F0,F0,FS,FS,FS,FS,FS,FS,FS,FS,
    FS,FS,FS,FS,FS,FS,FS,FS,F0,F0,F0,F0,FS,FS,FS,FS,FS,FS,FS,FS,
    FS,FS,FS,FS,FS,FS,FS,FS,FP,FP,FP,FP,FS,FS,FS,FS,FS,FS,FS,FS,
    FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,
};
static const WarpEntry map3_warps[] = {
    { 8, 18, 2, 1, WARP_CAVE }, /* exit → ROUTE_01 */
};
static const SpawnPoint map3_spawns[] = {
    { 9, 17, 0 }, /* entering dungeon */
};
static const NpcTemplate map3_npcs[] = {
    { 5, 9, 10, 0, NPC_MOVE_NONE, 0, {{0,0}}, 0, 2, 0, TRUE }, /* guardian NPC */
};

/* =========================================================
 * MAP 4 — TOWN_RIVERSIDE (28x28)
 * ========================================================= */
static const u16 map4_tiles[28 * 28] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,4,4,4,0,0,0,4,4,4,0,0,0,3,3,0,0,0,4,4,4,0,0,0,0,0,1,
    1,0,4,4,4,0,0,0,4,4,4,0,0,0,3,3,0,0,0,4,4,4,0,0,0,0,0,1,
    1,0,4,5,4,0,0,0,4,5,4,0,0,0,2,2,0,0,0,4,5,4,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,1,
    1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,1,
    1,0,2,0,4,4,4,4,4,0,0,0,0,0,0,0,4,4,4,4,4,0,0,0,2,0,0,1,
    1,0,2,0,4,4,4,4,4,0,0,0,0,0,0,0,4,4,4,4,4,0,0,0,2,0,0,1,
    1,0,2,0,4,5,4,5,4,0,0,0,8,0,0,0,4,5,4,5,4,0,0,0,2,0,0,1,
    1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,1,
    1,0,2,0,0,0,0,0,0,0,0,3,3,3,3,0,0,0,0,0,0,0,0,0,2,0,0,1,
    1,0,2,0,0,0,0,0,0,0,0,3,3,3,3,0,0,0,0,0,0,0,0,0,2,0,0,1,
    1,0,2,0,0,0,0,0,0,0,2,2,2,2,2,2,0,0,0,0,0,0,0,0,2,0,0,1,
    1,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,0,0,0,0,1,
    1,0,0,6,6,0,0,0,4,4,4,4,4,4,4,4,4,4,4,0,0,6,6,0,0,0,0,1,
    1,0,0,0,0,0,0,0,4,4,4,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,4,5,4,4,5,4,4,5,4,4,4,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};
static const u8 map4_flags[28 * 28] = {
    FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FSW,FSW,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,FS,FS,FS,F0,F0,F0,FS,FS,FS,F0,F0,F0,FSW,FSW,F0,F0,F0,FS,FS,FS,F0,F0,F0,F0,F0,FS,
    FS,F0,FS,FS,FS,F0,F0,F0,FS,FS,FS,F0,F0,F0,FSW,FSW,F0,F0,F0,FS,FS,FS,F0,F0,F0,F0,F0,FS,
    FS,F0,FS,FP,FS,F0,F0,F0,FS,FP,FS,F0,F0,F0,F0,F0,F0,F0,F0,FS,FP,FS,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,FS,FS,FS,FS,FS,F0,F0,F0,F0,F0,F0,F0,FS,FS,FS,FS,FS,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,FS,FS,FS,FS,FS,F0,F0,F0,F0,F0,F0,F0,FS,FS,FS,FS,FS,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,FS,FP,FS,FP,FS,F0,F0,F0,FE,F0,F0,F0,FS,FP,FS,FP,FS,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FSW,FSW,FSW,FSW,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FSW,FSW,FSW,FSW,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,FS,FP,FS,FS,FP,FS,FS,FP,FS,FS,FS,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FS,
    FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,
};
static const WarpEntry map4_warps[] = {
    { 3,  4,  5, 0, WARP_DOOR       }, /* house 1 door → inn */
    { 9,  4,  5, 0, WARP_DOOR       }, /* house 2 door */
    { 20, 4,  5, 0, WARP_DOOR       }, /* house 3 door */
    { 1,  26, 6, 0, WARP_TRANSITION }, /* south → ROUTE_02 */
};
static const SpawnPoint map4_spawns[] = {
    { 13, 5,  0 }, /* entering from north */
    { 3,  5,  0 }, /* exit inn */
    { 13, 25, 2 }, /* from south */
};
static const NpcTemplate map4_npcs[] = {
    { 1, 5,  10, 0, NPC_MOVE_WANDER, 3, {{0,0}}, 0, 0, 0, TRUE }, /* villager */
    { 2, 18, 12, 2, NPC_MOVE_LOOK,   0, {{0,0}}, 0, 1, 0, TRUE }, /* merchant */
    { 3, 12, 10, 0, NPC_MOVE_NONE,   0, {{0,0}}, 0, 3, 0, TRUE }, /* innkeeper */
};

/* =========================================================
 * MAP 5 — INN_RIVERSIDE (18x12)
 * ========================================================= */
static const u16 map5_tiles[18 * 12] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
    1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1,
    1,2,0,4,4,4,0,0,0,0,0,0,4,4,4,0,2,1,
    1,2,0,4,0,4,0,0,0,0,0,0,4,0,4,0,2,1,
    1,2,0,4,0,4,0,0,0,0,0,0,4,0,4,0,2,1,
    1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1,
    1,2,0,0,0,0,0,0,8,0,0,0,0,0,0,0,2,1,
    1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1,
    1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
    1,1,1,1,1,1,1,1,5,5,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};
static const u8 map5_flags[18 * 12] = {
    FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,FS,FS,FS,F0,F0,F0,F0,F0,F0,FS,FS,FS,F0,F0,FS,
    FS,F0,F0,FS,F0,FS,F0,F0,F0,F0,F0,F0,FS,F0,FS,F0,F0,FS,
    FS,F0,F0,FS,F0,FS,F0,F0,F0,F0,F0,F0,FS,F0,FS,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,FE,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,FS,FS,FS,FS,FS,FS,FS,FP,FP,FS,FS,FS,FS,FS,FS,FS,FS,
    FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,
};
static const WarpEntry map5_warps[] = {
    { 8, 10, 4, 1, WARP_DOOR }, /* exit → TOWN_RIVERSIDE */
};
static const SpawnPoint map5_spawns[] = {
    { 8, 8, 0 },
};
static const NpcTemplate map5_npcs[] = {
    { 4, 8, 7, 0, NPC_MOVE_NONE, 0, {{0,0}}, 0, 3, 0, TRUE }, /* innkeeper */
};

/* =========================================================
 * Generic small indoor room helper macros
 * ========================================================= */

/* 20x15 room: walls on border, open interior, warp at bottom center */
#define ROOM20x15_TILES(warp_x) \
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, \
    1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1, \
    1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1, \
    1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1, \
    1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1, \
    1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1, \
    1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1, \
    1,2,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,2,1, \
    1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1, \
    1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1, \
    1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1, \
    1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1, \
    1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1, \
    1,2,2,2,2,2,2,2,2,5,5,2,2,2,2,2,2,2,2,1, \
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1

/* flags for the same room */
#define ROOM20x15_FLAGS \
    FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS, \
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS, \
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS, \
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS, \
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS, \
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS, \
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS, \
    FS,F0,F0,F0,F0,F0,F0,F0,FE,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS, \
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS, \
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS, \
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS, \
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS, \
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS, \
    FS,F0,F0,F0,F0,F0,F0,F0,F0,FP,FP,F0,F0,F0,F0,F0,F0,F0,F0,FS, \
    FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS

/* Warps and spawns for generic rooms */
#define ROOM_WARP(dest) { 9, 13, (dest), 0, WARP_DOOR }
#define ROOM_SPAWN      { 9, 12, 0 }
#define ROOM_NPC_NONE   NULL

/* =========================================================
 * MAP 6 — ROUTE_02 Mountain Pass (24x32)
 * Reuse same structure as route_01 with mountain tiles
 * ========================================================= */
static const u16 map6_tiles[24 * 32] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,1,1,0,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,1,
    1,0,1,1,0,0,0,0,0,1,1,0,0,0,1,1,0,0,9,9,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,9,0,0,0,1,
    1,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,1,
    1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,1,
    1,0,1,2,0,9,9,0,0,0,0,0,0,9,9,0,0,2,0,0,0,0,0,1,
    1,0,1,2,0,9,9,0,0,0,0,0,0,9,9,0,0,2,1,0,0,0,0,1,
    1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,1,
    1,0,0,2,0,0,1,0,0,0,8,0,0,0,1,0,0,2,0,0,0,0,0,1,
    1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,1,
    1,0,0,2,0,9,9,9,0,0,0,9,9,9,0,0,0,2,0,0,0,0,0,1,
    1,0,0,2,0,9,9,9,0,0,0,9,9,9,0,0,0,2,0,0,0,0,0,1,
    1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,1,
    1,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,
    1,0,1,1,0,0,0,9,9,2,2,2,2,9,9,0,1,1,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,9,9,0,2,0,0,2,0,9,9,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,9,9,0,2,0,0,2,0,9,9,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,1,
    1,0,1,1,0,0,0,0,0,2,0,0,2,0,0,1,1,0,0,0,0,0,0,1,
    1,0,1,1,0,0,0,0,0,2,0,0,2,0,0,1,1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};
static const u8 map6_flags[24 * 32] = {
    FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,FS,FS,F0,F0,F0,F0,F0,FS,FS,F0,F0,F0,FS,FS,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,FS,FS,F0,F0,F0,F0,F0,FS,FS,F0,F0,F0,FS,FS,F0,F0,FG,FG,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FG,FG,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,FS,F0,F0,FG,FG,F0,F0,F0,F0,F0,F0,FG,FG,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,FS,F0,F0,FG,FG,F0,F0,F0,F0,F0,F0,FG,FG,F0,F0,F0,FS,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,FS,F0,F0,F0,FE,F0,F0,F0,FS,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,FG,FG,FG,F0,F0,F0,FG,FG,FG,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,FG,FG,FG,F0,F0,F0,FG,FG,FG,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,FS,FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,FS,F0,F0,F0,F0,F0,FS,
    FS,F0,FS,FS,F0,F0,F0,FG,FG,F0,F0,F0,F0,FG,FG,F0,FS,FS,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,FG,FG,F0,F0,F0,F0,F0,F0,FG,FG,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,FG,FG,F0,F0,F0,F0,F0,F0,FG,FG,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,FS,FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,FS,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,FS,FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,FS,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,F0,FS,
    FS,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FP,FS,
    FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,FS,
};
static const WarpEntry map6_warps[] = {
    { 1,  30, 8, 0, WARP_TRANSITION },
    { 9,  19, 7, 0, WARP_CAVE       },
};
static const SpawnPoint map6_spawns[] = {
    { 11, 1,  2 },
    { 10, 29, 0 },
};
static const NpcTemplate map6_npcs[] = {
    { 2, 11, 10, 0, NPC_MOVE_WANDER, 3, {{0,0}}, 0, 1, 0, TRUE },
};

/* =========================================================
 * MAPS 7-21 — Generic rooms using shared macros
 * Each room is 20x15, exits at bottom center.
 * ========================================================= */

/* MAP 7 — DUNGEON_02 Cave of Trials */
static const u16 map7_tiles[20 * 15]  = { ROOM20x15_TILES(9) };
static const u8  map7_flags[20 * 15]  = { ROOM20x15_FLAGS };
static const WarpEntry  map7_warps[]  = { ROOM_WARP(6)  };
static const SpawnPoint map7_spawns[] = { ROOM_SPAWN     };

/* MAP 8 — CASTLE_AETHERIA */
static const u16 map8_tiles[20 * 15]  = { ROOM20x15_TILES(9) };
static const u8  map8_flags[20 * 15]  = { ROOM20x15_FLAGS };
static const WarpEntry  map8_warps[]  = { ROOM_WARP(10) };
static const SpawnPoint map8_spawns[] = { ROOM_SPAWN     };

/* MAP 9 — CASTLE_THRONE */
static const u16 map9_tiles[20 * 15]  = { ROOM20x15_TILES(9) };
static const u8  map9_flags[20 * 15]  = { ROOM20x15_FLAGS };
static const WarpEntry  map9_warps[]  = { ROOM_WARP(8)  };
static const SpawnPoint map9_spawns[] = { ROOM_SPAWN     };

/* MAP 10 — TOWN_HARBOR */
static const u16 map10_tiles[20 * 15] = { ROOM20x15_TILES(9) };
static const u8  map10_flags[20 * 15] = { ROOM20x15_FLAGS };
static const WarpEntry  map10_warps[] = { ROOM_WARP(12) };
static const SpawnPoint map10_spawns[]= { ROOM_SPAWN     };

/* MAP 11 — DUNGEON_03 Ancient Crypt */
static const u16 map11_tiles[20 * 15] = { ROOM20x15_TILES(9) };
static const u8  map11_flags[20 * 15] = { ROOM20x15_FLAGS };
static const WarpEntry  map11_warps[] = { ROOM_WARP(10) };
static const SpawnPoint map11_spawns[]= { ROOM_SPAWN     };

/* MAP 12 — ROUTE_03 */
static const u16 map12_tiles[20 * 15] = { ROOM20x15_TILES(9) };
static const u8  map12_flags[20 * 15] = { ROOM20x15_FLAGS };
static const WarpEntry  map12_warps[] = { ROOM_WARP(13) };
static const SpawnPoint map12_spawns[]= { ROOM_SPAWN     };

/* MAP 13 — VILLAGE_FOREST */
static const u16 map13_tiles[20 * 15] = { ROOM20x15_TILES(9) };
static const u8  map13_flags[20 * 15] = { ROOM20x15_FLAGS };
static const WarpEntry  map13_warps[] = { ROOM_WARP(12) };
static const SpawnPoint map13_spawns[]= { ROOM_SPAWN     };

/* MAP 14 — DUNGEON_04 Volcano Cavern */
static const u16 map14_tiles[20 * 15] = { ROOM20x15_TILES(9) };
static const u8  map14_flags[20 * 15] = { ROOM20x15_FLAGS };
static const WarpEntry  map14_warps[] = { ROOM_WARP(15) };
static const SpawnPoint map14_spawns[]= { ROOM_SPAWN     };

/* MAP 15 — TOWN_SNOWPEAK */
static const u16 map15_tiles[20 * 15] = { ROOM20x15_TILES(9) };
static const u8  map15_flags[20 * 15] = { ROOM20x15_FLAGS };
static const WarpEntry  map15_warps[] = { ROOM_WARP(16) };
static const SpawnPoint map15_spawns[]= { ROOM_SPAWN     };

/* MAP 16 — DUNGEON_05 Ice Cave */
static const u16 map16_tiles[20 * 15] = { ROOM20x15_TILES(9) };
static const u8  map16_flags[20 * 15] = { ROOM20x15_FLAGS };
static const WarpEntry  map16_warps[] = { ROOM_WARP(15) };
static const SpawnPoint map16_spawns[]= { ROOM_SPAWN     };

/* MAP 17 — SECRET_ISLAND_01 */
static const u16 map17_tiles[20 * 15] = { ROOM20x15_TILES(9) };
static const u8  map17_flags[20 * 15] = { ROOM20x15_FLAGS };
static const WarpEntry  map17_warps[] = { ROOM_WARP(10) };
static const SpawnPoint map17_spawns[]= { ROOM_SPAWN     };

/* MAP 18 — SHADOW_REALM */
static const u16 map18_tiles[20 * 15] = { ROOM20x15_TILES(9) };
static const u8  map18_flags[20 * 15] = { ROOM20x15_FLAGS };
static const WarpEntry  map18_warps[] = { ROOM_WARP(19) };
static const SpawnPoint map18_spawns[]= { ROOM_SPAWN     };

/* MAP 19 — DUNGEON_BOSS_LAIR */
static const u16 map19_tiles[20 * 15] = { ROOM20x15_TILES(9) };
static const u8  map19_flags[20 * 15] = { ROOM20x15_FLAGS };
static const WarpEntry  map19_warps[] = { ROOM_WARP(20) };
static const SpawnPoint map19_spawns[]= { ROOM_SPAWN     };

/* MAP 20 — CASTLE_DARK */
static const u16 map20_tiles[20 * 15] = { ROOM20x15_TILES(9) };
static const u8  map20_flags[20 * 15] = { ROOM20x15_FLAGS };
static const WarpEntry  map20_warps[] = { ROOM_WARP(6)  };
static const SpawnPoint map20_spawns[]= { ROOM_SPAWN     };

/* MAP 21 — HOUSE_SHOP */
static const u16 map21_tiles[20 * 15] = { ROOM20x15_TILES(9) };
static const u8  map21_flags[20 * 15] = { ROOM20x15_FLAGS };
static const WarpEntry  map21_warps[] = { ROOM_WARP(4)  };
static const SpawnPoint map21_spawns[]= { ROOM_SPAWN     };

/* =========================================================
 * MapDef assembly
 * ========================================================= */
#define MAPDEF(id, w, h, ts, mus, out, tiles, flags, wps, sps, npcs, objs, n, s, e, ww) \
    { (id), (w), (h), (ts), (mus), (out), (tiles), (flags),               \
      (wps),  (u8)(sizeof(wps)/sizeof(wps[0])),                            \
      (sps),  (u8)(sizeof(sps)/sizeof(sps[0])),                            \
      (npcs), (u8)((npcs) ? sizeof(*npcs)/sizeof(*(npcs)) : 0),           \
      (objs), (u8)((objs) ? sizeof(*objs)/sizeof(*(objs)) : 0),           \
      (n), (s), (e), (ww) }

/* Wrapper for NPC count that handles NULL safely */
#define NPC_COUNT(arr) (u8)(sizeof(arr)/sizeof((arr)[0]))
#define OBJ_COUNT(arr) (u8)(sizeof(arr)/sizeof((arr)[0]))

static const MapDef s_map_2 = {
    2, 24, 32, 2, 2, TRUE,
    map2_tiles, map2_flags,
    map2_warps,  (u8)(sizeof(map2_warps)/sizeof(map2_warps[0])),
    map2_spawns, (u8)(sizeof(map2_spawns)/sizeof(map2_spawns[0])),
    map2_npcs,   NPC_COUNT(map2_npcs),
    NULL, 0,
    0, 4, MAP_ID_NONE, MAP_ID_NONE
};
static const MapDef s_map_3 = {
    3, 20, 20, 3, 3, FALSE,
    map3_tiles, map3_flags,
    map3_warps,  (u8)(sizeof(map3_warps)/sizeof(map3_warps[0])),
    map3_spawns, (u8)(sizeof(map3_spawns)/sizeof(map3_spawns[0])),
    map3_npcs,   NPC_COUNT(map3_npcs),
    NULL, 0,
    MAP_ID_NONE, MAP_ID_NONE, MAP_ID_NONE, MAP_ID_NONE
};
static const MapDef s_map_4 = {
    4, 28, 28, 0, 2, TRUE,
    map4_tiles, map4_flags,
    map4_warps,  (u8)(sizeof(map4_warps)/sizeof(map4_warps[0])),
    map4_spawns, (u8)(sizeof(map4_spawns)/sizeof(map4_spawns[0])),
    map4_npcs,   NPC_COUNT(map4_npcs),
    NULL, 0,
    2, 6, MAP_ID_NONE, MAP_ID_NONE
};
static const MapDef s_map_5 = {
    5, 18, 12, 1, 1, FALSE,
    map5_tiles, map5_flags,
    map5_warps,  (u8)(sizeof(map5_warps)/sizeof(map5_warps[0])),
    map5_spawns, (u8)(sizeof(map5_spawns)/sizeof(map5_spawns[0])),
    map5_npcs,   NPC_COUNT(map5_npcs),
    NULL, 0,
    MAP_ID_NONE, MAP_ID_NONE, MAP_ID_NONE, MAP_ID_NONE
};
static const MapDef s_map_6 = {
    6, 24, 32, 7, 2, TRUE,
    map6_tiles, map6_flags,
    map6_warps,  (u8)(sizeof(map6_warps)/sizeof(map6_warps[0])),
    map6_spawns, (u8)(sizeof(map6_spawns)/sizeof(map6_spawns[0])),
    map6_npcs,   NPC_COUNT(map6_npcs),
    NULL, 0,
    4, 8, MAP_ID_NONE, MAP_ID_NONE
};

/* idx, tileset_id, music_id, outdoor, data_prefix, map_north, map_south */
#define SIMPLE_MAPDEF(idx, ts, mus, out, pfx, mn, ms)               \
static const MapDef s_map_##idx = {                                  \
    idx, 20, 15, ts, mus, out,                                       \
    pfx##_tiles, pfx##_flags,                                        \
    pfx##_warps,  (u8)(sizeof(pfx##_warps)/sizeof(pfx##_warps[0])), \
    pfx##_spawns, (u8)(sizeof(pfx##_spawns)/sizeof(pfx##_spawns[0])),\
    NULL, 0, NULL, 0,                                                \
    mn, ms, MAP_ID_NONE, MAP_ID_NONE                                 \
}

SIMPLE_MAPDEF( 7,  3, 3, FALSE, map7,  MAP_ID_NONE,  6         );
SIMPLE_MAPDEF( 8,  4, 4, TRUE,  map8,  6,            MAP_ID_NONE);
SIMPLE_MAPDEF( 9,  4, 4, FALSE, map9,  MAP_ID_NONE,  8         );
SIMPLE_MAPDEF(10,  5, 2, TRUE,  map10, MAP_ID_NONE,  11        );
SIMPLE_MAPDEF(11,  3, 3, FALSE, map11, 10,           MAP_ID_NONE);
SIMPLE_MAPDEF(12,  0, 2, TRUE,  map12, 13,           MAP_ID_NONE);
SIMPLE_MAPDEF(13,  0, 2, TRUE,  map13, MAP_ID_NONE,  12        );
SIMPLE_MAPDEF(14,  8, 3, FALSE, map14, MAP_ID_NONE,  15        );
SIMPLE_MAPDEF(15,  9, 2, TRUE,  map15, 14,           MAP_ID_NONE);
SIMPLE_MAPDEF(16,  9, 3, FALSE, map16, MAP_ID_NONE,  15        );
SIMPLE_MAPDEF(17,  5, 2, TRUE,  map17, MAP_ID_NONE,  MAP_ID_NONE);
SIMPLE_MAPDEF(18, 10, 9, FALSE, map18, MAP_ID_NONE,  19        );
SIMPLE_MAPDEF(19,  3, 3, FALSE, map19, 18,           MAP_ID_NONE);
SIMPLE_MAPDEF(20,  4, 4, TRUE,  map20, MAP_ID_NONE,  MAP_ID_NONE);
SIMPLE_MAPDEF(21,  1, 2, FALSE, map21, MAP_ID_NONE,  4         );

/* =========================================================
 * Public API
 * ========================================================= */
const MapDef* g_ext_maps[20];
const u8      g_ext_map_count = 20;

void map_data_ext_init(void)
{
    g_ext_maps[ 0] = &s_map_2;
    g_ext_maps[ 1] = &s_map_3;
    g_ext_maps[ 2] = &s_map_4;
    g_ext_maps[ 3] = &s_map_5;
    g_ext_maps[ 4] = &s_map_6;
    g_ext_maps[ 5] = &s_map_7;
    g_ext_maps[ 6] = &s_map_8;
    g_ext_maps[ 7] = &s_map_9;
    g_ext_maps[ 8] = &s_map_10;
    g_ext_maps[ 9] = &s_map_11;
    g_ext_maps[10] = &s_map_12;
    g_ext_maps[11] = &s_map_13;
    g_ext_maps[12] = &s_map_14;
    g_ext_maps[13] = &s_map_15;
    g_ext_maps[14] = &s_map_16;
    g_ext_maps[15] = &s_map_17;
    g_ext_maps[16] = &s_map_18;
    g_ext_maps[17] = &s_map_19;
    g_ext_maps[18] = &s_map_20;
    g_ext_maps[19] = &s_map_21;
    world_add_maps(g_ext_maps, g_ext_map_count);
}
