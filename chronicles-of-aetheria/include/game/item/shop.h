#ifndef SHOP_H
#define SHOP_H

#include "item_db.h"
#include "inventory.h"

/* =========================================================
 * Shop System
 *
 * Shops are ROM-defined lists of item IDs for sale.
 * Prices use the ItemDef buy/sell prices with optional
 * per-shop multipliers.
 *
 * Buy:  player spends gold, item goes into g_bag.
 * Sell: item removed from g_bag, player gains gold
 *       (sell price = buy_price / 2, min 1).
 *
 * Each shop has:
 *   - Name
 *   - Stock list (up to 12 items)
 *   - Buy multiplier (default 100 = 100%)
 *   - Sell multiplier (default 50 = 50% of buy price)
 ========================================================= */

#define SHOP_STOCK_MAX    12
#define SHOP_COUNT         8

/* Shop definition (ROM) */
typedef struct {
    const char* name;
    const char* greeting;
    u8          stock[SHOP_STOCK_MAX];   /* item IDs, ITEM_NONE = end */
    u8          stock_count;
    u8          buy_pct;    /* price multiplier, 100 = normal */
    u8          sell_pct;   /* sell price pct of buy_price */
    u8          req_badge;  /* ITEM_NONE = no requirement */
} ShopDef;

/* ---- Shop IDs ------------------------------------------ */
#define SHOP_VILLAGE_GENERAL  0   /* Starter village: basic consumables */
#define SHOP_TOWN_WEAPONS     1   /* Sword, daggers, bows */
#define SHOP_TOWN_ARMOR       2   /* Helms, armor, shields */
#define SHOP_CITY_MAGIC       3   /* Staves, orbs, magic items */
#define SHOP_GUILD_SUPPLIES   4   /* Guild exclusive: potions + materials */
#define SHOP_BLACK_MARKET     5   /* Rare/expensive items, requires badge */
#define SHOP_LEGENDARY_FORGE  6   /* Legendary mats + items (late game) */
#define SHOP_ALCHEMIST        7   /* Consumables + crafting materials */

/* ---- ROM table ------------------------------------------ */
extern const ShopDef g_shop_table[SHOP_COUNT];

/* ---- Runtime state -------------------------------------- */
typedef struct {
    u8    open;          /* TRUE if a shop is currently open */
    u8    shop_id;       /* current shop */
    u8    cursor;        /* UI cursor position */
    u8    mode;          /* SHOP_MODE_BUY or SHOP_MODE_SELL */
} ShopState;

#define SHOP_MODE_BUY   0
#define SHOP_MODE_SELL  1

extern ShopState g_shop_state;

/* ---- API ------------------------------------------------ */

/* Open a shop (populates g_shop_state) */
void shop_open(u8 shop_id);

/* Close current shop */
void shop_close(void);

/* Get buy price for item in current shop */
u32 shop_buy_price(u8 item_id);

/* Get sell price for item in current shop */
u32 shop_sell_price(u8 item_id);

/* Buy qty of item_id. Returns FALSE if not enough gold,
   not in stock, or bag is full. */
BOOL shop_buy(u8 item_id, u8 qty);

/* Sell qty of item_id from g_bag. Returns FALSE if not
   enough in bag or item is unsellable (KEY_ITEM). */
BOOL shop_sell(u8 item_id, u8 qty);

/* Can we afford to buy qty of item_id? */
BOOL shop_can_afford(u8 item_id, u8 qty);

/* Is item_id sold in the current shop? */
BOOL shop_has_item(u8 item_id);

/* Get stock entry (NULL if shop not open) */
const ShopDef* shop_current(void);

#endif /* SHOP_H */
