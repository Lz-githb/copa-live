#ifndef BOSS_DB_H
#define BOSS_DB_H

#include "../../engine/engine.h"

/* =========================================================
 * Boss Database — 30 bosses (20 main + 10 optional/secret)
 *
 * Each boss has up to BOSS_PHASE_MAX phases. Phases are
 * triggered when the boss's HP drops below phase_thresholds[N]
 * percent. phase_thresholds[0] is the threshold to enter
 * phase 2, phase_thresholds[1] enters phase 3, etc.
 * A threshold of 0 means "unused" (no further phase).
 *
 * Stats per phase scale upward — each BossPhase is a full
 * combat stat block (mirrors EnemyDef layout) plus a string
 * shown as a cinematic prompt when the phase triggers.
 *
 * No dynamic allocation — all data lives in ROM.
 * ========================================================= */

/* ---- Counts --------------------------------------------- */
#define BOSS_COUNT      30
#define BOSS_PHASE_MAX   3

/* ---- Main boss IDs (0-19) ------------------------------- */
#define BOSS_FOREST_GUARDIAN    0
#define BOSS_CAVE_TROLL         1
#define BOSS_SEA_SERPENT        2
#define BOSS_SANDWORM           3
#define BOSS_THUNDER_ROOK       4
#define BOSS_SWAMP_HAG          5
#define BOSS_IRON_GOLEM         6
#define BOSS_SHADOW_WOLF        7
#define BOSS_LAVA_TITAN         8
#define BOSS_FROST_WYRM         9
#define BOSS_PLAGUE_WRAITH      10
#define BOSS_STORM_EAGLE        11
#define BOSS_STONE_COLOSSUS     12
#define BOSS_DARK_SORCERER      13
#define BOSS_UNDEAD_KING        14
#define BOSS_VOID_SERPENT       15
#define BOSS_ELDER_DRAGON       16
#define BOSS_LICH_KING          17
#define BOSS_DEMON_LORD         18
#define BOSS_VOID_GOD           19

/* ---- Optional / secret boss IDs (20-29) ----------------- */
#define BOSS_SECRET_KNIGHT      20
#define BOSS_PRISMATIC_SLIME    21
#define BOSS_DEATH_MERCHANT     22
#define BOSS_ANCIENT_GOLEM      23
#define BOSS_FALLEN_ANGEL       24
#define BOSS_PHANTOM_KING       25
#define BOSS_VOID_HERALD        26
#define BOSS_TITAN_CRAB         27
#define BOSS_MIRROR_DEMON       28
#define BOSS_TRUE_VOID_GOD      29

/* =========================================================
 * BossPhase — one phase of a multi-phase boss fight.
 * Essentially an EnemyDef extended with phase_intro text.
 * ========================================================= */
typedef struct {
    const char* name;           /* phase display name          */
    u16         hp_max;
    u16         mp_max;
    s16         atk;
    s16         def;
    s16         mag;
    s16         mdef;
    s16         spd;
    u8          element;        /* ElementType                 */
    u8          resist[7];      /* one entry per ELEM_COUNT    */
    u8          skill_ids[4];
    u8          skill_count;
    u8          sprite_id;
    u8          palette_id;
    u8          ai_type;        /* AI_BOSS_PHASE = 5           */
    const char* phase_intro;    /* <=28 chars, shown on trigger*/
} BossPhase;

/* =========================================================
 * BossDef — full boss definition stored in ROM.
 * ========================================================= */
typedef struct {
    const char* name;
    const char* title;                          /* subtitle / epithet          */
    u8          phase_count;
    BossPhase   phases[BOSS_PHASE_MAX];
    u8          phase_thresholds[BOSS_PHASE_MAX]; /* % HP to enter phase N+1  */
    u32         exp_reward;
    u16         gold_reward;
    u8          unlock_recipe;                  /* recipe ID, 0xFF = none      */
    u8          key_item_drop;                  /* item_id, 0xFF = none        */
    u8          region;                         /* REGION_* from monster_db.h  */
    u8          is_optional;
    const char* description;
} BossDef;

/* =========================================================
 * API
 * ========================================================= */
extern const BossDef g_boss_db[BOSS_COUNT];

/* Returns pointer into ROM; returns NULL if id >= BOSS_COUNT */
const BossDef* boss_get(u8 id);

/* ROM-resident data — nothing to initialise at runtime */
void boss_db_init(void);

#endif /* BOSS_DB_H */
