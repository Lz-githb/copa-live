#include "menu_types.h"
#include "menu_inventory.h"
#include "menu_equip.h"
#include "menu_quest.h"
#include "menu_status.h"
#include "menu_shop.h"
#include "menu_save.h"
#include "menu_crafting.h"
#include "gba_reg.h"
#include "gba_types.h"
#include "tile.h"

/* =========================================================
 * Menu Core
 *
 * Manages the top-level pause menu and dispatches to sub-menus.
 * All sub-menus share the BG0 screenblock (UI_SB_IDX = 31).
 *
 * Top-level menu items and their layout:
 *   Col 9, rows 5-11:
 *   > INVENTORY
 *     EQUIPMENT
 *     QUEST LOG
 *     STATUS
 *     SAVE
 *     LOAD
 *     CLOSE
 *
 * Visual style: translucent navy background, gold border,
 *               animated cursor (tile cycling), white text.
 * ========================================================= */

/* ---- Global state --------------------------------------- */
MenuState g_menu;

/* ---- Top-level menu items ------------------------------- */
#define MAIN_ITEM_COUNT  7
static const char* const s_main_labels[MAIN_ITEM_COUNT] = {
    "INVENTORY",
    "EQUIPMENT",
    "QUEST LOG",
    "STATUS",
    "SAVE",
    "LOAD",
    "CLOSE"
};

/* ---- GBA key bitmasks (REG_KEYINPUT) -------------------- */
#define KEY_A       0x0001
#define KEY_B       0x0002
#define KEY_SELECT  0x0004
#define KEY_START   0x0008
#define KEY_RIGHT   0x0010
#define KEY_LEFT    0x0020
#define KEY_UP      0x0040
#define KEY_DOWN    0x0080
#define KEY_R       0x0100
#define KEY_L       0x0200

#define REG_KEYINPUT  (*(vu16*)(0x04000130))

/* Held vs. just-pressed tracking */
static u16 s_keys_prev;
static u16 s_keys_cur;

static u16 _keys_down(void)
{
    return (~s_keys_cur) & s_keys_prev; /* bits low = pressed */
}

static __attribute__((unused)) u16 _keys_held(void)
{
    return (~s_keys_cur);
}

static void _keys_update(void)
{
    s_keys_prev = ~s_keys_cur; /* what was held last frame */
    s_keys_cur  = REG_KEYINPUT;
}

/* ---- Low-level tile writers ----------------------------- */

void ui_clear_sb(void)
{
    u16 i;
    for (i = 0; i < 32 * 32; i++)
        UI_SB[i] = SE_BLANK;
}

void ui_write_char(u8 col, u8 row, char c, u8 pal)
{
    u16 tile_id;
    if (col >= 30 || row >= 20) return;
    if (c < 0x20 || c > 0x7E) c = ' ';
    tile_id = (u16)(TILE_FONT_BASE + (c - 0x20));
    UI_SB[row * 32 + col] = SE_ENTRY(tile_id, pal);
}

void ui_write_str(u8 col, u8 row, const char* str, u8 pal)
{
    while (*str && col < 30) {
        ui_write_char(col++, row, *str++, pal);
    }
}

void ui_write_num(u8 col, u8 row, s32 num, u8 pal)
{
    char buf[12];
    s32 n = num;
    u8  i = 0, j;
    char tmp;

    if (n < 0) { buf[i++] = '-'; n = -n; }
    if (n == 0) { buf[i++] = '0'; }
    else {
        u8 start = i;
        while (n > 0) { buf[i++] = (char)('0' + n % 10); n /= 10; }
        /* reverse digit portion */
        for (j = 0; j < (i - start) / 2; j++) {
            tmp = buf[start + j];
            buf[start + j] = buf[i - 1 - j];
            buf[i - 1 - j] = tmp;
        }
    }
    buf[i] = '\0';
    ui_write_str(col, row, buf, pal);
}

void ui_write_num_padded(u8 col, u8 row, s32 num, u8 width, u8 pal)
{
    char buf[12];
    s32  n = (num < 0) ? -num : num;
    u8   i = 0, j, digits = 0, start;
    char tmp;

    if (num < 0) buf[i++] = '-';
    start = i;
    if (n == 0) buf[i++] = '0';
    else { while (n > 0) { buf[i++] = (char)('0' + n % 10); n /= 10; } }
    digits = (u8)(i - start);
    /* reverse */
    for (j = 0; j < digits / 2; j++) {
        tmp = buf[start + j];
        buf[start + j] = buf[i - 1 - j];
        buf[i - 1 - j] = tmp;
    }
    buf[i] = '\0';
    /* right-align within width */
    while (i < width) { buf[i++] = ' '; buf[i] = '\0'; } /* shouldn't overflow */
    /* shift right */
    if (digits < width) {
        u8 pad = (u8)(width - digits);
        for (j = width; j > 0; j--) buf[j] = (j > pad) ? buf[j - pad] : ' ';
        buf[0] = (num < 0 && pad == 0) ? '-' : buf[0];
    }
    buf[width] = '\0';
    ui_write_str(col, row, buf, pal);
}

void ui_fill_rect(u8 col, u8 row, u8 w, u8 h, u16 tile_entry)
{
    u8 r, c;
    for (r = row; r < row + h && r < 20; r++)
        for (c = col; c < col + w && c < 30; c++)
            UI_SB[r * 32 + c] = tile_entry;
}

void ui_draw_window(u8 col, u8 row, u8 w, u8 h)
{
    u8 r, c;
    /* Fill interior */
    ui_fill_rect((u8)(col+1), (u8)(row+1), (u8)(w-2), (u8)(h-2),
                 SE_ENTRY(TILE_WIN_FILL, PAL_WINDOW));
    /* Top/bottom edges */
    for (c = col + 1; c < col + w - 1; c++) {
        UI_SB[row * 32 + c]           = SE_ENTRY(TILE_WIN_TOP, PAL_BORDER);
        UI_SB[(row+h-1) * 32 + c]     = SE_ENTRY(TILE_WIN_BOT, PAL_BORDER);
    }
    /* Left/right edges */
    for (r = row + 1; r < row + h - 1; r++) {
        UI_SB[r * 32 + col]           = SE_ENTRY(TILE_WIN_LEFT,  PAL_BORDER);
        UI_SB[r * 32 + (col+w-1)]     = SE_ENTRY(TILE_WIN_RIGHT, PAL_BORDER);
    }
    /* Corners */
    UI_SB[row * 32 + col]             = SE_ENTRY(TILE_WIN_TL, PAL_BORDER);
    UI_SB[row * 32 + (col+w-1)]       = SE_ENTRY(TILE_WIN_TR, PAL_BORDER);
    UI_SB[(row+h-1) * 32 + col]       = SE_ENTRY(TILE_WIN_BL, PAL_BORDER);
    UI_SB[(row+h-1) * 32 + (col+w-1)] = SE_ENTRY(TILE_WIN_BR, PAL_BORDER);
}

void ui_draw_window_titled(u8 col, u8 row, u8 w, u8 h, const char* title)
{
    u8 c;
    ui_draw_window(col, row, w, h);
    /* Title bar: row+1, cols col+1..col+w-2 */
    UI_SB[(row+1) * 32 + col]         = SE_ENTRY(TILE_WIN_TITLE_L,   PAL_BORDER);
    UI_SB[(row+1) * 32 + (col+w-1)]   = SE_ENTRY(TILE_WIN_TITLE_R,   PAL_BORDER);
    for (c = (u8)(col+1); c < col+w-1; c++)
        UI_SB[(row+1) * 32 + c]       = SE_ENTRY(TILE_WIN_TITLE_MID, PAL_BORDER);
    /* Title text centered */
    {
        u8 tlen = 0;
        const char* p = title;
        while (*p++) tlen++;
        u8 tx = (u8)(col + 1 + (w - 2 - tlen) / 2);
        ui_write_str(tx, (u8)(row+1), title, PAL_HI);
    }
}

void ui_draw_bar(u8 col, u8 row, u8 w, s16 cur, s16 max, u8 pal)
{
    u8 filled, c;
    if (max <= 0) max = 1;
    if (cur < 0)  cur = 0;
    filled = (u8)((u32)cur * w / (u32)max);
    UI_SB[row * 32 + col] = SE_ENTRY(TILE_BAR_LEFT, pal);
    for (c = (u8)(col + 1); c < col + w - 1; c++) {
        u16 tid = (c - col - 1 < filled) ? TILE_BAR_FULL : TILE_BAR_EMPTY;
        UI_SB[row * 32 + c] = SE_ENTRY(tid, pal);
    }
    UI_SB[row * 32 + (col+w-1)] = SE_ENTRY(TILE_BAR_RIGHT, pal);
}

void ui_draw_cursor(u8 col, u8 row)
{
    /* Animate through 4 cursor frames based on anim_tick */
    u16 frames[4] = { TILE_CURSOR, TILE_CURSOR2, TILE_CURSOR3, TILE_CURSOR4 };
    u8  frame = (u8)((g_menu.anim_tick >> 3) & 3); /* change every 8 frames */
    UI_SB[row * 32 + col] = SE_ENTRY(frames[frame], PAL_CURSOR);
}

/* ---- Main menu render ----------------------------------- */

static void _render_main_menu(void)
{
    u8 i;
    /* Window: cols 8-21, rows 4-13 */
    ui_draw_window_titled(8, 4, 14, 10, "MENU");
    for (i = 0; i < MAIN_ITEM_COUNT; i++) {
        u8 row = (u8)(6 + i);
        u8 pal = (i == g_menu.cursor) ? PAL_HI : PAL_TEXT;
        ui_write_str(11, row, s_main_labels[i], pal);
    }
    ui_draw_cursor(9, (u8)(6 + g_menu.cursor));
}

/* ---- Public API ---------------------------------------- */

void menu_init(void)
{
    g_menu.current  = MENU_CLOSED;
    g_menu.prev     = MENU_CLOSED;
    g_menu.cursor   = 0;
    g_menu.scroll   = 0;
    g_menu.sub_cursor = 0;
    g_menu.anim_tick  = 0;
    g_menu.dirty      = FALSE;
    g_menu.char_idx   = 0;
    s_keys_prev = 0;
    s_keys_cur  = 0xFFFF; /* all released */
}

BOOL menu_is_open(void)
{
    return (BOOL)(g_menu.current != MENU_CLOSED);
}

void menu_open(MenuID id)
{
    g_menu.prev    = g_menu.current;
    g_menu.current = id;
    g_menu.cursor  = 0;
    g_menu.scroll  = 0;
    g_menu.dirty   = TRUE;

    switch (id) {
    case MENU_INVENTORY:  menu_inventory_open();          break;
    case MENU_EQUIPMENT:  menu_equip_open(g_menu.char_idx); break;
    case MENU_QUEST_LOG:  menu_quest_open();              break;
    case MENU_STATUS:     menu_status_open(g_menu.char_idx); break;
    case MENU_SAVE:       menu_save_open(SAVE_SCREEN_SAVE);  break;
    case MENU_CRAFTING:   menu_crafting_open();               break;
    default: break;
    }
}

void menu_close(void)
{
    g_menu.current = MENU_CLOSED;
    ui_clear_sb();
}

void menu_update(void)
{
    u16 pressed;
    if (g_menu.current == MENU_CLOSED) return;

    _keys_update();
    pressed = _keys_down();
    g_menu.anim_tick++;

    switch (g_menu.current) {
    case MENU_MAIN:
        if (pressed & KEY_UP) {
            if (g_menu.cursor > 0) g_menu.cursor--;
            else g_menu.cursor = MAIN_ITEM_COUNT - 1;
            g_menu.dirty = TRUE;
        }
        if (pressed & KEY_DOWN) {
            if (g_menu.cursor < MAIN_ITEM_COUNT - 1) g_menu.cursor++;
            else g_menu.cursor = 0;
            g_menu.dirty = TRUE;
        }
        if (pressed & KEY_A) {
            switch (g_menu.cursor) {
            case 0: menu_open(MENU_INVENTORY);  break;
            case 1: menu_open(MENU_EQUIPMENT);  break;
            case 2: menu_open(MENU_QUEST_LOG);  break;
            case 3: menu_open(MENU_STATUS);     break;
            case 4: menu_open(MENU_SAVE);       break;
            case 5: menu_save_open(SAVE_SCREEN_LOAD); g_menu.current = MENU_SAVE; break;
            case 6: menu_close(); return;
            default: break;
            }
        }
        if (pressed & KEY_B) menu_close();
        if (pressed & KEY_START) menu_close();
        break;

    case MENU_INVENTORY:  menu_inventory_update();  break;
    case MENU_EQUIPMENT:  menu_equip_update();      break;
    case MENU_QUEST_LOG:  menu_quest_update();      break;
    case MENU_STATUS:     menu_status_update();     break;
    case MENU_SHOP:       menu_shop_update();       break;
    case MENU_SAVE:       menu_save_update();       break;
    case MENU_CRAFTING:   menu_crafting_update();   break;
    default: break;
    }

    /* B to go back to main from any sub-menu */
    if (g_menu.current != MENU_CLOSED &&
        g_menu.current != MENU_MAIN &&
        (_keys_down() & KEY_B)) {
        g_menu.current = MENU_MAIN;
        g_menu.cursor  = 0;
        g_menu.dirty   = TRUE;
    }
}

void menu_render(void)
{
    if (g_menu.current == MENU_CLOSED) return;
    if (!g_menu.dirty) {
        /* Still update cursor animation every frame */
        if (g_menu.current == MENU_MAIN || g_menu.current == MENU_INVENTORY ||
            g_menu.current == MENU_EQUIPMENT || g_menu.current == MENU_QUEST_LOG ||
            g_menu.current == MENU_STATUS || g_menu.current == MENU_SHOP ||
            g_menu.current == MENU_SAVE   || g_menu.current == MENU_CRAFTING) {
            /* Cursor blink needs periodic redraw */
            if ((g_menu.anim_tick & 7) == 0) g_menu.dirty = TRUE;
        }
        if (!g_menu.dirty) return;
    }

    ui_clear_sb();

    switch (g_menu.current) {
    case MENU_MAIN:       _render_main_menu();       break;
    case MENU_INVENTORY:  menu_inventory_render();   break;
    case MENU_EQUIPMENT:  menu_equip_render();       break;
    case MENU_QUEST_LOG:  menu_quest_render();       break;
    case MENU_STATUS:     menu_status_render();      break;
    case MENU_SHOP:       menu_shop_render();        break;
    case MENU_SAVE:       menu_save_render();        break;
    case MENU_CRAFTING:   menu_crafting_render();    break;
    default: break;
    }

    g_menu.dirty = FALSE;
}
