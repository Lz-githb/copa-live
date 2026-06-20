#ifndef CHAR_TALENT_H
#define CHAR_TALENT_H

#include "char_types.h"
#include "char_stats.h"

/* =========================================================
 * Talent Tree System
 *
 * Each class has 3 branches × 5 nodes = 15 talent nodes.
 * Each branch focuses on a different playstyle aspect.
 *
 * GUARDIÃO branches:
 *   [0] Fortress  — DEF/HP scaling, damage reduction
 *   [1] Warlord   — ATK/STR, counter-attack
 *   [2] Paladin   — healing, holy damage, aura effects
 *
 * ESPADACHIM ARCANO branches:
 *   [0] Runic Edge — physical power + elemental infusion boost
 *   [1] Arcane Art  — magic scaling, spell enhancement
 *   [2] Resonance   — hybrid synergy, unique combo unlocks
 *
 * ARCANISTA branches:
 *   [0] Pyromancy   — fire spells amplified
 *   [1] Chronomancy — time/speed magic, slow/haste mastery
 *   [2] Void Walker — dark/light synergy, mana manipulation
 *
 * CAÇADOR branches:
 *   [0] Predator    — crit + marked prey, single-target burst
 *   [1] Shadowblade — dual wield, poison, evasion
 *   [2] Ranger      — bow mastery, AOE arrows, traps
 *
 * Node prerequisites:
 *   Node 0: always available (req 0 talents in branch)
 *   Node 1: req node 0 unlocked
 *   Node 2: req node 1 + level ≥ 20
 *   Node 3: req node 2 + level ≥ 40
 *   Node 4: req node 3 + level ≥ 60 (CAPSTONE)
 * ========================================================= */

/* Passive effect IDs for talent nodes */
typedef enum {
    TEFF_NONE        = 0,
    TEFF_ATTR_BONUS,        /* +val to one attribute */
    TEFF_DSTAT_BONUS,       /* +val to derived stat */
    TEFF_UNLOCK_SKILL,      /* unlock a CharSkill */
    TEFF_HP_REGEN_PCT,      /* regen X% max HP per turn */
    TEFF_MP_REGEN_PCT,      /* regen X% max MP per turn */
    TEFF_DMG_BONUS_PCT,     /* +X% physical damage */
    TEFF_MAGIC_AMP_PCT,     /* +X% magical damage */
    TEFF_CRIT_MULT,         /* crit damage ×(val/100 extra) */
    TEFF_ELEM_MASTERY,      /* +30% bonus to one element */
    TEFF_INFUSE_EXTEND,     /* infusion lasts +val turns */
    TEFF_DUAL_STRIKE,       /* dual wield hit chance +val% */
    TEFF_TRAP_POWER,        /* trap damage +val */
    TEFF_CHARGE_MULT,       /* spell charge multiplier +val */
    TEFF_GUARD_AURA,        /* party DEF +val */
    TEFF_COUNTER_CHANCE,    /* counter-attack chance val% */
    TEFF_LIFESTEAL_PCT,     /* lifesteal val% on physical */
    TEFF_MANA_SHIELD_PCT,   /* mana shield absorbs val% */
    TEFF_MARK_BONUS,        /* marked prey damage bonus +val% */
    TEFF_COUNT,
} TalentEffectID;

/* A single talent node */
typedef struct {
    const char*    name;
    const char*    description;
    TalentNodeType type;
    TalentEffectID effect;
    s16            effect_val;     /* value for the effect */
    u8             effect_param;   /* secondary param (attr/stat/skill id) */
    u8             cost;           /* talent points required (default 1) */
    u8             req_level;      /* character level prerequisite */
    u8             req_node;       /* prerequisite node index (0xFF=none) */
} TalentNode;

/* A branch in the talent tree */
typedef struct {
    const char* branch_name;
    TalentNode  nodes[TALENT_NODES];
} TalentBranch;

/* Full talent tree for one class */
typedef struct {
    TalentBranch branches[TALENT_BRANCHES];
} ClassTalentTree;

/* ---- ROM tables ----------------------------------------- */
extern const ClassTalentTree g_talent_trees[CLASS_COUNT];

/* ---- Runtime talent state (per character) --------------- */
/* Stored as a flat bitfield: bit (branch*5 + node) = unlocked */
typedef struct {
    u16  unlocked;       /* bitmask of 15 bits */
    u8   points_spent;
    u8   points_avail;
    /* Cached effect totals (recomputed on unlock) */
    s16  dstat_bonus[DSTAT_COUNT];
    s16  attr_bonus[ATTR_COUNT];
    u8   extra_skills[4]; /* up to 4 skill unlocks from talents */
    u8   extra_skill_count;
    /* Special flags set by capstone nodes */
    u8   flags;
} TalentState;

#define TFLG_GUARDIAN_OATH     (1<<0) /* Paladin capstone */
#define TFLG_ARCSWORD_RESONANCE (1<<1) /* Resonance capstone */
#define TFLG_ARCANIST_VOIDFORM (1<<2) /* Void Walker capstone */
#define TFLG_HUNTER_INSTINCT   (1<<3) /* Predator capstone */

/* ---- API ------------------------------------------------ */

void talent_state_init(TalentState* ts);

/* Gain talent point (call on level up) */
void talent_gain_point(TalentState* ts);

/* Attempt to unlock a talent node */
/* Returns FALSE if: not enough points, prereq not met, or already unlocked */
BOOL talent_unlock(TalentState* ts, CharClass cls, u8 branch, u8 node,
                   u8 char_level);

/* Check if a specific node is unlocked */
BOOL talent_is_unlocked(const TalentState* ts, u8 branch, u8 node);

/* Check if unlock prerequisites are met (does NOT spend points) */
BOOL talent_can_unlock(const TalentState* ts, CharClass cls,
                        u8 branch, u8 node, u8 char_level);

/* Recompute all cached bonuses from scratch */
void talent_recompute_bonuses(TalentState* ts, CharClass cls);

/* Get talent node definition */
const TalentNode* talent_get_node(CharClass cls, u8 branch, u8 node);

/* Get branch definition */
const TalentBranch* talent_get_branch(CharClass cls, u8 branch);

/* Reset entire talent tree (refund all points) */
void talent_reset(TalentState* ts);

#endif /* CHAR_TALENT_H */
