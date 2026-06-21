#include "item_use.h"
#include "inventory.h"
#include "char_equip.h"
#include "character.h"
#include "../../engine/event.h"
#include "gba_types.h"

/* ---- Forward declaration for XP boost flag -------------- */
/* Set by item_use when CEFF_XP_BOOST is used */
u8 g_xp_boost_active = 0;

/* ---- Internal helpers ----------------------------------- */

static s16 _pct_of(s16 max, u8 pct)
{
    return (s16)((s32)max * pct / 100);
}

/* Apply a single consumable effect to one character */
static ItemUseResult _apply_to_char(const ItemDef* def, Character* ch)
{
    ItemUseResult r;
    s16 hp_max, mp_max;
    mem_zero(&r, sizeof(r));

    if (character_is_ko(ch) && def->effect != CEFF_REVIVE)
        return r; /* cannot target KO'd char (except revive) */

    hp_max = ch->derived.val[DSTAT_HP_MAX];
    mp_max = ch->derived.val[DSTAT_MP_MAX];

    switch (def->effect) {
    case CEFF_HEAL_HP:
        r.hp_restored = def->effect_val;
        ch->hp_cur += r.hp_restored;
        if (ch->hp_cur > hp_max) { r.hp_restored -= (s16)(ch->hp_cur - hp_max); ch->hp_cur = hp_max; }
        break;

    case CEFF_HEAL_HP_PCT:
        r.hp_restored = _pct_of(hp_max, (u8)def->effect_val);
        ch->hp_cur += r.hp_restored;
        if (ch->hp_cur > hp_max) ch->hp_cur = hp_max;
        break;

    case CEFF_HEAL_MP:
        r.mp_restored = def->effect_val;
        ch->mp_cur += r.mp_restored;
        if (ch->mp_cur > mp_max) { r.mp_restored -= (s16)(ch->mp_cur - mp_max); ch->mp_cur = mp_max; }
        break;

    case CEFF_HEAL_MP_PCT:
        r.mp_restored = _pct_of(mp_max, (u8)def->effect_val);
        ch->mp_cur += r.mp_restored;
        if (ch->mp_cur > mp_max) ch->mp_cur = mp_max;
        break;

    case CEFF_FULL_HEAL:
        r.hp_restored = (s16)(hp_max - ch->hp_cur);
        r.mp_restored = (s16)(mp_max - ch->mp_cur);
        ch->hp_cur = hp_max;
        ch->mp_cur = mp_max;
        break;

    case CEFF_REVIVE:
        if (!character_is_ko(ch)) break;
        ch->persist_status &= (u8)~CHAR_STATUS_KO;
        {
            s16 revive_hp = _pct_of(hp_max, (u8)def->effect_val);
            if (revive_hp < 1) revive_hp = 1;
            ch->hp_cur = revive_hp;
        }
        r.target_revived = TRUE;
        r.hp_restored = ch->hp_cur;
        break;

    case CEFF_CURE_STATUS:
        r.status_cured = ch->persist_status;
        ch->persist_status = CHAR_STATUS_NORMAL;
        r.status_cured &= ~ch->persist_status;
        break;

    case CEFF_CURE_ALL:
        r.status_cured = ch->persist_status;
        ch->persist_status = CHAR_STATUS_NORMAL;
        break;

    case CEFF_BUFF_ATK:
    case CEFF_BUFF_DEF:
    case CEFF_BUFF_MAG:
    case CEFF_BUFF_SPD:
    case CEFF_BUFF_ALL:
        /* Buffs are tracked in battle via battle_action; in field
           they have no persistent effect (battle buffs only). */
        break;

    case CEFF_PARTY_HEAL_HP:
        /* Handled at the caller level for all party */
        r.hp_restored = _pct_of(hp_max, (u8)def->effect_val);
        ch->hp_cur += r.hp_restored;
        if (ch->hp_cur > hp_max) ch->hp_cur = hp_max;
        break;

    case CEFF_PARTY_FULL:
        r.hp_restored = (s16)(hp_max - ch->hp_cur);
        r.mp_restored = (s16)(mp_max - ch->mp_cur);
        ch->hp_cur = hp_max;
        ch->mp_cur = mp_max;
        if (character_is_ko(ch)) {
            ch->persist_status &= (u8)~CHAR_STATUS_KO;
            r.target_revived = TRUE;
        }
        ch->persist_status = CHAR_STATUS_NORMAL;
        break;

    case CEFF_XP_BOOST:
        g_xp_boost_active = 1;
        break;

    case CEFF_REGEN_HP:
        /* Applied each turn in battle — no immediate field effect */
        r.hp_restored = 0;
        break;

    default:
        break;
    }

    r.used = TRUE;
    return r;
}

/* ---- API ------------------------------------------------ */

BOOL item_can_use(u8 item_id, const ItemUseCtx* ctx)
{
    const ItemDef* def = item_get(item_id);
    if (!def || def->category == ICAT_MATERIAL || def->category == ICAT_KEY_ITEM)
        return FALSE;
    if (def->category == ICAT_EQUIPMENT)
        return TRUE; /* equip handled separately */
    if (ctx->in_battle && !(def->use_flags & USE_BATTLE)) return FALSE;
    if (!ctx->in_battle && !(def->use_flags & USE_FIELD)) return FALSE;
    if (bag_count(item_id) == 0) return FALSE;
    return TRUE;
}

ItemUseResult item_use_field(u8 item_id, const ItemUseCtx* ctx)
{
    const ItemDef* def = item_get(item_id);
    ItemUseResult  r;
    u8 i;
    mem_zero(&r, sizeof(r));

    if (!item_can_use(item_id, ctx)) return r;

    if (def->target_all) {
        /* Apply to entire party */
        for (i = 0; i < g_party_size; i++) {
            ItemUseResult sub = _apply_to_char(def, &g_party[i]);
            if (sub.target_revived)   r.target_revived = TRUE;
            r.hp_restored += sub.hp_restored;
            r.mp_restored += sub.mp_restored;
            r.status_cured |= sub.status_cured;
        }
        r.full_party = TRUE;
        r.used = TRUE;
    } else {
        u8 slot = ctx->target_slot;
        if (slot >= g_party_size) return r;
        r = _apply_to_char(def, &g_party[slot]);
    }

    if (r.used) {
        bag_remove(item_id, 1);
        event_push(EVT_SFX_PLAY, 5, 0); /* SFX: item use */
    }
    return r;
}

ItemUseResult item_use_battle(u8 item_id, u8 target_battler_idx)
{
    /* In battle, we target by party slot (0-3) via the battler system.
       This function applies the field-level effect to the Character
       struct; the battle system re-reads HP/MP from Character on sync. */
    ItemUseCtx ctx;
    ctx.in_battle   = TRUE;
    ctx.user_slot   = 0;
    ctx.target_slot = target_battler_idx;
    return item_use_field(item_id, &ctx);
}

BOOL item_equip_from_bag(u8 char_slot, u8 equip_slot, u8 item_id)
{
    Character* ch;
    u8 old_item;

    if (char_slot >= g_party_size) return FALSE;
    if (bag_count(item_id) == 0)   return FALSE;

    ch = &g_party[char_slot];

    if (!equip_can_use(ch, item_id)) return FALSE;

    /* Remove from bag */
    bag_remove(item_id, 1);

    /* Unequip existing item and return to bag */
    old_item = equip_remove(ch, equip_slot);
    if (old_item != EQUIP_NONE)
        bag_add(old_item, 1);

    /* Equip new item */
    equip_item(ch, equip_slot, item_id);
    character_recompute(ch);
    return TRUE;
}

BOOL item_unequip_to_bag(u8 char_slot, u8 equip_slot)
{
    Character* ch;
    u8 item_id;

    if (char_slot >= g_party_size) return FALSE;
    ch = &g_party[char_slot];

    item_id = equip_remove(ch, equip_slot);
    if (item_id == EQUIP_NONE) return FALSE;

    bag_add(item_id, 1);
    character_recompute(ch);
    return TRUE;
}
