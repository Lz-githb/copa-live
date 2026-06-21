#ifndef MENU_TYPES_H
#define MENU_TYPES_H

#include "gba_types.h"

/* =========================================================
 * Menu / UI shared types
 *
 * All menus use BG0 (screenblock 31) for the UI layer.
 * Tile layout:
 *   CB0 tiles 0-127   : font (ASCII subset, 6x8 packed in 8x8)
 *   CB0 tiles 128-191 : window chrome (corners, edges, fills)
 *   CB0 tiles 192-255 : icon tiles (item/status/etc.)
 *
 * Palette slots (BG palettes 0-15):
 *   Slot 0  : window background / fill (dark navy)
 *   Slot 1  : window border (gold)
 *   Slot 2  : text normal (white on transparent)
 *   Slot 3  : text highlight (yellow on transparent)
 *   Slot 4  : text dim (grey)
 *   Slot 5  : HP bar (green → yellow → red, palette cycling)
 *   Slot 6  : MP bar (blue)
 *   Slot 7  : EXP bar (purple)
 *   Slot 8  : cursor / selection highlight (cyan glow)
 *   Slot 9  : quest available (green icon)
 *   Slot 10 : quest active (yellow icon)
 *   Slot 11 : disabled / locked (dark grey)
 *
 * Screen: 240x160, 30x20 tiles.
 * ========================================================= */

/* Screenblock used for the UI overlay (BG0) */
#define UI_SB_IDX   31
#define UI_SB       ((vu16*)(MEM_VRAM + 0xF800))

/* Tile IDs within charblock 0 */
#define TILE_BLANK          0
#define TILE_FONT_BASE      0     /* ASCII 0x20 maps to tile TILE_FONT_BASE+0 */
#define TILE_WIN_TL         128   /* window top-left corner */
#define TILE_WIN_TR         129
#define TILE_WIN_BL         130
#define TILE_WIN_BR         131
#define TILE_WIN_TOP        132   /* horizontal edge */
#define TILE_WIN_BOT        133
#define TILE_WIN_LEFT       134   /* vertical edge */
#define TILE_WIN_RIGHT      135
#define TILE_WIN_FILL       136   /* interior fill */
#define TILE_WIN_TITLE_L    137   /* title bar left cap */
#define TILE_WIN_TITLE_MID  138   /* title bar fill */
#define TILE_WIN_TITLE_R    139   /* title bar right cap */
#define TILE_CURSOR         140   /* animated cursor triangle */
#define TILE_CURSOR2        141   /* cursor frame 2 */
#define TILE_CURSOR3        142   /* cursor frame 3 */
#define TILE_CURSOR4        143   /* cursor frame 4 */
#define TILE_BAR_EMPTY      144
#define TILE_BAR_FULL       145
#define TILE_BAR_LEFT       146
#define TILE_BAR_RIGHT      147
#define TILE_ICON_SWORD     192   /* item icons start */
#define TILE_ICON_SHIELD    193
#define TILE_ICON_HELM      194
#define TILE_ICON_ARMOR     195
#define TILE_ICON_BOOTS     196
#define TILE_ICON_RING      197
#define TILE_ICON_AMULET    198
#define TILE_ICON_OFFHAND   199
#define TILE_ICON_POTION    200
#define TILE_ICON_MATERIAL  201
#define TILE_ICON_KEY       202
#define TILE_ICON_LEGENDARY 203
#define TILE_ICON_QUEST     204
#define TILE_ICON_STAR      205
#define TILE_ICON_SKULL     206
#define TILE_ICON_CHECKMARK 207
#define TILE_ICON_LOCK      208
#define TILE_ICON_SCROLL    209
#define TILE_ICON_COIN      210
#define TILE_ICON_EXP       211
#define TILE_ICON_HP        212
#define TILE_ICON_MP        213

/* Palette indices for SE_PAL() macro */
#define PAL_WINDOW  0
#define PAL_BORDER  1
#define PAL_TEXT    2
#define PAL_HI      3
#define PAL_DIM     4
#define PAL_HP      5
#define PAL_MP      6
#define PAL_EXP     7
#define PAL_CURSOR  8
#define PAL_AVAIL   9
#define PAL_ACTIVE  10
#define PAL_LOCKED  11

/* Screen/tile dimensions */
#define UI_COLS     30
#define UI_ROWS     20

/* Helper: screenblock entry with tile + palette */
#define SE_TID(t)   ((u16)(t))
#define SE_PAL(p)   ((u16)((p) << 12))
#define SE_ENTRY(t,p) (SE_TID(t) | SE_PAL(p))
#define SE_BLANK    SE_ENTRY(TILE_BLANK, PAL_WINDOW)

/* ---- Global menu state ---------------------------------- */

typedef enum {
    MENU_CLOSED = 0,
    MENU_MAIN,          /* top-level: Inventory/Equip/Quests/Status/Save */
    MENU_INVENTORY,
    MENU_EQUIPMENT,
    MENU_QUEST_LOG,
    MENU_STATUS,
    MENU_SHOP,
    MENU_SAVE,
    MENU_CRAFTING,
    MENU_CONFIRM,       /* yes/no confirmation dialog */
    MENU_COUNT
} MenuID;

typedef struct {
    MenuID  current;
    MenuID  prev;           /* for "back" navigation */
    u8      cursor;         /* current selection row */
    u8      scroll;         /* top visible row in list */
    u8      sub_cursor;     /* secondary cursor (tabs, columns) */
    u8      anim_tick;      /* frame counter for cursor blink */
    BOOL    dirty;          /* needs redraw */
    u8      char_idx;       /* which party member is focused */
    /* Confirm dialog */
    char    confirm_msg[32];
    MenuID  confirm_yes_menu;
} MenuState;

extern MenuState g_menu;

/* ---- Shared UI primitives ------------------------------- */

void menu_init(void);
void menu_open(MenuID id);
void menu_close(void);
void menu_update(void);
void menu_render(void);
BOOL menu_is_open(void);

/* Low-level tile writers (shared with battle_ui) */
void ui_write_char(u8 col, u8 row, char c, u8 pal);
void ui_write_str(u8 col, u8 row, const char* str, u8 pal);
void ui_write_num(u8 col, u8 row, s32 num, u8 pal);
void ui_write_num_padded(u8 col, u8 row, s32 num, u8 width, u8 pal);
void ui_fill_rect(u8 col, u8 row, u8 w, u8 h, u16 tile_entry);
void ui_draw_window(u8 col, u8 row, u8 w, u8 h);
void ui_draw_window_titled(u8 col, u8 row, u8 w, u8 h, const char* title);
void ui_draw_bar(u8 col, u8 row, u8 w, s16 cur, s16 max, u8 pal);
void ui_draw_cursor(u8 col, u8 row);
void ui_clear_sb(void);

#endif /* MENU_TYPES_H */
