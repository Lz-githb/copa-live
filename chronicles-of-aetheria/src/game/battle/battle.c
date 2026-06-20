#include "battle.h"
#include "battle_data.h"
#include "battle_formula.h"
#include "battle_status.h"
#include "battle_combo.h"
#include "battle_anim.h"
#include "battle_ui.h"
#include "battler.h"
#include "gba_types.h"
#include "gba_reg.h"
#include "time_weather.h"
#include "event.h"

BattleState g_battle;

/* ---- Init helpers --------------------------------------- */

void battle_load_party(void)
{
    u8 i;
    for (i = 0; i < BATTLE_ALLY_MAX; i++)
        battler_from_char(&g_battle.battlers[i], i, &g_char_table[i]);
    g_battle.ally_count = BATTLE_ALLY_MAX;
}

void battle_load_enemies(u8 group_id)
{
    const EnemyGroup* grp = group_get(group_id);
    u8 i;
    g_battle.enemy_count = grp->count < BATTLE_ENEMY_MAX ? grp->count : BATTLE_ENEMY_MAX;
    for (i = 0; i < g_battle.enemy_count; i++)
        battler_from_enemy(&g_battle.battlers[ENEMY_IDX(i)],
                           (u8)ENEMY_IDX(i), enemy_get(grp->enemy_ids[i]));
    for (i = g_battle.enemy_count; i < BATTLE_ENEMY_MAX; i++)
        mem_zero(&g_battle.battlers[ENEMY_IDX(i)], sizeof(Battler));
}

void battle_new_round(void)
{
    u8 i, cnt;

    for (i = 0; i < BATTLE_BATTLER_MAX; i++)
        battler_new_round(&g_battle.battlers[i]);

    action_build_turn_order(g_battle.battlers, g_battle.turn_order, &g_battle.turn_count);
    g_battle.turn_idx = 0;
    g_battle.round++;

    /* Status tick */
    for (i = 0; i < BATTLE_BATTLER_MAX; i++)
        if (g_battle.battlers[i].alive)
            status_tick(&g_battle.battlers[i]);

    /* Build ally input queue */
    cnt = 0;
    for (i = 0; i < g_battle.turn_count; i++) {
        u8 bidx = g_battle.turn_order[i].battler_idx;
        if (bidx < BATTLE_ALLY_MAX && g_battle.battlers[bidx].alive)
            g_battle.ally_input_queue[cnt++] = bidx;
    }
    g_battle.ally_input_count = cnt;
    g_battle.ally_input_head  = 0;

    /* Pre-generate AI actions for enemies */
    action_queue_clear();
    for (i = 0; i < g_battle.turn_count; i++) {
        u8 bidx = g_battle.turn_order[i].battler_idx;
        if (bidx >= BATTLE_ALLY_MAX && g_battle.battlers[bidx].alive) {
            BattleAction ai = ai_choose_action(bidx, g_battle.battlers);
            if (ai.valid)
                action_submit(ai.actor, ai.type, ai.skill_id, ai.target_mask);
        }
    }

    battle_ui_set_turn_order(g_battle.turn_order, g_battle.turn_count);
    g_battle.phase = BPHASE_COLLECT_PLAYER;

    if (cnt > 0) {
        u8 first = g_battle.ally_input_queue[0];
        battle_ui_set_active_char(first, &g_battle.battlers[first]);
        battle_ui_show_cmd();
    }
}

void battle_advance_turn(void)
{
    g_battle.turn_idx++;
    g_battle.phase = (g_battle.turn_idx >= g_battle.turn_count) ?
                     BPHASE_ROUND_END : BPHASE_EXECUTE;
}

/* ---- Public API ----------------------------------------- */

void battle_begin(u8 group_id)
{
    mem_zero(&g_battle, sizeof(g_battle));
    g_battle.group_id = group_id;
    g_battle.phase    = BPHASE_ENTER;

    battle_load_party();
    battle_load_enemies(group_id);
    anim_init();
    battle_ui_init();
    action_queue_init();
    combo_system_init();
    anim_set_positions(g_battle.battlers);

    REG_BLDCNT = (vu16)(BLD_BG0|BLD_BG1|BLD_BG2|BLD_BG3|BLD_OBJ|BLD_BLACK);
    REG_BLDY   = 16;
    g_battle.phase_timer = 16;
}

void battle_update(void)
{
    BattleResult end;

    switch (g_battle.phase) {

    case BPHASE_ENTER:
        if (g_battle.phase_timer > 0) {
            g_battle.phase_timer--;
            REG_BLDY = g_battle.phase_timer;
        } else {
            REG_BLDY = 0;
            battle_new_round();
        }
        break;

    case BPHASE_COLLECT_PLAYER: {
        BattleAction action;
        if (g_battle.ally_input_head >= g_battle.ally_input_count) {
            g_battle.turn_idx = 0;
            g_battle.phase    = BPHASE_EXECUTE;
            battle_ui_hide();
            break;
        }
        if (battle_ui_update(g_battle.battlers, &action)) {
            action_submit(action.actor, action.type, action.skill_id, action.target_mask);
            g_battle.ally_input_head++;
            if (g_battle.ally_input_head < g_battle.ally_input_count) {
                u8 next = g_battle.ally_input_queue[g_battle.ally_input_head];
                battle_ui_set_active_char(next, &g_battle.battlers[next]);
                battle_ui_show_cmd();
            } else {
                battle_ui_hide();
                g_battle.turn_idx = 0;
                g_battle.phase    = BPHASE_EXECUTE;
            }
        }
        break;
    }

    case BPHASE_EXECUTE: {
        BattleAction* cur;
        if (!anim_done()) { anim_update(); break; }

        if (g_battle.turn_idx >= g_battle.turn_count) {
            g_battle.phase = BPHASE_ROUND_END; break;
        }

        cur = action_peek();
        if (!cur) { battle_advance_turn(); break; }

        /* Match actor to current turn slot */
        {
            u8 bidx = g_battle.turn_order[g_battle.turn_idx].battler_idx;
            if (cur->actor != bidx) { action_consume(); break; }
        }

        g_battle.cur_action = *cur;
        action_consume();

        if (!g_battle.battlers[g_battle.cur_action.actor].alive) {
            battle_advance_turn(); break;
        }

        g_battle.cur_result = action_resolve(&g_battle.cur_action, g_battle.battlers);

        /* Spawn damage numbers */
        {
            u8 h;
            for (h = 0; h < g_battle.cur_result.hit_count; h++) {
                HitResult* hr = &g_battle.cur_result.hits[h];
                Battler*   t  = &g_battle.battlers[hr->target];
                anim_spawn_damage_num(t->screen_x, t->screen_y,
                                      hr->dmg.damage, hr->dmg.is_critical,
                                      (BOOL)(hr->dmg.damage < 0), hr->dmg.is_miss);
            }
        }

        {
            const SkillDef* sk = skill_get(g_battle.cur_action.skill_id);
            anim_start((AnimID)sk->anim_id, g_battle.cur_action.actor,
                       g_battle.cur_action.target_mask);
        }

        if (g_battle.cur_result.flee_success) { g_battle.phase = BPHASE_FLEE; break; }

        end = action_check_end(g_battle.battlers);
        if (end == BATTLE_RESULT_WIN)  { g_battle.phase = BPHASE_VICTORY; break; }
        if (end == BATTLE_RESULT_LOSE) { g_battle.phase = BPHASE_DEFEAT;  break; }

        battle_advance_turn();
        break;
    }

    case BPHASE_ROUND_END:
        end = action_check_end(g_battle.battlers);
        if (end == BATTLE_RESULT_WIN)  { g_battle.phase = BPHASE_VICTORY; break; }
        if (end == BATTLE_RESULT_LOSE) { g_battle.phase = BPHASE_DEFEAT;  break; }
        battle_new_round();
        break;

    case BPHASE_VICTORY:
        if (g_battle.phase_timer == 0) {
            anim_flash(0, 30, TRUE);
            g_battle.phase_timer = 90;
            battle_ui_message("Victory!");
        } else {
            g_battle.phase_timer--;
            if (!g_battle.phase_timer) {
                g_battle.result = BATTLE_RESULT_WIN;
                event_push(EVT_BATTLE_END, BATTLE_RESULT_WIN, 0);
            }
        }
        break;

    case BPHASE_DEFEAT:
        if (g_battle.phase_timer == 0) {
            anim_flash(16, 30, FALSE);
            g_battle.phase_timer = 90;
            battle_ui_message("Defeated...");
        } else {
            g_battle.phase_timer--;
            if (!g_battle.phase_timer) {
                g_battle.result = BATTLE_RESULT_LOSE;
                event_push(EVT_BATTLE_END, BATTLE_RESULT_LOSE, 0);
            }
        }
        break;

    case BPHASE_FLEE:
        if (g_battle.phase_timer == 0) {
            battle_ui_message("Escaped!");
            g_battle.phase_timer = 60;
        } else {
            g_battle.phase_timer--;
            if (!g_battle.phase_timer) {
                g_battle.result = BATTLE_RESULT_FLEE;
                event_push(EVT_BATTLE_END, BATTLE_RESULT_FLEE, 0);
            }
        }
        break;

    default: break;
    }
}

void battle_render(void)
{
    anim_render_nums();
    battle_ui_render(g_battle.battlers);
}

void battle_end_cleanup(void)
{
    REG_BLDCNT = 0;
    REG_BLDY   = 0;
    battle_ui_hide();
    anim_init();
    action_queue_clear();
}
