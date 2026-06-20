#ifndef BATTLE_UI_H
#define BATTLE_UI_H

#include "battle_types.h"
#include "battler.h"
#include "battle_action.h"
#include "battle_combo.h"

/* =========================================================
 * Battle UI System
 *
 * Uses BG0 (screenblock 31) as the HUD overlay layer.
 * Tile-based text + custom HP/MP bar tiles.
 *
 * UI regions (in tile coordinates):
 *
 *  Turn indicator strip:  rows 0-1,  cols 0-29
 *  Enemy names+HP:        rows 0-5,  cols 0-14 (left side)
 *  Ally HP/MP bars:       rows 17-19, cols 0-29
 *  Command window:        rows 12-19, cols 20-29
 *  Skill/item list:       rows 12-19, cols 0-14
 *  Target cursor:         overlay on enemy/ally sprites
 *  Status icons:          above each battler sprite
 *
 * Screenblock entry format: SE_TID(tile) | SE_PAL(pal)
 * ========================================================= */

/* Turn indicator icons (small 8x8 sprites, one per battler) */
#define TURN_ICON_Y    2
#define TURN_ICON_X0   0  /* pixels */
#define TURN_ICON_STEP 12

/* Command menu items */
typedef enum {
    CMD_ATTACK = 0,
    CMD_MAGIC,
    CMD_ITEM,
    CMD_DEFEND,
    CMD_COMBO,
    CMD_FLEE,
    CMD_COUNT
} CommandID;

static const char* const CMD_LABELS[CMD_COUNT] = {
    "ATTACK", "MAGIC", "ITEM", "DEFEND", "COMBO", "FLEE"
};

/* Battle UI state */
typedef enum {
    UI_STATE_HIDDEN = 0,
    UI_STATE_CMD_SELECT,    /* picking command for current char */
    UI_STATE_SKILL_SELECT,  /* picking magic/skill */
    UI_STATE_ITEM_SELECT,
    UI_STATE_COMBO_SELECT,
    UI_STATE_TARGET_SELECT, /* picking target */
    UI_STATE_ANIMATING,     /* waiting for animation */
    UI_STATE_MESSAGE,       /* showing message text */
    UI_STATE_RESULT,        /* showing battle result */
} UIState;

typedef struct {
    UIState  state;
    u8       cmd_cursor;
    u8       skill_cursor;
    u8       item_cursor;
    u8       combo_cursor;
    u8       target_cursor;   /* which enemy/ally targeted */
    BOOL     targeting_enemy; /* TRUE = enemy target, FALSE = ally */
    u8       active_char;     /* which ally is currently choosing */

    /* Skill list for current char */
    u8       skill_ids[BATTLE_SKILL_MAX];
    u8       skill_count;

    /* Combo list for current char */
    u8       combo_ids[BATTLE_COMBO_MAX];
    u8       combo_count;

    /* Message display */
    char     msg_buf[64];
    u8       msg_timer;

    /* Turn indicator: which battler is next */
    u8       turn_order[BATTLE_BATTLER_MAX];
    u8       turn_count;
    u8       current_turn;  /* index into turn_order */
} BattleUI;

extern BattleUI g_battle_ui;

/* ---- UI API --------------------------------------------- */

void battle_ui_init(void);

/* Set which ally is selecting actions */
void battle_ui_set_active_char(u8 char_idx,
                                const Battler* b);

/* Called each frame for input + update */
/* Returns TRUE when player has committed an action */
BOOL battle_ui_update(Battler battlers[BATTLE_BATTLER_MAX],
                       BattleAction* out_action);

/* Render all UI elements to BG0 tiles */
void battle_ui_render(const Battler battlers[BATTLE_BATTLER_MAX]);

/* Show/hide the command window */
void battle_ui_show_cmd(void);
void battle_ui_hide(void);

/* Display a message (replaces command window temporarily) */
void battle_ui_message(const char* msg);
void battle_ui_message_clear(void);

/* Update turn order strip */
void battle_ui_set_turn_order(const TurnSlot* order, u8 count);

/* Draw HP/MP bars for all allies */
void battle_ui_draw_hp_bars(const Battler battlers[BATTLE_BATTLER_MAX]);

/* Draw enemy HP/name list */
void battle_ui_draw_enemies(const Battler battlers[BATTLE_BATTLER_MAX]);

/* Draw status icons above a battler sprite */
void battle_ui_draw_status(const Battler* b);

/* Highlight a target */
void battle_ui_highlight_target(u8 battler_idx, BOOL on);

/* Set turn indicator */
void battle_ui_mark_active_turn(u8 battler_idx);

/* ---- BG0 tile writer helpers (internal but exposed for test) */
void ui_write_char(u8 col, u8 row, char c, u8 pal);
void ui_write_str(u8 col, u8 row, const char* str, u8 pal);
void ui_write_num(u8 col, u8 row, s16 num, u8 pal);
void ui_fill_rect(u8 col, u8 row, u8 w, u8 h, u16 tile);
void ui_draw_window(u8 col, u8 row, u8 w, u8 h);
void ui_draw_bar(u8 col, u8 row, u8 w,
                  s16 current, s16 max, u8 color_pal);

#endif /* BATTLE_UI_H */
