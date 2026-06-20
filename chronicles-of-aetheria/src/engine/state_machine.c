#include "../../include/engine/state_machine.h"
#include "../../include/gba/gba_types.h"

/* =========================================================
 * State Machine Implementation
 * ========================================================= */

#define STATE_STACK_MAX 4

static GameState s_states[STATE_COUNT];

static struct {
    GameStateID stack[STATE_STACK_MAX];
    u32         depth;
} s_sm;

static GameStateID s_pending;       /* deferred transition */
static BOOL        s_pending_pop;   /* deferred pop */
static BOOL        s_pending_push;  /* deferred push */
static GameStateID s_push_id;

void state_register(const GameState* state) {
    if (state->id < STATE_COUNT) {
        s_states[state->id] = *state;
    }
}

static GameStateID _top(void) {
    if (s_sm.depth == 0) return STATE_NONE;
    return s_sm.stack[s_sm.depth - 1];
}

static void _enter(GameStateID id) {
    if (id < STATE_COUNT && s_states[id].on_enter) {
        s_states[id].on_enter();
    }
}

static void _exit(GameStateID id) {
    if (id < STATE_COUNT && s_states[id].on_exit) {
        s_states[id].on_exit();
    }
}

void state_change(GameStateID next) {
    s_pending      = next;
    s_pending_push = FALSE;
    s_pending_pop  = FALSE;
}

void state_push(GameStateID next) {
    s_push_id      = next;
    s_pending_push = TRUE;
    s_pending_pop  = FALSE;
}

void state_pop(void) {
    s_pending_pop  = TRUE;
    s_pending_push = FALSE;
}

void state_update(void) {
    /* Process deferred transition at start of next frame */
    if (s_pending_pop) {
        s_pending_pop = FALSE;
        if (s_sm.depth > 0) {
            _exit(_top());
            s_sm.depth--;
        }
        return;
    }

    if (s_pending_push) {
        s_pending_push = FALSE;
        if (s_sm.depth < STATE_STACK_MAX) {
            s_sm.stack[s_sm.depth++] = s_push_id;
            _enter(s_push_id);
        }
        return;
    }

    if (s_pending != STATE_NONE) {
        GameStateID next = s_pending;
        s_pending = STATE_NONE;
        /* Exit all stacked states */
        while (s_sm.depth > 0) {
            _exit(_top());
            s_sm.depth--;
        }
        /* Enter new state */
        s_sm.stack[0] = next;
        s_sm.depth    = 1;
        _enter(next);
    }
}

void state_tick(void) {
    /* Process pending transitions */
    state_update();

    /* Update active state */
    GameStateID cur = _top();
    if (cur < STATE_COUNT && s_states[cur].update) {
        s_states[cur].update();
    }
}

void state_render(void) {
    GameStateID cur = _top();
    if (cur < STATE_COUNT && s_states[cur].render) {
        s_states[cur].render();
    }
}

GameStateID state_current(void) {
    return _top();
}

GameStateID state_previous(void) {
    if (s_sm.depth < 2) return STATE_NONE;
    return s_sm.stack[s_sm.depth - 2];
}
