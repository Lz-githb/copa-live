#include "../../include/game/world.h"
#include "../../include/engine/engine.h"

/* =========================================================
 * World / Map System Implementation
 * ========================================================= */

/* ---- Global map registry -------------------------------- */
#define MAP_REGISTRY_MAX 64
static const MapDef* s_map_registry[MAP_REGISTRY_MAX];
const MapDef** g_map_table = s_map_registry;
u8             g_map_count = 0;

/* ---- Runtime state -------------------------------------- */
static MapArea  s_area;
static MapArea* s_active = &s_area;

/* Warp transition state */
typedef enum {
    WARP_IDLE = 0,
    WARP_FADE_OUT,      /* fading to black */
    WARP_LOAD,          /* perform map load */
    WARP_FADE_IN,       /* fading back in */
} WarpPhase;

static struct {
    WarpPhase phase;
    u8        timer;       /* 0..FADE_FRAMES */
    u8        dest_map;
    u8        dest_spawn;
    WarpType  type;
} s_warp;

#define FADE_FRAMES  15   /* frames for black fade in/out */

/* REG_BLDY — reuse from time_weather but also needed here */
#define REG_BLDY_W  (*(vu16*)(0x04000054))
#define REG_BLDCNT_W (*(vu16*)(0x04000050))
#define BLD_ALL     0x3F   /* all layers as source */
#define BLD_BLACK_W 0x00C0

/* Door animation state */
typedef struct {
    u8   tile_x, tile_y;
    u8   screenblock;
    u8   timer;
    BOOL opening;
    BOOL active;
    u16  open_tile;    /* tile ID for "open door" */
    u16  close_tile;   /* tile ID for "closed door" */
} DoorAnim;

static DoorAnim s_door;

/* ---- Helpers -------------------------------------------- */

static const MapDef* _get_map(u8 id) {
    if (!g_map_table || id >= g_map_count) return NULL;
    return g_map_table[id];
}

/* ---- API ------------------------------------------------ */

void world_init(const MapDef** map_table, u8 map_count) {
    u8 i;
    if (map_count > MAP_REGISTRY_MAX) map_count = MAP_REGISTRY_MAX;
    for (i = 0; i < map_count; i++)
        s_map_registry[i] = map_table[i];
    g_map_table    = s_map_registry;
    g_map_count    = map_count;
    s_active->def  = NULL;
    s_warp.phase   = WARP_IDLE;
    s_door.active  = FALSE;
    mem_zero(&s_area, sizeof(MapArea));
}

void world_add_maps(const MapDef** extra, u8 count) {
    u8 i;
    for (i = 0; i < count && g_map_count < MAP_REGISTRY_MAX; i++)
        s_map_registry[g_map_count++] = extra[i];
}

/* Upload full 32x32 screenblock from map data.
 * Uses circular mapping: screenblock[ty%32][tx%32] for seamless scroll. */
void world_upload_full_map(u8 screenblock, const MapDef* map) {
    u16* sb = (u16*)(MEM_VRAM + ((u32)screenblock * 0x800));

    u32 ty, tx;
    for (ty = 0; ty < 32; ty++) {
        for (tx = 0; tx < 32; tx++) {
            u16 tile_id = 0;
            if (tx < map->width && ty < map->height) {
                tile_id = map->tile_data[ty * map->width + tx];
            }
            /* Circular index within screenblock */
            u32 sb_x = tx & 31;
            u32 sb_y = ty & 31;
            sb[sb_y * 32 + sb_x] = SE_TID(tile_id);
        }
    }
}

void world_load_map(u8 map_id, u8 spawn_id) {
    const MapDef* map = _get_map(map_id);
    if (!map) return;

    s_active->def            = map;
    s_active->cam_tile_x     = -1;  /* force full reload on first update */
    s_active->cam_tile_y     = -1;
    s_active->needs_full_reload = TRUE;

    /* Upload BG2 (world layer), screenblock 29 */
    world_upload_full_map(29, map);

    /* Load tileset palette */
    /* (tileset loading is done by caller / state_overworld) */

    (void)spawn_id;
}

const MapDef* world_current_map(void) {
    return s_active->def;
}

MapArea* world_current_area(void) {
    return s_active;
}

/* Streaming: update one column or one row in the screenblock
 * when the camera has moved to reveal new tiles. */
void world_update_streaming(void) {
    const MapDef* map = s_active->def;
    if (!map) return;

    s32 cam_px = camera_x();
    s32 cam_py = camera_y();

    /* Current top-left tile of the camera */
    s16 new_tx = (s16)(cam_px >> 3);
    s16 new_ty = (s16)(cam_py >> 3);

    if (s_active->needs_full_reload) {
        world_upload_full_map(29, map);
        s_active->cam_tile_x    = new_tx;
        s_active->cam_tile_y    = new_ty;
        s_active->needs_full_reload = FALSE;
        return;
    }

    s16 old_tx = s_active->cam_tile_x;
    s16 old_ty = s_active->cam_tile_y;

    if (new_tx == old_tx && new_ty == old_ty) return;

    u16* sb = (u16*)(MEM_VRAM + (29 * 0x800));

    /* Refill columns that came into view on the right */
    s16 dx = new_tx - old_tx;
    if (dx > 0) {
        /* New rightmost visible column: new_tx + 30 (screen + buffer) */
        s16 fill_tx = new_tx + 30;
        u32 sb_x    = (u32)fill_tx & 31;
        u32 ty;
        for (ty = 0; ty < 32; ty++) {
            u32  world_ty = (u32)((new_ty + (s16)ty) & 0x3F);
            u16  tile_id  = 0;
            if (fill_tx >= 0 && fill_tx < map->width &&
                world_ty < map->height) {
                tile_id = map->tile_data[world_ty * map->width + fill_tx];
            }
            sb[ty * 32 + sb_x] = SE_TID(tile_id);
        }
    } else if (dx < 0) {
        /* New leftmost visible column */
        s16 fill_tx = new_tx - 1;
        u32 sb_x    = (u32)(fill_tx & 31);
        u32 ty;
        for (ty = 0; ty < 32; ty++) {
            u32  world_ty = (u32)((new_ty + (s16)ty) & 0x3F);
            u16  tile_id  = 0;
            if (fill_tx >= 0 && fill_tx < map->width &&
                world_ty < map->height) {
                tile_id = map->tile_data[world_ty * map->width + fill_tx];
            }
            sb[ty * 32 + sb_x] = SE_TID(tile_id);
        }
    }

    /* Refill rows that came into view on the bottom */
    s16 dy = new_ty - old_ty;
    if (dy > 0) {
        s16 fill_ty = new_ty + 21;  /* screen height in tiles + buffer */
        u32 sb_y    = (u32)fill_ty & 31;
        u32 tx;
        for (tx = 0; tx < 32; tx++) {
            u32  world_tx = (u32)((new_tx + (s16)tx) & 0x3F);
            u16  tile_id  = 0;
            if (fill_ty >= 0 && fill_ty < map->height &&
                world_tx < map->width) {
                tile_id = map->tile_data[(u32)fill_ty * map->width + world_tx];
            }
            sb[sb_y * 32 + tx] = SE_TID(tile_id);
        }
    } else if (dy < 0) {
        s16 fill_ty = new_ty - 1;
        u32 sb_y    = (u32)(fill_ty & 31);
        u32 tx;
        for (tx = 0; tx < 32; tx++) {
            u32  world_tx = (u32)((new_tx + (s16)tx) & 0x3F);
            u16  tile_id  = 0;
            if (fill_ty >= 0 && fill_ty < map->height &&
                world_tx < map->width) {
                tile_id = map->tile_data[(u32)fill_ty * map->width + world_tx];
            }
            sb[sb_y * 32 + tx] = SE_TID(tile_id);
        }
    }

    s_active->cam_tile_x = new_tx;
    s_active->cam_tile_y = new_ty;
}

u16 world_get_tile(s16 tx, s16 ty) {
    const MapDef* map = s_active->def;
    if (!map || tx < 0 || ty < 0 ||
        tx >= map->width || ty >= map->height) return 0;
    return map->tile_data[(u32)ty * map->width + (u32)tx];
}

u8 world_get_flags(s16 tx, s16 ty) {
    const MapDef* map = s_active->def;
    if (!map || !map->flag_data ||
        tx < 0 || ty < 0 ||
        tx >= map->width || ty >= map->height) return 0;
    return map->flag_data[(u32)ty * map->width + (u32)tx];
}

BOOL world_tile_solid(s16 tx, s16 ty) {
    return (world_get_flags(tx, ty) & TF_SOLID) != 0;
}

const WarpEntry* world_get_warp(s16 tx, s16 ty) {
    const MapDef* map = s_active->def;
    if (!map || !map->warps) return NULL;
    u8 i;
    for (i = 0; i < map->warp_count; i++) {
        if (map->warps[i].src_x == (u8)tx &&
            map->warps[i].src_y == (u8)ty) {
            return &map->warps[i];
        }
    }
    return NULL;
}

MapObject* world_get_object(s16 tx, s16 ty) {
    const MapDef* map = s_active->def;
    if (!map || !map->objects) return NULL;
    u8 i;
    for (i = 0; i < map->object_count; i++) {
        /* Cast away const — objects have runtime BOOL opened */
        MapObject* obj = (MapObject*)&map->objects[i];
        if (obj->x == (u8)tx && obj->y == (u8)ty) return obj;
    }
    return NULL;
}

SpawnPoint world_get_spawn(const MapDef* map, u8 spawn_id) {
    SpawnPoint sp = {0, 0, NPC_DIR_DOWN};
    if (!map || !map->spawns || spawn_id >= map->spawn_count) return sp;
    return map->spawns[spawn_id];
}

/* ---- Warp system ---------------------------------------- */

void world_start_warp(const WarpEntry* warp) {
    if (s_warp.phase != WARP_IDLE) return;
    s_warp.phase      = WARP_FADE_OUT;
    s_warp.timer      = 0;
    s_warp.dest_map   = warp->dest_map;
    s_warp.dest_spawn = warp->dest_spawn;
    s_warp.type       = warp->type;

    /* Open door animation for WARP_DOOR */
    if (warp->type == WARP_DOOR) {
        world_door_open((s16)warp->src_x, (s16)warp->src_y);
    }

    /* Configure hardware blend for fade */
    REG_BLDCNT_W = BLD_BLACK_W | BLD_ALL;
    REG_BLDY_W   = 0;
}

void world_warp_update(void) {
    if (s_warp.phase == WARP_IDLE) return;

    s_warp.timer++;

    switch (s_warp.phase) {
    case WARP_FADE_OUT:
        /* Increase darkness */
        REG_BLDY_W = (s_warp.timer * 16) / FADE_FRAMES;
        if (s_warp.timer >= FADE_FRAMES) {
            REG_BLDY_W   = 16;
            s_warp.phase = WARP_LOAD;
            s_warp.timer = 0;
        }
        break;

    case WARP_LOAD:
        /* One frame: load the new map (deferred to next frame) */
        event_push(EVT_WARP, s_warp.dest_map, s_warp.dest_spawn);
        s_warp.phase = WARP_FADE_IN;
        s_warp.timer = 0;
        break;

    case WARP_FADE_IN:
        /* Decrease darkness */
        REG_BLDY_W = 16 - (s_warp.timer * 16) / FADE_FRAMES;
        if (s_warp.timer >= FADE_FRAMES) {
            REG_BLDY_W   = 0;
            REG_BLDCNT_W = 0;
            s_warp.phase = WARP_IDLE;
        }
        break;

    default:
        break;
    }
}

BOOL world_is_warping(void) {
    return s_warp.phase != WARP_IDLE;
}

/* ---- Door animation ------------------------------------- */
#define DOOR_ANIM_FRAMES  8

/* Default open/close tile IDs (override per tileset) */
#define DOOR_TILE_OPEN    0    /* transparent / passable */
#define DOOR_TILE_CLOSE   1    /* door frame tile */

void world_door_open(s16 tx, s16 ty) {
    s_door.tile_x    = (u8)tx;
    s_door.tile_y    = (u8)ty;
    s_door.timer     = 0;
    s_door.opening   = TRUE;
    s_door.active    = TRUE;
    s_door.screenblock = 29;
    s_door.open_tile  = DOOR_TILE_OPEN;
    s_door.close_tile = DOOR_TILE_CLOSE;
}

void world_door_close(s16 tx, s16 ty) {
    s_door.tile_x    = (u8)tx;
    s_door.tile_y    = (u8)ty;
    s_door.timer     = 0;
    s_door.opening   = FALSE;
    s_door.active    = TRUE;
}

/* Call each frame (handles door tile swap animation) */
static void _door_update(void) {
    if (!s_door.active) return;
    s_door.timer++;
    if (s_door.timer >= DOOR_ANIM_FRAMES) {
        /* Commit tile change to screenblock */
        u16* sb = (u16*)(MEM_VRAM + ((u32)s_door.screenblock * 0x800));
        u32 sb_x = s_door.tile_x & 31;
        u32 sb_y = s_door.tile_y & 31;
        u16 tid  = s_door.opening ? s_door.open_tile : s_door.close_tile;
        sb[sb_y * 32 + sb_x] = SE_TID(tid);
        s_door.active = FALSE;
    }
}

/* Called from overworld update */
void world_tick(void) {
    world_warp_update();
    _door_update();
    world_update_streaming();
}
