#ifndef CHAR_SKILL_H
#define CHAR_SKILL_H

#include "char_types.h"
#include "../../game/battle/battle_types.h"

/* =========================================================
 * Character Skill System
 *
 * Two skill pools:
 *   1. Class skills (learned by leveling — CharSkillDef table)
 *      These are CHARACTER-level skills that map into the
 *      battle SkillDef table for resolution.
 *
 *   2. Talent passive skills (auto-applied, no slot needed)
 *
 * Active skills occupy CHAR_SKILL_SLOTS (8) hotbar slots.
 * A character can KNOW more skills than fit in the hotbar
 * (CHAR_KNOWN_MAX = 24) and swap freely outside battle.
 *
 * Skill power scales with character level and attributes:
 *   scaled_power = base_power + (level / 10) * power_growth
 *                + attr_scale * relevant_attr
 * ========================================================= */

#define CHAR_SKILL_ID_MAX   64   /* total skill definitions */

typedef struct {
    const char*  name;
    const char*  description;
    CharClass    class_req;      /* which class can learn this */
    SkillKind    kind;           /* active / passive / react / combo */
    u8           mp_cost;
    u8           base_power;
    u8           power_growth;   /* bonus power per 10 levels */
    u8           attr_scale;     /* attr points → power bonus */
    AttrType     scaling_attr;   /* which attr scales damage */
    DamageKind   dmg_kind;       /* physical / magical / hybrid */
    u8           element;        /* ElementType (0=NONE) */
    TargetType   target;
    EffectType   effect;
    StatusFlag   status_inflict;
    u8           status_chance;
    u8           cooldown;       /* turns before reuse (0=none) */
    u8           anim_id;        /* AnimID for battle */
    u8           req_level;      /* minimum character level */
    BOOL         req_dual_wield; /* Hunter dual-wield requirement */
    BOOL         req_bow;        /* Hunter bow requirement */
    const char*  icon_hint;      /* short UI label (4 chars) */
} CharSkillDef;

/* ---- Skill IDs by class --------------------------------- */

/* GUARDIÃO (0-15) */
#define CSKILL_SHIELD_BASH      0   /* stun + physical, req shield */
#define CSKILL_TAUNT            1   /* force enemies to target self */
#define CSKILL_GUARD            2   /* reduce party dmg 30% 1 turn */
#define CSKILL_IRON_WALL        3   /* DEF×2 for 2 turns */
#define CSKILL_GUARDIAN_STRIKE  4   /* phys + scales with DEF */
#define CSKILL_PROVOKE          5   /* single enemy ATK -20% */
#define CSKILL_FORTRESS         6   /* all phys damage →0 for 1 turn */
#define CSKILL_SACRED_GROUND    7   /* AOE holy + party regen */
#define CSKILL_LAST_STAND       8   /* when HP<25%: ATK×2 DEF×2 */
#define CSKILL_WARDEN_OATH      9   /* party cannot die for 1 turn */

/* ESPADACHIM ARCANO (10-25) */
#define CSKILL_RUNIC_SLASH      10  /* phys + fire element */
#define CSKILL_INFUSE_FIRE      11  /* weapon infusion: fire 5 turns */
#define CSKILL_INFUSE_ICE       12  /* weapon infusion: water 5 turns */
#define CSKILL_INFUSE_STORM     13  /* weapon infusion: air 5 turns */
#define CSKILL_ARCANE_STRIKE    14  /* hybrid dmg (avg ATK+MAG) */
#define CSKILL_MANA_BLADE       15  /* phys + drains MP from target */
#define CSKILL_SPELL_PARRY      16  /* reflect next magic attack */
#define CSKILL_RUNIC_BURST      17  /* AOE hybrid, costs infusion */
#define CSKILL_DUAL_RESONANCE   18  /* phys×2 + elem bonus */
#define CSKILL_ARCANE_UNLEASH   19  /* consume infusion → big burst */

/* ARCANISTA (20-35) */
#define CSKILL_ARC_FIRE         20  /* fire spell (scaled) */
#define CSKILL_ARC_BLIZZARD     21  /* water/ice AOE */
#define CSKILL_ARC_QUAKE        22  /* earth AOE */
#define CSKILL_ARC_TEMPEST      23  /* air AOE + blind */
#define CSKILL_ARC_NOVA         24  /* light burst */
#define CSKILL_ARC_VOID         25  /* dark single + drain */
#define CSKILL_SPELLCHARGE      26  /* skip turn → next spell ×2 */
#define CSKILL_MANA_SHIELD      27  /* absorb dmg with MP */
#define CSKILL_CHAIN_BOLT       28  /* hits 3 enemies in sequence */
#define CSKILL_GRAND_ARCANA     29  /* all-element + stagger */

/* CAÇADOR (30-45) */
#define CSKILL_QUICK_SHOT       30  /* fast bow attack */
#define CSKILL_POISON_ARROW     31  /* bow + poison */
#define CSKILL_SNIPE            32  /* high-power single bow */
#define CSKILL_SHADOW_STEP      33  /* evasion+1 turn + dash strike */
#define CSKILL_DUAL_FANG        34  /* two dagger hits, req dual */
#define CSKILL_SMOKE_BOMB       35  /* all enemies blinded */
#define CSKILL_TRAP_BIND        36  /* root 1 enemy 2 turns */
#define CSKILL_MARKED_PREY      37  /* target takes +30% dmg */
#define CSKILL_RAIN_OF_ARROWS   38  /* AOE bow: all enemies */
#define CSKILL_DEATHMARK        39  /* if target HP<20% → instant KO */

/* Shared skills (all classes) */
#define CSKILL_BASIC_ATTACK     40
#define CSKILL_DEFEND           41
#define CSKILL_USE_ITEM         42

#define CSKILL_COUNT            43

/* ---- ROM table ------------------------------------------ */
extern const CharSkillDef g_char_skill_table[CSKILL_COUNT];

/* ---- API ------------------------------------------------ */

void char_skill_init(void);

/* Compute effective skill power at given level/attr */
s16 skill_effective_power(u8 skill_id, u8 char_level, s16 scaling_attr_val);

/* Check if character can use skill right now */
BOOL skill_can_use(const struct Character* ch, u8 char_skill_id);

/* Learn a skill (add to known list, returns FALSE if already known) */
BOOL skill_learn(struct Character* ch, u8 char_skill_id);

/* Equip skill to hotbar slot (returns FALSE if not known) */
BOOL skill_equip_to_slot(struct Character* ch, u8 hotbar_slot, u8 char_skill_id);

/* Translate char_skill_id → battle system skill_id for resolution */
u8 skill_to_battle_id(u8 char_skill_id, u8 char_level,
                       const struct Character* ch);

/* Check if character knows a skill */
BOOL skill_is_known(const struct Character* ch, u8 char_skill_id);

const CharSkillDef* skill_char_get(u8 id);

#endif /* CHAR_SKILL_H */
