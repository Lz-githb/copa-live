#include "battler.h"
#include "battle_status.h"
#include "gba_types.h"

static void _str_copy(char* dst, const char* src, u8 max)
{
    u8 i = 0;
    while (i < max - 1 && src[i]) { dst[i] = src[i]; i++; }
    dst[i] = '\0';
}

void battler_from_char(Battler* b, u8 idx, const CharDef* ch)
{
    mem_zero(b, sizeof(Battler));
    _str_copy(b->name, ch->name, BATTLER_NAME_LEN);
    b->idx       = idx;
    b->is_enemy  = FALSE;
    b->sprite_id = ch->sprite_id;
    b->palette_id= ch->palette_id;
    b->hp        = ch->hp_base;  b->hp_max  = ch->hp_base;
    b->mp        = ch->mp_base;  b->mp_max  = ch->mp_base;
    b->atk       = ch->atk_base;
    b->def       = ch->def_base;
    b->mag       = ch->mag_base;
    b->mdef      = ch->mdef_base;
    b->spd       = ch->spd_base;
    b->element   = ch->element;
    {
        u8 i;
        for (i = 0; i < ELEM_COUNT; i++) b->resist[i] = ch->resist[i];
        b->skill_count = ch->skill_count < BATTLE_SKILL_MAX ? ch->skill_count : BATTLE_SKILL_MAX;
        for (i = 0; i < b->skill_count; i++) b->skill_ids[i] = ch->skill_ids[i];
    }
    b->combo_pts = 0;
    b->alive     = TRUE;
}

void battler_from_enemy(Battler* b, u8 idx, const EnemyDef* en)
{
    mem_zero(b, sizeof(Battler));
    _str_copy(b->name, en->name, BATTLER_NAME_LEN);
    b->idx        = idx;
    b->is_enemy   = TRUE;
    b->sprite_id  = en->sprite_id;
    b->palette_id = en->palette_id;
    b->hp         = en->hp;    b->hp_max  = en->hp;
    b->mp         = en->mp;    b->mp_max  = en->mp;
    b->atk        = en->atk;
    b->def        = en->def;
    b->mag        = en->mag;
    b->mdef       = en->mdef;
    b->spd        = en->spd;
    b->element    = en->element;
    b->exp_reward = en->exp_reward;
    b->gold_reward= en->gold_reward;
    {
        u8 i;
        for (i = 0; i < ELEM_COUNT; i++) b->resist[i] = en->resist[i];
        b->skill_count = en->skill_count < BATTLE_SKILL_MAX ? en->skill_count : BATTLE_SKILL_MAX;
        for (i = 0; i < b->skill_count; i++) b->skill_ids[i] = en->skill_ids[i];
    }
    b->alive = TRUE;
}

/* Stage multiplier table: index = stage+4, values ×100 */
const u16 STAT_MOD_TABLE[9] = { 25, 34, 50, 67, 100, 133, 150, 200, 300 };

static s16 _apply_stage(s16 base, s8 stage)
{
    u8  idx = (u8)(stage + 4);
    s32 v   = (s32)base * STAT_MOD_TABLE[idx] / 100;
    return (s16)(v < 1 ? 1 : v);
}

s16 battler_eff_atk(const Battler* b)  { return _apply_stage(b->atk,  b->atk_stage); }
s16 battler_eff_def(const Battler* b)  { return _apply_stage(b->def,  b->def_stage); }
s16 battler_eff_mag(const Battler* b)  { return _apply_stage(b->mag,  b->mag_stage); }
s16 battler_eff_mdef(const Battler* b) { return _apply_stage(b->mdef, b->mdef_stage); }

s16 battler_eff_spd(const Battler* b)
{
    s16 v = _apply_stage(b->spd, b->spd_stage);
    if (status_has(b, STATUS_HASTE)) v *= 2;
    if (status_has(b, STATUS_SLOW))  v /= 2;
    return v < 1 ? 1 : v;
}

void battler_apply_damage(Battler* b, s16 amount)
{
    b->hp -= amount;
    if (b->hp <= 0) { b->hp = 0; b->alive = FALSE; }
    if (b->hp > b->hp_max) b->hp = b->hp_max;
}

void battler_add_cp(Battler* b, u8 amount)
{
    b->combo_pts += amount;
    if (b->combo_pts > CP_MAX) b->combo_pts = CP_MAX;
}

void battler_compute_wait(Battler* b)
{
    b->turn_wait = (s16)(1000 / (battler_eff_spd(b) + 1));
}

BOOL battler_can_act(const Battler* b)
{
    return (BOOL)(b->alive && !status_has(b, STATUS_FREEZE) &&
                  !status_has(b, STATUS_SLEEP) && !status_has(b, STATUS_STONE));
}

BOOL battler_can_magic(const Battler* b)
{
    return (BOOL)(b->alive && !status_has(b, STATUS_SILENCE));
}

void battler_new_round(Battler* b)
{
    b->acted     = FALSE;
    b->defending = FALSE;
}
