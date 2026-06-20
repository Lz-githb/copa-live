#include "shop.h"
#include "gba_types.h"

/* ---- Shop ROM table ------------------------------------- */
const ShopDef g_shop_table[SHOP_COUNT] = {

/* 0: SHOP_VILLAGE_GENERAL */
{
    "Mercado da Vila",
    "Seja bem-vindo! Temos suprimentos básicos.",
    { ITEM_POTION, ITEM_ETHER, ITEM_ANTIDOTE, ITEM_ECHO_HERB,
      ITEM_EYE_DROP, ITEM_REVIVE, ITEM_TENT, ITEM_SMOKE_BOMB,
      ITEM_IRON_ORE, ITEM_HERB, ITEM_PURE_WATER, ITEM_NONE },
    11, 100, 50, ITEM_NONE
},

/* 1: SHOP_TOWN_WEAPONS */
{
    "Armaria do Rei",
    "As melhores armas do reino, forjadas à mão.",
    { IITEM_IRON_SWORD, IITEM_STEEL_SWORD, IITEM_IRON_GS,
      IITEM_RUNIC_SWORD, IITEM_BONE_DAGGER, IITEM_SHADOW_DAGGER,
      IITEM_SHORT_BOW, IITEM_HUNTER_BOW, IITEM_WOODEN_STAFF,
      IITEM_FIRE_STAFF, IITEM_ICE_STAFF, ITEM_NONE },
    11, 100, 50, ITEM_NONE
},

/* 2: SHOP_TOWN_ARMOR */
{
    "Forja do Guardião",
    "Proteção de qualidade para heróis corajosos.",
    { IITEM_IRON_HELM, IITEM_LEATHER_HOOD, IITEM_MAGE_HAT,
      IITEM_CHAIN_ARMOR, IITEM_LEATHER_VEST, IITEM_ROBE_ARCANE,
      IITEM_IRON_SHIELD, IITEM_TOWER_SHIELD, IITEM_FOCUS_CRYSTAL,
      IITEM_QUIVER, IITEM_RING_POWER, IITEM_RING_SPEED },
    12, 100, 50, ITEM_NONE
},

/* 3: SHOP_CITY_MAGIC */
{
    "Arcana Celeste",
    "Itens mágicos para os iniciados nas artes arcanas.",
    { IITEM_GRAND_ORB, IITEM_FOCUS_CRYSTAL, IITEM_AMULET_MANA,
      ITEM_ETHER, ITEM_HI_ETHER, ITEM_GRIMOIRE_FIRE,
      ITEM_GRIMOIRE_ICE, ITEM_MANA_CRYSTAL, ITEM_MANA_SHARD,
      ITEM_ARCANE_DUST, ITEM_MAGIC_INK, ITEM_CRYSTAL_SHARD },
    12, 110, 45, ITEM_NONE
},

/* 4: SHOP_GUILD_SUPPLIES */
{
    "Suprimentos da Guilda",
    "Exclusivo para membros da Guilda dos Aventureiros.",
    { ITEM_HI_POTION, ITEM_MEGA_POTION, ITEM_HI_ETHER,
      ITEM_MAIDEN_KISS, ITEM_REMEDY, ITEM_REVIVE,
      ITEM_POWER_DRINK, ITEM_SPEED_DRINK, ITEM_GUARD_DRINK,
      ITEM_MIND_DRINK, ITEM_REGEN_TONIC, ITEM_TENT },
    12, 90, 55, ITEM_GUILD_PASS
},

/* 5: SHOP_BLACK_MARKET */
{
    "Mercado Negro",
    "Não pergunte de onde vieram...",
    { ITEM_SOMA, ITEM_ELIXIR, ITEM_DOOM_SHARD,
      ITEM_GRIMOIRE_VOID, ITEM_LUNAR_CURTAIN, ITEM_HERO_DRINK,
      ITEM_DRAGON_BLOOD, ITEM_SHADOW_ESSENCE, ITEM_VOID_CRYSTAL,
      ITEM_DARK_ESSENCE, ITEM_CHAOS_ORB, ITEM_VOIDSTONE },
    12, 150, 40, ITEM_HERO_BADGE
},

/* 6: SHOP_LEGENDARY_FORGE */
{
    "Forja Lendária",
    "Somente os mais poderosos chegam até aqui.",
    { ITEM_DRAGON_SCALE, ITEM_ANCIENT_RUNE, ITEM_ETERNAL_FLAME,
      ITEM_FROZEN_TEAR, ITEM_STORM_CORE, ITEM_VOID_CRYSTAL,
      ITEM_HOLY_FRAGMENT, ITEM_SHADOW_ESSENCE, ITEM_TITAN_HEART,
      ITEM_MOONSTONE, ITEM_SUNSTONE, ITEM_ASTRAL_DUST },
    12, 120, 60, ITEM_AETHERIA_KEY
},

/* 7: SHOP_ALCHEMIST */
{
    "Alquimia do Velho Mago",
    "Componentes e consumíveis raros para alquimia.",
    { ITEM_POTION, ITEM_HI_POTION, ITEM_ETHER,
      ITEM_SOMA, ITEM_MANA_CRYSTAL, ITEM_REGEN_TONIC,
      ITEM_IRON_ORE, ITEM_MANA_SHARD, ITEM_FIRE_ESSENCE,
      ITEM_ICE_ESSENCE, ITEM_GLOWMOSS, ITEM_SPIDER_SILK },
    12, 105, 50, ITEM_NONE
},

};

/* ---- Runtime state -------------------------------------- */
ShopState g_shop_state;

/* ---- API ------------------------------------------------ */

void shop_open(u8 shop_id)
{
    if (shop_id >= SHOP_COUNT) return;
    g_shop_state.open    = TRUE;
    g_shop_state.shop_id = shop_id;
    g_shop_state.cursor  = 0;
    g_shop_state.mode    = SHOP_MODE_BUY;
}

void shop_close(void)
{
    g_shop_state.open = FALSE;
}

const ShopDef* shop_current(void)
{
    if (!g_shop_state.open) return NULL;
    return &g_shop_table[g_shop_state.shop_id];
}

u32 shop_buy_price(u8 item_id)
{
    const ShopDef* sh = shop_current();
    const ItemDef* it = item_get(item_id);
    u32 base;
    if (!sh || !it) return 0;
    base = (u32)it->buy_price;
    return (u32)(base * sh->buy_pct / 100);
}

u32 shop_sell_price(u8 item_id)
{
    const ShopDef* sh = shop_current();
    const ItemDef* it = item_get(item_id);
    u32 base;
    if (!sh || !it) return 0;
    if (!item_is_sellable(item_id)) return 0;
    base = (u32)it->buy_price;
    /* sell = original buy_price × shop sell_pct% */
    return MAX(1u, (u32)(base * sh->sell_pct / 100));
}

BOOL shop_can_afford(u8 item_id, u8 qty)
{
    u32 price = shop_buy_price(item_id);
    if (price == 0) return FALSE;
    return (BOOL)(g_bag.gold >= price * qty);
}

BOOL shop_has_item(u8 item_id)
{
    const ShopDef* sh = shop_current();
    u8 i;
    if (!sh) return FALSE;
    for (i = 0; i < sh->stock_count; i++) {
        if (sh->stock[i] == item_id) return TRUE;
    }
    return FALSE;
}

BOOL shop_buy(u8 item_id, u8 qty)
{
    const ShopDef* sh = shop_current();
    u32 total;
    u8  i;

    if (!sh) return FALSE;

    /* Check if item is in stock */
    {
        BOOL found = FALSE;
        for (i = 0; i < sh->stock_count; i++) {
            if (sh->stock[i] == item_id) { found = TRUE; break; }
        }
        if (!found) return FALSE;
    }

    /* Check requirement badge */
    if (sh->req_badge != ITEM_NONE &&
        bag_count(sh->req_badge) == 0) return FALSE;

    total = shop_buy_price(item_id) * qty;
    if (!gold_spend(total)) return FALSE;
    if (!bag_add(item_id, qty)) {
        /* Bag was full — refund gold */
        gold_add(total);
        return FALSE;
    }
    return TRUE;
}

BOOL shop_sell(u8 item_id, u8 qty)
{
    const ShopDef* sh = shop_current();
    u32 income;

    if (!sh) return FALSE;
    if (!item_is_sellable(item_id)) return FALSE;
    if (bag_count(item_id) < qty) return FALSE;

    income = shop_sell_price(item_id) * qty;
    bag_remove(item_id, qty);
    gold_add(income);
    return TRUE;
}
