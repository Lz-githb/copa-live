#ifndef ENGINE_EVENT_H
#define ENGINE_EVENT_H

#include "../gba/gba_types.h"

/* =========================================================
 * Event System
 *
 * A simple circular queue for decoupled communication
 * between systems. Events are 8 bytes (ID + two params).
 * Listeners register for specific event IDs.
 *
 * Example flow:
 *   collision trigger → push EVT_WARP { map_id, spawn_id }
 *   scene system pops → loads new map
 * ========================================================= */

#define EVENT_QUEUE_SIZE  64
#define EVENT_LISTENER_MAX 32

typedef enum {
    EVT_NONE = 0,

    /* Input */
    EVT_KEYDOWN,       /* param0 = key mask */
    EVT_KEYUP,

    /* Map / world */
    EVT_WARP,          /* param0 = map_id, param1 = spawn_id */
    EVT_TRIGGER,       /* param0 = trigger event_id */
    EVT_DOOR_OPEN,
    EVT_DOOR_CLOSE,

    /* Dialogue */
    EVT_DIALOGUE_START,  /* param0 = script_id */
    EVT_DIALOGUE_END,

    /* Battle */
    EVT_BATTLE_START,    /* param0 = enemy_group_id */
    EVT_BATTLE_END,      /* param0 = result (WIN/FLEE/LOSS) */

    /* Game flow */
    EVT_GAME_SAVE,
    EVT_GAME_LOAD,
    EVT_SCENE_CHANGE,    /* param0 = scene_id */

    /* Audio */
    EVT_BGM_PLAY,        /* param0 = bgm_id */
    EVT_BGM_STOP,
    EVT_SFX_PLAY,        /* param0 = sfx_id */

    /* Character */
    EVT_LEVEL_UP,        /* param0 = party_slot, param1 = new_level */
    EVT_ITEM_USED,       /* param0 = item_id, param1 = target_slot */

    /* Shop / Inventory */
    EVT_SHOP_BUY,        /* param0 = item_id, param1 = qty */
    EVT_SHOP_SELL,       /* param0 = item_id, param1 = qty */

    EVT_COUNT
} EventID;

typedef struct {
    EventID id;
    u32     param0;
    u32     param1;
} Event;

typedef void (*EventHandler)(const Event* evt);

/* ---- Event API ------------------------------------------ */

void event_init(void);

/* Push an event onto the queue */
void event_push(EventID id, u32 param0, u32 param1);

/* Register a listener for a specific event ID */
void event_listen(EventID id, EventHandler handler);

/* Remove a listener */
void event_unlisten(EventID id, EventHandler handler);

/* Dispatch all queued events to listeners (call each frame) */
void event_dispatch_all(void);

/* Peek at the next event without consuming it */
BOOL event_peek(Event* out);

/* Direct pop (bypass dispatch loop) */
BOOL event_pop(Event* out);

/* How many events are pending */
u32 event_count(void);

/* Flush all pending events */
void event_flush(void);

#endif /* ENGINE_EVENT_H */
