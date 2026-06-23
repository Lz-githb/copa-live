#ifndef QUEST_DB_H
#define QUEST_DB_H

#include "quest_types.h"

/* =========================================================
 * Quest Database — ROM Definitions
 *
 * All quest definitions live in ROM (const data).
 * No dynamic allocation; runtime state is in quest_state.h.
 * ========================================================= */

/* ---- ROM table ------------------------------------------ */
extern const QuestDef g_quest_db[QUEST_COUNT];

/* ---- Lookup --------------------------------------------- */
const QuestDef* quest_def_get(u8 quest_id);

/* True if ID is a main quest */
BOOL quest_is_main(u8 quest_id);

/* True if ID is a secret quest */
BOOL quest_is_secret(u8 quest_id);

/* Get objective definition */
const QuestObjDef* quest_obj_get(u8 quest_id, u8 obj_idx);

#endif /* QUEST_DB_H */
