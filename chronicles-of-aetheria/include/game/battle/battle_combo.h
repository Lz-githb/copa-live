#ifndef BATTLE_COMBO_H
#define BATTLE_COMBO_H

#include "battle_types.h"
#include "battler.h"

/* =========================================================
 * Combo / Joint Tech System (Chrono Trigger inspired)
 *
 * Combos require specific characters (by slot index 0-3)
 * to each have >= CP_COST_COMBO combo points.
 *
 * Dual Techs  = 2 characters
 * Triple Techs = 3 characters
 *
 * When a player selects COMBO, the system shows available
 * combos for their character slot. The partner's turn is
 * consumed as well.
 *
 * Effect: amplified elemental damage + secondary effect
 *         that no individual skill can produce alone.
 * ========================================================= */

#define COMBO_PART_MAX     3    /* max participants in a combo */

typedef struct {
    const char*  name;
    u8           part_slots[COMBO_PART_MAX]; /* which char slots (0-3) */
    u8           part_count;
    u8           power;            /* base power (higher than normal skills) */
    ElementType  element;
    TargetType   target;
    EffectType   primary_eff;
    EffectType   secondary_eff;   /* bonus effect (status, etc.) */
    StatusFlag   status_inflict;  /* if secondary_eff=STATUS_ADD */
    u8           status_chance;   /* 0-100 */
    u8           anim_id;         /* combo-specific animation */
    const char*  description;
} ComboDef;

/* ---- Global combo table (ROM) --------------------------- */
extern const ComboDef g_combo_table[];
extern const u8       g_combo_count;

/* ---- Combo system API ----------------------------------- */

void combo_system_init(void);

/* Check all combos available for a given character slot */
/* Returns count of available combos; fills ids[] array */
u8 combo_get_available(const Battler battlers[BATTLE_BATTLER_MAX],
                        u8 char_slot,
                        u8* out_ids, u8 max_out);

/* Check if a specific combo is executable */
BOOL combo_is_ready(const Battler battlers[BATTLE_BATTLER_MAX],
                    u8 combo_id);

/* Execute a combo: deduct CP from all partners, compute damage */
/* Returns bitmask of affected battlers */
u16 combo_execute(Battler battlers[BATTLE_BATTLER_MAX],
                  u8 combo_id, u8 primary_target_mask);

/* Get combo def by id */
const ComboDef* combo_get(u8 id);

#endif /* BATTLE_COMBO_H */
