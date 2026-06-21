#include "../../include/game/game_states.h"
#include "../../include/game/interaction.h"
#include "../../include/engine/engine.h"
#include "../../include/gba/gba_reg.h"
#include "../../include/gba/gba_types.h"

/* =========================================================
 * STATE_DIALOGUE — thin wrapper around the interaction system
 *
 * The interaction subsystem (interaction.c) runs the script
 * interpreter and renders the dialogue box.  This state owns
 * only display setup and teardown.
 * ========================================================= */

static void state_dialogue_enter(void);
static void state_dialogue_exit(void);
static void state_dialogue_update(void);
static void state_dialogue_render(void);

const GameState g_state_dialogue = {
    .id       = STATE_DIALOGUE,
    .on_enter = state_dialogue_enter,
    .on_exit  = state_dialogue_exit,
    .update   = state_dialogue_update,
    .render   = state_dialogue_render,
};

static u16 s_saved_dispcnt;

static void state_dialogue_enter(void)
{
    s_saved_dispcnt = REG_DISPCNT;

    /* Keep existing display mode; ensure BG0 (UI layer) is visible */
    REG_DISPCNT = (u16)(s_saved_dispcnt | DCNT_BG0 | DCNT_OBJ);

    /* BG0: priority 0, charblock 0, screenblock 31 (UI), 32x32, 4bpp */
    REG_BG0CNT = BG_CNT(0, 0, 0, 0, 31, 0, BG_SIZE_256x256);

    /* Dialogue box is screen-anchored — no scroll */
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
}

static void state_dialogue_exit(void)
{
    interact_close();
    REG_DISPCNT = s_saved_dispcnt;
}

static void state_dialogue_update(void)
{
    interact_update();
    if (!interact_busy())
        state_pop();
}

static void state_dialogue_render(void)
{
    interact_render();
}
