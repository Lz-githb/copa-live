#ifndef MENU_SHOP_H
#define MENU_SHOP_H

#include "menu_types.h"
#include "shop.h"
#include "item_db.h"

/* =========================================================
 * Shop Menu
 *
 * Layout (30x20 tiles):
 *
 *  ┌──────────────────┬───────────┐  row 0
 *  │  [shop name]     │ G:999999  │  row 1
 *  ├──────────────────┴───────────┤  row 2
 *  │  BUY    │    SELL            │  row 3  (mode tabs)
 *  ├────────────────────────────────┤  row 4
 *  │ > [icon] Item Name   P:9999  │  rows 5-15 (item list)
 *  │   [icon] Item Name   P:9999  │
 *  │   ...                        │
 *  ├────────────────────────────────┤  row 16
 *  │ [description]                 │  rows 16-17
 *  ├────────────────────────────────┤  row 18
 *  │  QTY: [--] 1 [++]  Total:xxx │  row 18 (qty select)
 *  │  [A] Confirm  [B] Cancel      │  row 19
 *  └────────────────────────────────┘
 *
 * ========================================================= */

#define SHOP_VISIBLE_ROWS  11

typedef u8 ShopMode;  /* SHOP_MODE_BUY / SHOP_MODE_SELL from shop.h */

typedef enum {
    SHOP_PHASE_LIST = 0,
    SHOP_PHASE_QTY,
    SHOP_PHASE_CONFIRM
} ShopPhase;

typedef struct {
    u8          shop_id;
    ShopMode    mode;
    ShopPhase   phase;
    u8          cursor;
    u8          scroll;
    /* BUY: indices into ShopDef.item_ids */
    /* SELL: indices into g_bag.slots */
    u8          list[PARTY_INV_SIZE]; /* large enough for both buy and sell modes */
    u8          list_count;
    u8          qty;            /* quantity to buy/sell */
    u32         total_price;
} ShopMenuState;

extern ShopMenuState g_shop_menu;

void menu_shop_open(u8 shop_id);
void menu_shop_update(void);
void menu_shop_render(void);
void menu_shop_rebuild_list(void);

#endif /* MENU_SHOP_H */
