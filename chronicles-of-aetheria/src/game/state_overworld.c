#include "../../include/game/game_states.h"
#include "../../include/game/world.h"
#include "../../include/game/npc.h"
#include "../../include/game/interaction.h"
#include "../../include/game/time_weather.h"

/* =========================================================
 * Overworld State — Full Exploration System
 *
 * Player movement: pixel-by-pixel (2px/frame) with tile snap
 * Interaction: A button triggers 4-priority check
 * Camera: smooth follow with world-edge clamping
 *
 * Pokémon-style: player moves freely but NPCs walk on grid.
 * Zelda-style: smooth camera, interaction triggers.
 * Golden Sun-style: smooth transitions, weather effects.
 * ========================================================= */

/* ---- Forward declarations (defined after Player struct) -- */
static void _update_hitbox_player(void);
static void _handle_warp(const Event* evt);
static void _on_battle_start(const Event* evt);
static BOOL _can_move_to(s32 nx, s32 ny);

/* ---- Player constants ----------------------------------- */
#define PLAYER_SPEED      2     /* pixels per frame */
#define PLAYER_RUN_SPEED  4     /* when holding B (run) */
#define PLAYER_W          16
#define PLAYER_H          16
#define PLAYER_HBOX_W     10    /* hitbox narrower than sprite */
#define PLAYER_HBOX_H      6    /* hitbox at feet */
#define PLAYER_HBOX_OX     3    /* hitbox offset from sprite top-left */
#define PLAYER_HBOX_OY    10

/* Animation */
#define PLAYER_ANIM_FRAMES     2
#define PLAYER_ANIM_SPEED_WALK 8
#define PLAYER_ANIM_SPEED_RUN  4

/* ---- Player direction ----------------------------------- */
#define DIR_DOWN  0
#define DIR_LEFT  1
#define DIR_RIGHT 2
#define DIR_UP    3

static const s8 DIR_DX[4] = {  0, -1,  1,  0 };
static const s8 DIR_DY[4] = {  1,  0,  0, -1 };

/* ---- Player movement state machine --------------------- */
typedef enum {
    PMOVE_IDLE = 0,
    PMOVE_WALK,
    PMOVE_RUN,
    PMOVE_STEP_ON_WARP,   /* triggered step animation */
    PMOVE_FROZEN,         /* dialogue / cutscene */
} PlayerMoveState;

/* ---- Player struct -------------------------------------- */
typedef struct {
    s32           world_x;       /* top-left of sprite in world pixels */
    s32           world_y;
    AABB          hitbox;        /* feet-level AABB for tile collision */
    u8            dir;
    u8            anim_frame;
    u8            anim_timer;
    PlayerMoveState move_state;
    SpriteHandle  spr;
    BOOL          running;
    /* Ledge jump */
    u8            ledge_jump_timer;
    /* Step-on-warp cooldown (don't re-trigger immediately after warp) */
    u8            warp_cooldown;
    /* Tile position (for NPC/interaction queries) */
    u8            tile_x;
    u8            tile_y;
} Player;

static Player s_player;

/* ---- Tile sprite IDs ------------------------------------ */
/* Player sprite tile layout in OBJ VRAM (all 4bpp, 16x16):
 * Each direction uses 4 tiles: [dir * 4 + frame * 2 = base tile]
 * DOWN:  tiles 0,2   LEFT: 4,6   RIGHT: 8,10   UP: 12,14 */
#define PLAYER_TILE_BASE(dir, frame) ((dir) * 4 + (frame) * 2)

/* Palette 0 = player palette */
#define PLAYER_PALETTE 0

/* ---- Step-on-grass (tall grass) ------------------------ */
#define GRASS_ENCOUNTER_RATE  12  /* out of 255 per step on tall grass */
static u8 s_encounter_step = 0;

/* ---- Warp event handler --------------------------------- */
static void _handle_warp(const Event* evt) {
    u8 map_id   = (u8)evt->param0;
    u8 spawn_id = (u8)evt->param1;

    world_load_map(map_id, spawn_id);
    npc_load_map(world_current_map());

    /* Place player at spawn */
    const MapDef* map = world_current_map();
    if (map) {
        SpawnPoint sp = world_get_spawn(map, spawn_id);
        s_player.world_x = (s32)sp.x * 8;
        s_player.world_y = (s32)sp.y * 8;
        s_player.dir     = sp.dir;
        s_player.tile_x  = sp.x;
        s_player.tile_y  = sp.y;
        _update_hitbox_player();

        /* Re-init camera for new map */
        s32 world_w = (s32)map->width  * 8;
        s32 world_h = (s32)map->height * 8;
        camera_init(world_w, world_h);
        camera_set_pos(s_player.world_x - SCREEN_W/2 + PLAYER_W/2,
                       s_player.world_y - SCREEN_H/2 + PLAYER_H/2);

        /* Weather: indoor maps override weather to clear */
        if (!map->outdoor) weather_set_instant(WEATHER_CLEAR);
    }

    s_player.warp_cooldown = 30;
}

/* ---- Hitbox helper -------------------------------------- */
static void _update_hitbox_player(void) {
    s_player.hitbox.x = (s16)(s_player.world_x + PLAYER_HBOX_OX);
    s_player.hitbox.y = (s16)(s_player.world_y + PLAYER_HBOX_OY);
    s_player.hitbox.w = PLAYER_HBOX_W;
    s_player.hitbox.h = PLAYER_HBOX_H;
}

/* ---- Check solid: tiles + NPCs -------------------------- */
static BOOL _can_move_to(s32 nx, s32 ny) {
    /* Compute hitbox at new position */
    s16 hx = (s16)(nx + PLAYER_HBOX_OX);
    s16 hy = (s16)(ny + PLAYER_HBOX_OY);

    /* Check 4 corners of hitbox against tile flags */
    s16 tx0 = (s16)(hx >> 3);
    s16 ty0 = (s16)(hy >> 3);
    s16 tx1 = (s16)((hx + PLAYER_HBOX_W - 1) >> 3);
    s16 ty1 = (s16)((hy + PLAYER_HBOX_H - 1) >> 3);

    if (world_tile_solid(tx0, ty0) || world_tile_solid(tx1, ty0) ||
        world_tile_solid(tx0, ty1) || world_tile_solid(tx1, ty1))
        return FALSE;

    /* Check NPC blocking */
    if (npc_at_tile((u8)tx0, (u8)ty0) || npc_at_tile((u8)tx1, (u8)ty1))
        return FALSE;

    return TRUE;
}

/* ---- Check ledge (jump down) ---------------------------- */
static BOOL _check_ledge(u8 dir) {
    if (dir != DIR_DOWN) return FALSE;
    s16 tx = s_player.tile_x;
    s16 ty = s_player.tile_y;
    return (world_get_flags(tx, ty) & TF_LEDGE_D) != 0;
}

/* ---- Ice sliding ---------------------------------------- */
static BOOL _on_ice(void) {
    return (world_get_flags(s_player.tile_x, s_player.tile_y) & TF_ICE) != 0;
}

/* ---- Tall grass encounter check ------------------------- */
static void _check_random_encounter(void) {
    if (!(world_get_flags(s_player.tile_x, s_player.tile_y) & TF_TALL_GRASS))
        return;

    s_encounter_step++;
    /* Only check every 8 steps */
    if ((s_encounter_step & 7) != 0) return;

    /* Roll for encounter (simple hash of step counter) */
    u32 roll = (u32)s_encounter_step * 1103515245u + 12345u;
    if ((roll >> 24) < GRASS_ENCOUNTER_RATE) {
        event_push(EVT_BATTLE_START, 0 /* TODO: area encounter table */, 0);
    }
}

/* ---- Step-on-warp check --------------------------------- */
static void _check_step_warp(void) {
    if (s_player.warp_cooldown > 0) {
        s_player.warp_cooldown--;
        return;
    }
    const WarpEntry* w = world_get_warp((s16)s_player.tile_x,
                                        (s16)s_player.tile_y);
    if (w && !world_is_warping()) {
        world_start_warp(w);
    }
}

/* ---- Player movement ------------------------------------ */
static void _player_move(void) {
    if (s_player.move_state == PMOVE_FROZEN) return;

    /* Run modifier */
    s_player.running = key_held(KEY_B) && (s_player.move_state == PMOVE_WALK ||
                                            s_player.move_state == PMOVE_IDLE);
    u8 speed = s_player.running ? PLAYER_RUN_SPEED : PLAYER_SPEED;

    /* Read D-pad */
    s32 dx = 0, dy = 0;
    u8 dir = s_player.dir;
    BOOL moving = FALSE;

    if (key_held(KEY_LEFT))  { dx = -speed; dir = DIR_LEFT;  moving = TRUE; }
    if (key_held(KEY_RIGHT)) { dx =  speed; dir = DIR_RIGHT; moving = TRUE; }
    if (key_held(KEY_UP))    { dy = -speed; dir = DIR_UP;    moving = TRUE; }
    if (key_held(KEY_DOWN))  { dy =  speed; dir = DIR_DOWN;  moving = TRUE; }

    /* Diagonal: only one axis at a time (RPG-style, not top-right Zelda) */
    if (dx != 0 && dy != 0) { dy = 0; }

    /* Update direction even when blocked (facing changes) */
    if (moving && dir != s_player.dir) {
        s_player.dir = dir;
        s_player.move_state = PMOVE_IDLE;
        /* Turn-in-place: don't move this frame, just change direction */
        return;
    }

    if (!moving) {
        s_player.move_state = PMOVE_IDLE;
        s_player.anim_frame = 0;
        s_player.anim_timer = 0;
        return;
    }

    /* Ledge check (jump over ledge facing down) */
    if (_check_ledge(dir)) {
        /* Jump: bypass solid check */
        s32 nx = s_player.world_x + dx;
        s32 ny = s_player.world_y + dy * 2;  /* skip one tile */
        s_player.world_x = nx;
        s_player.world_y = ny;
        _update_hitbox_player();
        goto update_tile;
    }

    /* Ice: continue in last direction */
    if (_on_ice()) {
        s32 nx = s_player.world_x + DIR_DX[s_player.dir] * speed;
        s32 ny = s_player.world_y + DIR_DY[s_player.dir] * speed;
        if (_can_move_to(nx, ny)) {
            s_player.world_x = nx;
            s_player.world_y = ny;
        }
        _update_hitbox_player();
        goto update_tile;
    }

    /* Normal movement */
    {
        s32 nx = s_player.world_x + dx;
        s32 ny = s_player.world_y + dy;

        /* Try combined movement first */
        if (_can_move_to(nx, ny)) {
            s_player.world_x = nx;
            s_player.world_y = ny;
        } else {
            /* Try X only */
            if (dx && _can_move_to(s_player.world_x + dx, s_player.world_y)) {
                s_player.world_x += dx;
            }
            /* Try Y only */
            else if (dy && _can_move_to(s_player.world_x, s_player.world_y + dy)) {
                s_player.world_y += dy;
            }
        }
    }
    _update_hitbox_player();

update_tile:
    s_player.tile_x = (u8)((s_player.world_x + PLAYER_W/2) >> 3);
    s_player.tile_y = (u8)((s_player.world_y + PLAYER_H/2) >> 3);
    s_player.move_state = s_player.running ? PMOVE_RUN : PMOVE_WALK;

    /* Animate */
    u8 anim_speed = s_player.running ?
                    PLAYER_ANIM_SPEED_RUN : PLAYER_ANIM_SPEED_WALK;
    s_player.anim_timer++;
    if (s_player.anim_timer >= anim_speed) {
        s_player.anim_timer = 0;
        s_player.anim_frame = (s_player.anim_frame + 1) % PLAYER_ANIM_FRAMES;
    }
}

/* ---- State callbacks ------------------------------------ */

void state_overworld_enter(void) {
    REG_DISPCNT = DCNT_MODE0      |
                  DCNT_BG0        |
                  DCNT_BG1        |
                  DCNT_BG2        |
                  DCNT_BG3        |
                  DCNT_OBJ        |
                  DCNT_OBJ_MAP_1D;

    /* Layer setup:
     * BG0 (SB31, CB0) = HUD overlay, priority 0, no scroll
     * BG1 (SB30, CB0) = Foreground (roofs, tree tops), priority 1
     * BG2 (SB29, CB1) = World / ground tiles, priority 2
     * BG3 (SB28, CB1) = Background / sky, priority 3 */
    tile_bg_setup(BG_LAYER_UI,    0, 31, 0, 0, BG_SIZE_256x256);
    tile_bg_setup(BG_LAYER_FG,    0, 30, 1, 0, BG_SIZE_256x256);
    tile_bg_setup(BG_LAYER_WORLD, 1, 29, 2, 0, BG_SIZE_256x256);
    tile_bg_setup(BG_LAYER_BG,    1, 28, 3, 0, BG_SIZE_256x256);

    tile_bg_enable(BG_LAYER_UI,    TRUE);
    tile_bg_enable(BG_LAYER_FG,    TRUE);
    tile_bg_enable(BG_LAYER_WORLD, TRUE);
    tile_bg_enable(BG_LAYER_BG,    TRUE);

    /* Allocate player sprite */
    s_player.spr        = sprite_alloc();
    s_player.dir        = DIR_DOWN;
    s_player.anim_frame = 0;
    s_player.anim_timer = 0;
    s_player.move_state = PMOVE_IDLE;
    s_player.running    = FALSE;
    s_player.warp_cooldown = 30;

    /* Load the starting map if none loaded */
    const MapDef* map = world_current_map();
    if (!map) {
        /* Default: map 0, spawn 0 */
        world_load_map(0, 0);
        map = world_current_map();
    }

    if (map) {
        SpawnPoint sp = world_get_spawn(map, 0);
        s_player.world_x = (s32)sp.x * 8;
        s_player.world_y = (s32)sp.y * 8;
        s_player.dir     = sp.dir;
        s_player.tile_x  = sp.x;
        s_player.tile_y  = sp.y;
        _update_hitbox_player();

        s32 world_w = (s32)map->width  * 8;
        s32 world_h = (s32)map->height * 8;
        camera_init(world_w, world_h);
        camera_center_on(s_player.world_x + PLAYER_W/2,
                         s_player.world_y + PLAYER_H/2);
        camera_update();

        /* Load NPCs */
        npc_load_map(map);
    }

    /* Init time (7:00 AM, CLEAR) */
    time_weather_init(7, 0, WEATHER_CLEAR);

    /* Register event listeners */
    event_listen(EVT_WARP,        _handle_warp);
    event_listen(EVT_BATTLE_START, _on_battle_start);
}

static void _on_battle_start(const Event* evt) {
    (void)evt;
    /* Freeze everything before entering battle */
    s_player.move_state = PMOVE_FROZEN;
    npc_freeze_all();
    state_push(STATE_BATTLE);
}

void state_overworld_exit(void) {
    event_unlisten(EVT_WARP,        _handle_warp);
    event_unlisten(EVT_BATTLE_START, _on_battle_start);

    sprite_free(s_player.spr);
    npc_unload_all();

    tile_bg_enable(BG_LAYER_UI,    FALSE);
    tile_bg_enable(BG_LAYER_FG,    FALSE);
    tile_bg_enable(BG_LAYER_WORLD, FALSE);
    tile_bg_enable(BG_LAYER_BG,    FALSE);

    REG_BLDCNT = 0;
    REG_BLDY   = 0;
}

void state_overworld_update(void) {
    /* ---- Skip input if busy ---- */
    BOOL is_busy = interact_busy() || world_is_warping();

    if (!is_busy) {
        /* ---- Player movement ---- */
        _player_move();

        /* ---- A button: interaction ---- */
        if (key_pressed(KEY_A)) {
            interact_try(s_player.tile_x, s_player.tile_y, s_player.dir);
        }

        /* ---- Step-on effects ---- */
        if (s_player.move_state == PMOVE_WALK ||
            s_player.move_state == PMOVE_RUN) {
            _check_step_warp();
            _check_random_encounter();
        }

        /* ---- Menu ---- */
        if (key_pressed(KEY_START)) {
            s_player.move_state = PMOVE_FROZEN;
            state_push(STATE_MENU);
        }

        /* ---- Debug: advance time with SELECT+L/R ---- */
        if (key_held(KEY_SELECT)) {
            if (key_pressed(KEY_R)) time_advance_hours(1);
            if (key_pressed(KEY_L)) time_advance_hours(23); /* -1 hour */
        }
    }

    /* ---- Interaction update (runs always, even when busy) ---- */
    interact_update();

    /* ---- Unfreeze after battle return ---- */
    if (s_player.move_state == PMOVE_FROZEN && !is_busy &&
        state_current() == STATE_OVERWORLD) {
        s_player.move_state = PMOVE_IDLE;
        npc_unfreeze_all();
    }

    /* ---- Camera follow ---- */
    if (s_player.move_state != PMOVE_FROZEN) {
        camera_center_on(s_player.world_x + PLAYER_W/2,
                         s_player.world_y + PLAYER_H/2);
        camera_update();
    }

    /* ---- NPC update ---- */
    npc_update_all(s_player.tile_x, s_player.tile_y,
                   s_player.dir,
                   s_player.move_state != PMOVE_IDLE);

    /* ---- World tick (warp transition, door anim, streaming) ---- */
    world_tick();

    /* ---- Time & weather ---- */
    const MapDef* map = world_current_map();
    BOOL outdoor = map ? map->outdoor : FALSE;
    time_weather_update(outdoor);
}

void state_overworld_render(void) {
    /* Apply camera scroll to BG hardware registers */
    camera_apply_bg_scroll();

    /* Apply day/night brightness to hardware blend */
    time_weather_apply_hw();

    /* Player sprite: position and animation */
    {
        s16 sx = (s16)(s_player.world_x - camera_x());
        s16 sy = (s16)(s_player.world_y - camera_y());
        sprite_set_pos(s_player.spr, sx, sy);

        u16 tile = PLAYER_TILE_BASE(s_player.dir, s_player.anim_frame);
        sprite_set_gfx(s_player.spr, tile, PLAYER_PALETTE, SPR_SIZE_16x16);
        sprite_set_priority(s_player.spr, 1);
    }

    /* NPCs */
    npc_render_all();

    /* Dialogue box */
    interact_render();
}

/* Forward-declare the callbacks so game_states.c can reference them */
const GameState g_state_overworld = {
    .id       = STATE_OVERWORLD,
    .on_enter = state_overworld_enter,
    .on_exit  = state_overworld_exit,
    .update   = state_overworld_update,
    .render   = state_overworld_render,
};
