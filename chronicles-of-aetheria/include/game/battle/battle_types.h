#ifndef BATTLE_TYPES_H
#define BATTLE_TYPES_H

#include "../../engine/engine.h"

/* =========================================================
 * Battle System — Shared Types & Constants
 *
 * Targeting uses a 10-bit mask:
 *   bits 0-3  = allies 0-3
 *   bits 4-9  = enemies 0-5
 *
 * All battle data (skills, enemies, combos) lives in ROM as
 * const arrays. Runtime battle state lives in EWRAM pools.
 * ========================================================= */

/* ---- Capacity ------------------------------------------- */
#define BATTLE_ALLY_MAX     4
#define BATTLE_ENEMY_MAX    6
#define BATTLE_BATTLER_MAX  (BATTLE_ALLY_MAX + BATTLE_ENEMY_MAX)
#define BATTLE_SKILL_MAX    8     /* skills per character */
#define BATTLE_ITEM_MAX     8     /* item slots in battle */
#define BATTLE_COMBO_MAX    16    /* total defined combos */

/* ---- Index helpers -------------------------------------- */
#define ALLY_IDX(n)          (n)          /* 0-3 */
#define ENEMY_IDX(n)         (4 + (n))    /* 4-9 */
#define IS_ALLY(idx)         ((idx) < 4)
#define IS_ENEMY(idx)        ((idx) >= 4)
#define ALLY_FROM_IDX(idx)   (idx)
#define ENEMY_FROM_IDX(idx)  ((idx) - 4)

/* ---- Target bitmask ------------------------------------- */
#define TARGET_ALLY(n)       (1u << (n))
#define TARGET_ENEMY(n)      (1u << (4 + (n)))
#define TARGET_ALL_ALLIES    0x000Fu
#define TARGET_ALL_ENEMIES   0x03F0u
#define TARGET_ALL           0x03FFu
#define TARGET_SELF(actor)   (1u << (actor))

/* ---- Elements ------------------------------------------- */
typedef enum {
    ELEM_NONE  = 0,
    ELEM_FIRE  = 1,
    ELEM_WATER = 2,
    ELEM_EARTH = 3,
    ELEM_AIR   = 4,
    ELEM_LIGHT = 5,
    ELEM_DARK  = 6,
    ELEM_COUNT = 7,
} ElementType;

/* Element affinity values (percentage of normal damage) */
#define ELEM_NULL      0    /* immune */
#define ELEM_ABSORB    255  /* absorbs as healing */
#define ELEM_RESIST    50   /* 0.5x */
#define ELEM_NEUTRAL   100  /* 1.0x */
#define ELEM_WEAK      150  /* 1.5x */
#define ELEM_VULN      200  /* 2.0x */

/* ---- Status effect flags -------------------------------- */
typedef enum {
    /* Negative (debuffs) */
    STATUS_POISON   = (1 << 0),   /* -5% maxHP/turn */
    STATUS_BURN     = (1 << 1),   /* -8% maxHP/turn, ATK-1 */
    STATUS_FREEZE   = (1 << 2),   /* skip turn; thaws on hit */
    STATUS_BLIND    = (1 << 3),   /* 50% miss chance */
    STATUS_SILENCE  = (1 << 4),   /* cannot use magic */
    STATUS_SLOW     = (1 << 5),   /* half effective SPD */
    STATUS_CONFUSE  = (1 << 6),   /* random target on attack */
    STATUS_SLEEP    = (1 << 7),   /* skip turn; wakes on damage */
    STATUS_STONE    = (1 << 8),   /* skip turn; takes 2x damage */
    STATUS_DOOM     = (1 << 9),   /* KO after doom_timer turns */
    /* Positive (buffs) */
    STATUS_REGEN    = (1 << 10),  /* +5% maxHP/turn */
    STATUS_HASTE    = (1 << 11),  /* double effective SPD */
    STATUS_BARRIER  = (1 << 12),  /* -50% physical damage */
    STATUS_MBARRIER = (1 << 13),  /* -50% magical damage */
    STATUS_REFLECT  = (1 << 14),  /* reflects single-target spells */
    STATUS_BERSERK  = (1 << 15),  /* +50% ATK, auto-attacks only */
} StatusFlag;

#define STATUS_COUNT    16
#define STATUS_NEGATIVE 0x03FFu   /* bits 0-9 */
#define STATUS_POSITIVE 0xFC00u   /* bits 10-15 */

/* Status tick order: debuffs first, then buffs */
#define STATUS_SKIP_TURN  (STATUS_FREEZE | STATUS_SLEEP | STATUS_STONE)
#define STATUS_DOT        (STATUS_POISON | STATUS_BURN)
#define STATUS_HOT        (STATUS_REGEN)

/* ---- Action types --------------------------------------- */
typedef enum {
    ACTION_NONE = 0,
    ACTION_ATTACK,     /* basic physical attack */
    ACTION_MAGIC,      /* spell from skill list */
    ACTION_ITEM,       /* use item from bag */
    ACTION_DEFEND,     /* guard: reduce damage this turn */
    ACTION_COMBO,      /* joint tech with partner */
    ACTION_SPECIAL,    /* character-unique unleash/limit */
    ACTION_FLEE,       /* attempt to escape */
} ActionType;

/* ---- Target types (for skill definitions) --------------- */
typedef enum {
    TGT_SINGLE_ENEMY = 0,
    TGT_ALL_ENEMIES,
    TGT_SINGLE_ALLY,
    TGT_ALL_ALLIES,
    TGT_SELF,
    TGT_ALL,
    TGT_RANDOM_ENEMY,
    TGT_RANDOM_ALLY,
} TargetType;

/* ---- Skill effect types --------------------------------- */
typedef enum {
    EFF_NONE = 0,
    EFF_DAMAGE,        /* deal element damage */
    EFF_HEAL,          /* restore HP */
    EFF_DRAIN,         /* damage + restore HP to caster */
    EFF_REVIVE,        /* revive KO'd ally */
    EFF_STATUS_ADD,    /* inflict status */
    EFF_STATUS_REMOVE, /* cure status */
    EFF_STAT_MOD,      /* modify ATK/DEF/etc */
    EFF_SUMMON,        /* summon elemental (AOE + secondary effect) */
} EffectType;

/* ---- Stat modifier for EFF_STAT_MOD -------------------- */
typedef enum {
    MOD_ATK = 0,
    MOD_DEF,
    MOD_MAG,
    MOD_MDEF,
    MOD_SPD,
} StatModType;

/* ---- Combo phase --------------------------------------- */
typedef enum {
    COMBO_IDLE = 0,
    COMBO_SELECTING,   /* player picking combo */
    COMBO_READY,       /* all partners committed */
    COMBO_ANIMATING,
} ComboPhase;

/* ---- Battle result ------------------------------------- */
typedef enum {
    BATTLE_RESULT_NONE = 0,
    BATTLE_RESULT_WIN,
    BATTLE_RESULT_LOSE,
    BATTLE_RESULT_FLEE,
    BATTLE_RESULT_DRAW,
} BattleResult;

/* ---- Damage result (returned by formula engine) --------- */
typedef struct {
    s16         damage;       /* positive = damage, negative = heal */
    BOOL        is_critical;
    BOOL        is_miss;
    BOOL        is_absorb;
    ElementType element;
    u8          effectiveness; /* ELEM_WEAK/NEUTRAL/RESIST/etc */
} DamageResult;

/* ---- Stat modifier stage (FF-style: -4 to +4) ----------- */
#define STAT_MOD_MIN  (-4)
#define STAT_MOD_MAX  ( 4)
/* Stage multipliers × 100: {25,34,50,66,100,133,150,200,300}
 * index = stage + 4  (e.g., stage 0 → index 4 → 100%) */
extern const u16 STAT_MOD_TABLE[9];

/* ---- Combo point constants ------------------------------ */
#define CP_MAX             100
#define CP_FROM_ATTACK      10
#define CP_FROM_DAMAGE        5
#define CP_FROM_DEFEND       15
#define CP_COST_COMBO        50   /* each participant spends this */
#define CP_COST_SPECIAL      30

#endif /* BATTLE_TYPES_H */
