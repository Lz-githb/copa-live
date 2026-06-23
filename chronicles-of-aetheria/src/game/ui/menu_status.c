#include "menu_status.h"
#include "menu_types.h"
#include "character.h"
#include "char_stats.h"
#include "char_types.h"
#include "gba_types.h"

/* =========================================================
 * Character Status Screen Implementation
 * ========================================================= */

#define KEY_L      0x0200
#define KEY_R      0x0100
#define KEY_B      0x0002
#define REG_KEYINPUT (*(vu16*)(0x04000130))

static u16 s_sp;
static u16 s_sc = 0xFFFF;

static u16 _sp(void)
{
    u16 now = REG_KEYINPUT;
    u16 p   = (~now) & s_sp;
    s_sp    = ~now;
    s_sc    = now;
    (void)s_sc;
    return p;
}

/* ---- Attribute labels ---------------------------------- */
static const char* const s_attr_labels[ATTR_COUNT] = {
    "VIT", "END", "STR", "DEX", "INT", "WIS", "LCK", "CHA"
};

/* ---- Derived stat labels ------------------------------- */
static const char* const s_dstat_labels2[DSTAT_COUNT] = {
    "HP ", "MP ", "ATK", "DEF", "MAG", "MDF", "SPD", "CRT", "EVA"
};

/* ---- Open / Update / Render ---------------------------- */

void menu_status_open(u8 char_idx)
{
    g_menu.char_idx = char_idx;
    s_sp = 0;
    s_sc = 0xFFFF;
}

void menu_status_update(void)
{
    u16 pressed = _sp();
    if (pressed & KEY_L) {
        if (g_menu.char_idx > 0) g_menu.char_idx--;
        else g_menu.char_idx = (u8)(g_party_size - 1);
        g_menu.dirty = TRUE;
    }
    if (pressed & KEY_R) {
        if (g_menu.char_idx < g_party_size - 1) g_menu.char_idx++;
        else g_menu.char_idx = 0;
        g_menu.dirty = TRUE;
    }
}

void menu_status_render(void)
{
    u8 i;
    const Character* ch = &g_party[g_menu.char_idx];

    ui_draw_window(0, 0, 30, 20);

    /* Title + character switch */
    ui_write_str(2, 1, "STATUS", PAL_HI);
    {
        u8 tx = 12;
        for (i = 0; i < g_party_size; i++) {
            u8 pal = (i == g_menu.char_idx) ? PAL_HI : PAL_DIM;
            ui_write_str(tx, 1, g_party[i].name, pal);
            tx += 9;
        }
    }

    /* Divider */
    for (i = 1; i < 29; i++)
        UI_SB[2 * 32 + i] = SE_ENTRY(TILE_WIN_TOP, PAL_BORDER);

    /* Character info block rows 3-8 (left side cols 1-13) */
    ui_write_str(2, 3, ch->name, PAL_HI);
    ui_write_str(9, 3, "Lv.", PAL_DIM);
    ui_write_num_padded(12, 3, (s32)ch->level, 2, PAL_TEXT);

    ui_write_str(2, 4, character_class_name(ch), PAL_DIM);

    /* HP bar */
    UI_SB[5 * 32 + 2] = SE_ENTRY(TILE_ICON_HP, PAL_HP);
    ui_draw_bar(3, 5, 8, ch->hp_cur, ch->derived.val[DSTAT_HP_MAX], PAL_HP);
    ui_write_num(12, 5, (s32)ch->hp_cur, PAL_TEXT);
    ui_write_char(15, 5, '/', PAL_DIM);
    ui_write_num(16, 5, (s32)ch->derived.val[DSTAT_HP_MAX], PAL_DIM);

    /* MP bar */
    UI_SB[6 * 32 + 2] = SE_ENTRY(TILE_ICON_MP, PAL_MP);
    ui_draw_bar(3, 6, 8, ch->mp_cur, ch->derived.val[DSTAT_MP_MAX], PAL_MP);
    ui_write_num(12, 6, (s32)ch->mp_cur, PAL_TEXT);
    ui_write_char(15, 6, '/', PAL_DIM);
    ui_write_num(16, 6, (s32)ch->derived.val[DSTAT_MP_MAX], PAL_DIM);

    /* EXP bar */
    UI_SB[7 * 32 + 2] = SE_ENTRY(TILE_ICON_EXP, PAL_EXP);
    if (ch->level < 99) {
        u32 cur_thresh  = xp_required_for_level(ch->level);
        u32 next_thresh = xp_required_for_level((u8)(ch->level + 1));
        u32 gained      = ch->xp_total - cur_thresh;
        u32 needed      = next_thresh  - cur_thresh;
        ui_draw_bar(3, 7, 8, (s16)(gained < 32767 ? gained : 32767),
                              (s16)(needed < 32767 ? needed : 32767), PAL_EXP);
        ui_write_num(12, 7, (s32)(next_thresh - ch->xp_total), PAL_DIM);
        ui_write_str(18, 7, "to Lv+", PAL_DIM);
    } else {
        ui_write_str(3, 7, "MAX LEVEL", PAL_HI);
    }

    /* Divider */
    for (i = 1; i < 29; i++)
        UI_SB[9 * 32 + i] = SE_ENTRY(TILE_WIN_TOP, PAL_BORDER);

    /* Derived stats: left column rows 10-17 */
    for (i = 0; i < DSTAT_COUNT; i++) {
        u8 row = (u8)(10 + i);
        ui_write_str(2, row, s_dstat_labels2[i], PAL_DIM);
        ui_write_num_padded(7, row, (s32)ch->derived.val[i], 4, PAL_TEXT);
    }

    /* Base attributes: right column rows 10-17 */
    for (i = 0; i < ATTR_COUNT; i++) {
        u8 row = (u8)(10 + i);
        ui_write_str(16, row, s_attr_labels[i], PAL_DIM);
        ui_write_num_padded(20, row, (s32)(ch->base.val[i] + ch->base.bonus[i]), 3, PAL_TEXT);
        /* Show bonus in parens if non-zero */
        if (ch->base.bonus[i] != 0) {
            ui_write_char(24, row, ch->base.bonus[i] > 0 ? '+' : '-', PAL_HP);
            ui_write_num(25, row, (s32)(ch->base.bonus[i] > 0
                          ? ch->base.bonus[i] : -ch->base.bonus[i]), PAL_HP);
        }
    }

    /* Status icons row 18 */
    ui_write_str(2, 18, "Status:", PAL_DIM);
    if (ch->persist_status == 0) {
        ui_write_str(10, 18, "Normal", PAL_AVAIL);
    } else {
        u8 tx = 10;
        if (ch->persist_status & 0x01) { ui_write_str(tx, 18, "PSN", PAL_LOCKED); tx += 4; }
        if (ch->persist_status & 0x02) { ui_write_str(tx, 18, "CRS", PAL_LOCKED); tx += 4; }
    }

    ui_write_str(2, 19, "L/R:Switch char  B:Back", PAL_DIM);
}
