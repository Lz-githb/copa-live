#ifndef MENU_EQUIP_H
#define MENU_EQUIP_H

#include "menu_types.h"
#include "char_equip.h"
#include "item_db.h"

/* =========================================================
 * Equipment Menu
 *
 * Layout (30x20 tiles):
 *
 *  ┌──────────────┬──────────────┐  row 0
 *  │  EQUIPMENT   │  [charname]  │  row 1
 *  ├──────────────┼──────────────┤  row 2
 *  │ [slot] WPN   │  ATK  +124  │  rows 3-10 (slot list)
 *  │ [slot] OFFH  │  DEF   +45  │
 *  │ [slot] HELM  │  MAG   +67  │
 *  │ [slot] ARMR  │  MDEF  +30  │
 *  │ [slot] BOOT  │  SPD   +12  │
 *  │ [slot] RING  │  CRIT  + 5  │
 *  │ [slot] AMLT  │  EVA   + 3  │
 *  ├──────────────┼──────────────┤  row 11
 *  │ [equipped item name]        │  row 12 (detail)
 *  │ [desc line 1]               │  row 13
 *  ├─────────────────────────────┤  row 14
 *  │  ◄ Kael   Mira ►           │  row 15 (char switch)
 *  ├─────────────────────────────┤  row 16
 *  │  [change list, 4 items]     │  rows 16-19 (swap panel)
 *  └─────────────────────────────┘  row 19
 *
 * ========================================================= */

#define EQUIP_SLOT_ROWS     EQUIP_SLOT_COUNT   /* 6 slots */
#define EQUIP_SWAP_VISIBLE  4

typedef struct {
    u8   slot_cursor;           /* which EquipSlot is selected */
    BOOL in_swap_list;          /* TRUE = browsing replacement items */
    u8   swap_cursor;           /* cursor within compatible items list */
    u8   swap_scroll;
    u8   swap_ids[PARTY_INV_SIZE]; /* bag indices of compatible items */
    u8   swap_count;
} EquipMenuState;

extern EquipMenuState g_equip_menu;

void menu_equip_open(u8 char_idx);
void menu_equip_update(void);
void menu_equip_render(void);
void menu_equip_rebuild_swap(void);

#endif /* MENU_EQUIP_H */
