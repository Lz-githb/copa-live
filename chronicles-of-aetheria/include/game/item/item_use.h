#ifndef ITEM_USE_H
#define ITEM_USE_H

#include "item_db.h"
#include "../character/character.h"

/* =========================================================
 * Item Use System
 *
 * Handles consuming items (potions, tonics, grimoires, etc.)
 * in both the field and battle contexts.
 *
 * Field use: targets one party member (or whole party).
 * Battle use: delegates effect application through battler.
 * Equipment: routed to equip_item() / equip_remove().
 * ========================================================= */

/* Context passed to item_use functions */
typedef struct {
    BOOL   in_battle;       /* TRUE if inside battle */
    u8     user_slot;       /* party slot of the character using it */
    u8     target_slot;     /* party slot of the target (0xFF = all) */
} ItemUseCtx;

/* Result of using an item */
typedef struct {
    BOOL used;              /* was the item actually consumed? */
    s16  hp_restored;       /* per target */
    s16  mp_restored;
    u8   status_cured;      /* CharPersistStatus bits cured */
    BOOL target_revived;
    BOOL full_party;        /* was it applied to everyone? */
} ItemUseResult;

/* ---- API ------------------------------------------------ */

/* Can this item be used in the current context? */
BOOL item_can_use(u8 item_id, const ItemUseCtx* ctx);

/* Use a consumable item on field (removes from g_bag).
   ctx->target_slot: party slot, or 0xFF for all.
   Returns result struct with what happened. */
ItemUseResult item_use_field(u8 item_id, const ItemUseCtx* ctx);

/* Use a consumable item in battle.
   target_battler: index in the battler array (0-3 = allies).
   Returns result struct. */
ItemUseResult item_use_battle(u8 item_id, u8 target_battler_idx);

/* Equip an item from the party bag to a character.
   Removes item from bag, adds previously equipped item back to bag. */
BOOL item_equip_from_bag(u8 char_slot, u8 equip_slot, u8 item_id);

/* Unequip item and put it in the bag. */
BOOL item_unequip_to_bag(u8 char_slot, u8 equip_slot);

#endif /* ITEM_USE_H */
