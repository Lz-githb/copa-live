#ifndef CHAR_XP_H
#define CHAR_XP_H

#include "char_types.h"

/* =========================================================
 * Experience & Level System
 *
 * XP curve: Medium-Fast growth (like Pokémon Medium-Fast)
 *   XP_to_level(n) = n^3  (for n = 2..99)
 *   Level 99 requires 970299 total XP
 *
 * On level up:
 *   1. Increment level
 *   2. Apply attribute growth (stats_apply_level_growth)
 *   3. Restore HP and MP to new max
 *   4. Gain 1 talent point
 *   5. Check for new skill unlocks
 *   6. Fire EVT_LEVEL_UP event
 *
 * XP is stored as u32 (max ~4 billion, plenty for lv99).
 * Each level threshold is computed on-the-fly with the
 * cube formula to save ROM (no 99-entry table needed).
 * ========================================================= */

/* XP required to REACH level n (cumulative from 0) */
/* Formula: n^3 (n >= 2), level 1 = 0 */
u32 xp_required_for_level(u8 level);

/* XP required to go FROM current level TO next level */
u32 xp_to_next_level(u8 current_level);

/* Level corresponding to a total XP amount */
u8  xp_level_for_xp(u32 total_xp);

/* How many levels would this XP grant from current_level? */
u8  xp_levels_gained(u8 current_level, u32 current_xp, u32 xp_gain);

/* Milestone bonuses awarded at specific levels */
typedef struct {
    u8  level;
    u8  attr_bonus_type;  /* AttrType */
    s16 attr_bonus_val;
    u8  unlock_skill_id;  /* 0xFF = none */
} LevelMilestone;

#define MILESTONE_COUNT  10

extern const LevelMilestone g_milestones[CLASS_COUNT][MILESTONE_COUNT];

/* ---- API ------------------------------------------------ */

/* Add XP to a character; handles multi-level-up; returns levels gained */
u8 xp_award(struct Character* ch, u32 amount);

/* Perform a single level-up (called internally by xp_award) */
void xp_do_levelup(struct Character* ch);

/* Check and unlock skills for current level */
void xp_check_skill_unlocks(struct Character* ch);

/* Apply class milestone rewards at specific levels */
void xp_apply_milestones(struct Character* ch, u8 old_level, u8 new_level);

#endif /* CHAR_XP_H */
