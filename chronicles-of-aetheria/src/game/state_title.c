#include "../../include/game/game_states.h"

/* =========================================================
 * Title Screen State
 *
 * Demonstrates the state callback pattern.
 * In production, load logo graphics, play jingle, wait
 * for START press, then transition to STATE_OVERWORLD.
 * ========================================================= */

static u32 s_blink_timer;
static BOOL s_press_start_visible;

void state_title_enter(void) {
    /* Configure display: Mode 0, BG0 for text/logo */
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_MAP_1D;

    /* Set up BG0 for UI (charblock 0, screenblock 30) */
    tile_bg_setup(BG_LAYER_UI, 0, 30, 0, 0, BG_SIZE_256x256);
    tile_bg_enable(BG_LAYER_UI, TRUE);

    /* Fill screen with color 0 (black via palette) */
    PAL_BG_MEM[0] = RGB15(0, 0, 0);
    PAL_BG_MEM[1] = RGB15(31, 31, 31);   /* white for text */

    s_blink_timer          = 0;
    s_press_start_visible  = TRUE;
}

void state_title_exit(void) {
    tile_bg_enable(BG_LAYER_UI, FALSE);
    sprite_clear_all();
}

void state_title_update(void) {
    /* Blink "PRESS START" every 30 frames */
    s_blink_timer++;
    if (s_blink_timer >= 30) {
        s_blink_timer          = 0;
        s_press_start_visible  = !s_press_start_visible;
    }

    /* Transition on START */
    if (key_pressed(KEY_START)) {
        state_change(STATE_OVERWORLD);
    }

    /* Open save menu on SELECT */
    if (key_pressed(KEY_SELECT)) {
        state_push(STATE_MENU);
    }
}

void state_title_render(void) {
    /* No per-frame render work in Mode 0 — tiles are static.
     * A real implementation would draw logo sprites here. */
    (void)s_press_start_visible;
}

const GameState g_state_title = {
    .id       = STATE_TITLE,
    .on_enter = state_title_enter,
    .on_exit  = state_title_exit,
    .update   = state_title_update,
    .render   = state_title_render,
};
