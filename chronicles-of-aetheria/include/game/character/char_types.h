#ifndef CHAR_TYPES_H
#define CHAR_TYPES_H

#include "../../engine/engine.h"

/* =========================================================
 * Character System — Shared Types & Constants
 *
 * Party of 4 characters, each with:
 *   - Class (determines stat growth + skill pool)
 *   - Level 1-99
 *   - 8 base attributes
 *   - 6 equipment slots
 *   - Up to 8 active skills
 *   - Talent tree (3 branches, 5 nodes each = 15 per class)
 *
 * ROM tables hold all definitions (class defs, skill defs,
 * talent defs, XP curve). Runtime Character structs live in
 * EWRAM persistent pool.
 * ========================================================= */

/* ---- Party & Level ------------------------------------- */
#define PARTY_SIZE          4
#define LEVEL_MIN           1
#define LEVEL_MAX           99
#define TALENT_POINTS_MAX   98  /* 1 per level after lv1 */

/* ---- Equipment slots ----------------------------------- */
#define EQUIP_SLOT_WEAPON   0
#define EQUIP_SLOT_OFFHAND  1   /* shield or second weapon */
#define EQUIP_SLOT_HELM     2
#define EQUIP_SLOT_ARMOR    3
#define EQUIP_SLOT_ACC1     4
#define EQUIP_SLOT_ACC2     5
#define EQUIP_SLOT_COUNT    6

/* ---- Active skill slots per character ------------------ */
#define CHAR_SKILL_SLOTS    8   /* equipped active skills */
#define CHAR_KNOWN_MAX      24  /* total learnable skills per class */

/* ---- Talent tree layout -------------------------------- */
#define TALENT_BRANCHES     3   /* branches per class */
#define TALENT_NODES        5   /* nodes per branch */
#define TALENT_TOTAL        15  /* TALENT_BRANCHES × TALENT_NODES */
#define TALENT_COST_PER     1   /* talent points per node (base) */

/* ---- Character classes --------------------------------- */
typedef enum {
    CLASS_GUARDIAN   = 0,  /* Guardião  — tank, fortress */
    CLASS_ARCSWORD   = 1,  /* Espadachim Arcano — hybrid */
    CLASS_ARCANIST   = 2,  /* Arcanista — pure magic */
    CLASS_HUNTER     = 3,  /* Caçador   — speed, ranged */
    CLASS_COUNT      = 4,
} CharClass;

/* ---- Primary attributes -------------------------------- */
typedef enum {
    ATTR_VIT  = 0,  /* Vitality   → HP pool */
    ATTR_END  = 1,  /* Endurance  → DEF, HP regen */
    ATTR_STR  = 2,  /* Strength   → ATK, carry weight */
    ATTR_DEX  = 3,  /* Dexterity  → SPD, crit chance */
    ATTR_INT  = 4,  /* Intellect  → MAG, MP pool */
    ATTR_WIS  = 5,  /* Wisdom     → MDEF, MP regen */
    ATTR_LCK  = 6,  /* Luck       → crit, item drop */
    ATTR_CHA  = 7,  /* Charisma   → shop prices, NPC */
    ATTR_COUNT = 8,
} AttrType;

/* ---- Derived stats (computed from attrs + equip) ------- */
typedef enum {
    DSTAT_HP_MAX  = 0,
    DSTAT_MP_MAX  = 1,
    DSTAT_ATK     = 2,
    DSTAT_DEF     = 3,
    DSTAT_MAG     = 4,
    DSTAT_MDEF    = 5,
    DSTAT_SPD     = 6,
    DSTAT_CRIT    = 7,   /* crit chance 0-100 */
    DSTAT_EVASION = 8,   /* evasion 0-100 */
    DSTAT_COUNT   = 9,
} DStatType;

/* ---- Item / equipment types ----------------------------- */
typedef enum {
    ITEM_NONE     = 0,
    ITEM_SWORD,         /* Espadachim Arcano, Guardião */
    ITEM_GREATSWORD,    /* Guardião */
    ITEM_DAGGER,        /* Caçador */
    ITEM_BOW,           /* Caçador */
    ITEM_STAFF,         /* Arcanista */
    ITEM_ORB,           /* Arcanista */
    ITEM_SHIELD,        /* Guardião */
    ITEM_FOCUS,         /* Espadachim Arcano offhand */
    ITEM_QUIVER,        /* Caçador offhand */
    ITEM_HELM_HEAVY,
    ITEM_HELM_MEDIUM,
    ITEM_HELM_LIGHT,
    ITEM_ARMOR_HEAVY,
    ITEM_ARMOR_MEDIUM,
    ITEM_ARMOR_LIGHT,
    ITEM_ROBE,          /* Arcanista */
    ITEM_ACCESSORY,
    ITEM_COUNT,
} ItemType;

/* ---- Skill types --------------------------------------- */
typedef enum {
    SKTYPE_ACTIVE  = 0,  /* uses MP, fills action slot */
    SKTYPE_PASSIVE = 1,  /* always-on stat/effect bonus */
    SKTYPE_REACT   = 2,  /* triggers on specific condition */
    SKTYPE_COMBO   = 3,  /* requires partner */
} SkillKind;

/* ---- Talent node types --------------------------------- */
typedef enum {
    TALENT_STAT_BONUS = 0,  /* +X to a derived stat */
    TALENT_UNLOCK_SKILL,    /* unlocks a new active skill */
    TALENT_PASSIVE_EFF,     /* passive effect (regen, reflect%) */
    TALENT_ENHANCE_SKILL,   /* upgrades an existing skill */
    TALENT_CAPSTONE,        /* branch capstone (powerful effect) */
} TalentNodeType;

/* ---- Elemental infusion (Arcsword-specific) ------------ */
typedef enum {
    INFUSE_NONE  = 0,
    INFUSE_FIRE  = 1,
    INFUSE_WATER = 2,
    INFUSE_EARTH = 3,
    INFUSE_AIR   = 4,
    INFUSE_LIGHT = 5,
    INFUSE_DARK  = 6,
} InfusionType;

/* ---- Persistent character status (out-of-battle) ------- */
typedef enum {
    CHAR_STATUS_NORMAL   = 0,
    CHAR_STATUS_KO       = (1 << 0),  /* knocked out, 1HP after rest */
    CHAR_STATUS_CURSED   = (1 << 1),  /* can't gain XP until cured */
    CHAR_STATUS_POISONED = (1 << 2),  /* -1 HP per map step */
    CHAR_STATUS_STONE    = (1 << 3),  /* cannot act at all */
} CharPersistStatus;

/* ---- Stat growth grade (per attribute per class) ------- */
/* Determines how much an attribute grows per level */
typedef enum {
    GROWTH_E = 0,  /* very slow */
    GROWTH_D = 1,
    GROWTH_C = 2,  /* average */
    GROWTH_B = 3,
    GROWTH_A = 4,  /* fast */
    GROWTH_S = 5,  /* exceptional */
} GrowthGrade;

/* ---- Damage types (for skill affinities) --------------- */
typedef enum {
    DMG_PHYSICAL = 0,
    DMG_MAGICAL  = 1,
    DMG_TRUE     = 2,  /* ignores DEF/MDEF */
    DMG_HYBRID   = 3,  /* avg of ATK and MAG */
} DamageKind;

#endif /* CHAR_TYPES_H */
