#include "crafting.h"
#include "gba_types.h"

/* ---- Ingredient shorthand ------------------------------- */
#define ING(id, qty) { id, qty }
#define ING0         { ITEM_NONE, 0 }

/* ---- Recipe ROM table ----------------------------------- */
const CraftRecipe g_recipe_table[RECIPE_COUNT] = {

/* 0 */ {
    "Lingote de Aço",
    "Forja minério de ferro bruto em aço refinado.",
    { ING(ITEM_IRON_ORE,2), ING(ITEM_COAL,1), ING0, ING0 },
    ITEM_STEEL_INGOT, 1, CRAFT_FORGE, 1, TRUE
},
/* 1 */ {
    "Hi-Poção",
    "Poção mais potente feita com ervas e água pura.",
    { ING(ITEM_HERB,2), ING(ITEM_PURE_WATER,1), ING0, ING0 },
    ITEM_HI_POTION, 1, CRAFT_ALCHEMY, 1, TRUE
},
/* 2 */ {
    "Éter",
    "Restaurador de mana destilado de cristais.",
    { ING(ITEM_MANA_SHARD,1), ING(ITEM_PURE_WATER,1), ING0, ING0 },
    ITEM_ETHER, 1, CRAFT_ALCHEMY, 1, TRUE
},
/* 3 */ {
    "Remédio",
    "Cura avançada de status combinando antídotos.",
    { ING(ITEM_ANTIDOTE,2), ING(ITEM_HERB,1), ING(ITEM_PURE_WATER,1), ING0 },
    ITEM_REMEDY, 1, CRAFT_ALCHEMY, 5, FALSE
},
/* 4 */ {
    "Armadura de Malha",
    "Armadura de correntes com escamas de monstro.",
    { ING(ITEM_STEEL_INGOT,3), ING(ITEM_MONSTER_HIDE,2), ING0, ING0 },
    IITEM_CHAIN_ARMOR, 1, CRAFT_FORGE, 10, FALSE
},
/* 5 */ {
    "Espada Rúnica",
    "Lâmina de aço infundida com magia rúnica.",
    { ING(ITEM_STEEL_INGOT,2), ING(ITEM_MANA_SHARD,2), ING(ITEM_ARCANE_DUST,1), ING0 },
    IITEM_RUNIC_SWORD, 1, CRAFT_FORGE, 12, FALSE
},
/* 6 */ {
    "Cajado de Fogo",
    "Cajado que canaliza chamas elementais.",
    { ING(ITEM_THORN_WOOD,2), ING(ITEM_FIRE_ESSENCE,3), ING(ITEM_RESIN,1), ING0 },
    IITEM_FIRE_STAFF, 1, CRAFT_ALCHEMY, 10, FALSE
},
/* 7 */ {
    "Cajado de Gelo",
    "Cajado que canaliza frio elemental.",
    { ING(ITEM_THORN_WOOD,2), ING(ITEM_ICE_ESSENCE,3), ING(ITEM_RESIN,1), ING0 },
    IITEM_ICE_STAFF, 1, CRAFT_ALCHEMY, 10, FALSE
},
/* 8 */ {
    "Adaga das Sombras",
    "Adaga envenenada forjada com tecido místico.",
    { ING(ITEM_STEEL_INGOT,2), ING(ITEM_SHADOW_CLOTH,2), ING(ITEM_SNAKE_VENOM,1), ING0 },
    IITEM_SHADOW_DAGGER, 1, CRAFT_FORGE, 14, FALSE
},
/* 9 */ {
    "Arco do Caçador",
    "Arco de longa distância com fio de aranha.",
    { ING(ITEM_THORN_WOOD,3), ING(ITEM_SPIDER_SILK,2), ING(ITEM_FEATHER,2), ING0 },
    IITEM_HUNTER_BOW, 1, CRAFT_BASIC, 14, FALSE
},
/* 10 */ {
    "Grande Orbe",
    "Orbe de cristal que potencializa o Arcanista.",
    { ING(ITEM_CRYSTAL_SHARD,3), ING(ITEM_ARCANE_DUST,2), ING(ITEM_MAGIC_INK,2), ING0 },
    IITEM_GRAND_ORB, 1, CRAFT_ALCHEMY, 16, FALSE
},
/* 11 */ {
    "Armadura de Placa",
    "A mais pesada proteção. Requer escamas de titã.",
    { ING(ITEM_STEEL_INGOT,4), ING(ITEM_TITAN_SCALE,2), ING(ITEM_COAL,2), ING0 },
    IITEM_PLATE_ARMOR, 1, CRAFT_FORGE, 20, FALSE
},
/* 12 */ {
    "Elixir",
    "Restaura HP e MP completamente.",
    { ING(ITEM_HI_POTION,2), ING(ITEM_HI_ETHER,2), ING(ITEM_GLOWMOSS,1), ING0 },
    ITEM_ELIXIR, 1, CRAFT_ALCHEMY, 15, FALSE
},
/* 13 */ {
    "Soma",
    "Poção avançada que recupera 1000 HP.",
    { ING(ITEM_HI_POTION,3), ING(ITEM_DRAGON_BLOOD,1), ING(ITEM_GLOWMOSS,2), ING0 },
    ITEM_SOMA, 1, CRAFT_ALCHEMY, 20, FALSE
},
/* 14 */ {
    "Lâmina do Guardião",
    "Espada abençoada com luz sagrada.",
    { ING(ITEM_STEEL_INGOT,3), ING(ITEM_LIGHT_ESSENCE,2), ING(ITEM_CRYSTAL_SHARD,1), ING0 },
    IITEM_GUARDIAN_BLADE, 1, CRAFT_FORGE, 18, FALSE
},
/* 15 */ {
    "Lâmina Arcana",
    "Espada Rúnica aprimorada com pó arcano.",
    { ING(IITEM_RUNIC_SWORD,1), ING(ITEM_ARCANE_DUST,3), ING(ITEM_MANA_SHARD,3), ING0 },
    IITEM_ARCSWORD_BLADE, 1, CRAFT_FORGE, 25, FALSE
},

/* ---- Legendary recipes (require rare materials) -------- */
/* 16 */ {
    "Espadão do Titã",
    "Lâmina lendária forjada com sangue de dragão e runas antigas.",
    { ING(ITEM_DRAGON_SCALE,2), ING(ITEM_TITAN_HEART,1), ING(ITEM_ANCIENT_RUNE,1), ING(ITEM_STEEL_INGOT,4) },
    IITEM_TITAN_GS, 1, CRAFT_LEGENDARY, 50, FALSE
},
/* 17 */ {
    "Manto das Sombras",
    "Capa lendária do Caçador. Feita de sombra cristalizada.",
    { ING(ITEM_SHADOW_ESSENCE,3), ING(ITEM_VOID_CRYSTAL,1), ING(ITEM_SPIDER_SILK,4), ING(ITEM_DARK_ESSENCE,3) },
    IITEM_CLOAK_SHADOW, 1, CRAFT_LEGENDARY, 50, FALSE
},
/* 18 */ {
    "Coroa Arcana",
    "Elmo lendário do Arcanista. Canaliza poder cósmico.",
    { ING(ITEM_MOONSTONE,1), ING(ITEM_VOID_CRYSTAL,1), ING(ITEM_CHAOS_ORB,1), ING(ITEM_ARCANE_DUST,5) },
    IITEM_CROWN_ARCANE, 1, CRAFT_LEGENDARY, 55, FALSE
},
/* 19 */ {
    "Crista do Guardião",
    "Elmo lendário do Guardião. Benzido por fragmentos sagrados.",
    { ING(ITEM_TITAN_HEART,1), ING(ITEM_HOLY_FRAGMENT,2), ING(ITEM_SUNSTONE,1), ING(ITEM_STEEL_INGOT,5) },
    IITEM_WARDEN_CREST, 1, CRAFT_LEGENDARY, 55, FALSE
},
/* 20 */ {
    "Relíquia Arcana",
    "Arma lendária do Espadachim forjada com presa de dragão.",
    { ING(ITEM_DRAGON_FANG,1), ING(ITEM_ETERNAL_FLAME,1), ING(ITEM_STORM_CORE,1), ING(ITEM_ANCIENT_RUNE,2) },
    IITEM_ARCSWORD_RELIC, 1, CRAFT_LEGENDARY, 60, FALSE
},
/* 21 */ {
    "Megalixir",
    "Restaura HP e MP de todo o grupo.",
    { ING(ITEM_ELIXIR,3), ING(ITEM_ASTRAL_DUST,2), ING0, ING0 },
    ITEM_MEGALIXIR, 1, CRAFT_ALCHEMY, 30, FALSE
},
/* 22 */ {
    "Fragmento da Morte",
    "Cristal sombrio com poder de KO instantâneo.",
    { ING(ITEM_DARK_ESSENCE,3), ING(ITEM_VOID_CRYSTAL,1), ING(ITEM_SHADOW_CLOTH,2), ING0 },
    ITEM_DOOM_SHARD, 2, CRAFT_ALCHEMY, 35, FALSE
},
/* 23 */ {
    "Lágrima Divina",
    "O item mais poderoso de cura. Revive e restaura todos.",
    { ING(ITEM_HOLY_FRAGMENT,3), ING(ITEM_ETERNAL_FLAME,1), ING(ITEM_PHOENIX_DOWN,1), ING(ITEM_ASTRAL_DUST,2) },
    ITEM_DIVINE_TEAR, 1, CRAFT_LEGENDARY, 60, FALSE
},
};

#undef ING
#undef ING0

/* ---- Runtime state -------------------------------------- */
u32 g_recipes_known = 0;

/* ---- API ------------------------------------------------ */

void crafting_init(void)
{
    u8 i;
    g_recipes_known = 0;
    for (i = 0; i < RECIPE_COUNT; i++) {
        if (g_recipe_table[i].known_default)
            g_recipes_known |= (u32)(1u << i);
    }
}

BOOL craft_is_known(u8 recipe_id)
{
    if (recipe_id >= RECIPE_COUNT) return FALSE;
    return (BOOL)((g_recipes_known & (u32)(1u << recipe_id)) != 0);
}

void craft_discover(u8 recipe_id)
{
    if (recipe_id < RECIPE_COUNT)
        g_recipes_known |= (u32)(1u << recipe_id);
}

BOOL craft_can_make(u8 recipe_id)
{
    const CraftRecipe* r;
    u8 i;
    if (!craft_is_known(recipe_id)) return FALSE;
    r = &g_recipe_table[recipe_id];
    for (i = 0; i < RECIPE_INGREDIENT_MAX; i++) {
        if (r->ingredients[i].item_id == ITEM_NONE) break;
        if (bag_count(r->ingredients[i].item_id) < r->ingredients[i].quantity)
            return FALSE;
    }
    return TRUE;
}

BOOL craft_execute(u8 recipe_id)
{
    const CraftRecipe* r;
    u8 i;
    if (!craft_can_make(recipe_id)) return FALSE;
    r = &g_recipe_table[recipe_id];

    /* Consume ingredients */
    for (i = 0; i < RECIPE_INGREDIENT_MAX; i++) {
        if (r->ingredients[i].item_id == ITEM_NONE) break;
        bag_remove(r->ingredients[i].item_id, r->ingredients[i].quantity);
    }

    /* Add result (may fail if bag is full) */
    return bag_add(r->result_id, r->result_qty);
}

u32 craft_serialize(void)
{
    return g_recipes_known;
}

void craft_deserialize(u32 data)
{
    g_recipes_known = data;
}
