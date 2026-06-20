#ifndef BATTLE_FORMULA_H
#define BATTLE_FORMULA_H

#include "battle_types.h"
#include "battler.h"

/* =========================================================
 * Battle Formula Engine
 *
 * Physical damage (FF6-inspired):
 *   base = (atk * atk * power) / (def * 32 + 1)
 *   crit = 15% chance → 2x damage
 *   miss = BLIND → 50% chance
 *
 * Magical damage (GS-inspired):
 *   base = (mag * power) / (mdef / 2 + 1) * 2
 *   element_mod = resist[element] / 100.0
 *
 * Variance: ±10% random spread
 *
 * Stat mod stages (-4..+4) use multiplier table:
 *   {25, 34, 50, 67, 100, 133, 150, 200, 300} / 100
 * ========================================================= */

/* ---- Element effectiveness table (7×7) ------------------
 * [attack_element][target_resist_element]
 * Value = percentage of damage (100 = neutral)
 * Elements: NONE, FIRE, WATER, EARTH, AIR, LIGHT, DARK   */
extern const u8 ELEM_MATRIX[ELEM_COUNT][ELEM_COUNT];

/* ---- Formulas ------------------------------------------- */

/* Physical attack */
DamageResult formula_physical(const Battler* attacker,
                               const Battler* defender,
                               u8 power, ElementType element);

/* Magical attack */
DamageResult formula_magical(const Battler* attacker,
                              const Battler* defender,
                              u8 power, ElementType element);

/* Healing (magic-based) */
s16 formula_heal(const Battler* caster, u8 power);

/* Drain (damage + self-heal portion) */
DamageResult formula_drain(const Battler* attacker,
                            const Battler* defender,
                            u8 power, ElementType element);

/* Status infliction chance (0-100) */
BOOL formula_status_hit(const Battler* attacker,
                        const Battler* defender,
                        u8 base_chance);

/* Flee chance (0-100) */
BOOL formula_flee(const Battler* fleeing_party, u8 flee_mult);

/* Combo damage multiplier (1.5× base damage) */
#define COMBO_DAMAGE_MULT  150  /* × 100 */

/* Critical hit multiplier */
#define CRIT_MULT          200  /* 2× */

/* Defend multiplier */
#define DEFEND_MULT         50  /* 0.5× */

/* Stat mod stage multipliers ×100 (index = stage + 4) */
extern const u16 STAT_MOD_TABLE[9];

/* Apply element matrix to raw damage */
s16 formula_apply_element(s16 raw_dmg, ElementType atk_elem,
                           u8 defender_resist);

/* Random variance ±10% */
s16 formula_variance(s16 value);

#endif /* BATTLE_FORMULA_H */
