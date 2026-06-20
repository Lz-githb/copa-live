#ifndef BATTLE_DATA_H
#define BATTLE_DATA_H

#include "battle_types.h"
#include "battler.h"
#include "battle_combo.h"

/* =========================================================
 * Battle Data — ROM Tables
 *
 * All skills, enemies, groups, and combos defined here.
 * Accessed by ID through global const arrays.
 * ========================================================= */

/* ---- Skill definition ----------------------------------- */
typedef struct {
    const char* name;
    u8          mp_cost;
    u8          power;         /* base power 0-255 */
    ElementType element;
    TargetType  target;
    EffectType  effect;
    BOOL        is_physical;   /* TRUE=phys, FALSE=magic */
    StatusFlag  status_inflict;
    u8          status_chance; /* 0-100 */
    StatModType stat_mod_type; /* for EFF_STAT_MOD */
    s8          stat_mod_val;  /* -4..+4 stages */
    u8          anim_id;
    const char* description;
} SkillDef;

/* ---- Skill IDs ------------------------------------------ */
/* Physical skills */
#define SKILL_ATTACK       0    /* basic attack (no MP cost) */
#define SKILL_SLASH        1
#define SKILL_DOUBLE_CUT   2
#define SKILL_BREAK        3    /* ATK down */
#define SKILL_SHATTER      4    /* DEF down */

/* Fire skills */
#define SKILL_FIRE         5
#define SKILL_FIRA         6
#define SKILL_FIRAGA       7
#define SKILL_METEOR       8    /* Fire + random 4 hits */

/* Water skills */
#define SKILL_WATER        9
#define SKILL_AQUA        10
#define SKILL_TIDAL       11    /* AOE water */
#define SKILL_TSUNAMI     12

/* Earth skills */
#define SKILL_QUAKE       13
#define SKILL_STONE_EDGE  14
#define SKILL_PETRIFY     15    /* inflict STONE */

/* Air skills */
#define SKILL_GUST        16
#define SKILL_CYCLONE     17
#define SKILL_TORNADO     18

/* Light skills */
#define SKILL_FLASH       19    /* inflict BLIND */
#define SKILL_HOLY        20
#define SKILL_RADIANCE    21
#define SKILL_REVIVE      22    /* revive ally at 50% HP */

/* Dark skills */
#define SKILL_SHADOW      23
#define SKILL_DRAIN       24    /* HP drain */
#define SKILL_DOOM        25    /* inflict DOOM */
#define SKILL_ECLIPSE     26    /* AOE dark + silence */

/* Support skills */
#define SKILL_HEAL        27
#define SKILL_CURA        28    /* AOE heal */
#define SKILL_REGEN_CAST  29    /* inflict REGEN */
#define SKILL_HASTE_CAST  30    /* inflict HASTE */
#define SKILL_SLOW_CAST   31    /* inflict SLOW */
#define SKILL_BARRIER_CAST 32
#define SKILL_SILENCE_CAST 33
#define SKILL_DISPEL      34    /* remove all buffs */
#define SKILL_FULL_CURE   35    /* cure all status */

/* Character specials (unleash / limit) */
#define SKILL_UNLEASH_SOLEIL 36  /* Char0 special: fire burst */
#define SKILL_UNLEASH_TIDE   37  /* Char1 special: water wave */
#define SKILL_UNLEASH_GAIA   38  /* Char2 special: earth shatter */
#define SKILL_UNLEASH_STORM  39  /* Char3 special: storm call */

#define SKILL_COUNT        40

/* ---- Enemy IDs ------------------------------------------ */
#define ENEMY_SLIME        0
#define ENEMY_GOBLIN       1
#define ENEMY_WOLF         2
#define ENEMY_ORC          3
#define ENEMY_MAGE         4
#define ENEMY_DRAGON       5
#define ENEMY_UNDEAD       6
#define ENEMY_ELEMENTAL_F  7   /* fire elemental */
#define ENEMY_ELEMENTAL_W  8   /* water elemental */
#define ENEMY_GOLEM        9
#define ENEMY_DARK_KNIGHT  10
#define ENEMY_BOSS_SERPENT 11
#define ENEMY_COUNT        12

/* ---- Enemy Group IDs ------------------------------------ */
#define GROUP_SLIMES       0    /* 3 slimes */
#define GROUP_GOBLINS      1    /* 4 goblins */
#define GROUP_MIXED_1      2    /* wolf + 2 goblins */
#define GROUP_ELEMENTAL    3    /* 2 fire + 1 water elemental */
#define GROUP_BOSS_SERPENT 4    /* 1 boss + 2 orcs */
#define GROUP_UNDEAD       5    /* 3 undead + 1 dark mage */
#define GROUP_COUNT        6

/* ---- Data access ---------------------------------------- */

void battle_data_init(void);

const SkillDef*   skill_get(u8 id);
const EnemyDef*   enemy_get(u8 id);
const EnemyGroup* group_get(u8 id);
const ComboDef*   combo_get_by_id(u8 id);

/* Global tables */
extern const SkillDef    g_skill_table[SKILL_COUNT];
extern const EnemyDef    g_enemy_table[ENEMY_COUNT];
extern const EnemyGroup  g_enemy_groups[GROUP_COUNT];
extern const ComboDef    g_combo_table[BATTLE_COMBO_MAX];
extern const u8          g_combo_count;

/* Character roster (4 party members, their stats + skills) */
extern const CharDef g_char_table[BATTLE_ALLY_MAX];

#endif /* BATTLE_DATA_H */
