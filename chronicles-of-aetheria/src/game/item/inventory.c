#include "inventory.h"
#include "gba_types.h"

/* ---- Global bag ----------------------------------------- */
PartyInventory g_bag;

void inventory_system_init(void)
{
    u8 i;
    for (i = 0; i < PARTY_INV_SIZE; i++) {
        g_bag.slots[i].item_id  = ITEM_NONE;
        g_bag.slots[i].quantity = 0;
    }
    g_bag.count = 0;
    g_bag.gold  = 0;
}

/* ---- Internal ------------------------------------------- */

static u8 _find_slot(u8 item_id)
{
    u8 i;
    for (i = 0; i < g_bag.count; i++) {
        if (g_bag.slots[i].item_id == item_id)
            return i;
    }
    return 0xFF;
}

static u8 _first_empty(void)
{
    /* count is always the first unused index after compaction */
    if (g_bag.count >= PARTY_INV_SIZE) return 0xFF;
    return g_bag.count;
}

/* ---- API ------------------------------------------------ */

BOOL bag_add(u8 item_id, u8 qty)
{
    const ItemDef* def;
    u8 idx;
    u8 space;

    if (item_id == ITEM_NONE || item_id >= ITEM_DB_COUNT || qty == 0)
        return FALSE;

    def = item_get(item_id);

    if (def->max_stack > 1) {
        /* Try to merge into existing stack */
        idx = _find_slot(item_id);
        if (idx != 0xFF) {
            space = (u8)(def->max_stack - g_bag.slots[idx].quantity);
            if (space >= qty) {
                g_bag.slots[idx].quantity += qty;
                return TRUE;
            }
            /* Partial fill and overflow to new slot */
            g_bag.slots[idx].quantity = def->max_stack;
            qty -= space;
        }
    }

    /* Need a new slot */
    idx = _first_empty();
    if (idx == 0xFF) return FALSE;

    g_bag.slots[idx].item_id  = item_id;
    g_bag.slots[idx].quantity = (qty > def->max_stack) ? def->max_stack : qty;
    g_bag.count++;
    return TRUE;
}

BOOL bag_remove(u8 item_id, u8 qty)
{
    u8 idx = _find_slot(item_id);
    if (idx == 0xFF) return FALSE;
    if (g_bag.slots[idx].quantity < qty) return FALSE;

    g_bag.slots[idx].quantity -= qty;
    if (g_bag.slots[idx].quantity == 0) {
        /* Compact: shift left */
        u8 j;
        for (j = idx; j + 1 < g_bag.count; j++)
            g_bag.slots[j] = g_bag.slots[j + 1];
        g_bag.slots[g_bag.count - 1].item_id  = ITEM_NONE;
        g_bag.slots[g_bag.count - 1].quantity = 0;
        g_bag.count--;
    }
    return TRUE;
}

u8 bag_count(u8 item_id)
{
    u8 idx = _find_slot(item_id);
    if (idx == 0xFF) return 0;
    return g_bag.slots[idx].quantity;
}

u8 bag_find(u8 item_id)
{
    return _find_slot(item_id);
}

void bag_compact(void)
{
    u8 write = 0;
    u8 i;
    for (i = 0; i < PARTY_INV_SIZE; i++) {
        if (g_bag.slots[i].item_id != ITEM_NONE &&
            g_bag.slots[i].quantity > 0) {
            if (write != i)
                g_bag.slots[write] = g_bag.slots[i];
            write++;
        }
    }
    /* Clear remaining */
    for (i = write; i < PARTY_INV_SIZE; i++) {
        g_bag.slots[i].item_id  = ITEM_NONE;
        g_bag.slots[i].quantity = 0;
    }
    g_bag.count = write;
}

/* Simple insertion sort: by category then by item_id */
void bag_sort(void)
{
    u8 i, j;
    InvSlot tmp;
    for (i = 1; i < g_bag.count; i++) {
        tmp = g_bag.slots[i];
        j = i;
        while (j > 0) {
            const ItemDef* a = item_get(g_bag.slots[j-1].item_id);
            const ItemDef* b = item_get(tmp.item_id);
            u8 key_a = (u8)((u8)a->category * 64 + g_bag.slots[j-1].item_id);
            u8 key_b = (u8)((u8)b->category * 64 + tmp.item_id);
            if (key_a <= key_b) break;
            g_bag.slots[j] = g_bag.slots[j-1];
            j--;
        }
        g_bag.slots[j] = tmp;
    }
}

/* ---- Gold ----------------------------------------------- */

void gold_add(u32 amount)
{
    g_bag.gold += amount;
    if (g_bag.gold > GOLD_MAX) g_bag.gold = GOLD_MAX;
}

BOOL gold_spend(u32 amount)
{
    if (g_bag.gold < amount) return FALSE;
    g_bag.gold -= amount;
    return TRUE;
}

u32 gold_get(void)
{
    return g_bag.gold;
}

/* ---- Serialize ------------------------------------------ */

u16 bag_serialize(u8* buf, u16 buf_size)
{
    /* Format: [count u8][gold u32][slots count*2 bytes] */
    u16 needed = (u16)(1 + 4 + g_bag.count * 2);
    u8  i;
    if (buf_size < needed) return 0;

    buf[0] = g_bag.count;
    buf[1] = (u8)(g_bag.gold & 0xFF);
    buf[2] = (u8)((g_bag.gold >> 8) & 0xFF);
    buf[3] = (u8)((g_bag.gold >> 16) & 0xFF);
    buf[4] = (u8)((g_bag.gold >> 24) & 0xFF);
    for (i = 0; i < g_bag.count; i++) {
        buf[5 + i * 2]     = g_bag.slots[i].item_id;
        buf[5 + i * 2 + 1] = g_bag.slots[i].quantity;
    }
    return needed;
}

BOOL bag_deserialize(const u8* buf, u16 buf_size)
{
    u8  cnt;
    u32 gold;
    u8  i;
    if (buf_size < 5) return FALSE;

    cnt  = buf[0];
    gold = (u32)buf[1] | ((u32)buf[2] << 8) | ((u32)buf[3] << 16) | ((u32)buf[4] << 24);

    if (buf_size < (u16)(5 + cnt * 2)) return FALSE;
    if (cnt > PARTY_INV_SIZE) return FALSE;

    inventory_system_init();
    g_bag.gold  = (gold > GOLD_MAX) ? GOLD_MAX : gold;
    g_bag.count = cnt;
    for (i = 0; i < cnt; i++) {
        g_bag.slots[i].item_id  = buf[5 + i * 2];
        g_bag.slots[i].quantity = buf[5 + i * 2 + 1];
    }
    return TRUE;
}
