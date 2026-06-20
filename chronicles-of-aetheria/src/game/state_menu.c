#include "../../include/game/game_states.h"

/* =========================================================
 * Pause / Main Menu State
 *
 * Overlay on top of overworld or title.
 * Items: RESUME | SAVE | LOAD | OPTIONS | QUIT
 * ========================================================= */

typedef enum {
    MENU_ITEM_RESUME = 0,
    MENU_ITEM_SAVE,
    MENU_ITEM_LOAD,
    MENU_ITEM_OPTIONS,
    MENU_ITEM_QUIT,
    MENU_ITEM_COUNT
} MenuItem;

static const char* MENU_LABELS[MENU_ITEM_COUNT] = {
    "RESUME", "SAVE", "LOAD", "OPTIONS", "QUIT TO TITLE"
};

static struct {
    u8     cursor;
    BOOL   save_confirm;
    u8     save_slot;
    u32    blink;
} s_menu;

void state_menu_enter(void) {
    /* Menu runs on BG0 as overlay — don't touch other layers */
    REG_DISPCNT |= DCNT_BG0;
    tile_bg_enable(BG_LAYER_UI, TRUE);

    s_menu.cursor       = MENU_ITEM_RESUME;
    s_menu.save_confirm = FALSE;
    s_menu.save_slot    = 0;
    s_menu.blink        = 0;
}

void state_menu_exit(void) {
    /* Nothing to unload; BG0 stays active under caller */
}

void state_menu_update(void) {
    s_menu.blink++;

    if (s_menu.save_confirm) {
        /* Slot selection mode */
        if (key_pressed(KEY_LEFT)  && s_menu.save_slot > 0) s_menu.save_slot--;
        if (key_pressed(KEY_RIGHT) && s_menu.save_slot < SAVE_SLOT_COUNT - 1)
            s_menu.save_slot++;

        if (key_pressed(KEY_A)) {
            /* Perform save */
            SaveSlot slot;
            save_capture(s_menu.save_slot, &slot);
            slot.play_time += g_frame / 60;   /* approx seconds */
            save_write(s_menu.save_slot, &slot);
            s_menu.save_confirm = FALSE;
        }
        if (key_pressed(KEY_B)) {
            s_menu.save_confirm = FALSE;
        }
        return;
    }

    /* Normal menu navigation */
    if (key_pressed(KEY_UP)) {
        if (s_menu.cursor > 0) s_menu.cursor--;
    }
    if (key_pressed(KEY_DOWN)) {
        if (s_menu.cursor < MENU_ITEM_COUNT - 1) s_menu.cursor++;
    }

    if (key_pressed(KEY_A)) {
        switch (s_menu.cursor) {
        case MENU_ITEM_RESUME:
            state_pop();
            break;
        case MENU_ITEM_SAVE:
            s_menu.save_confirm = TRUE;
            s_menu.save_slot    = 0;
            break;
        case MENU_ITEM_LOAD: {
            /* Load from first valid slot */
            u8 slot;
            for (slot = 0; slot < SAVE_SLOT_COUNT; slot++) {
                SaveSlot data;
                if (save_read(slot, &data)) {
                    save_apply(&data);
                    state_change(STATE_OVERWORLD);
                    return;
                }
            }
            break;
        }
        case MENU_ITEM_OPTIONS:
            /* TODO: options scene */
            break;
        case MENU_ITEM_QUIT:
            state_change(STATE_TITLE);
            break;
        }
    }

    if (key_pressed(KEY_B) || key_pressed(KEY_START)) {
        state_pop();
    }
}

void state_menu_render(void) {
    /* TODO: draw menu box and items using tile-based font */
    (void)MENU_LABELS;
}

const GameState g_state_menu = {
    .id       = STATE_MENU,
    .on_enter = state_menu_enter,
    .on_exit  = state_menu_exit,
    .update   = state_menu_update,
    .render   = state_menu_render,
};
