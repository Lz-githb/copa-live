#include "battle_anim.h"
#include "battler.h"
#include "gba_types.h"
#include "gba_reg.h"
#include "time_weather.h"

BattleAnim g_anim;

static const s16 ENEMY_X[6] = { 20, 80, 140, 20, 80, 140 };
static const s16 ENEMY_Y[6] = { 16, 16, 16, 56, 56, 56 };
static const s16 ALLY_X[4]  = { 20, 70, 130, 190 };

static const u8 ANIM_FRAMES[ANIM_COUNT] = {
    [ANIM_NONE]          = 0,
    [ANIM_ATTACK]        = 20,
    [ANIM_FIRE]          = 30,
    [ANIM_WATER]         = 30,
    [ANIM_EARTH]         = 35,
    [ANIM_AIR]           = 25,
    [ANIM_LIGHT]         = 28,
    [ANIM_DARK]          = 28,
    [ANIM_HEAL]          = 22,
    [ANIM_DRAIN]         = 30,
    [ANIM_STATUS]        = 18,
    [ANIM_COMBO_FIRE]    = 50,
    [ANIM_COMBO_TIDAL]   = 50,
    [ANIM_COMBO_STORM]   = 50,
    [ANIM_COMBO_HOLY]    = 55,
    [ANIM_COMBO_SHADOW]  = 48,
    [ANIM_UNLEASH_SOLEIL]= 60,
    [ANIM_UNLEASH_TIDE]  = 60,
    [ANIM_UNLEASH_GAIA]  = 60,
    [ANIM_UNLEASH_STORM] = 60,
    [ANIM_VICTORY]       = 90,
    [ANIM_KO]            = 30,
    [ANIM_HIT]           = 8,
    [ANIM_MISS]          = 20,
    [ANIM_CRITICAL]      = 12,
    [ANIM_FLEE]          = 25,
};

void anim_init(void)
{
    mem_zero(&g_anim, sizeof(g_anim));
    g_anim.phase = ANIM_STATE_IDLE;
    g_anim.id    = ANIM_NONE;
}

void anim_start(AnimID id, u8 actor_idx, u16 target_mask)
{
    u8 i;
    mem_zero(&g_anim, sizeof(g_anim));
    g_anim.id           = id;
    g_anim.phase        = (id == ANIM_NONE) ? ANIM_STATE_DONE : ANIM_STATE_SLIDE_OUT;
    g_anim.total_frames = ANIM_FRAMES[id];
    g_anim.actor_idx    = actor_idx;
    g_anim.target_mask  = target_mask;

    if (actor_idx < BATTLE_ALLY_MAX)
        g_anim.actor_orig_x = ALLY_X[actor_idx];
    else
        g_anim.actor_orig_x = ENEMY_X[actor_idx - BATTLE_ALLY_MAX];

    for (i = 0; i < BATTLE_BATTLER_MAX; i++) {
        if (target_mask & (1u << i)) {
            g_anim.target_x = (i < BATTLE_ALLY_MAX) ? ALLY_X[i] : ENEMY_X[i - BATTLE_ALLY_MAX];
            break;
        }
    }
}

void anim_update(void)
{
    if (g_anim.phase == ANIM_STATE_IDLE || g_anim.phase == ANIM_STATE_DONE) return;

    g_anim.timer++;

    switch (g_anim.phase) {
    case ANIM_STATE_SLIDE_OUT:
        if (g_anim.timer >= 8)  { g_anim.timer = 0; g_anim.phase = ANIM_STATE_EFFECT; }
        break;
    case ANIM_STATE_EFFECT:
        if (g_anim.timer >= g_anim.total_frames) { g_anim.timer = 0; g_anim.phase = ANIM_STATE_HIT; }
        break;
    case ANIM_STATE_HIT:
        if (g_anim.timer >= 8)  { g_anim.timer = 0; g_anim.phase = ANIM_STATE_SLIDE_IN; }
        break;
    case ANIM_STATE_SLIDE_IN:
        if (g_anim.timer >= 8)  { g_anim.timer = 0; g_anim.phase = ANIM_STATE_TEXT; }
        break;
    case ANIM_STATE_TEXT: {
        u8 i, any = 0;
        for (i = 0; i < DAMAGE_NUM_MAX; i++) {
            DamageNumber* dn = &g_anim.dmg_nums[i];
            if (!dn->active) continue;
            any = 1;
            dn->y  += dn->vy;
            dn->vy  = (s16)(dn->vy > -8 ? dn->vy - 1 : -8);
            dn->timer++;
            if (dn->timer > 60) dn->active = FALSE;
        }
        if (!any) g_anim.phase = ANIM_STATE_DONE;
        break;
    }
    default: break;
    }

    if (g_anim.shake_frames > 0) {
        s16 off = (s16)((g_anim.shake_frames & 1) ? 2 : -2);
        REG_BG0HOFS = (vu16)off;
        REG_BG1HOFS = (vu16)off;
        g_anim.shake_frames--;
        if (!g_anim.shake_frames) { REG_BG0HOFS = 0; REG_BG1HOFS = 0; }
    }
}

BOOL anim_done(void)
{
    return (BOOL)(g_anim.phase == ANIM_STATE_DONE || g_anim.phase == ANIM_STATE_IDLE);
}

void anim_spawn_damage_num(s16 screen_x, s16 screen_y,
                            s16 value, BOOL is_crit,
                            BOOL is_heal, BOOL is_miss)
{
    u8 i;
    for (i = 0; i < DAMAGE_NUM_MAX; i++) {
        DamageNumber* dn = &g_anim.dmg_nums[i];
        if (dn->active) continue;
        dn->x       = screen_x;
        dn->y       = screen_y;
        dn->vy      = -2;
        dn->value   = value;
        dn->timer   = 0;
        dn->active  = TRUE;
        dn->is_crit = is_crit;
        dn->is_heal = is_heal;
        dn->is_miss = is_miss;
        return;
    }
}

void anim_set_positions(Battler battlers[BATTLE_BATTLER_MAX])
{
    u8 i;
    for (i = 0; i < BATTLE_ALLY_MAX; i++) {
        battlers[i].screen_x = ALLY_X[i];
        battlers[i].screen_y = ALLY_Y_ROW;
    }
    for (i = 0; i < BATTLE_ENEMY_MAX; i++) {
        battlers[ENEMY_IDX(i)].screen_x = ENEMY_X[i];
        battlers[ENEMY_IDX(i)].screen_y = ENEMY_Y[i];
    }
}

void anim_render_nums(void)
{
    /* Damage number rendering via tile overlay — platform stub */
    u8 i;
    for (i = 0; i < DAMAGE_NUM_MAX; i++) {
        (void)&g_anim.dmg_nums[i];
    }
}

void anim_flash(u8 brightness, u8 frames, BOOL white)
{
    g_anim.shake_frames = frames;
    REG_BLDCNT = (vu16)(BLD_BG0|BLD_BG1|BLD_BG2|BLD_BG3|BLD_OBJ | (white ? BLD_WHITE : BLD_BLACK));
    REG_BLDY   = brightness;
}

void anim_ko(Battler* b)
{
    b->alive = FALSE;
    anim_start(ANIM_KO, b->idx, (u16)(1u << b->idx));
}
