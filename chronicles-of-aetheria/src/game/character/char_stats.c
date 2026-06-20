#include "char_stats.h"
#include "char_class.h"
#include "gba_types.h"

/* Attribute points gained per level, per growth grade */
const u8 GROWTH_PTS[6] = {
    [GROWTH_E] = 1,
    [GROWTH_D] = 2,
    [GROWTH_C] = 3,
    [GROWTH_B] = 4,
    [GROWTH_A] = 5,
    [GROWTH_S] = 7,
};

/* Growth grades [CLASS][ATTR] */
const u8 CLASS_GROWTH[CLASS_COUNT][ATTR_COUNT] = {
/*                 VIT      END      STR      DEX      INT      WIS      LCK      CHA */
[CLASS_GUARDIAN] = { GROWTH_S, GROWTH_A, GROWTH_B, GROWTH_D, GROWTH_E, GROWTH_D, GROWTH_C, GROWTH_C },
[CLASS_ARCSWORD] = { GROWTH_B, GROWTH_C, GROWTH_B, GROWTH_B, GROWTH_B, GROWTH_C, GROWTH_C, GROWTH_D },
[CLASS_ARCANIST] = { GROWTH_D, GROWTH_D, GROWTH_E, GROWTH_C, GROWTH_S, GROWTH_A, GROWTH_B, GROWTH_C },
[CLASS_HUNTER]   = { GROWTH_C, GROWTH_C, GROWTH_C, GROWTH_S, GROWTH_D, GROWTH_D, GROWTH_A, GROWTH_B },
};

/* Base stat bonuses added to derived stat formulas per class */
const ClassStatBonus CLASS_STAT_BONUS[CLASS_COUNT] = {
/*                hp_base  mp_base  atk_bonus  def_bonus  mag_bonus  mdef_bonus  spd_bonus  crit_bonus */
[CLASS_GUARDIAN] = { 80,    20,       10,        20,        0,          10,         -5,         0 },
[CLASS_ARCSWORD] = { 40,    40,        5,         5,        10,          5,          5,          5 },
[CLASS_ARCANIST] = { 10,    80,        0,         0,        20,         15,          0,          0 },
[CLASS_HUNTER]   = { 30,    20,        8,         0,         0,          0,         15,         10 },
};

static const char* ATTR_NAMES[ATTR_COUNT] = {
    "VIT","END","STR","DEX","INT","WIS","LCK","CHA"
};

static const char* DSTAT_NAMES[DSTAT_COUNT] = {
    "HP","MP","ATK","DEF","MAG","MDEF","SPD","CRIT","EVA"
};

void stats_init(BaseStats* bs, CharClass cls)
{
    u8 i;
    const ClassDef* def = class_get(cls);
    mem_zero(bs, sizeof(BaseStats));
    for (i = 0; i < ATTR_COUNT; i++)
        bs->val[i] = def->start_attrs[i];
}

void stats_apply_level_growth(BaseStats* bs, CharClass cls, u8 level)
{
    u8 i;
    (void)level; /* growth is the same each level */
    for (i = 0; i < ATTR_COUNT; i++) {
        u8 grade = CLASS_GROWTH[cls][i];
        bs->val[i] += GROWTH_PTS[grade];
    }
}

static s16 _clamp(s32 v, s16 lo, s16 hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return (s16)v;
}

void stats_compute_derived(const BaseStats* bs,
                            DerivedStats* out,
                            CharClass cls,
                            const s16 equip_bonus[DSTAT_COUNT],
                            const s16 talent_bonus[DSTAT_COUNT])
{
    const ClassStatBonus* cb = &CLASS_STAT_BONUS[cls];
    s16 vit = bs->val[ATTR_VIT] + bs->bonus[ATTR_VIT];
    s16 end = bs->val[ATTR_END] + bs->bonus[ATTR_END];
    s16 str = bs->val[ATTR_STR] + bs->bonus[ATTR_STR];
    s16 dex = bs->val[ATTR_DEX] + bs->bonus[ATTR_DEX];
    s16 itl = bs->val[ATTR_INT] + bs->bonus[ATTR_INT];
    s16 wis = bs->val[ATTR_WIS] + bs->bonus[ATTR_WIS];
    s16 lck = bs->val[ATTR_LCK] + bs->bonus[ATTR_LCK];
    u8  d;

    out->val[DSTAT_HP_MAX]  = (s16)(vit * 12 + end * 4 + cb->hp_base);
    out->val[DSTAT_MP_MAX]  = (s16)(itl * 8  + wis * 4 + cb->mp_base);
    out->val[DSTAT_ATK]     = (s16)(str * 3  + dex     + cb->atk_bonus);
    out->val[DSTAT_DEF]     = (s16)(end * 3  + str     + cb->def_bonus);
    out->val[DSTAT_MAG]     = (s16)(itl * 3  + wis     + cb->mag_bonus);
    out->val[DSTAT_MDEF]    = (s16)(wis * 3  + itl     + cb->mdef_bonus);
    out->val[DSTAT_SPD]     = (s16)(dex * 3  + lck     + cb->spd_bonus);
    out->val[DSTAT_CRIT]    = _clamp((s32)(lck / 3 + dex / 5 + cb->crit_bonus), 0, 75);
    out->val[DSTAT_EVASION] = _clamp((s32)(dex / 4 + lck / 5), 0, 60);

    /* Add equipment and talent bonuses */
    for (d = 0; d < DSTAT_COUNT; d++) {
        s32 v = (s32)out->val[d];
        if (equip_bonus)  v += equip_bonus[d];
        if (talent_bonus) v += talent_bonus[d];
        out->val[d] = _clamp(v, 1, 9999);
    }
}

s16 stats_get_derived(const DerivedStats* ds, DStatType which)
{
    if (which >= DSTAT_COUNT) return 0;
    return ds->val[which];
}

void stats_add_equip_bonus(BaseStats* bs, AttrType attr, s16 amount)
{
    if (attr < ATTR_COUNT) bs->bonus[attr] += amount;
}

void stats_remove_equip_bonus(BaseStats* bs, AttrType attr, s16 amount)
{
    if (attr < ATTR_COUNT) bs->bonus[attr] -= amount;
}

const char* stats_attr_name(AttrType attr)
{
    if (attr >= ATTR_COUNT) return "???";
    return ATTR_NAMES[attr];
}

const char* stats_dstat_name(DStatType dstat)
{
    if (dstat >= DSTAT_COUNT) return "???";
    return DSTAT_NAMES[dstat];
}
