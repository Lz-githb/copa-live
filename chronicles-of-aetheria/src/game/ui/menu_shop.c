#include "menu_shop.h"
#include "menu_types.h"
#include "item_db.h"
#include "inventory.h"
#include "shop.h"
#include "gba_types.h"

/* =========================================================
 * Shop Menu Implementation
 * ========================================================= */

ShopMenuState g_shop_menu;


#define KEY_A      0x0001
#define KEY_B      0x0002
#define KEY_LEFT   0x0020
#define KEY_RIGHT  0x0010
#define KEY_UP     0x0040
#define KEY_DOWN   0x0080
#define KEY_L      0x0200
#define KEY_R      0x0100
#define REG_KEYINPUT (*(vu16*)(0x04000130))

static u16 s_shp;
static u16 s_shc = 0xFFFF;

static u16 _shpressed(void)
{
    u16 now = REG_KEYINPUT;
    u16 p   = (~now) & s_shp;
    s_shp   = ~now;
    s_shc   = now;
    (void)s_shc;
    return p;
}

/* ---- Rebuild list -------------------------------------- */
void menu_shop_rebuild_list(void)
{
    u8 i;
    const ShopDef* sd = &g_shop_table[g_shop_menu.shop_id];
    g_shop_menu.list_count = 0;

    if (g_shop_menu.mode == SHOP_MODE_BUY) {
        for (i = 0; i < sd->stock_count && i < SHOP_STOCK_MAX; i++) {
            if (sd->stock[i] != ITEM_NONE)
                g_shop_menu.list[g_shop_menu.list_count++] = i; /* stock index */
        }
    } else {
        /* Sell: show bag items that are sellable */
        for (i = 0; i < g_bag.count; i++) {
            u8 item_id = g_bag.slots[i].item_id;
            if (item_id == ITEM_NONE) continue;
            {
                const ItemDef* d = item_get(item_id);
                if (!d || d->category == ICAT_KEY_ITEM) continue;
                g_shop_menu.list[g_shop_menu.list_count++] = i;
            }
        }
    }
    if (g_shop_menu.cursor >= g_shop_menu.list_count && g_shop_menu.list_count > 0)
        g_shop_menu.cursor = (u8)(g_shop_menu.list_count - 1);
    if (g_shop_menu.list_count == 0) g_shop_menu.cursor = 0;
}

/* ---- Get item ID from current list entry --------------- */
static u8 _shop_item_id(void)
{
    u8 idx;
    if (g_shop_menu.list_count == 0) return ITEM_NONE;
    idx = g_shop_menu.list[g_shop_menu.cursor];
    if (g_shop_menu.mode == SHOP_MODE_BUY) {
        return g_shop_table[g_shop_menu.shop_id].stock[idx];
    } else {
        return g_bag.slots[idx].item_id;
    }
}

/* ---- Open ---------------------------------------------- */

void menu_shop_open(u8 shop_id)
{
    g_shop_menu.shop_id    = shop_id;
    g_shop_menu.mode       = SHOP_MODE_BUY;
    g_shop_menu.phase      = SHOP_PHASE_LIST;
    g_shop_menu.cursor     = 0;
    g_shop_menu.scroll     = 0;
    g_shop_menu.qty        = 1;
    g_shop_menu.total_price = 0;
    shop_open(shop_id);
    menu_shop_rebuild_list();
    s_shp = 0;
    s_shc = 0xFFFF;
}

/* ---- Update -------------------------------------------- */

void menu_shop_update(void)
{
    u16 pressed = _shpressed();
    u8  lc      = g_shop_menu.list_count;

    if (g_shop_menu.phase == SHOP_PHASE_LIST) {
        /* Mode switch with L/R */
        if (pressed & KEY_L) {
            g_shop_menu.mode   = SHOP_MODE_BUY;
            g_shop_menu.cursor = 0;
            g_shop_menu.scroll = 0;
            menu_shop_rebuild_list();
            g_menu.dirty = TRUE;
        }
        if (pressed & KEY_R) {
            g_shop_menu.mode   = SHOP_MODE_SELL;
            g_shop_menu.cursor = 0;
            g_shop_menu.scroll = 0;
            menu_shop_rebuild_list();
            g_menu.dirty = TRUE;
        }

        if (lc > 0) {
            if (pressed & KEY_UP) {
                if (g_shop_menu.cursor > 0) g_shop_menu.cursor--;
                else g_shop_menu.cursor = (u8)(lc - 1);
                g_menu.dirty = TRUE;
            }
            if (pressed & KEY_DOWN) {
                if (g_shop_menu.cursor < lc - 1) g_shop_menu.cursor++;
                else g_shop_menu.cursor = 0;
                g_menu.dirty = TRUE;
            }
            /* Scroll */
            if (g_shop_menu.cursor < g_shop_menu.scroll)
                g_shop_menu.scroll = g_shop_menu.cursor;
            if (g_shop_menu.cursor >= g_shop_menu.scroll + SHOP_VISIBLE_ROWS)
                g_shop_menu.scroll = (u8)(g_shop_menu.cursor - SHOP_VISIBLE_ROWS + 1);

            if (pressed & KEY_A) {
                g_shop_menu.qty         = 1;
                g_shop_menu.total_price = (g_shop_menu.mode == SHOP_MODE_BUY)
                    ? shop_buy_price(_shop_item_id())
                    : shop_sell_price(_shop_item_id());
                g_shop_menu.phase = SHOP_PHASE_QTY;
                g_menu.dirty = TRUE;
            }
        }
    } else if (g_shop_menu.phase == SHOP_PHASE_QTY) {
        u8 item_id = _shop_item_id();
        u32 unit   = (g_shop_menu.mode == SHOP_MODE_BUY)
                      ? shop_buy_price(item_id) : shop_sell_price(item_id);

        if (pressed & KEY_RIGHT) {
            if (g_shop_menu.qty < 99) { g_shop_menu.qty++; g_menu.dirty = TRUE; }
        }
        if (pressed & KEY_LEFT) {
            if (g_shop_menu.qty > 1)  { g_shop_menu.qty--; g_menu.dirty = TRUE; }
        }
        g_shop_menu.total_price = unit * g_shop_menu.qty;

        if (pressed & KEY_A) {
            if (g_shop_menu.mode == SHOP_MODE_BUY) {
                shop_buy(item_id, g_shop_menu.qty);
            } else {
                shop_sell(item_id, g_shop_menu.qty);
            }
            g_shop_menu.phase = SHOP_PHASE_LIST;
            menu_shop_rebuild_list();
            g_menu.dirty = TRUE;
        }
        if (pressed & KEY_B) {
            g_shop_menu.phase = SHOP_PHASE_LIST;
            g_menu.dirty = TRUE;
        }
    }
}

/* ---- Render -------------------------------------------- */

void menu_shop_render(void)
{
    u8 i;
    const ShopDef* sd = &g_shop_table[g_shop_menu.shop_id];

    ui_draw_window(0, 0, 30, 20);

    /* Shop name + gold */
    ui_write_str(2, 1, sd->name, PAL_HI);
    UI_SB[1 * 32 + 20] = SE_ENTRY(TILE_ICON_COIN, PAL_HI);
    ui_write_num(21, 1, (s32)g_bag.gold, PAL_HI);

    /* Mode tabs */
    ui_write_str(3, 3, "BUY", g_shop_menu.mode == SHOP_MODE_BUY ? PAL_HI : PAL_DIM);
    ui_write_str(12, 3, "SELL", g_shop_menu.mode == SHOP_MODE_SELL ? PAL_HI : PAL_DIM);

    /* Divider row 4 */
    for (i = 1; i < 29; i++)
        UI_SB[4 * 32 + i] = SE_ENTRY(TILE_WIN_TOP, PAL_BORDER);

    /* Item list */
    {
        u8 vis = (g_shop_menu.list_count < SHOP_VISIBLE_ROWS)
                  ? g_shop_menu.list_count : SHOP_VISIBLE_ROWS;
        if (g_shop_menu.list_count == 0) {
            ui_write_str(3, 7, "(nothing available)", PAL_DIM);
        }
        for (i = 0; i < vis; i++) {
            u8 li      = (u8)(g_shop_menu.scroll + i);
            u8 row     = (u8)(5 + i);
            u8 item_id;
            u32 price;
            const ItemDef* d;
            BOOL sel   = (BOOL)(li == g_shop_menu.cursor);
            u8 pal     = sel ? PAL_HI : PAL_TEXT;

            if (li >= g_shop_menu.list_count) break;
            {
                u8 idx = g_shop_menu.list[li];
                item_id = (g_shop_menu.mode == SHOP_MODE_BUY)
                           ? sd->stock[idx]
                           : g_bag.slots[idx].item_id;
            }
            d     = item_get(item_id);
            price = (g_shop_menu.mode == SHOP_MODE_BUY)
                     ? shop_buy_price(item_id) : shop_sell_price(item_id);

            if (sel) ui_draw_cursor(1, row);
            if (d) {
                ui_write_str(3, row, d->name, pal);
                UI_SB[row * 32 + 21] = SE_ENTRY(TILE_ICON_COIN, PAL_DIM);
                ui_write_num_padded(22, row, (s32)price, 6, pal);
                /* Affordable indicator */
                if (g_shop_menu.mode == SHOP_MODE_BUY && g_bag.gold < price)
                    UI_SB[row * 32 + 28] = SE_ENTRY(TILE_ICON_LOCK, PAL_LOCKED);
            }
        }
    }

    /* Divider row 16 */
    for (i = 1; i < 29; i++)
        UI_SB[16 * 32 + i] = SE_ENTRY(TILE_WIN_TOP, PAL_BORDER);

    /* Description row 17 */
    {
        u8 item_id = _shop_item_id();
        const ItemDef* d = item_get(item_id);
        if (d) {
            ui_write_str(2, 17, d->description, PAL_TEXT);
        }
    }

    /* Qty selection / confirm row 18-19 */
    if (g_shop_menu.phase == SHOP_PHASE_QTY) {
        ui_write_str(2, 18, "Qty:", PAL_DIM);
        ui_write_char(8, 18, '<', PAL_HI);
        ui_write_num_padded(10, 18, (s32)g_shop_menu.qty, 2, PAL_HI);
        ui_write_char(13, 18, '>', PAL_HI);
        ui_write_str(16, 18, "Total:", PAL_DIM);
        UI_SB[18 * 32 + 23] = SE_ENTRY(TILE_ICON_COIN, PAL_HI);
        ui_write_num(24, 18, (s32)g_shop_menu.total_price, PAL_HI);
        ui_write_str(2, 19, "A:Confirm  B:Cancel", PAL_DIM);
    } else {
        ui_write_str(2, 19, "A:Select  L/R:Mode  B:Back", PAL_DIM);
    }
}
