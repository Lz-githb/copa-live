#include "../../include/game/game_states.h"

/* =========================================================
 * Overworld State
 *
 * Main exploration loop:
 *   - Player sprite moves on tile map
 *   - Camera follows player
 *   - Collision against solid tiles
 *   - Trigger zones fire events (warps, dialogue, battles)
 * ========================================================= */

/* Player movement speed in pixels per frame */
#define PLAYER_SPEED   2
/* Player sprite size */
#define PLAYER_W       16
#define PLAYER_H       16
/* Player AABB hitbox (smaller than sprite for feel) */
#define HITBOX_W       12
#define HITBOX_H        8
#define HITBOX_OX       2   /* offset from sprite top-left */
#define HITBOX_OY       8

/* Minimal 4-direction animation: 2 frames per direction */
typedef enum { DIR_DOWN=0, DIR_LEFT, DIR_RIGHT, DIR_UP } Direction;

#define ANIM_FRAMES    2
#define ANIM_SPEED     8   /* frames per anim step */

/* Minimal stub map (overwritten by real map loader) */
static Tilemap s_map;
static BOOL    s_map_loaded = FALSE;

/* Player state */
static struct {
    s32       world_x, world_y;   /* top-left of sprite in world */
    AABB      hitbox;
    Direction dir;
    u8        anim_frame;
    u8        anim_timer;
    SpriteHandle spr;
    BOOL      moving;
} s_player;

/* Tile IDs for the player sprite:
 * Real game loads from ROM; these are placeholder values.
 * Layout: [dir * ANIM_FRAMES + frame] → tile index in OBJ VRAM */
static const u16 s_player_tiles[4][ANIM_FRAMES] = {
    { 0,  2 },   /* DOWN  */
    { 4,  6 },   /* LEFT  */
    { 8,  10},   /* RIGHT */
    { 12, 14},   /* UP    */
};

/* ---- Helper: update hitbox from world position ----------- */
static void _update_hitbox(void) {
    s_player.hitbox.x = (s16)(s_player.world_x + HITBOX_OX);
    s_player.hitbox.y = (s16)(s_player.world_y + HITBOX_OY);
    s_player.hitbox.w = HITBOX_W;
    s_player.hitbox.h = HITBOX_H;
}

/* ---- Helper: apply screen position to sprite ------------- */
static void _update_sprite(void) {
    s16 sx = (s16)(s_player.world_x - camera_x());
    s16 sy = (s16)(s_player.world_y - camera_y());
    sprite_set_pos(s_player.spr, sx, sy);

    u16 tile = s_player_tiles[s_player.dir][s_player.anim_frame];
    sprite_set_gfx(s_player.spr, tile, 0, SPR_SIZE_16x16);
}

/* ---- Event handlers -------------------------------------- */
static void _on_warp(const Event* evt) {
    /* Load new map and reposition player */
    (void)evt;   /* TODO: load map evt->param0, spawn evt->param1 */
}

static void _on_battle(const Event* evt) {
    (void)evt;
    state_push(STATE_BATTLE);
}

/* ---- State callbacks ------------------------------------- */

void state_overworld_enter(void) {
    REG_DISPCNT = DCNT_MODE0      |
                  DCNT_BG0        |   /* HUD */
                  DCNT_BG1        |   /* foreground */
                  DCNT_BG2        |   /* world */
                  DCNT_BG3        |   /* background */
                  DCNT_OBJ        |
                  DCNT_OBJ_MAP_1D;

    /* Configure layers */
    tile_bg_setup(BG_LAYER_BG,    1, 28, 3, 0, BG_SIZE_256x256);
    tile_bg_setup(BG_LAYER_WORLD, 1, 29, 2, 0, BG_SIZE_256x256);
    tile_bg_setup(BG_LAYER_FG,    0, 30, 1, 0, BG_SIZE_256x256);
    tile_bg_setup(BG_LAYER_UI,    0, 31, 0, 0, BG_SIZE_256x256);

    tile_bg_enable(BG_LAYER_BG,    TRUE);
    tile_bg_enable(BG_LAYER_WORLD, TRUE);
    tile_bg_enable(BG_LAYER_FG,    TRUE);
    tile_bg_enable(BG_LAYER_UI,    TRUE);

    /* Allocate player sprite */
    s_player.spr       = sprite_alloc();
    s_player.world_x   = 128;
    s_player.world_y   = 128;
    s_player.dir       = DIR_DOWN;
    s_player.anim_frame = 0;
    s_player.anim_timer = 0;
    s_player.moving    = FALSE;
    _update_hitbox();

    /* Init camera centered on player */
    camera_init(MAP_MAX_W * TILE_SIZE, MAP_MAX_H * TILE_SIZE);
    camera_center_on(s_player.world_x + PLAYER_W / 2,
                     s_player.world_y + PLAYER_H / 2);
    camera_update();

    /* Init stub map if not already loaded */
    if (!s_map_loaded) {
        mem_zero(&s_map, sizeof(Tilemap));
        s_map.width  = MAP_MAX_W;
        s_map.height = MAP_MAX_H;
        s_map_loaded = TRUE;
    }

    col_set_map(&s_map);
    col_trigger_clear();

    /* Register event listeners */
    event_listen(EVT_WARP,         _on_warp);
    event_listen(EVT_BATTLE_START,  _on_battle);
}

void state_overworld_exit(void) {
    event_unlisten(EVT_WARP,        _on_warp);
    event_unlisten(EVT_BATTLE_START, _on_battle);

    sprite_free(s_player.spr);

    tile_bg_enable(BG_LAYER_BG,    FALSE);
    tile_bg_enable(BG_LAYER_WORLD, FALSE);
    tile_bg_enable(BG_LAYER_FG,    FALSE);
    tile_bg_enable(BG_LAYER_UI,    FALSE);
}

void state_overworld_update(void) {
    /* --- Input → movement --------------------------------- */
    s16 dx = 0, dy = 0;
    s_player.moving = FALSE;

    if (key_held(KEY_LEFT))  { dx = -PLAYER_SPEED; s_player.dir = DIR_LEFT;  s_player.moving = TRUE; }
    if (key_held(KEY_RIGHT)) { dx =  PLAYER_SPEED; s_player.dir = DIR_RIGHT; s_player.moving = TRUE; }
    if (key_held(KEY_UP))    { dy = -PLAYER_SPEED; s_player.dir = DIR_UP;    s_player.moving = TRUE; }
    if (key_held(KEY_DOWN))  { dy =  PLAYER_SPEED; s_player.dir = DIR_DOWN;  s_player.moving = TRUE; }

    /* --- Collision + move --------------------------------- */
    MoveResult mv = col_move(&s_player.hitbox, dx, dy);
    s_player.world_x += mv.dx;
    s_player.world_y += mv.dy;
    _update_hitbox();

    /* --- Trigger check ------------------------------------ */
    col_trigger_check(&s_player.hitbox);

    /* --- Animation ---------------------------------------- */
    if (s_player.moving) {
        s_player.anim_timer++;
        if (s_player.anim_timer >= ANIM_SPEED) {
            s_player.anim_timer = 0;
            s_player.anim_frame = (s_player.anim_frame + 1) % ANIM_FRAMES;
        }
    } else {
        s_player.anim_frame = 0;
        s_player.anim_timer = 0;
    }

    /* --- Camera ------------------------------------------- */
    camera_center_on(s_player.world_x + PLAYER_W / 2,
                     s_player.world_y + PLAYER_H / 2);
    camera_update();

    /* --- Open menu ---------------------------------------- */
    if (key_pressed(KEY_START)) {
        state_push(STATE_MENU);
    }
}

void state_overworld_render(void) {
    /* Apply camera scroll to BG registers */
    camera_apply_bg_scroll();

    /* Position player sprite relative to camera */
    _update_sprite();
}

const GameState g_state_overworld = {
    .id       = STATE_OVERWORLD,
    .on_enter = state_overworld_enter,
    .on_exit  = state_overworld_exit,
    .update   = state_overworld_update,
    .render   = state_overworld_render,
};
