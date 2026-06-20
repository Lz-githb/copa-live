#ifndef ITEM_DB_H
#define ITEM_DB_H

#include "../../gba/gba_types.h"
#include "../character/char_types.h"

/* =========================================================
 * Unified Item Database
 *
 * Item ID space (u8, 0-254):
 *   0         = ITEM_NONE
 *   1-34      = Equipment (cross-references g_equip_table)
 *   35-54     = Consumables (common/uncommon)
 *   55-69     = Consumables (rare)
 *   70-94     = Crafting materials (common)
 *   95-109    = Crafting materials (rare/legendary)
 *   110-119   = Key items
 *   120-127   = Legendary consumables
 *   254       = ITEM_ID_NONE sentinel
 *
 * The party bag (PartyInventory) uses this ID space.
 * Equipment IDs also appear in PartyInventory (stackable qty=1).
 * ========================================================= */

#define ITEM_ID_NONE   0xFF   /* sentinel: "no item" */
#define ITEM_DB_COUNT  128

/* Item category */
typedef enum {
    ICAT_EQUIPMENT  = 0,  /* weapon/armor/accessory */
    ICAT_CONSUMABLE = 1,  /* used and consumed */
    ICAT_MATERIAL   = 2,  /* crafting ingredient */
    ICAT_KEY_ITEM   = 3,  /* story items, unsellable */
} ItemCategory;

/* When/where a consumable can be used */
typedef enum {
    USE_FIELD       = (1 << 0),  /* usable on field/overworld */
    USE_BATTLE      = (1 << 1),  /* usable in battle */
    USE_FIELD_ONLY  = USE_FIELD,
    USE_BATTLE_ONLY = USE_BATTLE,
    USE_ANYWHERE    = (USE_FIELD | USE_BATTLE),
} ItemUseFlags;

/* What a consumable does */
typedef enum {
    CEFF_NONE      = 0,
    CEFF_HEAL_HP,        /* restore val HP */
    CEFF_HEAL_HP_PCT,    /* restore val% of max HP */
    CEFF_HEAL_MP,        /* restore val MP */
    CEFF_HEAL_MP_PCT,    /* restore val% of max MP */
    CEFF_FULL_HEAL,      /* full HP+MP */
    CEFF_REVIVE,         /* revive KO'd char at val% HP */
    CEFF_CURE_STATUS,    /* cure CharPersistStatus bit (param) */
    CEFF_CURE_ALL,       /* cure all status effects */
    CEFF_BUFF_ATK,       /* +val ATK for param turns */
    CEFF_BUFF_DEF,       /* +val DEF for param turns */
    CEFF_BUFF_MAG,       /* +val MAG for param turns */
    CEFF_BUFF_SPD,       /* +val SPD for param turns */
    CEFF_BUFF_ALL,       /* +val all stats for param turns */
    CEFF_PARTY_HEAL_HP,  /* heal all party val HP */
    CEFF_PARTY_FULL,     /* full restore entire party */
    CEFF_FLEE_BATTLE,    /* force escape from battle */
    CEFF_DAMAGE_ENEMY,   /* deal val fire/magic damage (grimoire) */
    CEFF_REGEN_HP,       /* HP regen val per turn for param turns */
    CEFF_XP_BOOST,       /* double XP next battle */
    CEFF_COUNT,
} ConsumableEffect;

/* Single item definition (ROM) */
typedef struct {
    const char*     name;
    const char*     description;
    ItemCategory    category;
    ItemRarity      rarity;

    /* Consumable fields (ignored for ICAT_EQUIPMENT / ICAT_MATERIAL) */
    ConsumableEffect effect;
    s16              effect_val;    /* primary value */
    u8               effect_param;  /* secondary param (turns, status bit) */
    u8               use_flags;     /* ItemUseFlags bitmask */
    u8               target_all;    /* TRUE = affects whole party */

    /* Stacking */
    u8               max_stack;     /* 1 for equipment, 99 for consumables */

    /* Economy */
    u16              buy_price;     /* 0 = not sold in shops */
    u16              sell_price;

    /* For equipment items: cross-ref index into g_equip_table */
    u8               equip_id;      /* EQUIP_NONE if not equipment */
} ItemDef;

/* ---- Item IDs ------------------------------------------- */

#define ITEM_NONE           0

/* Equipment (1-34, mirrors EQUIP_* in char_equip.h) */
#define IITEM_IRON_SWORD    1
#define IITEM_STEEL_SWORD   2
#define IITEM_GUARDIAN_BLADE 3
#define IITEM_IRON_GS       4
#define IITEM_TITAN_GS      5
#define IITEM_RUNIC_SWORD   6
#define IITEM_ARCSWORD_BLADE 7
#define IITEM_WOODEN_STAFF  8
#define IITEM_FIRE_STAFF    9
#define IITEM_ICE_STAFF     10
#define IITEM_GRAND_ORB     11
#define IITEM_BONE_DAGGER   12
#define IITEM_SHADOW_DAGGER 13
#define IITEM_SHORT_BOW     14
#define IITEM_HUNTER_BOW    15
#define IITEM_IRON_SHIELD   16
#define IITEM_TOWER_SHIELD  17
#define IITEM_FOCUS_CRYSTAL 18
#define IITEM_QUIVER        19
#define IITEM_IRON_HELM     20
#define IITEM_LEATHER_HOOD  21
#define IITEM_MAGE_HAT      22
#define IITEM_CHAIN_ARMOR   23
#define IITEM_PLATE_ARMOR   24
#define IITEM_LEATHER_VEST  25
#define IITEM_ROBE_ARCANE   26
#define IITEM_RING_POWER    27
#define IITEM_RING_SPEED    28
#define IITEM_AMULET_MANA   29
#define IITEM_PENDANT_CRIT  30
#define IITEM_CLOAK_SHADOW  31
#define IITEM_CROWN_ARCANE  32
#define IITEM_WARDEN_CREST  33
#define IITEM_ARCSWORD_RELIC 34

/* Common consumables (35-54) */
#define ITEM_POTION         35   /* Heal 80 HP */
#define ITEM_HI_POTION      36   /* Heal 200 HP */
#define ITEM_MEGA_POTION    37   /* Heal 500 HP */
#define ITEM_ETHER          38   /* Restore 30 MP */
#define ITEM_HI_ETHER       39   /* Restore 80 MP */
#define ITEM_ANTIDOTE       40   /* Cure Poison */
#define ITEM_ECHO_HERB      41   /* Cure Silence */
#define ITEM_EYE_DROP       42   /* Cure Blind */
#define ITEM_GOLD_NEEDLE    43   /* Cure Stone */
#define ITEM_MAIDEN_KISS    44   /* Cure all status */
#define ITEM_REVIVE         45   /* Revive at 50% HP */
#define ITEM_TENT           46   /* Field: restore 50% HP/MP party */
#define ITEM_POWER_DRINK    47   /* +10 ATK for 5 turns */
#define ITEM_SPEED_DRINK    48   /* +10 SPD for 5 turns */
#define ITEM_GUARD_DRINK    49   /* +10 DEF for 5 turns */
#define ITEM_MIND_DRINK     50   /* +10 MAG for 5 turns */
#define ITEM_SMOKE_BOMB     51   /* Battle: force flee */
#define ITEM_GRIMOIRE_FIRE  52   /* Deal 150 fire DMG to one enemy */
#define ITEM_GRIMOIRE_ICE   53   /* Deal 150 ice DMG to one enemy */
#define ITEM_REMEDY         54   /* Cure all status (better Maiden Kiss) */

/* Rare consumables (55-69) */
#define ITEM_ELIXIR         55   /* Full HP+MP one char */
#define ITEM_MEGALIXIR      56   /* Full HP+MP all party */
#define ITEM_PHOENIX_DOWN   57   /* Revive at full HP */
#define ITEM_SOMA           58   /* Heal 1000 HP */
#define ITEM_SOMA_PLUS      59   /* Heal 2500 HP */
#define ITEM_LUNAR_CURTAIN  60   /* +50 MDEF 5 turns */
#define ITEM_HERO_DRINK     61   /* All stats +15 for 5 turns */
#define ITEM_DRAGON_BLOOD   62   /* Restore 75% HP+MP */
#define ITEM_AMBROSIA       63   /* Remove curse + full restore */
#define ITEM_DOOM_SHARD     64   /* 50% instant KO one enemy */
#define ITEM_HOLY_WATER     65   /* Double XP next battle */
#define ITEM_REGEN_TONIC    66   /* HP regen for 10 turns */
#define ITEM_MANA_CRYSTAL   67   /* Restore 200 MP */
#define ITEM_COTTAGE        68   /* Field: full restore all party */
#define ITEM_GRIMOIRE_VOID  69   /* Deal 400 dark DMG all enemies */

/* Common crafting materials (70-94) */
#define ITEM_IRON_ORE       70
#define ITEM_STEEL_INGOT    71
#define ITEM_MANA_SHARD     72
#define ITEM_FIRE_ESSENCE   73
#define ITEM_ICE_ESSENCE    74
#define ITEM_EARTH_ESSENCE  75
#define ITEM_WIND_ESSENCE   76
#define ITEM_LIGHT_ESSENCE  77
#define ITEM_DARK_ESSENCE   78
#define ITEM_MONSTER_HIDE   79
#define ITEM_SHADOW_CLOTH   80
#define ITEM_TITAN_SCALE    81
#define ITEM_BONE_FRAGMENT  82
#define ITEM_SPIDER_SILK    83
#define ITEM_CRYSTAL_SHARD  84
#define ITEM_ARCANE_DUST    85
#define ITEM_HERB           86
#define ITEM_PURE_WATER     87
#define ITEM_RESIN          88
#define ITEM_COAL           89
#define ITEM_FEATHER        90
#define ITEM_SNAKE_VENOM    91
#define ITEM_GLOWMOSS       92
#define ITEM_THORN_WOOD     93
#define ITEM_MAGIC_INK      94

/* Rare/Legendary crafting materials (95-109) */
#define ITEM_DRAGON_SCALE   95
#define ITEM_ANCIENT_RUNE   96
#define ITEM_ETERNAL_FLAME  97
#define ITEM_FROZEN_TEAR    98
#define ITEM_STORM_CORE     99
#define ITEM_VOID_CRYSTAL   100
#define ITEM_HOLY_FRAGMENT  101
#define ITEM_SHADOW_ESSENCE 102
#define ITEM_TITAN_HEART    103
#define ITEM_CHAOS_ORB      104
#define ITEM_MOONSTONE      105
#define ITEM_SUNSTONE       106
#define ITEM_VOIDSTONE      107
#define ITEM_DRAGON_FANG    108
#define ITEM_ASTRAL_DUST    109

/* Key items (110-119) */
#define ITEM_DUNGEON_KEY_1  110
#define ITEM_DUNGEON_KEY_2  111
#define ITEM_WORLD_MAP      112
#define ITEM_ANCIENT_CODEX  113
#define ITEM_CRYSTAL_ORB    114
#define ITEM_HERO_BADGE     115
#define ITEM_GUILD_PASS     116
#define ITEM_CELESTIAL_LENS 117
#define ITEM_STAR_SHARD     118
#define ITEM_AETHERIA_KEY   119

/* Legendary consumables (120-127) */
#define ITEM_PHILOSOPHERS_STONE  120  /* +1 permanent level */
#define ITEM_ESSENCE_INFINITY    121  /* Full restore + max stats +5 */
#define ITEM_DIVINE_TEAR         122  /* Revive all + full HP/MP party */
#define ITEM_SOUL_GEM            123  /* Permanent +10% XP multiplier */
#define ITEM_ARCANE_CODEX        124  /* Unlock all skills for character */
#define ITEM_BLADE_HEART         125  /* Legendary material: best weapon */
#define ITEM_SHIELD_HEART        126  /* Legendary material: best armor */
#define ITEM_ASTRAL_CORE         127  /* Legendary material: accessories */

/* ---- ROM table ------------------------------------------ */
extern const ItemDef g_item_db[ITEM_DB_COUNT];

/* Lookup item definition (NULL if id >= ITEM_DB_COUNT) */
const ItemDef* item_get(u8 id);

/* String name for rarity */
const char* item_rarity_name(ItemRarity r);

/* True if this item can be sold */
BOOL item_is_sellable(u8 id);

/* True if this item can stack */
BOOL item_is_stackable(u8 id);

#endif /* ITEM_DB_H */
