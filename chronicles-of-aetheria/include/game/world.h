#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#include "../engine/engine.h"

/* =========================================================
 * World / Map System
 *
 * Hierarchy:
 *   World → MapDef[] → MapArea (runtime state)
 *
 * MapDef: const ROM data describing a map (tiles, warps, NPCs)
 * MapArea: runtime state (which chunks are loaded, NPC states)
 *
 * Map size: up to 64x64 tiles (512x512 pixels)
 * Tile size: 8x8 pixels (4bpp)
 * Chunk size: 16x16 tiles (128x128 pixels) — streaming unit
 *
 * VRAM layout uses a 32x32 screenblock as a circular buffer:
 *   screenblock[wy % 32][wx % 32] = tile at world pos (wx,wy)
 * When camera scrolls past a tile boundary, we refill the
 * newly visible row/column.
 * ========================================================= */

/* ---- Constants ------------------------------------------ */
#define MAP_MAX_TILES_W    64
#define MAP_MAX_TILES_H    64
#define MAP_CHUNK_SIZE     16     /* tiles per chunk side */
#define MAP_MAX_WARPS      32
#define MAP_MAX_NPCS       16
#define MAP_MAX_OBJECTS    24
#define MAP_MAX_SCRIPTS    64
#define MAP_ID_INVALID     0xFF
#define MAP_ID_NONE        0xFF

/* ---- Tile flags (per-tile in flag_data[]) --------------- */
#define TF_SOLID     0x01   /* impassable */
#define TF_WATER     0x02   /* requires surf */
#define TF_EVENT     0x04   /* has event at this tile */
#define TF_WARP      0x08   /* step-on warp trigger */
#define TF_TALL_GRASS 0x10  /* random encounter zone */
#define TF_LADDER    0x20   /* directional pass-through */
#define TF_LEDGE_D   0x40   /* can jump down */
#define TF_ICE       0x80   /* slides player */

/* ---- Warp types ----------------------------------------- */
typedef enum {
    WARP_DOOR = 0,       /* door open animation then warp */
    WARP_CAVE,           /* instant warp (cave entrance) */
    WARP_TELEPORT,       /* sparkle animation */
    WARP_SLIDE,          /* stairs/escalator */
    WARP_TRANSITION,     /* screen-edge map transition */
} WarpType;

typedef struct {
    u8       src_x, src_y;      /* tile position of warp trigger */
    u8       dest_map;          /* destination map ID */
    u8       dest_spawn;        /* which spawn point to use */
    WarpType type;
} WarpEntry;

typedef struct {
    u8 x, y;    /* tile position for player spawn */
    u8 dir;     /* facing direction (DIR_DOWN etc.) */
} SpawnPoint;

/* ---- Map object (interactive prop) ---------------------- */
typedef enum {
    OBJ_SIGN = 0,       /* read sign → dialogue */
    OBJ_CHEST,          /* open chest → item */
    OBJ_DOOR_LOCKED,    /* requires key item */
    OBJ_BOOKSHELF,
    OBJ_NOTICE_BOARD,
} ObjectType;

typedef struct {
    u8         x, y;
    ObjectType type;
    u16        script_id;
    u16        item_id;     /* for CHEST */
    BOOL       opened;      /* runtime: already used */
} MapObject;

/* ---- NPC movement types --------------------------------- */
typedef enum {
    NPC_MOVE_NONE = 0,    /* stationary, face fixed direction */
    NPC_MOVE_LOOK,        /* rotate facing on a timer */
    NPC_MOVE_WANDER,      /* random walk within range */
    NPC_MOVE_PATROL,      /* follow fixed waypoint list */
    NPC_MOVE_TRAINER,     /* face direction, sight-trigger battle */
} NpcMoveType;

#define NPC_WAYPOINT_MAX 8

typedef struct {
    u8          sprite_id;       /* which sprite tile set to use */
    u8          x, y;            /* start tile position */
    u8          dir;             /* initial facing */
    NpcMoveType move_type;
    u8          move_range;      /* tiles, for WANDER */
    u8          sight_range;     /* tiles, for TRAINER */
    u16         script_id;       /* dialogue/battle script */
    /* PATROL waypoints (local coords relative to start) */
    u8          waypoint_count;
    s8          waypoints[NPC_WAYPOINT_MAX][2];  /* dx,dy per step */
} NpcTemplate;

/* ---- Tileset descriptor --------------------------------- */
typedef struct {
    u8          charblock;       /* VRAM charblock 0-3 */
    u16         tile_count;
    const u32*  tile_gfx;        /* 4bpp tile graphics (ROM) */
    u8          palette_count;
    const COLOR* palettes;       /* 16-color palettes (ROM) */
} TilesetDef;

/* ---- Map definition (ROM data) -------------------------- */
typedef struct {
    u8                 id;
    u8                 width;           /* in tiles */
    u8                 height;
    u8                 tileset_id;
    u8                 music_id;
    u8                 outdoor;         /* TRUE = affected by day/night */
    const u16*         tile_data;       /* width*height tile IDs */
    const u8*          flag_data;       /* width*height TF_ flags */
    const WarpEntry*   warps;
    u8                 warp_count;
    const SpawnPoint*  spawns;
    u8                 spawn_count;
    const NpcTemplate* npc_templates;
    u8                 npc_count;
    const MapObject*   objects;
    u8                 object_count;
    /* Neighbor maps for screen-edge transitions */
    u8                 map_north;
    u8                 map_south;
    u8                 map_east;
    u8                 map_west;
} MapDef;

/* ---- Runtime map area ----------------------------------- */
typedef struct {
    const MapDef* def;
    /* Tile streaming: track last camera tile position */
    s16  cam_tile_x;    /* camera origin in tile coords */
    s16  cam_tile_y;
    BOOL needs_full_reload;
} MapArea;

/* ---- World API ------------------------------------------ */

void    world_init(const MapDef** map_table, u8 map_count);

/* Load a map by ID; player spawns at spawn_id */
void    world_load_map(u8 map_id, u8 spawn_id);

/* Get the active map */
const MapDef*  world_current_map(void);
MapArea*       world_current_area(void);

/* Called each frame to stream tiles as camera moves */
void    world_update_streaming(void);

/* Full screenblock upload (called on map load) */
void    world_upload_full_map(u8 screenblock, const MapDef* map);

/* Get tile ID at world tile position */
u16     world_get_tile(s16 tx, s16 ty);

/* Get flags at world tile position */
u8      world_get_flags(s16 tx, s16 ty);

/* Check if world tile is solid */
BOOL    world_tile_solid(s16 tx, s16 ty);

/* Check warp at tile position; returns warp entry or NULL */
const WarpEntry* world_get_warp(s16 tx, s16 ty);

/* Check map object at tile position */
MapObject* world_get_object(s16 tx, s16 ty);

/* Get spawn position */
SpawnPoint world_get_spawn(const MapDef* map, u8 spawn_id);

/* Warp transition: fade → load map → fade in */
void    world_start_warp(const WarpEntry* warp);
void    world_warp_update(void);
BOOL    world_is_warping(void);

/* Door animation */
void    world_door_open(s16 tx, s16 ty);
void    world_door_close(s16 tx, s16 ty);

/* Per-frame tick: warp update, door anim, chunk streaming */
void    world_tick(void);

/* Registers map table (global, from ROM) */
extern const MapDef**  g_map_table;
extern u8              g_map_count;

#endif /* GAME_WORLD_H */
