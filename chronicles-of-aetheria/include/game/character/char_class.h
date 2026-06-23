#ifndef CHAR_CLASS_H
#define CHAR_CLASS_H

#include "char_types.h"
#include "char_stats.h"

/* =========================================================
 * Class Definitions
 *
 * Guardião    — High VIT/END/STR, heavy armor, shields.
 *               Specializes in taunting enemies, guarding
 *               allies, and dealing sustained physical damage.
 *               Drawback: low SPD, no magic.
 *
 * Espadachim Arcano — Balanced STR/INT/DEX. Can infuse
 *               physical attacks with elemental magic.
 *               Exclusive mechanic: INFUSION (5 turns),
 *               changes damage type and element.
 *               Drawback: master of none, MP-hungry.
 *
 * Arcanista   — High INT/WIS, low physical stats.
 *               Largest spell pool, elemental mastery.
 *               Exclusive mechanic: SPELLCHARGE — charge
 *               1 turn to double next spell damage.
 *               Drawback: glass cannon, can't equip armor.
 *
 * Caçador     — High DEX/LCK/SPD, medium STR.
 *               Can attack twice (dual wield daggers) or
 *               from range (bow). Sets traps that trigger
 *               on enemy movement.
 *               Drawback: low DEF/MDEF, low HP.
 * ========================================================= */

/* Allowed weapon types per class (bitmask) */
#define CLS_GUARDIAN_WEAPONS  ((1<<ITEM_SWORD)|(1<<ITEM_GREATSWORD))
#define CLS_ARCSWORD_WEAPONS  ((1<<ITEM_SWORD)|(1<<ITEM_DAGGER))
#define CLS_ARCANIST_WEAPONS  ((1<<ITEM_STAFF)|(1<<ITEM_ORB))
#define CLS_HUNTER_WEAPONS    ((1<<ITEM_DAGGER)|(1<<ITEM_BOW))

/* Allowed offhand types */
#define CLS_GUARDIAN_OFFHAND  ((1<<ITEM_SHIELD))
#define CLS_ARCSWORD_OFFHAND  ((1<<ITEM_SWORD)|(1<<ITEM_FOCUS))
#define CLS_ARCANIST_OFFHAND  ((1<<ITEM_ORB))
#define CLS_HUNTER_OFFHAND    ((1<<ITEM_QUIVER)|(1<<ITEM_DAGGER))

/* Allowed armor types */
#define CLS_GUARDIAN_ARMOR    ((1<<ITEM_ARMOR_HEAVY)|(1<<ITEM_ARMOR_MEDIUM))
#define CLS_ARCSWORD_ARMOR    ((1<<ITEM_ARMOR_MEDIUM)|(1<<ITEM_ARMOR_LIGHT))
#define CLS_ARCANIST_ARMOR    ((1<<ITEM_ROBE))
#define CLS_HUNTER_ARMOR      ((1<<ITEM_ARMOR_LIGHT)|(1<<ITEM_ARMOR_MEDIUM))

/* Allowed helm types */
#define CLS_GUARDIAN_HELM     ((1<<ITEM_HELM_HEAVY)|(1<<ITEM_HELM_MEDIUM))
#define CLS_ARCSWORD_HELM     ((1<<ITEM_HELM_MEDIUM)|(1<<ITEM_HELM_LIGHT))
#define CLS_ARCANIST_HELM     ((1<<ITEM_HELM_LIGHT))
#define CLS_HUNTER_HELM       ((1<<ITEM_HELM_LIGHT)|(1<<ITEM_HELM_MEDIUM))

/* Skill unlock levels per class (CHAR_KNOWN_MAX = 24 skills) */
#define CLASS_SKILL_UNLOCK_MAX  24

typedef struct {
    u8  skill_id;    /* index into g_char_skill_table */
    u8  req_level;   /* minimum level to unlock */
} SkillUnlock;

/* Full class definition (ROM) */
typedef struct {
    const char*     name;
    const char*     description;
    u8              sprite_base;     /* base tile for class icon */

    /* Starting attribute values at level 1 */
    s16             start_attrs[ATTR_COUNT];

    /* Equipment allowed (bitmask, bit = ItemType) */
    u32             weapon_mask;
    u32             offhand_mask;
    u32             armor_mask;
    u32             helm_mask;

    /* Class stat bonuses added to derived stat formula */
    ClassStatBonus  stat_bonus;

    /* Skill unlock table for this class */
    SkillUnlock     skill_unlocks[CLASS_SKILL_UNLOCK_MAX];
    u8              skill_unlock_count;

    /* Class-unique passive: description */
    const char*     passive_name;
    const char*     passive_desc;

    /* Class-unique mechanic ID (for special logic) */
    u8              mechanic_id;

} ClassDef;

/* Mechanic IDs */
#define MECH_GUARDIAN_TAUNT    0   /* taunt active while HP>50% */
#define MECH_ARCSWORD_INFUSE   1   /* weapon infusion system */
#define MECH_ARCANIST_CHARGE   2   /* spell charge for 2× damage */
#define MECH_HUNTER_DUALSTRIKE 3   /* auto-dual attack if dual-wield */

/* ---- ROM tables ----------------------------------------- */
extern const ClassDef g_class_table[CLASS_COUNT];

/* ---- API ------------------------------------------------ */

const ClassDef* class_get(CharClass cls);

/* Returns TRUE if item_type is allowed for this class in slot */
BOOL class_can_equip(CharClass cls, u8 slot, ItemType item_type);

/* Returns the battle sprite set for a class */
u8 class_get_sprite(CharClass cls);

/* Short class name (for UI) */
const char* class_name(CharClass cls);

#endif /* CHAR_CLASS_H */
