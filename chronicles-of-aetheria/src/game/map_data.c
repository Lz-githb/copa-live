#include "../../include/game/world.h"
#include "../../include/game/interaction.h"
#include "../../include/game/npc.h"
#define DIR_DOWN  NPC_DIR_DOWN
#define DIR_UP    NPC_DIR_UP
#define DIR_LEFT  NPC_DIR_LEFT
#define DIR_RIGHT NPC_DIR_RIGHT

/* =========================================================
 * Map Data — ROM-resident map definitions
 *
 * This file contains the actual map/NPC/warp data for the
 * game. In production, maps are exported from a map editor
 * (Tiled, AdvanceMap, etc.) and converted to C arrays.
 *
 * Map IDs
 *   0  — TOWN_START    (Starting village, outdoor)
 *   1  — HOUSE_01      (Village house interior, indoor)
 *   2  — ROUTE_01      (First route, outdoor, encounters)
 * ========================================================= */

/* =========================================================
 * MAP 0 — TOWN_START (32x32 tiles = 256x256 pixels)
 *
 * Tile legend (abbreviated):
 *   0  = grass (walkable)
 *   1  = tree (solid)
 *   2  = path (walkable)
 *   3  = water (solid, needs surf)
 *   4  = house wall (solid)
 *   5  = house door (warp trigger)
 *   6  = flower (walkable)
 *   7  = fence (solid)
 *   8  = sign post (solid, but TF_EVENT set for interaction)
 * ========================================================= */

/* 32x32 tile layout for starting town.
 * Each row is 32 entries. 0=grass, 1=tree/solid, 2=path, etc. */
static const u16 map0_tiles[32 * 32] = {
    /* Row 0 */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    /* Row 1 */
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    /* Row 2 */
    1,0,0,0,0,0,6,6,0,0,4,4,4,0,0,0,4,4,4,0,0,0,6,6,0,0,0,0,0,0,0,1,
    /* Row 3 */
    1,0,0,0,0,0,6,6,0,0,4,4,4,0,0,0,4,4,4,0,0,0,6,6,0,0,0,0,0,0,0,1,
    /* Row 4 */
    1,0,0,0,0,0,0,0,0,0,4,5,4,0,0,0,4,5,4,0,0,0,0,0,0,0,0,0,0,0,0,1,
    /* Row 5 */
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    /* Row 6 */
    1,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,1,
    /* Row 7 */
    1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,1,
    /* Row 8 */
    1,0,2,0,8,0,0,0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,0,0,2,0,0,1,
    /* Row 9 */
    1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,0,0,2,0,0,1,
    /* Row 10 */
    1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,0,0,2,0,0,1,
    /* Row 11 */
    1,0,2,0,0,0,0,0,4,4,4,4,4,0,0,0,0,0,0,4,4,4,4,4,0,0,0,0,2,0,0,1,
    /* Row 12 */
    1,0,2,0,0,0,0,0,4,4,4,4,4,0,0,0,0,0,0,4,4,4,4,4,0,0,0,0,2,0,0,1,
    /* Row 13 */
    1,0,2,0,0,0,0,0,4,5,4,5,4,0,0,0,0,0,0,4,5,4,5,4,0,0,0,0,2,0,0,1,
    /* Row 14 */
    1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,1,
    /* Row 15 */
    1,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,1,
    /* Row 16 */
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    /* Row 17 */
    1,0,0,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,0,0,0,1,
    /* Row 18 */
    1,0,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,1,
    /* Row 19 */
    1,0,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,1,
    /* Row 20 */
    1,0,0,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,0,0,0,1,
    /* Row 21 - Route transition at bottom */
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    /* Row 22 */
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    /* Row 23-31: edge walls */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

/* Collision flags for map 0 */
static const u8 map0_flags[32 * 32] = {
    /* Row 0: all solid trees */
    TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,
    TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,
    TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,
    TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,
    /* Rows 1-22: mostly walkable (simplified, expand per tile above) */
    /* For brevity we just zero-fill (walkable), with the few solids
     * being walls. A real game would have these fully specified. */
    /* ... (all remaining rows zero = walkable for prototype) */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,TF_SOLID,TF_SOLID,TF_SOLID,0,0,0,TF_SOLID,TF_SOLID,TF_SOLID,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,TF_SOLID,TF_SOLID,TF_SOLID,0,0,0,TF_SOLID,TF_SOLID,TF_SOLID,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,TF_SOLID,TF_WARP,TF_SOLID,0,0,0,TF_SOLID,TF_WARP,TF_SOLID,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,TF_EVENT,0,0,0,0,0,0,0,0,0,0,TF_WATER,TF_WATER,TF_WATER,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,TF_WATER,TF_WATER,TF_WATER,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,TF_WATER,TF_WATER,TF_WATER,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,0,0,0,0,0,0,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,0,0,0,0,0,0,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,TF_SOLID,TF_WARP,TF_SOLID,TF_WARP,TF_SOLID,0,0,0,0,0,0,TF_SOLID,TF_WARP,TF_SOLID,TF_WARP,TF_SOLID,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,0,0,0,0,
    0,0,0,TF_SOLID,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,TF_SOLID,0,0,0,0,
    0,0,0,TF_SOLID,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,TF_SOLID,0,0,0,0,
    0,0,0,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,0,0,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,
    TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,0,0,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,
    TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,0,0,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,
    TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,0,0,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,
    TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,0,0,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,
    TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,0,0,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,
    TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,0,0,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,
    TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,0,0,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,
    TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,0,0,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,
};

/* Warps: door tiles at (11,4) and (17,4) → house map (id=1) */
static const WarpEntry map0_warps[] = {
    { 11, 4,  1, 0, WARP_DOOR  },   /* house 1 door → map 1 spawn 0 */
    { 17, 4,  1, 1, WARP_DOOR  },   /* house 2 door → map 1 spawn 1 */
    { 9,  13, 1, 2, WARP_DOOR  },   /* large house left door */
    { 11, 13, 1, 2, WARP_DOOR  },   /* large house right door */
    { 19, 13, 1, 3, WARP_DOOR  },
    { 21, 13, 1, 3, WARP_DOOR  },
    { 15, 23, 2, 0, WARP_TRANSITION }, /* south exit → route 01 */
};

/* Spawns */
static const SpawnPoint map0_spawns[] = {
    { 15, 7, DIR_DOWN },   /* spawn 0: default, top of main path */
};

/* NPC: village elder (stationary, faces down) */
static const NpcTemplate map0_npcs[] = {
    {
        .sprite_id    = 0,
        .x = 8, .y = 8,
        .dir          = NPC_DIR_DOWN,
        .move_type    = NPC_MOVE_LOOK,
        .move_range   = 0,
        .sight_range  = 0,
        .script_id    = 0,
        .waypoint_count = 0,
    },
    {
        .sprite_id    = 1,
        .x = 18, .y = 19,
        .dir          = NPC_DIR_LEFT,
        .move_type    = NPC_MOVE_WANDER,
        .move_range   = 3,
        .sight_range  = 0,
        .script_id    = 1,
        .waypoint_count = 0,
    },
};

/* Objects: sign at (4,8) */
static const MapObject map0_objects[] = {
    { 4, 8, OBJ_SIGN, 2, 0, FALSE },
};

/* =========================================================
 * MAP 1 — HOUSE_01 (interior, 20x15 tiles)
 * ========================================================= */
static const u16 map1_tiles[20 * 15] = {
    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
    4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
    4,0,9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9,0,4,
    4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
    4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
    4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
    4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
    4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
    4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
    4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
    4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
    4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
    4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
    4,0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,4,
    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
};

static const u8 map1_flags[20 * 15] = {
    /* Row 0: solid walls */
    TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,
    TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,
    /* Rows 1-13: walls on edges, open in middle */
    TF_SOLID,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,TF_SOLID,
    TF_SOLID,0,TF_SOLID,TF_SOLID,TF_SOLID,0,0,0,0,0,0,0,0,0,0,TF_SOLID,TF_SOLID,TF_SOLID,0,TF_SOLID,
    TF_SOLID,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,TF_SOLID,
    TF_SOLID,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,TF_SOLID,
    TF_SOLID,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,TF_SOLID,
    TF_SOLID,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,TF_SOLID,
    TF_SOLID,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,TF_SOLID,
    TF_SOLID,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,TF_SOLID,
    TF_SOLID,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,TF_SOLID,
    TF_SOLID,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,TF_SOLID,
    TF_SOLID,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,TF_SOLID,
    TF_SOLID,0,0,0,0,0,0,0,0,0,TF_WARP,0,0,0,0,0,0,0,0,TF_SOLID,
    /* Row 14: solid bottom wall */
    TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,
    TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,TF_SOLID,
};

static const WarpEntry map1_warps[] = {
    { 10, 13, 0, 0, WARP_DOOR },   /* exit → map 0 spawn 0 */
};

static const SpawnPoint map1_spawns[] = {
    { 10, 12, DIR_DOWN },   /* spawn 0: inside house, facing door */
    { 10, 12, DIR_DOWN },   /* spawn 1 */
    { 10, 12, DIR_DOWN },   /* spawn 2 */
    { 10, 12, DIR_DOWN },   /* spawn 3 */
};

static const NpcTemplate map1_npcs[] = {
    {
        .sprite_id    = 2,
        .x = 10, .y = 4,
        .dir          = NPC_DIR_DOWN,
        .move_type    = NPC_MOVE_NONE,
        .move_range   = 0,
        .sight_range  = 0,
        .script_id    = 3,
        .waypoint_count = 0,
    },
};

/* =========================================================
 * Script data (placeholder — real scripts use SCMD_* arrays)
 * ========================================================= */

/* Text table */
static const DialogueText s_text_table[] = {
    /* 0: Elder */
    { { "Welcome, young hero.",
        "The world of Aetheria",
        "awaits your courage.",
        "Safe travels!" }, 4 },
    /* 1: Wandering villager */
    { { "What a fine day for",
        "a walk around the village.", NULL, NULL }, 2 },
    /* 2: Sign post */
    { { "AETHERIA VILLAGE",
        "Population: 12",
        "A peaceful town at the",
        "edge of the forest." }, 4 },
    /* 3: House NPC */
    { { "Oh, a visitor!",
        "Please, make yourself",
        "at home.", NULL }, 3 },
};

/* Script 0: elder dialogue */
static const ScriptCmd s_script_0[] = {
    { SCMD_FACE_PLAYER, 0, 0 },
    { SCMD_DIALOGUE, 0, 0 },
    { SCMD_END, 0, 0 },
};

/* Script 1: wanderer */
static const ScriptCmd s_script_1[] = {
    { SCMD_FACE_PLAYER, 0, 0 },
    { SCMD_DIALOGUE, 1, 0 },
    { SCMD_END, 0, 0 },
};

/* Script 2: sign */
static const ScriptCmd s_script_2[] = {
    { SCMD_DIALOGUE, 2, 0 },
    { SCMD_END, 0, 0 },
};

/* Script 3: house NPC */
static const ScriptCmd s_script_3[] = {
    { SCMD_FACE_PLAYER, 0, 0 },
    { SCMD_DIALOGUE, 3, 0 },
    { SCMD_END, 0, 0 },
};

static const ScriptCmd* s_script_table[] = {
    s_script_0, s_script_1, s_script_2, s_script_3,
};

/* =========================================================
 * Map definitions (assembled)
 * ========================================================= */

static const MapDef s_map_0 = {
    .id             = 0,
    .width          = 32,
    .height         = 32,
    .tileset_id     = 0,
    .music_id       = 0,
    .outdoor        = TRUE,
    .tile_data      = map0_tiles,
    .flag_data      = map0_flags,
    .warps          = map0_warps,
    .warp_count     = ARRAY_SIZE(map0_warps),
    .spawns         = map0_spawns,
    .spawn_count    = ARRAY_SIZE(map0_spawns),
    .npc_templates  = map0_npcs,
    .npc_count      = ARRAY_SIZE(map0_npcs),
    .objects        = map0_objects,
    .object_count   = ARRAY_SIZE(map0_objects),
    .map_north      = MAP_ID_NONE,
    .map_south      = 2,
    .map_east       = MAP_ID_NONE,
    .map_west       = MAP_ID_NONE,
};

static const MapDef s_map_1 = {
    .id             = 1,
    .width          = 20,
    .height         = 15,
    .tileset_id     = 1,
    .music_id       = 1,
    .outdoor        = FALSE,
    .tile_data      = map1_tiles,
    .flag_data      = map1_flags,
    .warps          = map1_warps,
    .warp_count     = ARRAY_SIZE(map1_warps),
    .spawns         = map1_spawns,
    .spawn_count    = ARRAY_SIZE(map1_spawns),
    .npc_templates  = map1_npcs,
    .npc_count      = ARRAY_SIZE(map1_npcs),
    .objects        = NULL,
    .object_count   = 0,
    .map_north      = MAP_ID_NONE,
    .map_south      = MAP_ID_NONE,
    .map_east       = MAP_ID_NONE,
    .map_west       = MAP_ID_NONE,
};

/* Global map registry — passed to world_init() */
static const MapDef* s_map_list[] = {
    &s_map_0,
    &s_map_1,
};

/* Called from main() to register everything.
   interact_init is called by dialogue_db_init() with the full tables. */
void map_data_init(void) {
    world_init(s_map_list, ARRAY_SIZE(s_map_list));
}
