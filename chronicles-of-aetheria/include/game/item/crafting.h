#ifndef CRAFTING_H
#define CRAFTING_H

#include "item_db.h"
#include "inventory.h"

/* =========================================================
 * Crafting System
 *
 * Fixed recipe table in ROM. Each recipe has up to 4
 * ingredient slots and produces 1-99 of a result item.
 *
 * Recipes are discovered progressively:
 *   - Some start unlocked (basic recipes)
 *   - Others unlock on finding an item, reaching a level,
 *     or reading an ITEM_ANCIENT_CODEX
 *
 * Unlocked state stored as a bitmask in save data.
 * ========================================================= */

#define RECIPE_INGREDIENT_MAX  4
#define RECIPE_COUNT           24

/* Crafting difficulty (shown in UI) */
typedef enum {
    CRAFT_BASIC    = 0,   /* no special tools needed */
    CRAFT_FORGE,          /* requires forge (blacksmith NPC) */
    CRAFT_ALCHEMY,        /* requires alchemy kit */
    CRAFT_LEGENDARY,      /* requires legendary forge */
} CraftDifficulty;

/* One ingredient requirement */
typedef struct {
    u8 item_id;   /* ITEM_NONE = slot unused */
    u8 quantity;
} Ingredient;

/* A recipe definition */
typedef struct {
    const char*    name;
    const char*    description;
    Ingredient     ingredients[RECIPE_INGREDIENT_MAX];
    u8             result_id;     /* what you get */
    u8             result_qty;    /* how many */
    CraftDifficulty difficulty;
    u8             req_level;     /* min character level to craft */
    BOOL           known_default; /* TRUE = unlocked from the start */
} CraftRecipe;

/* ---- Recipe IDs ---------------------------------------- */
#define RECIPE_STEEL_INGOT      0   /* Iron Ore x2 → Steel Ingot */
#define RECIPE_HI_POTION        1   /* Herb x2 + Pure Water → Hi-Potion */
#define RECIPE_ETHER            2   /* Mana Shard x1 + Pure Water → Ether */
#define RECIPE_ANTIDOTE_PLUS    3   /* Antidote x2 + Herb → Remedy */
#define RECIPE_CHAIN_ARMOR      4   /* Steel Ingot x3 + Monster Hide → Chain Armor */
#define RECIPE_RUNIC_SWORD      5   /* Steel Ingot x2 + Mana Shard x2 → Runic Sword */
#define RECIPE_FIRE_STAFF       6   /* Thorn Wood x2 + Fire Essence x3 → Fire Staff */
#define RECIPE_ICE_STAFF        7   /* Thorn Wood x2 + Ice Essence x3 → Ice Staff */
#define RECIPE_SHADOW_DAGGER    8   /* Steel Ingot x2 + Shadow Cloth x2 → Shadow Dagger */
#define RECIPE_HUNTER_BOW       9   /* Thorn Wood x3 + Spider Silk x2 → Hunter Bow */
#define RECIPE_GRAND_ORB        10  /* Crystal Shard x3 + Arcane Dust x2 → Grand Orb */
#define RECIPE_PLATE_ARMOR      11  /* Steel Ingot x4 + Titan Scale x2 → Plate Armor */
#define RECIPE_ELIXIR           12  /* Hi-Potion x2 + Hi-Ether x2 → Elixir */
#define RECIPE_SOMA             13  /* Hi-Potion x3 + Dragon Blood x1 → Soma */
#define RECIPE_GUARDIAN_BLADE   14  /* Steel Ingot x3 + Light Essence x2 → Guardian Blade */
#define RECIPE_ARCSWORD_BLADE   15  /* Runic Sword x1 + Arcane Dust x3 → Arcsword Blade */
/* Legendary recipes (require rare materials) */
#define RECIPE_TITAN_GS         16  /* Dragon Scale x2 + Titan Heart + Ancient Rune → Titan GS */
#define RECIPE_CLOAK_SHADOW     17  /* Shadow Essence x3 + Void Crystal + Spider Silk x4 → Cloak of Shadows */
#define RECIPE_CROWN_ARCANE     18  /* Moonstone + Void Crystal + Chaos Orb → Crown Arcane */
#define RECIPE_WARDEN_CREST     19  /* Titan Heart + Holy Fragment x2 + Sunstone → Warden Crest */
#define RECIPE_ARCSWORD_RELIC   20  /* Dragon Fang + Eternal Flame + Storm Core → Arcsword Relic */
#define RECIPE_MEGALIXIR        21  /* Elixir x3 + Astral Dust x2 → Megalixir */
#define RECIPE_DOOM_SHARD       22  /* Dark Essence x3 + Void Crystal → Doom Shard */
#define RECIPE_DIVINE_TEAR      23  /* Holy Fragment x3 + Eternal Flame + Phoenix Down → Divine Tear */

/* ---- ROM table ------------------------------------------ */
extern const CraftRecipe g_recipe_table[RECIPE_COUNT];

/* ---- Runtime state -------------------------------------- */
/* Bitmask of discovered recipes (RECIPE_COUNT <= 32) */
extern u32 g_recipes_known;

/* ---- API ------------------------------------------------ */

/* Init crafting system (default recipes unlocked) */
void crafting_init(void);

/* Is recipe discovered? */
BOOL craft_is_known(u8 recipe_id);

/* Unlock a recipe */
void craft_discover(u8 recipe_id);

/* Do we have the ingredients in g_bag? */
BOOL craft_can_make(u8 recipe_id);

/* Consume ingredients, produce result. Returns FALSE if missing
   ingredients or recipe not known. */
BOOL craft_execute(u8 recipe_id);

/* Serialize discovery bitmask for save */
u32  craft_serialize(void);
void craft_deserialize(u32 data);

#endif /* CRAFTING_H */
