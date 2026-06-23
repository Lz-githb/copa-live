#ifndef ENGINE_STATE_MACHINE_H
#define ENGINE_STATE_MACHINE_H

#include "../gba/gba_types.h"

/* =========================================================
 * Game State Machine
 *
 * States are discrete game modes. Only one is active at a
 * time. Transitions are deferred to the end of the current
 * frame to avoid re-entrancy issues.
 * ========================================================= */

typedef enum {
    STATE_NONE       = 0,
    STATE_BOOT,          /* first frame: hardware init */
    STATE_TITLE,         /* title screen */
    STATE_OVERWORLD,     /* top-down exploration */
    STATE_BATTLE,        /* turn-based battle */
    STATE_MENU,          /* pause/main menu */
    STATE_DIALOGUE,      /* NPC/cutscene text */
    STATE_TRANSITION,    /* fade between states */
    STATE_GAMEOVER,      /* game over screen */
    STATE_COUNT
} GameStateID;

/* Each state provides four callbacks */
typedef struct {
    GameStateID id;
    void (*on_enter)(void);          /* called when state becomes active */
    void (*on_exit)(void);           /* called before leaving state */
    void (*update)(void);            /* called every frame (logic) */
    void (*render)(void);            /* called every frame (draw) */
} GameState;

/* ---- State machine API ----------------------------------- */

/* Register a state's callbacks */
void state_register(const GameState* state);

/* Request a transition (takes effect next frame) */
void state_change(GameStateID next);

/* Stack-based push/pop (e.g., open menu over overworld) */
void state_push(GameStateID next);
void state_pop(void);

/* Process deferred transition (call at end of frame) */
void state_update(void);

/* Dispatch render to active state */
void state_render(void);

/* Query */
GameStateID state_current(void);
GameStateID state_previous(void);

/* Run one full frame (update + render) */
void state_tick(void);

#endif /* ENGINE_STATE_MACHINE_H */
