#include "../../include/game/npc.h"
#include "../../include/game/world.h"
#include "../../include/engine/engine.h"

/* =========================================================
 * NPC System Implementation
 * ========================================================= */

static Npc   s_npcs[MAP_MAX_NPCS];
static u8    s_npc_count = 0;

/* Pseudo-random for wander AI (LCG, seeded per NPC id) */
static u32 s_rng_state = 0x12345678;
static u8 _rand8(void) {
    s_rng_state = s_rng_state * 1664525u + 1013904223u;
    return (u8)(s_rng_state >> 24);
}

/* Direction deltas: DOWN, LEFT, RIGHT, UP */
static const s8 DIR_DX[4] = {  0, -1,  1,  0 };
static const s8 DIR_DY[4] = {  1,  0,  0, -1 };

/* Opposite direction lookup */
static const u8 DIR_OPP[4] = { NPC_DIR_UP, NPC_DIR_RIGHT,
                                NPC_DIR_LEFT, NPC_DIR_DOWN };

/* ---- Helpers -------------------------------------------- */

static void _start_walk(Npc* npc, u8 dir) {
    npc->dir       = dir;
    npc->ai_state  = NPC_STATE_WALKING;
    npc->walk_timer = 0;
    npc->target_x  = npc->world_x + DIR_DX[dir] * 8;
    npc->target_y  = npc->world_y + DIR_DY[dir] * 8;
    npc->move_dx   = DIR_DX[dir];
    npc->move_dy   = DIR_DY[dir];
}

static BOOL _tile_walkable(u8 tx, u8 ty) {
    if (world_tile_solid((s16)tx, (s16)ty)) return FALSE;
    /* Also check NPC blocking */
    if (npc_at_tile(tx, ty)) return FALSE;
    return TRUE;
}

static void _update_sprite_pos(Npc* npc) {
    s16 sx = (s16)(npc->world_x - camera_x());
    s16 sy = (s16)(npc->world_y - camera_y());
    sprite_set_pos(npc->spr, sx, sy);
}

static void _update_sprite_anim(Npc* npc) {
    u16 base = NPC_SPRITE_BASE(npc->tmpl->sprite_id);
    u16 tile = NPC_SPRITE_TILE(base, npc->dir, npc->anim_frame);
    sprite_set_gfx(npc->spr, tile, 1, SPR_SIZE_16x16);
}

/* ---- Public API ----------------------------------------- */

void npc_system_init(void) {
    npc_unload_all();
}

void npc_load_map(const MapDef* map) {
    npc_unload_all();
    if (!map || !map->npc_templates) return;

    u8 i;
    for (i = 0; i < map->npc_count && i < MAP_MAX_NPCS; i++) {
        const NpcTemplate* t = &map->npc_templates[i];
        Npc* npc = &s_npcs[i];

        mem_zero(npc, sizeof(Npc));
        npc->tmpl       = t;
        npc->id         = i;
        npc->active     = TRUE;
        npc->tile_x     = t->x;
        npc->tile_y     = t->y;
        npc->home_tx    = t->x;
        npc->home_ty    = t->y;
        npc->world_x    = (s16)(t->x * 8);
        npc->world_y    = (s16)(t->y * 8);
        npc->dir        = t->dir;
        npc->ai_state   = NPC_STATE_IDLE;
        npc->script_id  = t->script_id;
        npc->wander_timer = 60 + (_rand8() & 0x3F);
        npc->look_timer   = 30 + (_rand8() & 0x3F);

        npc->spr = sprite_alloc();
        _update_sprite_anim(npc);
        _update_sprite_pos(npc);
    }
    s_npc_count = map->npc_count < MAP_MAX_NPCS ?
                  map->npc_count : MAP_MAX_NPCS;
}

void npc_unload_all(void) {
    u8 i;
    for (i = 0; i < MAP_MAX_NPCS; i++) {
        if (s_npcs[i].active && s_npcs[i].spr != INVALID_SPRITE) {
            sprite_free(s_npcs[i].spr);
        }
        s_npcs[i].active = FALSE;
    }
    s_npc_count = 0;
}

void npc_update_all(s16 player_tx, s16 player_ty,
                    u8 player_dir, BOOL player_moving) {
    u8 i;
    (void)player_moving;

    for (i = 0; i < s_npc_count; i++) {
        Npc* npc = &s_npcs[i];
        if (!npc->active) continue;

        const NpcTemplate* t = npc->tmpl;

        /* ---- Walking animation ---- */
        if (npc->ai_state == NPC_STATE_WALKING ||
            npc->ai_state == NPC_STATE_TRAINER_WALK) {
            npc->walk_timer++;
            npc->world_x += npc->move_dx;
            npc->world_y += npc->move_dy;

            npc->anim_timer++;
            if (npc->anim_timer >= NPC_ANIM_SPEED) {
                npc->anim_timer = 0;
                npc->anim_frame ^= 1;
            }

            if (npc->walk_timer >= NPC_WALK_FRAMES) {
                /* Snap to target */
                npc->world_x    = npc->target_x;
                npc->world_y    = npc->target_y;
                npc->tile_x     = (u8)(npc->world_x >> 3);
                npc->tile_y     = (u8)(npc->world_y >> 3);
                npc->walk_timer = 0;
                npc->anim_frame = 0;

                if (npc->ai_state == NPC_STATE_TRAINER_WALK) {
                    /* Check if we've reached the player */
                    s16 dtx = (s16)npc->tile_x - player_tx;
                    s16 dty = (s16)npc->tile_y - player_ty;
                    s16 dist = (dtx < 0 ? -dtx : dtx) +
                               (dty < 0 ? -dty : dty);
                    if (dist <= 1) {
                        npc->ai_state = NPC_STATE_TRIGGERED;
                        npc_face_player(npc, player_tx, player_ty);
                        npc->script_queued = TRUE;
                    } else {
                        /* Keep walking toward player */
                        u8 d;
                        if (dtx < 0)      d = NPC_DIR_RIGHT;
                        else if (dtx > 0) d = NPC_DIR_LEFT;
                        else if (dty < 0) d = NPC_DIR_DOWN;
                        else              d = NPC_DIR_UP;
                        _start_walk(npc, d);
                    }
                } else {
                    npc->ai_state = NPC_STATE_IDLE;
                }
            }
            _update_sprite_anim(npc);
            continue;
        }

        /* ---- Frozen/triggered: no AI ---- */
        if (npc->ai_state == NPC_STATE_FROZEN ||
            npc->ai_state == NPC_STATE_TRIGGERED) continue;

        /* ---- AI by movement type ---- */
        switch (t->move_type) {

        case NPC_MOVE_NONE:
            break;

        case NPC_MOVE_LOOK: {
            npc->look_timer++;
            if (npc->look_timer >= 90) {
                npc->look_timer = 0;
                /* Cycle through directions */
                static const u8 look_dirs[4] = {
                    NPC_DIR_DOWN, NPC_DIR_LEFT,
                    NPC_DIR_RIGHT, NPC_DIR_UP
                };
                npc->look_seq = (npc->look_seq + 1) & 3;
                npc->dir = look_dirs[npc->look_seq];
                _update_sprite_anim(npc);
            }
            break;
        }

        case NPC_MOVE_WANDER: {
            npc->wander_timer--;
            if (npc->wander_timer == 0) {
                npc->wander_timer = 60 + (_rand8() & 0x7F);

                /* Pick a random direction within home range */
                u8 tries = 4;
                while (tries--) {
                    u8 d = _rand8() & 3;
                    s16 ntx = (s16)npc->tile_x + DIR_DX[d];
                    s16 nty = (s16)npc->tile_y + DIR_DY[d];
                    /* Stay within range of home */
                    s16 rhx = (s16)ntx - npc->home_tx;
                    s16 rhy = (s16)nty - npc->home_ty;
                    s16 dist = (rhx < 0 ? -rhx : rhx) +
                               (rhy < 0 ? -rhy : rhy);
                    if (dist <= t->move_range &&
                        _tile_walkable((u8)ntx, (u8)nty)) {
                        _start_walk(npc, d);
                        break;
                    }
                }
            }
            break;
        }

        case NPC_MOVE_PATROL: {
            if (t->waypoint_count == 0) break;
            npc->wander_timer--;
            if (npc->wander_timer == 0) {
                npc->wander_timer = NPC_WALK_FRAMES + 30;
                /* Next waypoint */
                u8 wp = npc->patrol_idx;
                s8 dx = t->waypoints[wp][0];
                s8 dy = t->waypoints[wp][1];
                /* Determine direction from delta */
                u8 d;
                if      (dx < 0) d = NPC_DIR_LEFT;
                else if (dx > 0) d = NPC_DIR_RIGHT;
                else if (dy < 0) d = NPC_DIR_UP;
                else             d = NPC_DIR_DOWN;

                s16 ntx = (s16)npc->tile_x + dx;
                s16 nty = (s16)npc->tile_y + dy;
                if (_tile_walkable((u8)ntx, (u8)nty)) {
                    _start_walk(npc, d);
                }

                /* Advance waypoint index */
                npc->patrol_idx = (wp + 1) % t->waypoint_count;
            }
            break;
        }

        case NPC_MOVE_TRAINER: {
            if (npc->defeated) break;

            /* Check sight line in facing direction */
            u8 range = t->sight_range;
            u8 d     = npc->dir;
            u8 tx    = npc->tile_x;
            u8 ty    = npc->tile_y;
            u8 r;
            for (r = 1; r <= range; r++) {
                tx = (u8)((s16)npc->tile_x + DIR_DX[d] * r);
                ty = (u8)((s16)npc->tile_y + DIR_DY[d] * r);
                if (world_tile_solid((s16)tx, (s16)ty)) break;
                if (tx == (u8)player_tx && ty == (u8)player_ty) {
                    /* Player spotted! */
                    npc->ai_state = NPC_STATE_TRAINER_ALERT;
                    /* Play trainer alert (handled by event) */
                    event_push(EVT_SFX_PLAY, 5, 0);  /* sfx_id 5 = alert */
                    /* Start walking toward player */
                    _start_walk(npc, d);
                    npc->ai_state = NPC_STATE_TRAINER_WALK;
                    break;
                }
            }
            break;
        }
        }
    }
}

void npc_render_all(void) {
    u8 i;
    for (i = 0; i < s_npc_count; i++) {
        Npc* npc = &s_npcs[i];
        if (!npc->active) continue;

        /* Cull off-screen */
        if (!camera_is_visible(npc->world_x, npc->world_y, 16, 16)) {
            sprite_set_hidden(npc->spr, TRUE);
            continue;
        }
        sprite_set_hidden(npc->spr, FALSE);
        _update_sprite_pos(npc);
    }
}

Npc* npc_at_tile(u8 tx, u8 ty) {
    u8 i;
    for (i = 0; i < s_npc_count; i++) {
        Npc* npc = &s_npcs[i];
        if (!npc->active) continue;
        if (npc->tile_x == tx && npc->tile_y == ty) return npc;
    }
    return NULL;
}

Npc* npc_facing_player(u8 player_tx, u8 player_ty, u8 player_dir) {
    /* Tile directly in front of player */
    s16 front_tx = player_tx + DIR_DX[player_dir];
    s16 front_ty = player_ty + DIR_DY[player_dir];
    return npc_at_tile((u8)front_tx, (u8)front_ty);
}

void npc_freeze(u8 id) {
    if (id < s_npc_count) s_npcs[id].ai_state = NPC_STATE_FROZEN;
}

void npc_unfreeze(u8 id) {
    if (id < s_npc_count && s_npcs[id].ai_state == NPC_STATE_FROZEN)
        s_npcs[id].ai_state = NPC_STATE_IDLE;
}

void npc_freeze_all(void) {
    u8 i;
    for (i = 0; i < s_npc_count; i++) npc_freeze(i);
}

void npc_unfreeze_all(void) {
    u8 i;
    for (i = 0; i < s_npc_count; i++) npc_unfreeze(i);
}

void npc_face_player(Npc* npc, u8 player_tx, u8 player_ty) {
    s16 dx = (s16)player_tx - npc->tile_x;
    s16 dy = (s16)player_ty - npc->tile_y;
    u8 d;
    if      (dx < 0) d = NPC_DIR_LEFT;
    else if (dx > 0) d = NPC_DIR_RIGHT;
    else if (dy < 0) d = NPC_DIR_UP;
    else             d = NPC_DIR_DOWN;
    npc->dir = d;
    _update_sprite_anim(npc);
}
