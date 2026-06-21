#ifndef CHAR_EQUIP_H
#define CHAR_EQUIP_H

#include "char_types.h"
#include "char_stats.h"
#include "../item/item_db.h"

typedef struct Character Character;  /* forward declaration */

/* =========================================================
 * Equipment System
 *
 * Items live in a flat inventory (INVENTORY_SIZE slots).
 * The Character struct holds 6 equipment slot references
 * (indices into inventory or ITEM_NONE_IDX).
 *
 * Each EquipDef defines:
 *   - Which slot it occupies
 *   - Which classes can use it
 *   - Stat bonuses (flat + percentage)
 *   - Special passive effect (passive_id)
 *   - Level requirement
 *   - Element affinity (for Arcanist staves)
 * ========================================================= */

#define INVENTORY_SIZE    40
#define ITEM_NONE_IDX     0xFF  /* "no item" sentinel in equip slots */

/* Passive equipment effect IDs */
typedef enum {
    EPASSIVE_NONE        = 0,
    EPASSIVE_HP_REGEN,      /* +X HP per turn in battle */
    EPASSIVE_MP_REGEN,      /* +X MP per turn in battle */
    EPASSIVE_ELEM_BOOST,    /* +25% damage for one element */
    EPASSIVE_REFLECT_5,     /* 5% chance to reflect magic */
    EPASSIVE_AUTO_GUARD,    /* 10% chance to block physical */
    EPASSIVE_LIFESTEAL_10,  /* 10% of damage dealt → HP */
    EPASSIVE_CRIT_UP_10,    /* +10 crit chance */
    EPASSIVE_DEF_AURA,      /* party DEF +5 (Guardião only) */
    EPASSIVE_SPELL_AMP,     /* +15% magic damage */
    EPASSIVE_DOUBLESHOT,    /* bow attacks hit twice (Hunter) */
    EPASSIVE_QUICK_DRAW,    /* +10% evasion when bow equipped */
    EPASSIVE_COUNT,
} EquipPassiveID;

typedef struct {
    const char*    name;
    const char*    description;
    ItemType       type;         /* weapon, armor, etc. */
    u8             slot;         /* EQUIP_SLOT_* */
    u8             req_level;    /* minimum level to equip */
    u32            class_mask;   /* which classes can equip (1<<CharClass) */
    ItemRarity     rarity;

    /* Flat stat bonuses applied to derived stats */
    s16            bonus[DSTAT_COUNT];

    /* Attribute bonuses applied to BaseStats */
    s8             attr_bonus[ATTR_COUNT];

    /* Element affinity for staves/orbs */
    u8             element;      /* ElementType, 0=none */

    /* Passive effect */
    EquipPassiveID passive;
    s16            passive_val;  /* value for the passive (e.g., regen amount) */

    u16            buy_price;
    u16            sell_price;
} EquipDef;

/* Inventory slot (runtime) */
typedef struct {
    u8   item_id;   /* index into g_equip_table, 0=empty */
    u8   count;     /* always 1 for equip; >1 for consumables */
} InventorySlot;

/* ---- Item IDs ------------------------------------------- */
/* Weapons */
#define EQUIP_NONE          0
/* Guardian weapons */
#define EQUIP_IRON_SWORD    1
#define EQUIP_STEEL_SWORD   2
#define EQUIP_GUARDIAN_BLADE 3
#define EQUIP_IRON_GS       4   /* greatsword */
#define EQUIP_TITAN_GS      5
/* Arcsword weapons */
#define EQUIP_RUNIC_SWORD   6
#define EQUIP_ARCSWORD_BLADE 7
/* Arcanist weapons */
#define EQUIP_WOODEN_STAFF  8
#define EQUIP_FIRE_STAFF    9
#define EQUIP_ICE_STAFF     10
#define EQUIP_GRAND_ORB     11
/* Hunter weapons */
#define EQUIP_BONE_DAGGER   12
#define EQUIP_SHADOW_DAGGER 13
#define EQUIP_SHORT_BOW     14
#define EQUIP_HUNTER_BOW    15
/* Offhands */
#define EQUIP_IRON_SHIELD   16
#define EQUIP_TOWER_SHIELD  17
#define EQUIP_FOCUS_CRYSTAL 18
#define EQUIP_QUIVER        19
/* Helms */
#define EQUIP_IRON_HELM     20
#define EQUIP_LEATHER_HOOD  21
#define EQUIP_MAGE_HAT      22
/* Armors */
#define EQUIP_CHAIN_ARMOR   23
#define EQUIP_PLATE_ARMOR   24
#define EQUIP_LEATHER_VEST  25
#define EQUIP_ROBE_ARCANE   26
/* Accessories */
#define EQUIP_RING_POWER    27
#define EQUIP_RING_SPEED    28
#define EQUIP_AMULET_MANA   29
#define EQUIP_PENDANT_CRIT  30
#define EQUIP_CLOAK_SHADOW  31  /* Hunter legendary */
#define EQUIP_CROWN_ARCANE  32  /* Arcanist legendary */
#define EQUIP_WARDEN_CREST  33  /* Guardian legendary */
#define EQUIP_ARCSWORD_RELIC 34 /* Arcsword legendary */

#define EQUIP_COUNT         35

/* ---- ROM tables ----------------------------------------- */
extern const EquipDef g_equip_table[EQUIP_COUNT];

/* ---- API ------------------------------------------------ */

void equip_system_init(void);

/* Equip item from inventory into slot (returns FALSE if invalid) */
BOOL equip_item(Character* ch, u8 slot, u8 item_id);

/* Unequip slot (returns item_id or EQUIP_NONE) */
u8 equip_remove(Character* ch, u8 slot);

/* Check if a character can equip this item */
BOOL equip_can_use(const Character* ch, u8 item_id);

/* Compute total equipment bonuses for derived stats */
void equip_compute_bonus(const Character* ch,
                          s16 out_dstat_bonus[DSTAT_COUNT]);

/* Compute total equipment bonuses for base attrs */
void equip_compute_attr_bonus(const Character* ch,
                               s8 out_attr_bonus[ATTR_COUNT]);

/* Get equipped item def (NULL if slot empty) */
const EquipDef* equip_get_slot(const Character* ch, u8 slot);

/* Add item to inventory (returns FALSE if full) */
BOOL inventory_add(Character* ch, u8 item_id);

/* Remove item from inventory (returns FALSE if not found) */
BOOL inventory_remove(Character* ch, u8 item_id);

/* Find item in inventory (returns index or 0xFF) */
u8 inventory_find(const Character* ch, u8 item_id);

const EquipDef* equip_get(u8 id);

#endif /* CHAR_EQUIP_H */
