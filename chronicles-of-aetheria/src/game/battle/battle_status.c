#include "battle_status.h"
#include "battler.h"
#include "gba_types.h"

const char* STATUS_NAMES[STATUS_COUNT] = {
    "Poison","Burn","Freeze","Blind","Silence","Slow",
    "Confuse","Sleep","Stone","Doom",
    "Regen","Haste","Barrier","MBarrier","Reflect","Berserk"
};

/* Ordered to match StatusFlag bit positions 0-15 */
const StatusMeta STATUS_META[STATUS_COUNT] = {
/*  name         dur  pos    skip  dot  hot  cured_by  wake */
  { "Poison",    STATUS_DUR_POISON,  FALSE, FALSE,  5,  0, 0, FALSE },
  { "Burn",      STATUS_DUR_BURN,    FALSE, FALSE,  8,  0, 0, FALSE },
  { "Freeze",    STATUS_DUR_FREEZE,  FALSE, TRUE,   0,  0, 0, TRUE  },
  { "Blind",     STATUS_DUR_BLIND,   FALSE, FALSE,  0,  0, 0, FALSE },
  { "Silence",   STATUS_DUR_SILENCE, FALSE, FALSE,  0,  0, 0, FALSE },
  { "Slow",      STATUS_DUR_SLOW,    FALSE, FALSE,  0,  0, 0, FALSE },
  { "Confuse",   STATUS_DUR_CONFUSE, FALSE, FALSE,  0,  0, 0, FALSE },
  { "Sleep",     STATUS_DUR_SLEEP,   FALSE, TRUE,   0,  0, 0, TRUE  },
  { "Stone",     STATUS_DUR_STONE,   FALSE, TRUE,   0,  0, 0, FALSE },
  { "Doom",      STATUS_DUR_DOOM,    FALSE, FALSE,  0,  0, 0, FALSE },
  { "Regen",     STATUS_DUR_REGEN,   TRUE,  FALSE,  0,  5, 0, FALSE },
  { "Haste",     STATUS_DUR_HASTE,   TRUE,  FALSE,  0,  0, 0, FALSE },
  { "Barrier",   STATUS_DUR_BARRIER, TRUE,  FALSE,  0,  0, 0, FALSE },
  { "MBarrier",  STATUS_DUR_MBARRIER,TRUE,  FALSE,  0,  0, 0, FALSE },
  { "Reflect",   STATUS_DUR_REFLECT, TRUE,  FALSE,  0,  0, 0, FALSE },
  { "Berserk",   STATUS_DUR_BERSERK, FALSE, FALSE,  0,  0, 0, FALSE },
};

/* Return bit index of a StatusFlag power-of-two value */
static u8 _flag_bit(StatusFlag flag)
{
    u8 i;
    for (i = 0; i < STATUS_COUNT; i++)
        if ((StatusFlag)(1u << i) == flag) return i;
    return 0;
}

BOOL status_has(const Battler* b, StatusFlag flag)
{
    return (BOOL)((b->status_bits & (u32)flag) != 0);
}

BOOL status_apply(Battler* target, StatusFlag flag, u8 duration)
{
    u8 bit;
    if (!target->alive) return FALSE;

    /* Simple immunities */
    if (flag == STATUS_BURN   && target->element == ELEM_FIRE)  return FALSE;
    if (flag == STATUS_FREEZE && target->element == ELEM_WATER) return FALSE;

    /* Don't stack same debuff if already active with higher duration */
    if (status_has(target, flag)) return FALSE;

    bit = _flag_bit(flag);
    target->status_bits |= (u32)flag;
    target->status_dur[bit] = duration ? duration : STATUS_META[bit].default_dur;
    if (flag == STATUS_DOOM) target->doom_timer = target->status_dur[bit];
    return TRUE;
}

void status_remove(Battler* target, StatusFlag flag)
{
    u8 bit = _flag_bit(flag);
    target->status_bits &= ~(u32)flag;
    target->status_dur[bit] = 0;
}

void status_cure_all_neg(Battler* target)
{
    u8 i;
    for (i = 0; i < STATUS_COUNT; i++) {
        if (!STATUS_META[i].is_positive) {
            target->status_bits &= ~(1u << i);
            target->status_dur[i] = 0;
        }
    }
}

void status_dispel_all_pos(Battler* target)
{
    u8 i;
    for (i = 0; i < STATUS_COUNT; i++) {
        if (STATUS_META[i].is_positive) {
            target->status_bits &= ~(1u << i);
            target->status_dur[i] = 0;
        }
    }
}

void status_tick(Battler* b)
{
    u8 i;
    if (!b->alive) return;

    for (i = 0; i < STATUS_COUNT; i++) {
        StatusFlag flag = (StatusFlag)(1u << i);
        if (!status_has(b, flag)) continue;

        if (STATUS_META[i].dot_pct > 0) {
            s16 dmg = (s16)((u32)b->hp_max * STATUS_META[i].dot_pct / 100);
            if (dmg < 1) dmg = 1;
            battler_apply_damage(b, dmg);
            if (!b->alive) { status_remove(b, flag); return; }
        }

        if (STATUS_META[i].hot_pct > 0) {
            s16 heal = (s16)((u32)b->hp_max * STATUS_META[i].hot_pct / 100);
            if (heal < 1) heal = 1;
            battler_apply_damage(b, (s16)-heal);
        }

        if (flag == STATUS_DOOM) {
            if (b->doom_timer > 0) b->doom_timer--;
            if (b->doom_timer == 0) {
                b->hp    = 0;
                b->alive = FALSE;
                status_remove(b, flag);
                return;
            }
            continue; /* DOOM doesn't use normal duration */
        }

        /* Duration countdown */
        if (b->status_dur[i] > 0) {
            b->status_dur[i]--;
            if (b->status_dur[i] == 0)
                status_remove(b, flag);
        }
    }
}

void status_on_damage(Battler* b)
{
    if (STATUS_META[_flag_bit(STATUS_SLEEP)].wakes_on_hit && status_has(b, STATUS_SLEEP))
        status_remove(b, STATUS_SLEEP);
    if (STATUS_META[_flag_bit(STATUS_FREEZE)].wakes_on_hit && status_has(b, STATUS_FREEZE))
        status_remove(b, STATUS_FREEZE);
}

BOOL status_modify_action(Battler* actor, u8* target_mask, ActionType* action)
{
    if (status_has(actor, STATUS_BERSERK)) {
        *action = ACTION_ATTACK;
        return TRUE;
    }
    (void)target_mask;
    return FALSE;
}

s8 status_atk_modifier(const Battler* b)
{
    if (status_has(b, STATUS_BURN)) return -1;
    if (status_has(b, STATUS_BERSERK)) return 2;
    return 0;
}

void status_describe(const Battler* b, char* buf, u8 buf_len)
{
    u8 i, pos = 0;
    for (i = 0; i < STATUS_COUNT && pos < buf_len - 1; i++) {
        if (status_has(b, (StatusFlag)(1u << i))) {
            const char* nm = STATUS_NAMES[i];
            u8 j = 0;
            while (nm[j] && pos < buf_len - 2) buf[pos++] = nm[j++];
            buf[pos++] = ' ';
        }
    }
    buf[pos] = '\0';
}
