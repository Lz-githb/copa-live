#ifndef BATTLE_STATUS_H
#define BATTLE_STATUS_H

#include "battle_types.h"
#include "battler.h"

/* =========================================================
 * Status Effect System
 *
 * Status effects are stored as a bitmask + duration array.
 * Each effect has:
 *   - Apply condition (resistance check)
 *   - On-turn tick (DoT / HoT)
 *   - Action modifier (skip turn, confuse, etc.)
 *   - Removal conditions (time, damage, other status)
 * ========================================================= */

/* Default durations (turns) */
#define STATUS_DUR_POISON    5
#define STATUS_DUR_BURN      4
#define STATUS_DUR_FREEZE    2
#define STATUS_DUR_BLIND     4
#define STATUS_DUR_SILENCE   3
#define STATUS_DUR_SLOW      4
#define STATUS_DUR_CONFUSE   3
#define STATUS_DUR_SLEEP     3
#define STATUS_DUR_STONE     2
#define STATUS_DUR_DOOM      5
#define STATUS_DUR_REGEN     4
#define STATUS_DUR_HASTE     3
#define STATUS_DUR_BARRIER   4
#define STATUS_DUR_MBARRIER  4
#define STATUS_DUR_REFLECT   3
#define STATUS_DUR_BERSERK   4

/* Status metadata */
typedef struct {
    const char* name;
    u8          default_dur;    /* turns */
    BOOL        is_positive;
    BOOL        skip_turn;      /* true = battler skips action */
    u8          dot_pct;        /* damage % of maxHP per turn (0=none) */
    u8          hot_pct;        /* heal % of maxHP per turn (0=none) */
    StatusFlag  cured_by;       /* which other status can cure this */
    BOOL        wakes_on_hit;   /* for SLEEP / FREEZE */
} StatusMeta;

extern const StatusMeta STATUS_META[STATUS_COUNT];

/* ---- Status API ----------------------------------------- */

/* Attempt to apply a status to target (checks resistance, existing) */
BOOL status_apply(Battler* target, StatusFlag flag, u8 duration);

/* Remove a specific status */
void status_remove(Battler* target, StatusFlag flag);

/* Remove all negative statuses */
void status_cure_all_neg(Battler* target);

/* Remove all positive statuses (dispel) */
void status_dispel_all_pos(Battler* target);

/* Check if a status is active */
BOOL status_has(const Battler* b, StatusFlag flag);

/* Tick all statuses (call at start of battler's turn) */
void status_tick(Battler* b);

/* Wake from SLEEP or FREEZE when hit */
void status_on_damage(Battler* b);

/* Modify action based on status (confuse, berserk) */
/* Returns TRUE if action was overridden */
BOOL status_modify_action(Battler* actor, u8* target_mask,
                          ActionType* action);

/* Get effective ATK mod from status (BURN reduces ATK) */
s8 status_atk_modifier(const Battler* b);

/* Describe active statuses into a short string (for UI) */
void status_describe(const Battler* b, char* buf, u8 buf_len);

/* String names for each status (indexed by STATUS_COUNT position) */
extern const char* STATUS_NAMES[STATUS_COUNT];

#endif /* BATTLE_STATUS_H */
