#include "menu_equip.h"
#include "menu_types.h"
#include "character.h"
#include "item_db.h"
#include "inventory.h"
#include "char_equip.h"
#include "gba_types.h"

/* =========================================================
 * Equipment Menu Implementation
 * ========================================================= */

EquipMenuState g_equip_menu;

#define KEY_A      0x0001
#define KEY_B      0x0002
#define KEY_LEFT   0x0020
#define KEY_UP     0x0040
#define KEY_DOWN   0x0080
#define KEY_L      0x0200
#define KEY_R      0x0100
#define REG_KEYINPUT (*(vu16*)(0x04000130))

static u16 s_eq_prev;
static u16 s_eq_cur = 0xFFFF;

static u16 _eq_pressed(void)
{
    u16 now = REG_KEYINPUT;
    u16 p   = (~now) & s_eq_prev;
    s_eq_prev = ~now;
    s_eq_cur  = now;
    (void)s_eq_cur;
    return p;
}

/* ---- Slot names ---------------------------------------- */
static const char* const s_slot_names[EQUIP_SLOT_COUNT] = {
    "WEAPON ", "OFFHAND", "HELM   ", "ARMOR  ",
    "ACC 1  ", "ACC 2  "
};

/* ---- Icon per slot ------------------------------------- */
static u16 _slot_icon(u8 slot)
{
    switch (slot) {
    case EQUIP_SLOT_WEAPON:  return TILE_ICON_SWORD;
    case EQUIP_SLOT_OFFHAND: return TILE_ICON_OFFHAND;
    case EQUIP_SLOT_HELM:    return TILE_ICON_HELM;
    case EQUIP_SLOT_ARMOR:   return TILE_ICON_ARMOR;
    case EQUIP_SLOT_ACC1:    return TILE_ICON_RING;
    case EQUIP_SLOT_ACC2:    return TILE_ICON_AMULET;
    default:                 return TILE_ICON_SWORD;
    }
}

/* ---- Build swap list ----------------------------------- */
void menu_equip_rebuild_swap(void)
{
    u8 i;
    u8 slot = g_equip_menu.slot_cursor;
    const Character* ch = &g_party[g_menu.char_idx];

    g_equip_menu.swap_count  = 0;
    g_equip_menu.swap_cursor = 0;
    g_equip_menu.swap_scroll = 0;

    for (i = 0; i < g_bag.count; i++) {
        u8 item_id = g_bag.slots[i].item_id;
        if (item_id == ITEM_NONE) continue;
        {
            const ItemDef* d = item_get(item_id);
            if (!d || d->category != ICAT_EQUIPMENT) continue;
            {
                const EquipDef* ed = equip_get(d->equip_id);
                if (!ed || ed->slot != slot) continue;
            }
            if (!equip_can_use(ch, item_id)) continue;
            g_equip_menu.swap_ids[g_equip_menu.swap_count++] = i;
        }
    }
}

/* ---- Stat label pairs ---------------------------------- */
static const char* const s_dstat_labels[DSTAT_COUNT] = {
    "HP  ", "MP  ", "ATK ", "DEF ", "MAG ", "MDEF", "SPD ", "CRIT", "EVA "
};

/* ---- Open ---------------------------------------------- */

void menu_equip_open(u8 char_idx)
{
    g_menu.char_idx              = char_idx;
    g_equip_menu.slot_cursor     = 0;
    g_equip_menu.in_swap_list    = FALSE;
    g_equip_menu.swap_count      = 0;
    s_eq_prev = 0;
    s_eq_cur  = 0xFFFF;
}

/* ---- Update -------------------------------------------- */

void menu_equip_update(void)
{
    u16 pressed = _eq_pressed();

    /* Switch character with L/R */
    if (pressed & KEY_L) {
        if (g_menu.char_idx > 0) g_menu.char_idx--;
        else g_menu.char_idx = (u8)(g_party_size - 1);
        g_equip_menu.in_swap_list = FALSE;
        g_menu.dirty = TRUE;
    }
    if (pressed & KEY_R) {
        if (g_menu.char_idx < g_party_size - 1) g_menu.char_idx++;
        else g_menu.char_idx = 0;
        g_equip_menu.in_swap_list = FALSE;
        g_menu.dirty = TRUE;
    }

    if (!g_equip_menu.in_swap_list) {
        /* Navigate slot list */
        if (pressed & KEY_UP) {
            if (g_equip_menu.slot_cursor > 0) g_equip_menu.slot_cursor--;
            else g_equip_menu.slot_cursor = (u8)(EQUIP_SLOT_COUNT - 1);
            g_menu.dirty = TRUE;
        }
        if (pressed & KEY_DOWN) {
            if (g_equip_menu.slot_cursor < EQUIP_SLOT_COUNT - 1) g_equip_menu.slot_cursor++;
            else g_equip_menu.slot_cursor = 0;
            g_menu.dirty = TRUE;
        }
        if (pressed & KEY_A) {
            menu_equip_rebuild_swap();
            g_equip_menu.in_swap_list = TRUE;
            g_menu.dirty = TRUE;
        }
    } else {
        /* Navigate swap list */
        if (pressed & KEY_UP) {
            if (g_equip_menu.swap_cursor > 0) g_equip_menu.swap_cursor--;
            g_menu.dirty = TRUE;
        }
        if (pressed & KEY_DOWN) {
            if (g_equip_menu.swap_cursor < g_equip_menu.swap_count - 1)
                g_equip_menu.swap_cursor++;
            g_menu.dirty = TRUE;
        }
        if (pressed & KEY_A) {
            if (g_equip_menu.swap_count > 0) {
                u8 bag_idx = g_equip_menu.swap_ids[g_equip_menu.swap_cursor];
                u8 item_id = g_bag.slots[bag_idx].item_id;
                Character* ch = &g_party[g_menu.char_idx];
                /* Unequip current → bag, equip new */
                {
                    u8 old = ch->equip[g_equip_menu.slot_cursor];
                    if (old != ITEM_NONE) bag_add(old, 1);
                }
                bag_remove(item_id, 1);
                ch->equip[g_equip_menu.slot_cursor] = item_id;
                character_recompute(ch);
            }
            g_equip_menu.in_swap_list = FALSE;
            g_menu.dirty = TRUE;
        }
        if (pressed & KEY_B) {
            g_equip_menu.in_swap_list = FALSE;
            g_menu.dirty = TRUE;
        }
    }
}

/* ---- Render -------------------------------------------- */

void menu_equip_render(void)
{
    u8 i;
    const Character* ch = &g_party[g_menu.char_idx];

    /* Main window */
    ui_draw_window(0, 0, 30, 20);

    /* Title row: character name + class */
    ui_write_str(2, 1, "EQUIPMENT", PAL_HI);
    ui_write_str(16, 1, ch->name, PAL_TEXT);

    /* Divider */
    for (i = 1; i < 29; i++)
        UI_SB[2 * 32 + i] = SE_ENTRY(TILE_WIN_TOP, PAL_BORDER);

    /* Slot list: rows 3-9 (7 slots) */
    for (i = 0; i < EQUIP_SLOT_COUNT && i < EQUIP_SLOT_ROWS; i++) {
        u8   row      = (u8)(3 + i);
        u8   item_id  = ch->equip[i];
        BOOL selected = (BOOL)(i == g_equip_menu.slot_cursor);
        u8   pal      = selected ? PAL_HI : PAL_TEXT;

        if (selected && !g_equip_menu.in_swap_list)
            ui_draw_cursor(1, row);

        UI_SB[row * 32 + 3] = SE_ENTRY(_slot_icon(i), pal);
        ui_write_str(5, row, s_slot_names[i], PAL_DIM);

        if (item_id != ITEM_NONE) {
            const ItemDef* d = item_get(item_id);
            if (d) ui_write_str(13, row, d->name, pal);
        } else {
            ui_write_str(13, row, "---", PAL_DIM);
        }
    }

    /* Stat panel (right side) — derived stats of this character */
    for (i = 0; i < DSTAT_COUNT && i < 9; i++) {
        u8 row = (u8)(3 + i);
        ui_write_str(22, row, s_dstat_labels[i], PAL_DIM);
        ui_write_num(27, row, (s32)ch->derived.val[i], PAL_TEXT);
    }

    /* Divider at row 10 */
    for (i = 1; i < 29; i++)
        UI_SB[10 * 32 + i] = SE_ENTRY(TILE_WIN_TOP, PAL_BORDER);

    /* Equipped item description row 11-12 */
    {
        u8 item_id = ch->equip[g_equip_menu.slot_cursor];
        const ItemDef* d = item_get(item_id);
        if (d) {
            ui_write_str(2, 11, d->name, PAL_HI);
            ui_write_str(2, 12, d->description, PAL_TEXT);
        } else {
            ui_write_str(2, 11, "(empty slot)", PAL_DIM);
        }
    }

    /* Divider at row 13 */
    for (i = 1; i < 29; i++)
        UI_SB[13 * 32 + i] = SE_ENTRY(TILE_WIN_TOP, PAL_BORDER);

    /* Swap list rows 14-18 */
    if (g_equip_menu.in_swap_list) {
        ui_write_str(2, 14, "Select item:", PAL_DIM);
        if (g_equip_menu.swap_count == 0) {
            ui_write_str(4, 15, "(none available)", PAL_DIM);
        } else {
            u8 vis = (g_equip_menu.swap_count < EQUIP_SWAP_VISIBLE)
                      ? g_equip_menu.swap_count : EQUIP_SWAP_VISIBLE;
            for (i = 0; i < vis; i++) {
                u8 sc       = (u8)(g_equip_menu.swap_scroll + i);
                u8 bag_idx  = g_equip_menu.swap_ids[sc];
                u8 item_id  = g_bag.slots[bag_idx].item_id;
                const ItemDef* d = item_get(item_id);
                u8 row      = (u8)(15 + i);
                u8 pal      = (sc == g_equip_menu.swap_cursor) ? PAL_HI : PAL_TEXT;
                if (sc == g_equip_menu.swap_cursor) ui_draw_cursor(3, row);
                if (d) ui_write_str(5, row, d->name, pal);
            }
        }
    } else {
        ui_write_str(2, 14, "A:Change  L/R:Char  B:Back", PAL_DIM);
    }

    /* Character switch indicator row 19 */
    {
        u8 tx = 2;
        u8 ci;
        for (ci = 0; ci < g_party_size; ci++) {
            u8 pal = (ci == g_menu.char_idx) ? PAL_HI : PAL_DIM;
            ui_write_str(tx, 19, g_party[ci].name, pal);
            tx += 9;
        }
    }
}
