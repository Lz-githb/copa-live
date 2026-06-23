#include "../../include/game/game_states.h"
#include "../../include/game/battle/battle.h"

static u8 s_pending_group;

static void _on_battle_start(const Event* evt) {
    s_pending_group = (u8)evt->param0;
}

static void _on_battle_end(const Event* evt) {
    BattleResult res = (BattleResult)evt->param0;
    if (res == BATTLE_RESULT_LOSE)
        state_change(STATE_GAMEOVER);
    else
        state_pop();
}

void state_battle_enter(void) {
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 |
                  DCNT_OBJ  | DCNT_OBJ_MAP_1D;

    tile_bg_setup(BG_LAYER_UI,    0, 31, 0, 0, BG_SIZE_256x256);
    tile_bg_setup(BG_LAYER_WORLD, 0, 30, 1, 0, BG_SIZE_256x256);
    tile_bg_enable(BG_LAYER_UI,    TRUE);
    tile_bg_enable(BG_LAYER_WORLD, TRUE);
    tile_bg_scroll(BG_LAYER_UI,    0, 0);
    tile_bg_scroll(BG_LAYER_WORLD, 0, 0);

    event_listen(EVT_BATTLE_START, _on_battle_start);
    event_listen(EVT_BATTLE_END,   _on_battle_end);

    battle_data_init();
    battle_begin(s_pending_group);
}

void state_battle_exit(void) {
    event_unlisten(EVT_BATTLE_START, _on_battle_start);
    event_unlisten(EVT_BATTLE_END,   _on_battle_end);
    battle_end_cleanup();
    tile_bg_enable(BG_LAYER_UI,    FALSE);
    tile_bg_enable(BG_LAYER_WORLD, FALSE);
    sprite_clear_all();
}

void state_battle_update(void) {
    battle_update();
}

void state_battle_render(void) {
    battle_render();
}

const GameState g_state_battle = {
    .id       = STATE_BATTLE,
    .on_enter = state_battle_enter,
    .on_exit  = state_battle_exit,
    .update   = state_battle_update,
    .render   = state_battle_render,
};
