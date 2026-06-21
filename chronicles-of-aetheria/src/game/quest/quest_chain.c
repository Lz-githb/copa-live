#include "quest_state.h"
#include "crafting.h"
#include "item_use.h"
#include "../../engine/event.h"
#include "gba_types.h"

/* =========================================================
 * Quest Chain System
 *
 * Bridges the engine event bus to the quest notification
 * system. Registers listeners on EVT_* events at init and
 * dispatches them to quest_notify() with the right
 * QuestEventType + parameters.
 *
 * Also handles special scripted chain transitions that
 * cannot be driven by a simple counter (escort sequences,
 * multi-stage scripted scenes, conditional flags).
 * ========================================================= */

/* ---- Chain flag helpers --------------------------------- */

/* Read/write individual chain_flags bits */
BOOL chain_flag_get(u32 bit)
{
    return (BOOL)((g_quest_state.chain_flags & bit) != 0);
}

void chain_flag_set(u32 bit)
{
    g_quest_state.chain_flags |= bit;
}

void chain_flag_clear(u32 bit)
{
    g_quest_state.chain_flags &= ~bit;
}

/* ---- Monster-hunt tracker ------------------------------- */
/* Tracks which of the 12 enemy types have been killed at least once.
   Stored in a u16 (bitmask). Persisted via chain_flags bits 16-27. */
#define MH_FLAG_SHIFT  16u
#define MH_ALL_MASK    ((u32)0x0FFFu << MH_FLAG_SHIFT)

static void _monster_hunt_check(u8 enemy_id)
{
    u32 bit;
    if (enemy_id >= 12) return;
    bit = (u32)(1u << (MH_FLAG_SHIFT + enemy_id));
    chain_flag_set(bit);

    /* All 12 types killed? */
    if ((g_quest_state.chain_flags & MH_ALL_MASK) == MH_ALL_MASK) {
        quest_notify(QEVT_SCRIPT_FLAG, QSIDE_MONSTER_HUNT, 1);
    }
}

/* ---- Event listeners (registered at chain_init) --------- */

static void _on_battle_end(const Event* evt)
{
    /* param0 = result (WIN=0, LOSE=1, FLEE=2), param1 = group_id */
    if (evt->param0 != 0) return; /* only count wins */
    quest_notify(QEVT_BATTLE_WON, evt->param1, 0);
}

static void _on_battle_start(const Event* evt)
{
    /* Used for encounter tracking — no quest use currently */
    (void)evt;
}

static void _on_warp(const Event* evt)
{
    /* param0 = map_id, param1 = spawn_id */
    quest_notify(QEVT_MAP_ENTERED, evt->param0, evt->param1);
}

static void _on_dialogue_start(const Event* evt)
{
    /* param0 = script_id */
    quest_notify(QEVT_NPC_TALKED, evt->param0, 0);
}

static void _on_item_used(const Event* evt)
{
    /* param0 = item_id, param1 = target_slot */
    quest_notify(QEVT_ITEM_OBTAINED, evt->param0, evt->param1);
}

static void _on_shop_buy(const Event* evt)
{
    /* param0 = item_id, param1 = qty */
    quest_notify(QEVT_ITEM_BOUGHT, evt->param0, evt->param1);
    quest_notify(QEVT_ITEM_OBTAINED, evt->param0, evt->param1);
}

static void _on_level_up(const Event* evt)
{
    /* param0 = party_slot, param1 = new_level */
    (void)evt;
    quest_update_availability();
}

/* ---- Public API ----------------------------------------- */

void chain_init(void)
{
    event_listen(EVT_BATTLE_END,      _on_battle_end);
    event_listen(EVT_BATTLE_START,    _on_battle_start);
    event_listen(EVT_WARP,            _on_warp);
    event_listen(EVT_DIALOGUE_START,  _on_dialogue_start);
    event_listen(EVT_ITEM_USED,       _on_item_used);
    event_listen(EVT_SHOP_BUY,        _on_shop_buy);
    event_listen(EVT_LEVEL_UP,        _on_level_up);
}

/* Called from battle system when enemies die in battle */
void chain_on_enemy_killed(u8 enemy_id)
{
    quest_notify(QEVT_ENEMY_KILLED, (u32)enemy_id, 0);
    _monster_hunt_check(enemy_id);
}

/* Called from item_use / inventory when item added */
void chain_on_item_obtained(u8 item_id, u8 qty)
{
    quest_notify(QEVT_ITEM_OBTAINED, (u32)item_id, (u32)qty);
}

/* Called from crafting system after successful craft */
void chain_on_crafted(u8 recipe_id)
{
    quest_notify(QEVT_ITEM_CRAFTED, (u32)recipe_id, 0);
}

/* Called from equip system after item equipped */
void chain_on_equipped(u8 item_id)
{
    quest_notify(QEVT_ITEM_EQUIPPED, (u32)item_id, 0);
}

/* Called by dialogue/scripted events to tick a specific
   quest objective. E.g.: chain_script_flag(QSIDE_LOST_CAT, 1)
   advances the "Find Mochi" objective. */
void chain_script_flag(u8 quest_id, u8 stage)
{
    quest_notify(QEVT_SCRIPT_FLAG, (u32)quest_id, (u32)stage);
}

/* Trigger escort success for a quest */
void chain_escort_success(u8 quest_id)
{
    quest_notify(QEVT_ESCORT_SUCCESS, (u32)quest_id, 0);
}

/* Escort failure: fail the quest */
void chain_escort_fail(u8 quest_id)
{
    quest_fail(quest_id);
}

/* ---- World-flag wrappers (called by story scripts) ------ */

void chain_set_world_flag(u32 flag_bit)
{
    chain_flag_set(flag_bit);
    quest_update_availability();
}

BOOL chain_world_flag(u32 flag_bit)
{
    return chain_flag_get(flag_bit);
}

/* ---- Completion check (call each frame in story scenes) - */

/* Auto-complete quest if all objectives are done and it's ACTIVE.
   Useful after scripted objectives are ticked. */
void chain_try_complete(u8 quest_id)
{
    if (quest_status(quest_id) == QSTAT_ACTIVE &&
        quest_all_objectives_done(quest_id)) {
        quest_complete(quest_id);
    }
}
