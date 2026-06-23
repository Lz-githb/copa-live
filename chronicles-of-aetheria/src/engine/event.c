#include "../../include/engine/event.h"
#include "../../include/gba/gba_bios.h"

/* =========================================================
 * Event System Implementation — Circular Queue
 * ========================================================= */

/* Listener table: one list per event ID */
#define MAX_LISTENERS_PER_EVENT 4

typedef struct {
    EventHandler handlers[MAX_LISTENERS_PER_EVENT];
    u32          count;
} ListenerList;

static Event        s_queue[EVENT_QUEUE_SIZE];
static u32          s_head;   /* read  index */
static u32          s_tail;   /* write index */
static u32          s_count;

static ListenerList s_listeners[EVT_COUNT];

void event_init(void) {
    s_head  = 0;
    s_tail  = 0;
    s_count = 0;
    mem_zero(s_queue,     sizeof(s_queue));
    mem_zero(s_listeners, sizeof(s_listeners));
}

void event_push(EventID id, u32 param0, u32 param1) {
    if (s_count >= EVENT_QUEUE_SIZE) return;   /* queue full, drop */

    s_queue[s_tail].id     = id;
    s_queue[s_tail].param0 = param0;
    s_queue[s_tail].param1 = param1;
    s_tail  = (s_tail + 1) % EVENT_QUEUE_SIZE;
    s_count++;
}

BOOL event_peek(Event* out) {
    if (s_count == 0) return FALSE;
    *out = s_queue[s_head];
    return TRUE;
}

BOOL event_pop(Event* out) {
    if (s_count == 0) return FALSE;
    *out   = s_queue[s_head];
    s_head = (s_head + 1) % EVENT_QUEUE_SIZE;
    s_count--;
    return TRUE;
}

void event_dispatch_all(void) {
    Event evt;
    while (event_pop(&evt)) {
        if (evt.id >= EVT_COUNT) continue;
        ListenerList* list = &s_listeners[evt.id];
        u32 i;
        for (i = 0; i < list->count; i++) {
            if (list->handlers[i]) {
                list->handlers[i](&evt);
            }
        }
    }
}

void event_listen(EventID id, EventHandler handler) {
    if (id >= EVT_COUNT || !handler) return;
    ListenerList* list = &s_listeners[id];
    if (list->count >= MAX_LISTENERS_PER_EVENT) return;
    list->handlers[list->count++] = handler;
}

void event_unlisten(EventID id, EventHandler handler) {
    if (id >= EVT_COUNT || !handler) return;
    ListenerList* list = &s_listeners[id];
    u32 i;
    for (i = 0; i < list->count; i++) {
        if (list->handlers[i] == handler) {
            /* Shift remaining handlers down */
            u32 j;
            for (j = i; j < list->count - 1; j++) {
                list->handlers[j] = list->handlers[j + 1];
            }
            list->handlers[--list->count] = NULL;
            return;
        }
    }
}

u32 event_count(void) { return s_count; }

void event_flush(void) {
    s_head  = 0;
    s_tail  = 0;
    s_count = 0;
}
