#ifndef QUEST_STATE_H
#define QUEST_STATE_H

#include "quest_types.h"
#include "quest_db.h"
#include "../character/character.h"
#include "../../engine/save.h"

/* =========================================================
 * Quest Runtime State
 *
 * Lives in EWRAM. Serialized into SaveSlot.extra[] on save.
 *
 * Save layout (fits in extra[400]):
 *   Offset  0:  magic byte (0x51)
 *   Offset  1:  status[28]           = 28 bytes
 *   Offset 29:  obj_progress[28][4]  = 112 bytes
 *   Offset 141: secrets_found u32    = 4 bytes
 *   Offset 145: chain_flags   u32    = 4 bytes
 *   Offset 149: total_completed u8   = 1 byte
 *   Offset 150: _pad[6]             = 6 bytes
 *   TOTAL: 156 bytes
 * ========================================================= */

#define QUEST_SAVE_MAGIC    0x51u
#define QUEST_SAVE_OFFSET   0        /* offset in SaveSlot.extra[] */
#define QUEST_SAVE_SIZE     156

/* ---- Runtime quest progress ----------------------------- */
typedef struct {
    u8  magic;
    u8  status[QUEST_COUNT];                    /* QuestStatus per quest */
    u8  obj_progress[QUEST_COUNT][QUEST_OBJ_MAX]; /* progress counters */
    u32 secrets_found;  /* bitmask: bit N = secret quest N-22 is revealed */
    u32 chain_flags;    /* internal chain event flags */
    u8  total_completed;
    u8  _pad[6];
} QuestSaveData;

_Static_assert(sizeof(QuestSaveData) == QUEST_SAVE_SIZE,
               "QuestSaveData size mismatch");

/* Global runtime state */
extern QuestSaveData g_quest_state;

/* ---- Lifecycle ------------------------------------------ */

/* Initialize quest system (call once at game start) */
void quest_system_init(void);

/* ---- Query ---------------------------------------------- */

QuestStatus quest_status(u8 quest_id);

/* How far is objective obj_idx (0..required-1)? */
u8 quest_obj_progress(u8 quest_id, u8 obj_idx);

/* Is this objective complete? */
BOOL quest_obj_done(u8 quest_id, u8 obj_idx);

/* Are ALL objectives of a quest complete? */
BOOL quest_all_objectives_done(u8 quest_id);

/* Is this secret quest revealed (known to player)? */
BOOL quest_secret_revealed(u8 quest_id);

/* ---- Mutation ------------------------------------------- */

/* Accept a quest (AVAILABLE → ACTIVE). Returns FALSE if not available. */
BOOL quest_accept(u8 quest_id);

/* Complete a quest (ACTIVE → COMPLETED), give reward, fire events.
   Checks all objectives internally. */
BOOL quest_complete(u8 quest_id);

/* Force-fail a quest (ACTIVE → FAILED). */
void quest_fail(u8 quest_id);

/* Reveal a secret quest (sets HIDDEN → AVAILABLE if prereqs met). */
void quest_reveal_secret(u8 quest_id);

/* Advance one objective counter by delta.
   Automatically checks for quest completion after advance. */
void quest_advance_obj(u8 quest_id, u8 obj_idx, u8 delta);

/* Check if a quest can become AVAILABLE (prereqs + level) and
   transition it from LOCKED to AVAILABLE if so. */
BOOL quest_check_unlock(u8 quest_id, u8 party_max_level);

/* Evaluate all LOCKED/HIDDEN quests; unlock any that are now available. */
void quest_update_availability(void);

/* ---- Event-driven progress ------------------------------ */

/* Called from game systems on relevant events.
   Iterates all ACTIVE quests, advances matching objectives. */
void quest_notify(QuestEventType evt, u32 param0, u32 param1);

/* ---- Reward --------------------------------------------- */

/* Apply the reward of a completed quest to the party.
   (gold, XP, items). Called internally by quest_complete. */
void quest_give_reward(u8 quest_id);

/* ---- Save/Load ------------------------------------------ */

void quest_save(SaveSlot* slot);
BOOL quest_load(const SaveSlot* slot);

/* ---- UI helpers ----------------------------------------- */

/* Count quests in a given status (optionally filtered by category).
   cat=0xFF = count all categories. */
u8 quest_count_by_status(QuestStatus stat, u8 cat);

/* Fill array with quest IDs that match status+category.
   Returns count filled. out must be at least QUEST_COUNT long. */
u8 quest_list(QuestStatus stat, u8 cat, u8* out);

/* One-line summary string for UI (e.g., "3/5 objetivos") */
const char* quest_progress_str(u8 quest_id);

#endif /* QUEST_STATE_H */
