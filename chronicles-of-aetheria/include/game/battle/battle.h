#ifndef BATTLE_H
#define BATTLE_H

#include "battle_types.h"
#include "battler.h"
#include "battle_action.h"
#include "battle_ui.h"
#include "battle_anim.h"
#include "battle_data.h"

/* =========================================================
 * Battle Coordinator
 *
 * Phase state machine:
 *   ENTER      → fade in, init battlers, build turn order
 *   ROUND_START→ re-sort speed, status ticks
 *   PLAYER_INPUT → for each ally in turn order, collect action
 *   EXECUTE    → process one action at a time (anim → resolve)
 *   ENEMY_INPUT→ AI chooses actions for enemies
 *   ROUND_END  → end-of-round status, regen, doom countdown
 *   VICTORY    → XP/loot screen, fade out
 *   DEFEAT     → game over screen
 *   FLEE       → flee animation then pop battle state
 * ========================================================= */

typedef enum {
    BPHASE_ENTER = 0,
    BPHASE_ROUND_START,
    BPHASE_COLLECT_PLAYER,  /* collect input for next ally in order */
    BPHASE_COLLECT_ENEMY,   /* AI fills remaining actions */
    BPHASE_EXECUTE,         /* animate + resolve one action */
    BPHASE_ROUND_END,
    BPHASE_VICTORY,
    BPHASE_DEFEAT,
    BPHASE_FLEE,
} BattlePhase;

typedef struct {
    BattlePhase   phase;
    u8            phase_timer;

    Battler       battlers[BATTLE_BATTLER_MAX];
    u8            ally_count;
    u8            enemy_count;

    TurnSlot      turn_order[BATTLE_BATTLER_MAX];
    u8            turn_count;
    u8            turn_idx;         /* which turn_order slot is active */

    /* Which allies still need to choose this round */
    u8            ally_input_queue[BATTLE_ALLY_MAX];
    u8            ally_input_head;
    u8            ally_input_count;

    /* Current executing action */
    BattleAction  cur_action;
    ActionResult  cur_result;
    u8            cur_hit_idx;      /* processing cur_result.hits[] */

    /* XP/rewards */
    u32           xp_gained;
    u32           gold_gained;

    BattleResult  result;
    u8            group_id;         /* enemy group that was loaded */

    u8            round;
} BattleState;

extern BattleState g_battle;

/* ---- Coordinator API ------------------------------------ */

/* Called by state_battle on enter: load enemy group, init party */
void battle_begin(u8 group_id);

/* Called each frame */
void battle_update(void);
void battle_render(void);

/* Called by state_battle on exit */
void battle_end_cleanup(void);

/* Internal helpers (exposed for testing) */
void battle_load_party(void);
void battle_load_enemies(u8 group_id);
void battle_new_round(void);
void battle_advance_turn(void);

#endif /* BATTLE_H */
