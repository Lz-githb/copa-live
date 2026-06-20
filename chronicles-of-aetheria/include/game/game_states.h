#ifndef GAME_STATES_H
#define GAME_STATES_H

#include "../engine/engine.h"

/* =========================================================
 * Game State Declarations
 * Each state registers its GameState struct at startup
 * ========================================================= */

/* Title Screen */
extern const GameState g_state_title;
void state_title_enter(void);
void state_title_exit(void);
void state_title_update(void);
void state_title_render(void);

/* Overworld */
extern const GameState g_state_overworld;
void state_overworld_enter(void);
void state_overworld_exit(void);
void state_overworld_update(void);
void state_overworld_render(void);

/* Battle */
extern const GameState g_state_battle;
void state_battle_enter(void);
void state_battle_exit(void);
void state_battle_update(void);
void state_battle_render(void);

/* Menu */
extern const GameState g_state_menu;
void state_menu_enter(void);
void state_menu_exit(void);
void state_menu_update(void);
void state_menu_render(void);

/* Register all game states with the state machine */
void game_register_states(void);

#endif /* GAME_STATES_H */
