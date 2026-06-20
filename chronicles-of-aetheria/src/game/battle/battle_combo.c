#include "battle_combo.h"
#include "battle_data.h"
#include "battle_formula.h"
#include "battler.h"
#include "gba_types.h"

/* Combo table defined in battle_data.c, declared extern in battle_combo.h */

void combo_system_init(void)
{
    /* Nothing to initialize — data is ROM */
}

BOOL combo_is_ready(const Battler battlers[BATTLE_BATTLER_MAX], u8 combo_id)
{
    const ComboDef* def;
    u8 i;

    if (combo_id >= g_combo_count) return FALSE;
    def = &g_combo_table[combo_id];

    for (i = 0; i < def->part_count; i++) {
        u8 slot = def->part_slots[i];
        const Battler* b = &battlers[slot];
        if (!b->alive) return FALSE;
        if (b->combo_pts < CP_COST_COMBO) return FALSE;
    }
    return TRUE;
}

u8 combo_get_available(const Battler battlers[BATTLE_BATTLER_MAX],
                        u8 char_slot,
                        u8* out_ids, u8 max_out)
{
    u8 count = 0;
    u8 i, j;

    for (i = 0; i < g_combo_count && count < max_out; i++) {
        const ComboDef* def = &g_combo_table[i];
        BOOL has_slot = FALSE;

        /* Check this character participates */
        for (j = 0; j < def->part_count; j++) {
            if (def->part_slots[j] == char_slot) { has_slot = TRUE; break; }
        }
        if (!has_slot) continue;

        if (combo_is_ready(battlers, i))
            out_ids[count++] = i;
    }
    return count;
}

u16 combo_execute(Battler battlers[BATTLE_BATTLER_MAX],
                  u8 combo_id, u8 primary_target_mask)
{
    const ComboDef* def;
    u16 hit_mask = 0;
    u8 i;

    if (combo_id >= g_combo_count) return 0;
    def = &g_combo_table[combo_id];

    /* Deduct CP from all participants */
    for (i = 0; i < def->part_count; i++) {
        u8 slot = def->part_slots[i];
        if (battlers[slot].combo_pts >= CP_COST_COMBO)
            battlers[slot].combo_pts -= CP_COST_COMBO;
        else
            battlers[slot].combo_pts = 0;
    }

    /* Resolve damage to targets (done via action_resolve in coordinator) */
    hit_mask = (u16)primary_target_mask;
    return hit_mask;
}

const ComboDef* combo_get(u8 id)
{
    if (id >= g_combo_count) return 0;
    return &g_combo_table[id];
}
