#include "../../include/game/game_states.h"

/* =========================================================
 * Register all game states with the state machine
 * ========================================================= */

void game_register_states(void) {
    state_register(&g_state_title);
    state_register(&g_state_overworld);
    state_register(&g_state_battle);
    state_register(&g_state_menu);
}
