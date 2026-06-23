#ifndef ENGINE_COLLISION_H
#define ENGINE_COLLISION_H

#include "../gba/gba_types.h"
#include "tile.h"

/* =========================================================
 * Collision System
 *
 * Two-phase collision:
 *   1. Tile collision: query tile flags at 4 corners of
 *      an AABB to determine if movement is blocked.
 *   2. Entity AABB: dynamic vs dynamic rectangles.
 *
 * Coordinates are in pixels (integer, not fixed-point),
 * since the GBA map is tile-aligned and 8px resolution
 * is sufficient for RPG movement.
 * ========================================================= */

/* Axis-Aligned Bounding Box */
typedef struct {
    s16 x, y;     /* top-left corner */
    s16 w, h;     /* dimensions */
} AABB;

/* Result of a move attempt */
typedef struct {
    s16  dx;          /* actual displacement applied */
    s16  dy;
    BOOL hit_x;       /* blocked on X axis */
    BOOL hit_y;       /* blocked on Y axis */
    u8   tile_flags;  /* flags of the blocking tile(s) */
} MoveResult;

/* ---- Tile-based collision -------------------------------- */

/* Set the active tilemap for collision queries */
void col_set_map(const Tilemap* map);

/* Check if a world-space rectangle overlaps any solid tile */
BOOL col_rect_solid(s16 x, s16 y, s16 w, s16 h);

/* Attempt to move AABB by (dx,dy); returns actual motion */
MoveResult col_move(AABB* box, s16 dx, s16 dy);

/* Return tile flags at a world pixel position */
u8 col_flags_at(s16 px, s16 py);

/* ---- AABB vs AABB ---------------------------------------- */

/* Simple overlap test */
BOOL col_aabb_overlap(const AABB* a, const AABB* b);

/* Minimum translation vector to separate overlapping boxes */
typedef struct { s16 x; s16 y; } Vec2s;
Vec2s col_aabb_mtv(const AABB* a, const AABB* b);

/* ---- Point tests ----------------------------------------- */
BOOL col_point_in_rect(s16 px, s16 py, const AABB* r);

/* ---- Trigger zones --------------------------------------- */
#define TRIGGER_MAX 32

typedef struct {
    AABB box;
    u16  event_id;    /* dispatched to event system */
    BOOL once;        /* fire only first time */
    BOOL fired;
    BOOL active;
} Trigger;

void col_trigger_register(const Trigger* t);
void col_trigger_clear(void);

/* Call once per frame with player AABB */
void col_trigger_check(const AABB* player);

#endif /* ENGINE_COLLISION_H */
