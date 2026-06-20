#include "battle_action.h"
#include "battle_data.h"
#include "battle_formula.h"
#include "battle_status.h"
#include "battle_combo.h"
#include "battler.h"
#include "gba_types.h"

/* ---- Action queue --------------------------------------- */

static BattleAction s_queue[ACTION_QUEUE_MAX];
static u8           s_q_head;
static u8           s_q_tail;
static u8           s_q_count;

void action_queue_init(void)
{
    s_q_head = s_q_tail = s_q_count = 0;
    mem_zero(s_queue, sizeof(s_queue));
}

void action_submit(u8 actor, ActionType type, u8 param, u16 target_mask)
{
    BattleAction* a;
    if (s_q_count >= ACTION_QUEUE_MAX) return;
    a              = &s_queue[s_q_tail];
    a->actor       = actor;
    a->type        = type;
    a->skill_id    = (type == ACTION_MAGIC || type == ACTION_SPECIAL || type == ACTION_ATTACK) ? param : SKILL_ATTACK;
    a->item_id     = (type == ACTION_ITEM)  ? param : 0;
    a->combo_id    = (type == ACTION_COMBO) ? param : 0;
    a->target_mask = target_mask;
    a->valid       = TRUE;
    s_q_tail = (s_q_tail + 1) % ACTION_QUEUE_MAX;
    s_q_count++;
}

void action_submit_combo(u8 actor, u8 combo_id, u16 target_mask)
{
    action_submit(actor, ACTION_COMBO, combo_id, target_mask);
}

BattleAction* action_peek(void)
{
    if (!s_q_count) return 0;
    return &s_queue[s_q_head];
}

void action_consume(void)
{
    if (!s_q_count) return;
    s_queue[s_q_head].valid = FALSE;
    s_q_head = (s_q_head + 1) % ACTION_QUEUE_MAX;
    s_q_count--;
}

u8 action_pending_count(void) { return s_q_count; }

void action_queue_clear(void)
{
    s_q_head = s_q_tail = s_q_count = 0;
}

/* ---- Turn order ----------------------------------------- */

static int _ts_cmp(const TurnSlot* a, const TurnSlot* b)
{
    if (b->eff_spd != a->eff_spd)
        return (int)(b->eff_spd - a->eff_spd);
    return (int)(a->battler_idx - b->battler_idx);
}

static void _sort_turns(TurnSlot* order, u8 count)
{
    u8 i, j;
    for (i = 1; i < count; i++) {
        TurnSlot tmp = order[i];
        j = i;
        while (j > 0 && _ts_cmp(&order[j-1], &tmp) > 0) {
            order[j] = order[j-1]; j--;
        }
        order[j] = tmp;
    }
}

void action_build_turn_order(Battler battlers[BATTLE_BATTLER_MAX],
                             TurnSlot* order, u8* out_count)
{
    u8 i, n = 0;
    for (i = 0; i < BATTLE_BATTLER_MAX; i++) {
        if (!battlers[i].alive) continue;
        order[n].battler_idx = i;
        order[n].eff_spd     = battler_eff_spd(&battlers[i]);
        n++;
    }
    _sort_turns(order, n);
    *out_count = n;
}

/* ---- Resolution ----------------------------------------- */

static u32 s_rng = 0xABCD1234u;
static u32 _rand(void) { s_rng = s_rng * 1664525u + 1013904223u; return s_rng; }

static u16 _random_enemy_mask(const Battler* battlers)
{
    u8 alive[BATTLE_ENEMY_MAX], cnt = 0, i;
    for (i = 0; i < BATTLE_ENEMY_MAX; i++)
        if (battlers[ENEMY_IDX(i)].alive)
            alive[cnt++] = i;
    if (!cnt) return 0;
    return (u16)(1u << ENEMY_IDX(alive[_rand() % cnt]));
}

static void _apply_stat_mod(Battler* target, StatModType mod, s8 stages)
{
    s8* p = 0;
    switch (mod) {
    case MOD_ATK:  p = &target->atk_stage;  break;
    case MOD_DEF:  p = &target->def_stage;  break;
    case MOD_MAG:  p = &target->mag_stage;  break;
    case MOD_MDEF: p = &target->mdef_stage; break;
    case MOD_SPD:  p = &target->spd_stage;  break;
    }
    if (!p) return;
    *p = (s8)(*p + stages);
    if (*p >  4) *p =  4;
    if (*p < -4) *p = -4;
}

ActionResult action_resolve(BattleAction* action,
                             Battler battlers[BATTLE_BATTLER_MAX])
{
    ActionResult result;
    Battler*     actor;
    u8           i;

    mem_zero(&result, sizeof(result));
    if (!action->valid) return result;

    actor = &battlers[action->actor];
    if (!actor->alive) return result;

    /* Skip-turn check */
    if (!battler_can_act(actor)) return result;

    /* Status overrides */
    {
        ActionType  ov_type   = action->type;
        u8          ov_tmask  = (u8)action->target_mask;
        if (status_modify_action(actor, &ov_tmask, &ov_type)) {
            action->type        = ov_type;
            action->target_mask = ov_tmask;
            action->skill_id    = SKILL_ATTACK;
            /* Berserk picks random enemy */
            if (action->type == ACTION_ATTACK && (action->target_mask & TARGET_ALL_ALLIES))
                action->target_mask = _random_enemy_mask(battlers);
        }
    }

    /* Confuse: 40% chance self-target on attack */
    if (status_has(actor, STATUS_CONFUSE) && action->type == ACTION_ATTACK) {
        if ((_rand() % 100) < 40) {
            u8 alive[BATTLE_ALLY_MAX], cnt = 0, j;
            for (j = 0; j < BATTLE_ALLY_MAX; j++)
                if (battlers[j].alive && j != action->actor)
                    alive[cnt++] = j;
            if (cnt)
                action->target_mask = (u16)(1u << alive[_rand() % cnt]);
        }
    }

    switch (action->type) {
    case ACTION_ATTACK:
    case ACTION_MAGIC:
    case ACTION_SPECIAL: {
        const SkillDef* sk = skill_get(action->skill_id);
        if (action->type == ACTION_MAGIC && !battler_can_magic(actor)) break;
        if (actor->mp < sk->mp_cost) break;
        actor->mp -= sk->mp_cost;

        for (i = 0; i < BATTLE_BATTLER_MAX && result.hit_count < HIT_MAX; i++) {
            HitResult* hr;
            Battler*   t;

            if (!(action->target_mask & (1u << i))) continue;
            t = &battlers[i];
            if (!t->alive && sk->effect != EFF_REVIVE) continue;

            hr         = &result.hits[result.hit_count++];
            hr->target = i;
            mem_zero(&hr->dmg, sizeof(hr->dmg));
            hr->status_applied = 0;
            hr->target_ko      = FALSE;

            switch (sk->effect) {
            case EFF_DAMAGE: {
                DamageResult dr = sk->is_physical ?
                    formula_physical(actor, t, sk->power, sk->element) :
                    formula_magical(actor, t, sk->power, sk->element);
                hr->dmg = dr;
                if (!dr.is_miss && !dr.is_absorb) {
                    battler_apply_damage(t, dr.damage);
                    status_on_damage(t);
                    battler_add_cp(t, CP_FROM_DAMAGE);
                } else if (dr.is_absorb) {
                    battler_apply_damage(t, dr.damage); /* negative = heal */
                }
                break;
            }
            case EFF_HEAL: {
                s16 hp = formula_heal(actor, sk->power);
                battler_apply_damage(t, (s16)-hp);
                hr->dmg.damage = (s16)-hp;
                break;
            }
            case EFF_DRAIN: {
                DamageResult dr = formula_drain(actor, t, sk->power, sk->element);
                hr->dmg = dr;
                if (!dr.is_miss) {
                    battler_apply_damage(t, dr.damage);
                    battler_apply_damage(actor, (s16)-(dr.damage / 2));
                    status_on_damage(t);
                }
                break;
            }
            case EFF_STATUS_ADD:
                if (formula_status_hit(actor, t, sk->status_chance)) {
                    status_apply(t, (StatusFlag)sk->status_inflict, 0);
                    hr->status_applied = (StatusFlag)sk->status_inflict;
                }
                break;
            case EFF_STATUS_REMOVE:
                if (sk->target == TGT_SINGLE_ENEMY) status_dispel_all_pos(t);
                else                                 status_cure_all_neg(t);
                break;
            case EFF_STAT_MOD:
                _apply_stat_mod(t, sk->stat_mod_type, sk->stat_mod_val);
                break;
            case EFF_REVIVE:
                if (!t->alive) {
                    t->alive = TRUE;
                    t->hp    = t->hp_max / 2;
                }
                break;
            default:
                break;
            }

            if (t->alive == FALSE) hr->target_ko = TRUE;
        }

        battler_add_cp(actor, CP_FROM_ATTACK);
        break;
    }

    case ACTION_COMBO: {
        const ComboDef* def = combo_get(action->combo_id);
        if (!def || !combo_is_ready(battlers, action->combo_id)) break;

        combo_execute(battlers, action->combo_id, action->target_mask);

        for (i = 0; i < BATTLE_BATTLER_MAX && result.hit_count < HIT_MAX; i++) {
            HitResult* hr;
            Battler*   t;
            DamageResult dr;

            if (!(action->target_mask & (1u << i))) continue;
            t = &battlers[i];
            if (!t->alive) continue;

            hr         = &result.hits[result.hit_count++];
            hr->target = i;
            mem_zero(&hr->dmg, sizeof(hr->dmg));
            hr->status_applied = 0;
            hr->target_ko      = FALSE;

            dr = formula_magical(actor, t, (u8)(def->power * COMBO_DAMAGE_MULT / 100), def->element);
            hr->dmg = dr;
            if (!dr.is_miss) {
                battler_apply_damage(t, dr.damage);
                if (def->secondary_eff == EFF_STATUS_ADD && def->status_inflict)
                    if (formula_status_hit(actor, t, def->status_chance))
                        status_apply(t, (StatusFlag)def->status_inflict, 0);
            }
            if (!t->alive) hr->target_ko = TRUE;
        }
        break;
    }

    case ACTION_DEFEND:
        actor->defending = TRUE;
        battler_add_cp(actor, CP_FROM_DEFEND);
        break;

    case ACTION_FLEE:
        {
            const EnemyGroup* grp = 0; /* flee_mult not available here; use 1 */
            result.flee_success = formula_flee(actor, grp ? grp->flee_multiplier : 1);
        }
        break;

    default:
        break;
    }

    return result;
}

/* ---- AI ------------------------------------------------- */

BattleAction ai_choose_action(u8 enemy_idx,
                               Battler battlers[BATTLE_BATTLER_MAX])
{
    BattleAction a;
    const Battler* self = &battlers[enemy_idx];
    u8 alive_allies[BATTLE_ALLY_MAX], cnt = 0, i;
    u32 roll;

    mem_zero(&a, sizeof(a));
    a.actor = enemy_idx;
    a.valid = TRUE;

    for (i = 0; i < BATTLE_ALLY_MAX; i++)
        if (battlers[i].alive) alive_allies[cnt++] = i;

    if (!cnt) { a.valid = FALSE; return a; }

    roll = _rand() % 100;

    if (roll < 60 || self->skill_count == 0) {
        u8 target = alive_allies[_rand() % cnt];
        a.type        = ACTION_ATTACK;
        a.skill_id    = SKILL_ATTACK;
        a.target_mask = (u16)(1u << target);
    } else {
        u8 sk_idx = (u8)(_rand() % self->skill_count);
        u8 sk_id  = self->skill_ids[sk_idx];
        const SkillDef* sk = skill_get(sk_id);

        if (self->mp < sk->mp_cost || (sk->is_physical == FALSE && !battler_can_magic(self))) {
            a.type     = ACTION_ATTACK;
            a.skill_id = SKILL_ATTACK;
            a.target_mask = (u16)(1u << alive_allies[_rand() % cnt]);
        } else {
            a.type     = sk->is_physical ? ACTION_ATTACK : ACTION_MAGIC;
            a.skill_id = sk_id;

            if (sk->target == TGT_ALL_ENEMIES || sk->target == TGT_ALL_ALLIES) {
                u16 mask = 0;
                for (i = 0; i < BATTLE_ALLY_MAX; i++)
                    if (battlers[i].alive) mask |= (u16)(1u << i);
                a.target_mask = mask;
            } else {
                a.target_mask = (u16)(1u << alive_allies[_rand() % cnt]);
            }
        }
    }
    return a;
}

/* ---- End condition -------------------------------------- */

BattleResult action_check_end(const Battler battlers[BATTLE_BATTLER_MAX])
{
    u8 allies = 0, enemies = 0, i;
    for (i = 0; i < BATTLE_ALLY_MAX; i++)  if (battlers[i].alive) allies++;
    for (i = BATTLE_ALLY_MAX; i < BATTLE_BATTLER_MAX; i++) if (battlers[i].alive) enemies++;
    if (!allies)  return BATTLE_RESULT_LOSE;
    if (!enemies) return BATTLE_RESULT_WIN;
    return BATTLE_RESULT_NONE;
}
