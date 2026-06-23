#include "../../include/engine/collision.h"
#include "../../include/engine/event.h"

/* =========================================================
 * Collision System Implementation
 * ========================================================= */

static const Tilemap* s_map = NULL;

/* Trigger registry */
static Trigger s_triggers[TRIGGER_MAX];
static u32     s_trigger_count = 0;

/* ---- Tile collision -------------------------------------- */

void col_set_map(const Tilemap* map) {
    s_map = map;
}

u8 col_flags_at(s16 px, s16 py) {
    if (!s_map) return 0;
    u8 col = (u8)(px >> 3);   /* divide by tile size (8) */
    u8 row = (u8)(py >> 3);
    return tile_get_flags(s_map, col, row);
}

BOOL col_rect_solid(s16 x, s16 y, s16 w, s16 h) {
    /* Test 4 corners (sufficient for tile-aligned movement) */
    if (col_flags_at(x,         y        ) & TILE_FLAG_SOLID) return TRUE;
    if (col_flags_at(x + w - 1, y        ) & TILE_FLAG_SOLID) return TRUE;
    if (col_flags_at(x,         y + h - 1) & TILE_FLAG_SOLID) return TRUE;
    if (col_flags_at(x + w - 1, y + h - 1) & TILE_FLAG_SOLID) return TRUE;
    return FALSE;
}

MoveResult col_move(AABB* box, s16 dx, s16 dy) {
    MoveResult r;
    r.dx         = 0;
    r.dy         = 0;
    r.hit_x      = FALSE;
    r.hit_y      = FALSE;
    r.tile_flags = 0;

    /* Move X */
    if (dx != 0) {
        s16 nx = box->x + dx;
        if (!col_rect_solid(nx, box->y, box->w, box->h)) {
            box->x = nx;
            r.dx   = dx;
        } else {
            r.hit_x      = TRUE;
            r.tile_flags |= col_flags_at(nx, box->y);
        }
    }

    /* Move Y */
    if (dy != 0) {
        s16 ny = box->y + dy;
        if (!col_rect_solid(box->x, ny, box->w, box->h)) {
            box->y = ny;
            r.dy   = dy;
        } else {
            r.hit_y      = TRUE;
            r.tile_flags |= col_flags_at(box->x, ny);
        }
    }

    return r;
}

/* ---- AABB vs AABB ---------------------------------------- */

BOOL col_aabb_overlap(const AABB* a, const AABB* b) {
    return (a->x < b->x + b->w) && (a->x + a->w > b->x) &&
           (a->y < b->y + b->h) && (a->y + a->h > b->y);
}

Vec2s col_aabb_mtv(const AABB* a, const AABB* b) {
    Vec2s mtv = {0, 0};
    if (!col_aabb_overlap(a, b)) return mtv;

    s16 ox = (a->x + a->w / 2) - (b->x + b->w / 2);
    s16 oy = (a->y + a->h / 2) - (b->y + b->h / 2);
    s16 hw = (a->w + b->w) / 2;
    s16 hh = (a->h + b->h) / 2;

    s16 px = hw - (ox < 0 ? -ox : ox);
    s16 py = hh - (oy < 0 ? -oy : oy);

    if (px < py) {
        mtv.x = (ox < 0) ? -px : px;
    } else {
        mtv.y = (oy < 0) ? -py : py;
    }
    return mtv;
}

BOOL col_point_in_rect(s16 px, s16 py, const AABB* r) {
    return px >= r->x && px < r->x + r->w &&
           py >= r->y && py < r->y + r->h;
}

/* ---- Trigger zones --------------------------------------- */

void col_trigger_register(const Trigger* t) {
    if (s_trigger_count >= TRIGGER_MAX) return;
    s_triggers[s_trigger_count++] = *t;
}

void col_trigger_clear(void) {
    s_trigger_count = 0;
}

void col_trigger_check(const AABB* player) {
    u32 i;
    for (i = 0; i < s_trigger_count; i++) {
        Trigger* t = &s_triggers[i];
        if (!t->active) continue;
        if (t->once && t->fired) continue;

        if (col_aabb_overlap(player, &t->box)) {
            t->fired = TRUE;
            event_push(EVT_TRIGGER, t->event_id, 0);
            if (t->once) t->active = FALSE;
        }
    }
}
