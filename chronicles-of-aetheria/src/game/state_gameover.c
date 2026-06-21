#include "../../include/game/game_states.h"

/* =========================================================
 * Game Over State
 *
 * Shown when the party is wiped out.
 * Displays "GAME OVER" text, waits for A/START, then
 * returns to title screen.
 * ========================================================= */

static u32  s_timer;
static BOOL s_blink;

void state_gameover_enter(void)
{
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ_MAP_1D;

    tile_bg_setup(BG_LAYER_UI, 0, 31, 0, 0, BG_SIZE_256x256);
    tile_bg_enable(BG_LAYER_UI, TRUE);

    /* Black background, red text */
    PAL_BG_MEM[0]  = RGB15(0,  0,  0);   /* backdrop */
    PAL_BG_MEM[1]  = RGB15(31, 4,  4);   /* red — game over text */
    PAL_BG_MEM[2]  = RGB15(24, 24, 24);  /* grey — press start hint */

    ui_clear_sb();
    ui_write_str(9, 8,  "G A M E  O V E R", 1);
    ui_write_str(6, 12, "PRESS START TO CONTINUE", 2);

    s_timer = 0;
    s_blink = TRUE;
}

void state_gameover_exit(void)
{
    tile_bg_enable(BG_LAYER_UI, FALSE);
    sprite_clear_all();
}

void state_gameover_update(void)
{
    s_timer++;

    /* Blink "PRESS START" every 30 frames */
    if (s_timer % 30 == 0) {
        s_blink = !s_blink;
        if (s_blink)
            ui_write_str(6, 12, "PRESS START TO CONTINUE", 2);
        else
            ui_write_str(6, 12, "                       ", 2);
    }

    /* Any confirm key → title */
    if (key_pressed(KEY_START) || key_pressed(KEY_A)) {
        state_change(STATE_TITLE);
    }
}

void state_gameover_render(void)
{
    /* Static tiles — no per-frame draw needed */
}

const GameState g_state_gameover = {
    .id       = STATE_GAMEOVER,
    .on_enter = state_gameover_enter,
    .on_exit  = state_gameover_exit,
    .update   = state_gameover_update,
    .render   = state_gameover_render,
};
