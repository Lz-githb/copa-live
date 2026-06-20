#include "character.h"
#include "char_stats.h"
#include "char_class.h"
#include "char_equip.h"
#include "char_skill.h"
#include "char_talent.h"
#include "char_xp.h"
#include "battle_types.h"
#include "battler.h"
#include "event_system.h"
#include "gba_types.h"

/* ---- Global party --------------------------------------- */
Character g_party[PARTY_SIZE];
u8        g_party_size = 0;

/* ---- System init ---------------------------------------- */

void character_system_init(void)
{
    mem_zero(g_party, sizeof(g_party));
    g_party_size = 0;
}

/* ---- Create --------------------------------------------- */

void character_create(Character* ch, const char* name, CharClass cls,
                      u8 party_slot)
{
    u8 i;
    mem_zero(ch, sizeof(Character));

    /* Copy name (null-terminated, max CHAR_NAME_MAX-1 chars) */
    for (i = 0; i < CHAR_NAME_MAX - 1 && name[i]; i++)
        ch->name[i] = name[i];
    ch->name[i] = '\0';

    ch->cls        = cls;
    ch->party_slot = party_slot;
    ch->level      = LEVEL_MIN;
    ch->xp_total   = 0;

    stats_init(&ch->base, cls);
    talent_state_init(&ch->talents);

    /* Learn starting skills */
    xp_check_skill_unlocks(ch);

    /* Default hotbar: first CHAR_SKILL_SLOTS known skills */
    for (i = 0; i < CHAR_SKILL_SLOTS && i < ch->known_count; i++)
        ch->hotbar[i] = ch->known_skills[i];

    character_recompute(ch);

    ch->hp_cur = ch->derived.val[DSTAT_HP_MAX];
    ch->mp_cur = ch->derived.val[DSTAT_MP_MAX];
}

/* ---- Recompute derived stats ----------------------------- */

void character_recompute(Character* ch)
{
    s16 equip_bonus[DSTAT_COUNT];
    s16 talent_bonus[DSTAT_COUNT];
    u8  i;

    equip_compute_bonus(ch, equip_bonus);

    for (i = 0; i < DSTAT_COUNT; i++)
        talent_bonus[i] = ch->talents.dstat_bonus[i];

    /* Apply talent attribute bonuses to base.bonus */
    for (i = 0; i < ATTR_COUNT; i++)
        ch->base.bonus[i] = ch->talents.attr_bonus[i];

    /* Add equipment attribute bonuses on top */
    {
        s16 attr_eq[ATTR_COUNT];
        equip_compute_attr_bonus(ch, attr_eq);
        for (i = 0; i < ATTR_COUNT; i++)
            ch->base.bonus[i] += attr_eq[i];
    }

    stats_compute_derived(&ch->base, &ch->derived, ch->cls,
                          equip_bonus, talent_bonus);

    /* Clamp current HP/MP to new max */
    if (ch->hp_cur > ch->derived.val[DSTAT_HP_MAX])
        ch->hp_cur = ch->derived.val[DSTAT_HP_MAX];
    if (ch->mp_cur > ch->derived.val[DSTAT_MP_MAX])
        ch->mp_cur = ch->derived.val[DSTAT_MP_MAX];
}

/* ---- Restore -------------------------------------------- */

void character_full_restore(Character* ch)
{
    ch->hp_cur       = ch->derived.val[DSTAT_HP_MAX];
    ch->mp_cur       = ch->derived.val[DSTAT_MP_MAX];
    ch->persist_status = CHAR_STATUS_NORMAL;
    ch->infusion       = INFUSE_NONE;
    ch->infusion_turns = 0;
    ch->spellcharge    = FALSE;
    ch->taunt_active   = 0;
}

void character_rest(Character* ch, u8 hp_pct, u8 mp_pct)
{
    s16 hp_max = ch->derived.val[DSTAT_HP_MAX];
    s16 mp_max = ch->derived.val[DSTAT_MP_MAX];

    ch->hp_cur += (s16)((s32)hp_max * hp_pct / 100);
    ch->mp_cur += (s16)((s32)mp_max * mp_pct / 100);

    if (ch->hp_cur > hp_max) ch->hp_cur = hp_max;
    if (ch->mp_cur > mp_max) ch->mp_cur = mp_max;

    /* Rest cures KO (character comes back with rested HP) */
    if (ch->hp_cur > 0)
        ch->persist_status &= (u8)~CHAR_STATUS_KO;
}

/* ---- Field damage (poison steps, etc.) ------------------ */

void character_apply_field_damage(Character* ch, s16 dmg)
{
    ch->hp_cur -= dmg;
    if (ch->hp_cur <= 0) {
        ch->hp_cur = 1; /* cannot die from field damage */
        ch->persist_status |= CHAR_STATUS_KO;
    }
}

/* ---- KO check ------------------------------------------- */

BOOL character_is_ko(const Character* ch)
{
    return (BOOL)(ch->hp_cur <= 0 ||
                  (ch->persist_status & CHAR_STATUS_KO) != 0);
}

/* ---- Can act (not KO or stone) -------------------------- */

BOOL character_can_act(const Character* ch)
{
    return (BOOL)(!character_is_ko(ch) &&
                  !(ch->persist_status & CHAR_STATUS_STONE));
}

/* ---- Step counter (poison, etc.) ------------------------ */

void character_on_step(Character* ch)
{
    ch->step_count++;
    if (ch->persist_status & CHAR_STATUS_POISONED) {
        if ((ch->step_count & 3) == 0) { /* every 4 steps */
            character_apply_field_damage(ch, 1);
        }
    }
}

/* ---- Serialize / Deserialize ---------------------------- */

u16 character_serialize(const Character* ch, u8* buf, u16 buf_size)
{
    u16 sz = (u16)sizeof(Character);
    u16 i;
    if (buf_size < sz) return 0;
    for (i = 0; i < sz; i++)
        buf[i] = ((const u8*)ch)[i];
    return sz;
}

BOOL character_deserialize(Character* ch, const u8* buf, u16 buf_size)
{
    u16 sz = (u16)sizeof(Character);
    u16 i;
    if (buf_size < sz) return FALSE;
    for (i = 0; i < sz; i++)
        ((u8*)ch)[i] = buf[i];
    return TRUE;
}

/* ---- Battle integration --------------------------------- */

void character_to_battler(const Character* ch, struct Battler* out)
{
    mem_zero(out, sizeof(struct Battler));
    battler_from_char(out, ch);
}

void character_from_battle_result(Character* ch,
                                   s16 hp_delta,
                                   s16 mp_delta,
                                   u32 xp_gained)
{
    ch->hp_cur += hp_delta;
    if (ch->hp_cur < 0)   ch->hp_cur = 0;
    if (ch->hp_cur > ch->derived.val[DSTAT_HP_MAX])
        ch->hp_cur = ch->derived.val[DSTAT_HP_MAX];

    ch->mp_cur += mp_delta;
    if (ch->mp_cur < 0)   ch->mp_cur = 0;
    if (ch->mp_cur > ch->derived.val[DSTAT_MP_MAX])
        ch->mp_cur = ch->derived.val[DSTAT_MP_MAX];

    if (ch->hp_cur <= 0)
        ch->persist_status |= CHAR_STATUS_KO;

    if (xp_gained > 0)
        xp_award(ch, xp_gained);
}

/* ---- Utility -------------------------------------------- */

const char* character_class_name(const Character* ch)
{
    return class_name(ch->cls);
}

s16 character_hp_pct(const Character* ch)
{
    s16 mx = ch->derived.val[DSTAT_HP_MAX];
    if (mx <= 0) return 0;
    return (s16)((s32)ch->hp_cur * 100 / mx);
}

s16 character_mp_pct(const Character* ch)
{
    s16 mx = ch->derived.val[DSTAT_MP_MAX];
    if (mx <= 0) return 0;
    return (s16)((s32)ch->mp_cur * 100 / mx);
}
