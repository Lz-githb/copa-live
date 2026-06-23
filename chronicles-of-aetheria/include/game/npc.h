#ifndef GAME_NPC_H
#define GAME_NPC_H

#include "../engine/engine.h"
#include "world.h"

/* =========================================================
 * NPC System
 *
 * Supports up to MAP_MAX_NPCS (16) NPCs per loaded map.
 * Each NPC has:
 *   - A sprite handle from the sprite system
 *   - A pixel position (world coords)
 *   - An AABB for collision with player
 *   - An AI state machine
 *
 * NPC movement is tile-snapped (moves in 8-px steps,
 * animates over WALK_FRAMES frames per tile).
 *
 * Trainer sight: when facing an NPC of type NPC_MOVE_TRAINER,
 * if the player enters its sight cone (straight line ahead,
 * up to sight_range tiles), the NPC walks to the player and
 * triggers a battle event.
 * ========================================================= */

#define NPC_WALK_FRAMES  8     /* frames per tile movement */
#define NPC_ANIM_FRAMES  2     /* sprite animation frames */
#define NPC_ANIM_SPEED   4     /* game frames per anim step */

/* NPC facing directions (same as player DIR_*) */
#define NPC_DIR_DOWN  0
#define NPC_DIR_LEFT  1
#define NPC_DIR_RIGHT 2
#define NPC_DIR_UP    3

/* NPC AI states */
typedef enum {
    NPC_STATE_IDLE = 0,
    NPC_STATE_WALKING,
    NPC_STATE_TURNING,
    NPC_STATE_PATROLLING,
    NPC_STATE_WANDER_WAIT,
    NPC_STATE_TRAINER_ALERT,  /* just spotted player */
    NPC_STATE_TRAINER_WALK,   /* walking to player */
    NPC_STATE_TRIGGERED,      /* script running */
    NPC_STATE_FROZEN,         /* during dialogue */
} NpcAIState;

typedef struct {
    /* Template reference */
    const NpcTemplate* tmpl;
    u8                 id;
    BOOL               active;

    /* World position (pixels) */
    s16 world_x;
    s16 world_y;

    /* Tile home position (for wander range) */
    u8  home_tx;
    u8  home_ty;

    /* Current tile position */
    u8  tile_x;
    u8  tile_y;

    /* Facing direction */
    u8  dir;

    /* Sprite */
    SpriteHandle spr;

    /* Movement animation */
    NpcAIState  ai_state;
    u8          walk_timer;    /* counts up to NPC_WALK_FRAMES */
    s16         target_x;     /* pixel target for current move */
    s16         target_y;
    s16         move_dx;      /* pixel delta per frame */
    s16         move_dy;
    u8          anim_frame;
    u8          anim_timer;

    /* Patrol */
    u8  patrol_idx;           /* current waypoint index */
    u8  patrol_dir;           /* 1=forward, 0=backward */

    /* Wander */
    u32 wander_timer;         /* frames until next wander step */

    /* Look timer */
    u32 look_timer;
    u8  look_seq;

    /* Trainer */
    BOOL defeated;            /* don't re-trigger after battle */

    /* Script */
    u16  script_id;
    BOOL script_queued;
} Npc;

/* ---- NPC System API ------------------------------------- */

void npc_system_init(void);

/* Load NPCs from a map definition */
void npc_load_map(const MapDef* map);

/* Unload all NPCs (call before loading new map) */
void npc_unload_all(void);

/* Update all NPCs (call each frame) */
void npc_update_all(s16 player_tx, s16 player_ty,
                    u8 player_dir, BOOL player_moving);

/* Apply camera offset to sprite positions */
void npc_render_all(void);

/* Get NPC blocking the given tile (for collision) */
Npc* npc_at_tile(u8 tx, u8 ty);

/* Get NPC adjacent to player in facing direction (for A press) */
Npc* npc_facing_player(u8 player_tx, u8 player_ty, u8 player_dir);

/* Freeze/unfreeze NPC (during dialogue) */
void npc_freeze(u8 id);
void npc_unfreeze(u8 id);
void npc_freeze_all(void);
void npc_unfreeze_all(void);

/* Face NPC toward player */
void npc_face_player(Npc* npc, u8 player_tx, u8 player_ty);

/* Tile sprite sheet layout for NPC sprites:
 * Base tile + dir*4 + frame*2 = tile index */
#define NPC_SPRITE_BASE(sprite_id)  ((sprite_id) * 16)
#define NPC_SPRITE_TILE(base, dir, frame) \
    ((base) + (dir) * 4 + (frame) * 2)

#endif /* GAME_NPC_H */
