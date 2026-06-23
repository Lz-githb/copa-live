#include "menu_save.h"
#include "menu_types.h"
#include "save.h"
#include "character.h"
#include "gba_types.h"

/* =========================================================
 * Save / Load Screen Implementation
 * ========================================================= */

SaveMenuState g_save_menu;

static BOOL s_done;

#define KEY_A      0x0001
#define KEY_B      0x0002
#define KEY_UP     0x0040
#define KEY_DOWN   0x0080
#define KEY_LEFT   0x0020
#define KEY_RIGHT  0x0010
#define KEY_L      0x0200
#define KEY_R      0x0100
#define REG_KEYINPUT (*(vu16*)(0x04000130))

static u16 s_svp;
static u16 s_svc = 0xFFFF;

static u16 _svpressed(void)
{
    u16 now = REG_KEYINPUT;
    u16 p   = (~now) & s_svp;
    s_svp   = ~now;
    s_svc   = now;
    (void)s_svc;
    return p;
}

/* ---- Format play time as HH:MM:SS ---------------------- */
static void _fmt_time(char* buf, u32 secs)
{
    u32 h = secs / 3600;
    u32 m = (secs % 3600) / 60;
    u32 s = secs % 60;
    buf[0] = (char)('0' + h / 10); buf[1] = (char)('0' + h % 10);
    buf[2] = ':';
    buf[3] = (char)('0' + m / 10); buf[4] = (char)('0' + m % 10);
    buf[5] = ':';
    buf[6] = (char)('0' + s / 10); buf[7] = (char)('0' + s % 10);
    buf[8] = '\0';
}

/* ---- Open ---------------------------------------------- */

void menu_save_open(SaveScreenMode mode)
{
    u8 i;
    g_save_menu.mode       = mode;
    g_save_menu.cursor     = 0;
    g_save_menu.confirming = FALSE;
    s_done = FALSE;

    for (i = 0; i < SAVE_SLOT_COUNT; i++) {
        g_save_menu.slot_valid[i] = save_read(i, &g_save_menu.previews[i]);
    }
    s_svp = 0;
    s_svc = 0xFFFF;
}

BOOL menu_save_is_done(void)
{
    return s_done;
}

/* ---- Update -------------------------------------------- */

void menu_save_update(void)
{
    u16 pressed = _svpressed();

    if (g_save_menu.confirming) {
        if (pressed & KEY_LEFT) {
            /* "YES" side — execute */
            if (g_save_menu.mode == SAVE_SCREEN_SAVE) {
                SaveSlot slot;
                save_capture(g_save_menu.cursor, &slot);
                save_write(g_save_menu.cursor, &slot);
                /* Refresh preview */
                g_save_menu.slot_valid[g_save_menu.cursor] = TRUE;
                g_save_menu.previews[g_save_menu.cursor]   = slot;
            } else {
                if (g_save_menu.slot_valid[g_save_menu.cursor])
                    save_apply(&g_save_menu.previews[g_save_menu.cursor]);
            }
            g_save_menu.confirming = FALSE;
            s_done = TRUE;
            g_menu.dirty = TRUE;
        }
        if (pressed & (KEY_B | KEY_RIGHT)) {
            g_save_menu.confirming = FALSE;
            g_menu.dirty = TRUE;
        }
        return;
    }

    if (pressed & KEY_UP) {
        if (g_save_menu.cursor > 0) g_save_menu.cursor--;
        g_menu.dirty = TRUE;
    }
    if (pressed & KEY_DOWN) {
        if (g_save_menu.cursor < SAVE_SLOT_COUNT - 1) g_save_menu.cursor++;
        g_menu.dirty = TRUE;
    }
    if (pressed & KEY_L) {
        g_save_menu.mode = SAVE_SCREEN_SAVE;
        g_menu.dirty = TRUE;
    }
    if (pressed & KEY_R) {
        g_save_menu.mode = SAVE_SCREEN_LOAD;
        g_menu.dirty = TRUE;
    }

    if (pressed & KEY_A) {
        /* Load requires valid slot */
        if (g_save_menu.mode == SAVE_SCREEN_LOAD &&
            !g_save_menu.slot_valid[g_save_menu.cursor]) return;
        g_save_menu.confirming = TRUE;
        g_menu.dirty = TRUE;
    }
}

/* ---- Render slot card ---------------------------------- */
static void _render_slot_card(u8 slot, u8 top_row)
{
    BOOL valid = g_save_menu.slot_valid[slot];
    BOOL sel   = (BOOL)(slot == g_save_menu.cursor);
    u8   pal   = sel ? PAL_HI : PAL_TEXT;

    ui_draw_window((u8)2, top_row, 26, 4);
    if (sel) ui_draw_cursor(3, (u8)(top_row + 1));

    /* Slot number */
    ui_write_str(5, (u8)(top_row + 1), "Slot ", PAL_DIM);
    ui_write_num(10, (u8)(top_row + 1), (s32)(slot + 1), pal);

    if (!valid) {
        ui_write_str(14, (u8)(top_row + 1), "--- Empty ---", PAL_DIM);
        return;
    }

    {
        const SaveSlot* sv = &g_save_menu.previews[slot];
        char timebuf[9];
        _fmt_time(timebuf, sv->play_time);

        /* Party name + level from stats */
        ui_write_str(14, (u8)(top_row + 1), g_party[0].name, pal);
        ui_write_str(22, (u8)(top_row + 1), "Lv.", PAL_DIM);
        ui_write_num(25, (u8)(top_row + 1), (s32)sv->stats.level, pal);

        /* Location + time */
        ui_write_str(5, (u8)(top_row + 2), "Map:", PAL_DIM);
        ui_write_num(9, (u8)(top_row + 2), (s32)sv->location.map_id, PAL_TEXT);
        ui_write_str(18, (u8)(top_row + 2), timebuf, PAL_DIM);
    }
}

/* ---- Render -------------------------------------------- */

void menu_save_render(void)
{
    u8 i;
    ui_draw_window_titled(0, 0, 30, 20, "SAVE / LOAD");

    /* Mode tabs */
    ui_write_str(4, 3,
        "SAVE", g_save_menu.mode == SAVE_SCREEN_SAVE ? PAL_HI : PAL_DIM);
    ui_write_str(18, 3,
        "LOAD", g_save_menu.mode == SAVE_SCREEN_LOAD ? PAL_HI : PAL_DIM);

    /* Divider */
    for (i = 1; i < 29; i++)
        UI_SB[4 * 32 + i] = SE_ENTRY(TILE_WIN_TOP, PAL_BORDER);

    /* Three slot cards at rows 5, 9, 13 */
    for (i = 0; i < SAVE_SLOT_COUNT; i++)
        _render_slot_card(i, (u8)(5 + i * 4));

    /* Divider */
    for (i = 1; i < 29; i++)
        UI_SB[17 * 32 + i] = SE_ENTRY(TILE_WIN_TOP, PAL_BORDER);

    /* Confirm dialog overlay */
    if (g_save_menu.confirming) {
        ui_draw_window(5, 8, 20, 4);
        if (g_save_menu.mode == SAVE_SCREEN_SAVE)
            ui_write_str(8, 9, "Save to this slot?", PAL_HI);
        else
            ui_write_str(8, 9, "Load this file?", PAL_HI);
        ui_write_str(7,  11, "YES", PAL_AVAIL);
        ui_write_str(18, 11, "NO",  PAL_LOCKED);
        ui_draw_cursor(6, 11);
    } else {
        /* Controls */
        ui_write_str(3, 18, "A:Select  L/R:Mode  B:Back", PAL_DIM);
    }
}
