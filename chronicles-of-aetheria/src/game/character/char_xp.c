#include "char_xp.h"
#include "char_stats.h"
#include "char_class.h"
#include "char_talent.h"
#include "character.h"
#include "../../engine/event.h"
#include "gba_types.h"

/* ---- Milestone tables ----------------------------------- */

static const LevelMilestone s_guardian_milestones[MILESTONE_COUNT] = {
    { 10, ATTR_VIT,  2, 0xFF },
    { 15, ATTR_END,  2, CSKILL_IRON_WALL },
    { 20, ATTR_STR,  2, 0xFF },
    { 25, ATTR_VIT,  3, CSKILL_PROVOKE },
    { 30, ATTR_END,  3, CSKILL_FORTRESS },
    { 40, ATTR_VIT,  4, CSKILL_SACRED_GROUND },
    { 50, ATTR_END,  4, CSKILL_LAST_STAND },
    { 60, ATTR_STR,  4, 0xFF },
    { 70, ATTR_VIT,  5, CSKILL_WARDEN_OATH },
    { 99, ATTR_END,  8, 0xFF },
};

static const LevelMilestone s_arcsword_milestones[MILESTONE_COUNT] = {
    { 10, ATTR_STR,  2, 0xFF },
    { 14, ATTR_INT,  2, CSKILL_MANA_BLADE },
    { 18, ATTR_DEX,  2, CSKILL_INFUSE_STORM },
    { 22, ATTR_STR,  3, CSKILL_SPELL_PARRY },
    { 28, ATTR_INT,  3, CSKILL_RUNIC_BURST },
    { 40, ATTR_DEX,  3, CSKILL_DUAL_RESONANCE },
    { 50, ATTR_STR,  4, 0xFF },
    { 60, ATTR_INT,  4, CSKILL_ARCANE_UNLEASH },
    { 70, ATTR_DEX,  4, 0xFF },
    { 99, ATTR_STR,  8, 0xFF },
};

static const LevelMilestone s_arcanist_milestones[MILESTONE_COUNT] = {
    { 10, ATTR_INT,  2, 0xFF },
    { 14, ATTR_WIS,  2, CSKILL_ARC_TEMPEST },
    { 18, ATTR_INT,  2, CSKILL_MANA_SHIELD },
    { 22, ATTR_WIS,  3, CSKILL_ARC_NOVA },
    { 30, ATTR_INT,  3, CSKILL_CHAIN_BOLT },
    { 40, ATTR_WIS,  3, CSKILL_ARC_VOID },
    { 50, ATTR_INT,  4, 0xFF },
    { 65, ATTR_WIS,  4, CSKILL_GRAND_ARCANA },
    { 75, ATTR_INT,  5, 0xFF },
    { 99, ATTR_WIS,  8, 0xFF },
};

static const LevelMilestone s_hunter_milestones[MILESTONE_COUNT] = {
    { 10, ATTR_DEX,  2, 0xFF },
    { 14, ATTR_LCK,  2, CSKILL_SMOKE_BOMB },
    { 18, ATTR_DEX,  2, CSKILL_TRAP_BIND },
    { 22, ATTR_LCK,  3, CSKILL_SNIPE },
    { 28, ATTR_DEX,  3, CSKILL_MARKED_PREY },
    { 40, ATTR_LCK,  3, CSKILL_RAIN_OF_ARROWS },
    { 50, ATTR_DEX,  4, 0xFF },
    { 60, ATTR_LCK,  4, CSKILL_DEATHMARK },
    { 70, ATTR_DEX,  5, 0xFF },
    { 99, ATTR_LCK,  8, 0xFF },
};

const LevelMilestone g_milestones[CLASS_COUNT][MILESTONE_COUNT] = {
    [CLASS_GUARDIAN] = { { 10,ATTR_VIT,2,0xFF},{15,ATTR_END,2,CSKILL_IRON_WALL},{20,ATTR_STR,2,0xFF},{25,ATTR_VIT,3,CSKILL_PROVOKE},{30,ATTR_END,3,CSKILL_FORTRESS},{40,ATTR_VIT,4,CSKILL_SACRED_GROUND},{50,ATTR_END,4,CSKILL_LAST_STAND},{60,ATTR_STR,4,0xFF},{70,ATTR_VIT,5,CSKILL_WARDEN_OATH},{99,ATTR_END,8,0xFF} },
    [CLASS_ARCSWORD] = { { 10,ATTR_STR,2,0xFF},{14,ATTR_INT,2,CSKILL_MANA_BLADE},{18,ATTR_DEX,2,CSKILL_INFUSE_STORM},{22,ATTR_STR,3,CSKILL_SPELL_PARRY},{28,ATTR_INT,3,CSKILL_RUNIC_BURST},{40,ATTR_DEX,3,CSKILL_DUAL_RESONANCE},{50,ATTR_STR,4,0xFF},{60,ATTR_INT,4,CSKILL_ARCANE_UNLEASH},{70,ATTR_DEX,4,0xFF},{99,ATTR_STR,8,0xFF} },
    [CLASS_ARCANIST] = { { 10,ATTR_INT,2,0xFF},{14,ATTR_WIS,2,CSKILL_ARC_TEMPEST},{18,ATTR_INT,2,CSKILL_MANA_SHIELD},{22,ATTR_WIS,3,CSKILL_ARC_NOVA},{30,ATTR_INT,3,CSKILL_CHAIN_BOLT},{40,ATTR_WIS,3,CSKILL_ARC_VOID},{50,ATTR_INT,4,0xFF},{65,ATTR_WIS,4,CSKILL_GRAND_ARCANA},{75,ATTR_INT,5,0xFF},{99,ATTR_WIS,8,0xFF} },
    [CLASS_HUNTER]   = { { 10,ATTR_DEX,2,0xFF},{14,ATTR_LCK,2,CSKILL_SMOKE_BOMB},{18,ATTR_DEX,2,CSKILL_TRAP_BIND},{22,ATTR_LCK,3,CSKILL_SNIPE},{28,ATTR_DEX,3,CSKILL_MARKED_PREY},{40,ATTR_LCK,3,CSKILL_RAIN_OF_ARROWS},{50,ATTR_DEX,4,0xFF},{60,ATTR_LCK,4,CSKILL_DEATHMARK},{70,ATTR_DEX,5,0xFF},{99,ATTR_LCK,8,0xFF} },
};

/* ---- XP curve ------------------------------------------- */

u32 xp_required_for_level(u8 level)
{
    u32 n;
    if (level <= 1) return 0;
    n = (u32)level;
    return n * n * n;
}

u32 xp_to_next_level(u8 current_level)
{
    if (current_level >= LEVEL_MAX) return 0xFFFFFFFFu;
    return xp_required_for_level((u8)(current_level + 1)) -
           xp_required_for_level(current_level);
}

u8 xp_level_for_xp(u32 total_xp)
{
    u8 lv = LEVEL_MIN;
    while (lv < LEVEL_MAX && xp_required_for_level((u8)(lv + 1)) <= total_xp)
        lv++;
    return lv;
}

u8 xp_levels_gained(u8 current_level, u32 current_xp, u32 xp_gain)
{
    u8 new_level = xp_level_for_xp(current_xp + xp_gain);
    if (new_level <= current_level) return 0;
    return (u8)(new_level - current_level);
}

/* ---- Level-up logic ------------------------------------- */

void xp_check_skill_unlocks(struct Character* ch)
{
    const ClassDef* def = class_get(ch->cls);
    u8 i;
    for (i = 0; i < def->skill_unlock_count; i++) {
        u8 sid  = def->skill_unlocks[i].skill_id;
        u8 rlvl = def->skill_unlocks[i].req_level;
        if (ch->level >= rlvl) {
            skill_learn(ch, sid);
        }
    }
}

void xp_apply_milestones(struct Character* ch, u8 old_level, u8 new_level)
{
    const LevelMilestone* ms = g_milestones[ch->cls];
    u8 i;
    for (i = 0; i < MILESTONE_COUNT; i++) {
        if (ms[i].level > old_level && ms[i].level <= new_level) {
            /* Apply attribute bonus */
            if (ms[i].attr_bonus_type < ATTR_COUNT) {
                ch->base.val[ms[i].attr_bonus_type] += (s16)ms[i].attr_bonus_val;
            }
            /* Unlock skill */
            if (ms[i].unlock_skill_id != 0xFF) {
                skill_learn(ch, ms[i].unlock_skill_id);
            }
        }
    }
}

void xp_do_levelup(struct Character* ch)
{
    u8 old_level = ch->level;
    if (ch->level >= LEVEL_MAX) return;
    ch->level++;
    stats_apply_level_growth(&ch->base, ch->cls, ch->level);
    talent_gain_point(&ch->talents);
    xp_check_skill_unlocks(ch);
    xp_apply_milestones(ch, old_level, ch->level);
    character_recompute(ch);
    ch->hp_cur = ch->derived.val[DSTAT_HP_MAX];
    ch->mp_cur = ch->derived.val[DSTAT_MP_MAX];
    event_push(EVT_LEVEL_UP, (u16)ch->party_slot, (u16)ch->level);
}

u8 xp_award(struct Character* ch, u32 amount)
{
    u8  gained = 0;
    u8  old_lv = ch->level;

    if (ch->persist_status & CHAR_STATUS_CURSED) return 0;
    if (ch->level >= LEVEL_MAX) return 0;

    ch->xp_total += amount;

    while (ch->level < LEVEL_MAX &&
           ch->xp_total >= xp_required_for_level((u8)(ch->level + 1))) {
        xp_do_levelup(ch);
        gained++;
    }
    (void)old_lv;
    return gained;
}
