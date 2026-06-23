#ifndef QUEST_CHAIN_H
#define QUEST_CHAIN_H

#include "quest_types.h"
#include "quest_state.h"

/* =========================================================
 * Quest Chain System
 *
 * Connects the engine event bus to quest_notify().
 * Also exposes direct call-sites for scripted triggers
 * (story scenes, escort sequences, world flags).
 *
 * Call chain_init() once at game startup (after event_init).
 * ========================================================= */

/* Register all event listeners — call once at game start */
void chain_init(void);

/* ---- Direct triggers (called by game logic) ------------- */

/* Enemy died in battle (battle system calls this per kill) */
void chain_on_enemy_killed(u8 enemy_id);

/* Item obtained (inventory system calls after add) */
void chain_on_item_obtained(u8 item_id, u8 qty);

/* Recipe crafted successfully */
void chain_on_crafted(u8 recipe_id);

/* Item equipped on any character */
void chain_on_equipped(u8 item_id);

/* Advance a scripted objective by quest + stage number.
   Dialogue scripts call this to mark story checkpoints.
   stage matches the obj->required value in QuestObjDef. */
void chain_script_flag(u8 quest_id, u8 stage);

/* Escort quest: NPC arrived safely */
void chain_escort_success(u8 quest_id);

/* Escort quest: NPC died/failed */
void chain_escort_fail(u8 quest_id);

/* ---- World flags ---------------------------------------- */

/* Set a world flag bit and re-evaluate quest availability */
void chain_set_world_flag(u32 flag_bit);

/* Read a world flag bit */
BOOL chain_world_flag(u32 flag_bit);

/* Try to auto-complete quest if all objectives are done */
void chain_try_complete(u8 quest_id);

/* ---- Low-level flag access ------------------------------ */
BOOL chain_flag_get(u32 bit);
void chain_flag_set(u32 bit);
void chain_flag_clear(u32 bit);

/* ---- World flag constants (shared with quest_db.c) ------ */
#define WF_VILLAGE_SAVED     (1u<<0)
#define WF_GUILD_MEMBER      (1u<<1)
#define WF_DUNGEON1_CLEAR    (1u<<2)
#define WF_CRYSTALS_FOUND    (1u<<3)
#define WF_BETRAYAL_KNOWN    (1u<<4)
#define WF_RUINS_EXPLORED    (1u<<5)
#define WF_DRAGON_SLAIN      (1u<<6)
#define WF_GATE_OPENED       (1u<<7)
#define WF_TRIALS_DONE       (1u<<8)
#define WF_AETHERIA_SAVED    (1u<<9)
#define WF_SHOP_BLACK_OPEN   (1u<<10)
#define WF_FORGE_LEGEND_OPEN (1u<<11)
#define WF_TRUE_KING_KNOWN   (1u<<12)
#define WF_VOID_WALKER       (1u<<13)

#endif /* QUEST_CHAIN_H */
