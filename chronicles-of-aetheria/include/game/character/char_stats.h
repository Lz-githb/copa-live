#ifndef CHAR_STATS_H
#define CHAR_STATS_H

#include "char_types.h"

/* =========================================================
 * Character Stats System
 *
 * Two-layer stats:
 *   1. BaseStats   — raw attribute values (from level + class)
 *   2. DerivedStats — combat values computed from BaseStats
 *                     + equipment bonuses + talent bonuses
 *
 * Formula summary:
 *   HP_MAX  = VIT * 12 + END * 4 + class_base_hp
 *   MP_MAX  = INT * 8  + WIS * 4 + class_base_mp
 *   ATK     = STR * 3  + DEX     + weapon_atk
 *   DEF     = END * 3  + STR     + armor_def
 *   MAG     = INT * 3  + WIS     + staff_mag
 *   MDEF    = WIS * 3  + INT     + armor_mdef
 *   SPD     = DEX * 3  + LCK     + equip_spd
 *   CRIT    = LCK / 3  + DEX / 5 + talent_crit  (0-75 cap)
 *   EVASION = DEX / 4  + LCK / 5 + talent_eva   (0-60 cap)
 * ========================================================= */

typedef struct {
    s16 val[ATTR_COUNT];  /* current attribute values */
    s16 bonus[ATTR_COUNT];/* bonus from equipment/talents */
} BaseStats;

typedef struct {
    s16 val[DSTAT_COUNT]; /* computed derived stats */
} DerivedStats;

/* ---- Growth tables (ROM) -------------------------------- */

/* Points awarded per attribute per level, by GrowthGrade */
extern const u8 GROWTH_PTS[6];  /* index = GrowthGrade */

/* Per-class attribute growth grades [CLASS][ATTR] */
extern const u8 CLASS_GROWTH[CLASS_COUNT][ATTR_COUNT];

/* Per-class base derived stat bonuses (added to formula) */
typedef struct {
    s16 hp_base;
    s16 mp_base;
    s16 atk_bonus;
    s16 def_bonus;
    s16 mag_bonus;
    s16 mdef_bonus;
    s16 spd_bonus;
    s16 crit_bonus;
} ClassStatBonus;

extern const ClassStatBonus CLASS_STAT_BONUS[CLASS_COUNT];

/* ---- API ------------------------------------------------ */

/* Initialize BaseStats from class + level 1 allocation */
void stats_init(BaseStats* bs, CharClass cls);

/* Apply one level's attribute point growth */
void stats_apply_level_growth(BaseStats* bs, CharClass cls, u8 level);

/* Recompute DerivedStats from BaseStats + equip/talent bonuses */
void stats_compute_derived(const BaseStats* bs,
                            DerivedStats* out,
                            CharClass cls,
                            const s16 equip_bonus[DSTAT_COUNT],
                            const s16 talent_bonus[DSTAT_COUNT]);

/* Get a single derived stat value */
s16 stats_get_derived(const DerivedStats* ds, DStatType which);

/* Add/remove equipment bonus to base stats */
void stats_add_equip_bonus(BaseStats* bs, AttrType attr, s16 amount);
void stats_remove_equip_bonus(BaseStats* bs, AttrType attr, s16 amount);

/* Format an attribute name (for UI) */
const char* stats_attr_name(AttrType attr);
const char* stats_dstat_name(DStatType dstat);

#endif /* CHAR_STATS_H */
