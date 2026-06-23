#ifndef MENU_INVENTORY_H
#define MENU_INVENTORY_H

#include "menu_types.h"
#include "item_db.h"
#include "inventory.h"

/* =========================================================
 * Inventory Menu
 *
 * Layout (30x20 tiles):
 *
 *  ┌─────────────────────────────┐  row 0
 *  │  INVENTORY          [gold]  │  row 1  (title bar)
 *  ├──────┬──────────────────────┤  row 2
 *  │ ALL  │ EQUIP │ USE  │ MAT   │  row 3  (category tabs)
 *  ├──────┴──────────────────────┤  row 4
 *  │ > [icon] Item Name   x99   │  rows 5-16 (item list, 12 visible)
 *  │   [icon] Item Name   x99   │
 *  │   ...                       │
 *  ├─────────────────────────────┤  row 17
 *  │ [description text]          │  rows 17-19 (description panel)
 *  └─────────────────────────────┘  row 19
 *
 * Category tabs: ALL / EQUIP / CONSUMABLE / MATERIAL / KEY
 * ========================================================= */

#define INV_VISIBLE_ROWS  12
#define INV_LIST_TOP_ROW   5
#define INV_DESC_ROW      17

typedef enum {
    ITAB_ALL = 0,
    ITAB_EQUIP,
    ITAB_USE,
    ITAB_MATERIAL,
    ITAB_KEY,
    ITAB_COUNT
} InventoryTab;

typedef struct {
    InventoryTab tab;
    u8           filtered[PARTY_INV_SIZE]; /* indices into g_bag.slots */
    u8           filtered_count;
    u8           cursor;               /* row within filtered list */
    u8           scroll;               /* first visible row */
} InvMenuState;

extern InvMenuState g_inv_menu;

void menu_inventory_open(void);
void menu_inventory_update(void);
void menu_inventory_render(void);
void menu_inventory_rebuild_filter(void);

/* Returns item_id of selected slot, 0 if none */
u8 menu_inventory_selected_id(void);

#endif /* MENU_INVENTORY_H */
