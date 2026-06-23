#include "menu_inventory.h"
#include "menu_types.h"
#include "item_db.h"
#include "inventory.h"
#include "item_use.h"
#include "char_equip.h"
#include "gba_types.h"

/* =========================================================
 * Inventory Menu Implementation
 * ========================================================= */

InvMenuState g_inv_menu;


/* Key bitmasks (shared with menu_core, but redefined locally) */
#define KEY_A      0x0001
#define KEY_B      0x0002
#define KEY_LEFT   0x0020
#define KEY_UP     0x0040
#define KEY_DOWN   0x0080
#define KEY_L      0x0200
#define KEY_R      0x0100
#define REG_KEYINPUT (*(vu16*)(0x04000130))

static u16 s_prev;
static u16 s_cur = 0xFFFF;

static u16 _pressed(void)
{
    u16 now = REG_KEYINPUT;
    u16 p   = (~now) & s_prev;
    s_prev  = ~now;
    s_cur   = now;
    (void)s_cur;
    return p;
}

/* ---- Category filter ------------------------------------ */

static BOOL _item_in_tab(u8 item_id, InventoryTab tab)
{
    if (tab == ITAB_ALL) return TRUE;
    if (item_id == ITEM_NONE) return FALSE;
    {
        const ItemDef* d = item_get(item_id);
        if (!d) return FALSE;
        switch (tab) {
        case ITAB_EQUIP:    return (BOOL)(d->category == ICAT_EQUIPMENT);
        case ITAB_USE:      return (BOOL)(d->category == ICAT_CONSUMABLE);
        case ITAB_MATERIAL: return (BOOL)(d->category == ICAT_MATERIAL);
        case ITAB_KEY:      return (BOOL)(d->category == ICAT_KEY_ITEM);
        default:            return FALSE;
        }
    }
}

void menu_inventory_rebuild_filter(void)
{
    u8 i;
    g_inv_menu.filtered_count = 0;
    for (i = 0; i < g_bag.count; i++) {
        if (g_bag.slots[i].item_id != ITEM_NONE &&
            _item_in_tab(g_bag.slots[i].item_id, g_inv_menu.tab)) {
            g_inv_menu.filtered[g_inv_menu.filtered_count++] = i;
        }
    }
    /* Clamp cursor */
    if (g_inv_menu.cursor >= g_inv_menu.filtered_count && g_inv_menu.filtered_count > 0)
        g_inv_menu.cursor = (u8)(g_inv_menu.filtered_count - 1);
    if (g_inv_menu.filtered_count == 0) g_inv_menu.cursor = 0;
}

u8 menu_inventory_selected_id(void)
{
    u8 slot_idx;
    if (g_inv_menu.filtered_count == 0) return ITEM_NONE;
    slot_idx = g_inv_menu.filtered[g_inv_menu.cursor];
    return g_bag.slots[slot_idx].item_id;
}

/* ---- Icon tile for item type ---------------------------- */
static u16 _item_icon(u8 item_id)
{
    const ItemDef* d = item_get(item_id);
    if (!d) return TILE_ICON_KEY;
    switch (d->category) {
    case ICAT_EQUIPMENT: {
        /* Use the equip table slot field for icon */
        const EquipDef* ed = equip_get(d->equip_id);
        if (!ed) return TILE_ICON_SWORD;
        switch (ed->slot) {
        case EQUIP_SLOT_WEAPON:  return TILE_ICON_SWORD;
        case EQUIP_SLOT_OFFHAND: return TILE_ICON_OFFHAND;
        case EQUIP_SLOT_HELM:    return TILE_ICON_HELM;
        case EQUIP_SLOT_ARMOR:   return TILE_ICON_ARMOR;
        case EQUIP_SLOT_ACC1:
        case EQUIP_SLOT_ACC2:    return TILE_ICON_RING;
        default:                 return TILE_ICON_SWORD;
        }
    }
    case ICAT_CONSUMABLE:  return TILE_ICON_POTION;
    case ICAT_MATERIAL:    return TILE_ICON_MATERIAL;
    case ICAT_KEY_ITEM:    return TILE_ICON_KEY;
    default:               return TILE_ICON_LEGENDARY;
    }
}

/* ---- Tab labels ---------------------------------------- */
static const char* const s_tab_labels[ITAB_COUNT] = {
    "ALL", "EQUIP", "USE", "MAT", "KEY"
};

/* ---- Open ---------------------------------------------- */

void menu_inventory_open(void)
{
    g_inv_menu.tab    = ITAB_ALL;
    g_inv_menu.cursor = 0;
    g_inv_menu.scroll = 0;
    menu_inventory_rebuild_filter();
    s_prev = 0;
    s_cur  = 0xFFFF;
}

/* ---- Update -------------------------------------------- */

void menu_inventory_update(void)
{
    u16 pressed = _pressed();
    u8  fc      = g_inv_menu.filtered_count;

    /* Tab switch with L/R */
    if (pressed & KEY_L) {
        if (g_inv_menu.tab > 0) g_inv_menu.tab--;
        else g_inv_menu.tab = (InventoryTab)(ITAB_COUNT - 1);
        g_inv_menu.cursor = 0;
        g_inv_menu.scroll = 0;
        menu_inventory_rebuild_filter();
        g_menu.dirty = TRUE;
    }
    if (pressed & KEY_R) {
        if (g_inv_menu.tab < ITAB_COUNT - 1) g_inv_menu.tab++;
        else g_inv_menu.tab = ITAB_ALL;
        g_inv_menu.cursor = 0;
        g_inv_menu.scroll = 0;
        menu_inventory_rebuild_filter();
        g_menu.dirty = TRUE;
    }

    if (fc == 0) return;

    if (pressed & KEY_UP) {
        if (g_inv_menu.cursor > 0) g_inv_menu.cursor--;
        else g_inv_menu.cursor = (u8)(fc - 1);
        g_menu.dirty = TRUE;
    }
    if (pressed & KEY_DOWN) {
        if (g_inv_menu.cursor < fc - 1) g_inv_menu.cursor++;
        else g_inv_menu.cursor = 0;
        g_menu.dirty = TRUE;
    }

    /* Scroll adjustment */
    if (g_inv_menu.cursor < g_inv_menu.scroll)
        g_inv_menu.scroll = g_inv_menu.cursor;
    if (g_inv_menu.cursor >= g_inv_menu.scroll + INV_VISIBLE_ROWS)
        g_inv_menu.scroll = (u8)(g_inv_menu.cursor - INV_VISIBLE_ROWS + 1);

    /* A = use/equip item */
    if (pressed & KEY_A) {
        u8 item_id = menu_inventory_selected_id();
        if (item_id != ITEM_NONE) {
            item_use(item_id, 0); /* target char 0 by default */
            menu_inventory_rebuild_filter();
            g_menu.dirty = TRUE;
        }
    }
}

/* ---- Render -------------------------------------------- */

void menu_inventory_render(void)
{
    u8 i, vis;
    char gold_str[12];
    u32 g;

    /* Main window */
    ui_draw_window_titled(0, 0, 30, 20, "INVENTORY");

    /* Gold display (top-right) */
    g = g_bag.gold;
    {
        u8 di = 10;
        char tmp[8];
        u8 j = 0;
        u32 n = g;
        if (n == 0) { tmp[j++] = '0'; }
        else { while (n) { tmp[j++] = (char)('0' + n % 10); n /= 10; } }
        /* reverse */
        { u8 a = 0, b = (u8)(j-1); while (a<b) { char t=tmp[a]; tmp[a]=tmp[b]; tmp[b]=t; a++; b--; } }
        tmp[j] = '\0';
        gold_str[0] = 'G'; gold_str[1] = ':';
        { u8 k=0; while(tmp[k]) gold_str[2+k]=tmp[k], k++; gold_str[2+k]='\0'; }
        ui_write_str(19, 1, gold_str, PAL_HI);
    }
    UI_SB[1 * 32 + 18] = SE_ENTRY(TILE_ICON_COIN, PAL_HI);

    /* Category tabs (row 3) */
    {
        u8 tx = 2;
        for (i = 0; i < ITAB_COUNT; i++) {
            u8 pal = (i == (u8)g_inv_menu.tab) ? PAL_HI : PAL_DIM;
            ui_write_str(tx, 3, s_tab_labels[i], pal);
            tx += 6;
        }
    }

    /* Separator row 4 */
    for (i = 1; i < 29; i++)
        UI_SB[4 * 32 + i] = SE_ENTRY(TILE_WIN_TOP, PAL_BORDER);

    /* Item list */
    vis = (g_inv_menu.filtered_count < INV_VISIBLE_ROWS)
          ? g_inv_menu.filtered_count : INV_VISIBLE_ROWS;

    for (i = 0; i < vis; i++) {
        u8  list_idx = (u8)(g_inv_menu.scroll + i);
        u8  row      = (u8)(INV_LIST_TOP_ROW + i);
        u8  slot_idx, item_id, qty;
        const ItemDef* d;
        u8  pal;

        if (list_idx >= g_inv_menu.filtered_count) break;
        slot_idx = g_inv_menu.filtered[list_idx];
        item_id  = g_bag.slots[slot_idx].item_id;
        qty      = g_bag.slots[slot_idx].quantity;
        d        = item_get(item_id);
        pal      = (list_idx == g_inv_menu.cursor) ? PAL_HI : PAL_TEXT;

        /* Cursor */
        if (list_idx == g_inv_menu.cursor)
            ui_draw_cursor(1, row);

        /* Icon */
        UI_SB[row * 32 + 3] = SE_ENTRY(_item_icon(item_id), pal);

        /* Name */
        if (d) ui_write_str(5, row, d->name, pal);

        /* Quantity (right-aligned col 25-27) */
        if (qty > 1) {
            ui_write_char(25, row, 'x', PAL_DIM);
            ui_write_num_padded(26, row, (s32)qty, 2, pal);
        }
    }

    /* Scroll indicator */
    if (g_inv_menu.filtered_count > INV_VISIBLE_ROWS) {
        if (g_inv_menu.scroll > 0)
            ui_write_char(29, 5, '^', PAL_DIM);
        if (g_inv_menu.scroll + INV_VISIBLE_ROWS < g_inv_menu.filtered_count)
            ui_write_char(29, 16, 'v', PAL_DIM);
    }

    /* Description panel */
    {
        u8 sel = menu_inventory_selected_id();
        const ItemDef* d = item_get(sel);
        ui_fill_rect(1, INV_DESC_ROW, 28, 3, SE_ENTRY(TILE_WIN_FILL, PAL_WINDOW));
        if (d) {
            ui_write_str(2, INV_DESC_ROW, d->name, PAL_HI);
            ui_write_str(2, (u8)(INV_DESC_ROW+1), d->description, PAL_TEXT);
            /* Rarity stars */
            {
                u8 r;
                for (r = 0; r < (u8)d->rarity && r < 5; r++)
                    UI_SB[(INV_DESC_ROW+2) * 32 + 2 + r] = SE_ENTRY(TILE_ICON_STAR, PAL_HI);
            }
        } else if (g_inv_menu.filtered_count == 0) {
            ui_write_str(2, INV_DESC_ROW, "Empty", PAL_DIM);
        }
    }

    /* Controls hint */
    ui_write_str(2, 19, "A:Use  L/R:Tab  B:Back", PAL_DIM);
}
