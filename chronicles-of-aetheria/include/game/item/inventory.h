#ifndef INVENTORY_H
#define INVENTORY_H

#include "item_db.h"

/* =========================================================
 * Party Inventory (shared bag)
 *
 * One global bag shared by all 4 party members.
 * Separate from the per-character equipment slots.
 *
 * Slots are compacted (no gaps) — removing an item shifts
 * remaining entries left. Max 80 distinct item stacks.
 *
 * Gold is stored here (u32, max 999999).
 * ========================================================= */

#define PARTY_INV_SIZE   80     /* max distinct item stacks */
#define GOLD_MAX         999999u
#define STACK_MAX        99     /* max per stack */

typedef struct {
    u8  item_id;   /* ITEM_NONE = empty */
    u8  quantity;
} InvSlot;

typedef struct {
    InvSlot slots[PARTY_INV_SIZE];
    u8      count;       /* occupied slots */
    u32     gold;
} PartyInventory;

/* ---- Global --------------------------------------------- */
extern PartyInventory g_bag;

/* ---- Init ----------------------------------------------- */
void inventory_system_init(void);

/* ---- Item operations ------------------------------------ */

/* Add item to bag (merges into existing stack when possible).
   Returns FALSE if bag is full and cannot stack. */
BOOL bag_add(u8 item_id, u8 qty);

/* Remove qty items. Returns FALSE if not enough in bag. */
BOOL bag_remove(u8 item_id, u8 qty);

/* How many of item_id are in the bag? */
u8   bag_count(u8 item_id);

/* Find slot index (0xFF if not found) */
u8   bag_find(u8 item_id);

/* Compact bag (remove empty slots, called internally) */
void bag_compact(void);

/* Sort bag: by category, then by item_id */
void bag_sort(void);

/* ---- Gold ----------------------------------------------- */
void gold_add(u32 amount);
BOOL gold_spend(u32 amount);   /* FALSE if not enough */
u32  gold_get(void);

/* ---- Save/Load integration ------------------------------ */
/* Serialize bag into flat byte buffer (max buf_size bytes) */
u16  bag_serialize(u8* buf, u16 buf_size);

/* Deserialize from buffer. Returns FALSE on error. */
BOOL bag_deserialize(const u8* buf, u16 buf_size);

#endif /* INVENTORY_H */
