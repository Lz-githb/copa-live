#ifndef BATTLE_ACTION_H
#define BATTLE_ACTION_H

#include "battle_types.h"
#include "battler.h"

/* =========================================================
 * Battle Action Queue & Turn Resolution
 *
 * Turn order: at start of each round, sort all alive
 * battlers by effective SPD descending. HASTE doubles SPD,
 * SLOW halves it. Ties broken by battler index.
 *
 * Resolution order:
 *   1. Status tick (DoT/HoT/DOOM)
 *   2. Check skip-turn statuses
 *   3. Execute action (damage/heal/status/etc.)
 *   4. Post-action status triggers (wake, freeze-thaw)
 *   5. Check battle end condition
 * ========================================================= */

/* Max pending actions per round */
#define ACTION_QUEUE_MAX  BATTLE_BATTLER_MAX

typedef struct {
    u8          actor;         /* battler index 0-9 */
    ActionType  type;
    u8          skill_id;      /* for ACTION_MAGIC / ACTION_SPECIAL */
    u8          item_id;       /* for ACTION_ITEM */
    u8          combo_id;      /* for ACTION_COMBO */
    u16         target_mask;   /* 10-bit bitmask */
    BOOL        valid;
} BattleAction;

/* Turn order slot */
typedef struct {
    u8   battler_idx;
    s16  eff_spd;       /* effective speed after modifiers */
} TurnSlot;

/* ---- Action queue API ----------------------------------- */

void    action_queue_init(void);

/* Build turn order from alive battlers */
void    action_build_turn_order(Battler battlers[BATTLE_BATTLER_MAX],
                                TurnSlot* order, u8* out_count);

/* Submit a player's chosen action */
void    action_submit(u8 actor, ActionType type, u8 param,
                      u16 target_mask);

/* Submit combo action (marks partner turns consumed) */
void    action_submit_combo(u8 actor, u8 combo_id, u16 target_mask);

/* Get the next pending action from the queue */
BattleAction* action_peek(void);
void          action_consume(void);
u8            action_pending_count(void);

/* Clear all pending actions */
void    action_queue_clear(void);

/* ---- Resolution ----------------------------------------- */

typedef struct {
    u8           target;
    DamageResult dmg;
    StatusFlag   status_applied;
    BOOL         target_ko;
} HitResult;

#define HIT_MAX BATTLE_BATTLER_MAX

typedef struct {
    u8        hit_count;
    HitResult hits[HIT_MAX];
    BOOL      actor_ko;     /* caster died (drain, reflect) */
    BOOL      flee_success;
} ActionResult;

/* Resolve one action against the battler array */
ActionResult action_resolve(BattleAction* action,
                             Battler battlers[BATTLE_BATTLER_MAX]);

/* AI: generate action for an enemy */
BattleAction ai_choose_action(u8 enemy_idx,
                               Battler battlers[BATTLE_BATTLER_MAX]);

/* Check battle-end condition */
BattleResult action_check_end(const Battler battlers[BATTLE_BATTLER_MAX]);

#endif /* BATTLE_ACTION_H */
