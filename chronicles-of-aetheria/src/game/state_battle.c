#include "../../include/game/game_states.h"

/* =========================================================
 * Battle State (Turn-Based RPG)
 *
 * Skeleton for a standard menu-driven battle:
 *   PHASE_PLAYER_SELECT → PHASE_EXECUTE → PHASE_ENEMY → repeat
 * ========================================================= */

typedef enum {
    BATTLE_PHASE_ENTER,
    BATTLE_PHASE_PLAYER_SELECT,
    BATTLE_PHASE_EXECUTE,
    BATTLE_PHASE_ENEMY,
    BATTLE_PHASE_VICTORY,
    BATTLE_PHASE_DEFEAT,
} BattlePhase;

typedef enum {
    CMD_NONE = 0,
    CMD_ATTACK,
    CMD_MAGIC,
    CMD_ITEM,
    CMD_FLEE,
    CMD_COUNT
} BattleCommand;

static const char* CMD_NAMES[CMD_COUNT] = {
    "", "ATTACK", "MAGIC", "ITEM", "FLEE"
};

static struct {
    BattlePhase phase;
    u32         enemy_group_id;
    u8          cursor;            /* menu cursor */
    u8          anim_timer;
    BattleCommand selected_cmd;
} s_battle;

static void _on_start(const Event* evt) {
    s_battle.enemy_group_id = evt->param0;
}

void state_battle_enter(void) {
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 |
                  DCNT_OBJ  | DCNT_OBJ_MAP_1D;

    tile_bg_setup(BG_LAYER_UI,    0, 31, 0, 0, BG_SIZE_256x256);
    tile_bg_setup(BG_LAYER_WORLD, 0, 30, 1, 0, BG_SIZE_256x256);
    tile_bg_enable(BG_LAYER_UI,    TRUE);
    tile_bg_enable(BG_LAYER_WORLD, TRUE);

    /* No scrolling in battle */
    tile_bg_scroll(BG_LAYER_UI,    0, 0);
    tile_bg_scroll(BG_LAYER_WORLD, 0, 0);

    s_battle.phase        = BATTLE_PHASE_ENTER;
    s_battle.cursor       = CMD_ATTACK;
    s_battle.anim_timer   = 0;
    s_battle.selected_cmd = CMD_NONE;

    event_listen(EVT_BATTLE_START, _on_start);
}

void state_battle_exit(void) {
    event_unlisten(EVT_BATTLE_START, _on_start);
    tile_bg_enable(BG_LAYER_UI,    FALSE);
    tile_bg_enable(BG_LAYER_WORLD, FALSE);
    sprite_clear_all();
}

void state_battle_update(void) {
    s_battle.anim_timer++;

    switch (s_battle.phase) {

    case BATTLE_PHASE_ENTER:
        /* Brief entry animation (e.g., 30 frames) */
        if (s_battle.anim_timer >= 30) {
            s_battle.anim_timer = 0;
            s_battle.phase = BATTLE_PHASE_PLAYER_SELECT;
        }
        break;

    case BATTLE_PHASE_PLAYER_SELECT:
        /* Navigate command menu */
        if (key_pressed(KEY_UP)) {
            if (s_battle.cursor > 1) s_battle.cursor--;
        }
        if (key_pressed(KEY_DOWN)) {
            if (s_battle.cursor < CMD_COUNT - 1) s_battle.cursor++;
        }
        if (key_pressed(KEY_A)) {
            s_battle.selected_cmd = (BattleCommand)s_battle.cursor;
            s_battle.anim_timer   = 0;
            s_battle.phase        = BATTLE_PHASE_EXECUTE;
        }
        if (key_pressed(KEY_B)) {
            /* Cancel: go back to cursor default */
            s_battle.cursor = CMD_ATTACK;
        }
        break;

    case BATTLE_PHASE_EXECUTE:
        if (s_battle.selected_cmd == CMD_FLEE) {
            event_push(EVT_BATTLE_END, 2 /* FLEE */, 0);
            state_pop();
            break;
        }
        /* Resolve action (TODO: damage calculation) */
        if (s_battle.anim_timer >= 40) {
            s_battle.anim_timer = 0;
            s_battle.phase = BATTLE_PHASE_ENEMY;
        }
        break;

    case BATTLE_PHASE_ENEMY:
        /* Enemy AI action (TODO) */
        if (s_battle.anim_timer >= 40) {
            s_battle.anim_timer = 0;
            s_battle.phase = BATTLE_PHASE_PLAYER_SELECT;
        }
        break;

    case BATTLE_PHASE_VICTORY:
        if (s_battle.anim_timer >= 90) {
            event_push(EVT_BATTLE_END, 0 /* WIN */, 0);
            state_pop();
        }
        break;

    case BATTLE_PHASE_DEFEAT:
        if (s_battle.anim_timer >= 90) {
            state_change(STATE_GAMEOVER);
        }
        break;
    }
}

void state_battle_render(void) {
    /* TODO: draw enemies, HP bars, command menu using tiles */
    (void)CMD_NAMES;
}

const GameState g_state_battle = {
    .id       = STATE_BATTLE,
    .on_enter = state_battle_enter,
    .on_exit  = state_battle_exit,
    .update   = state_battle_update,
    .render   = state_battle_render,
};
