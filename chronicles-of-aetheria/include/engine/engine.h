#ifndef ENGINE_H
#define ENGINE_H

/* =========================================================
 * Engine Master Header
 * Include this single header to access all engine systems
 * ========================================================= */

#include "../gba/gba_types.h"
#include "../gba/gba_reg.h"
#include "../gba/gba_bios.h"
#include "../gba/gba_dma.h"

#include "memory.h"
#include "state_machine.h"
#include "scene.h"
#include "sprite.h"
#include "tile.h"
#include "camera.h"
#include "collision.h"
#include "event.h"
#include "audio.h"
#include "save.h"

/* =========================================================
 * Input Manager
 * Maintained here (small enough to live in engine.h)
 * ========================================================= */

typedef struct {
    u16 held;       /* keys currently held */
    u16 pressed;    /* keys newly pressed this frame */
    u16 released;   /* keys released this frame */
} InputState;

extern InputState g_input;

INLINE void input_update(void) {
    u16 raw    = (~REG_KEYINPUT) & KEY_MASK;
    g_input.pressed  = raw & ~g_input.held;
    g_input.released = g_input.held & ~raw;
    g_input.held     = raw;
}

INLINE BOOL key_held(u16 mask)     { return (g_input.held    & mask) != 0; }
INLINE BOOL key_pressed(u16 mask)  { return (g_input.pressed & mask) != 0; }
INLINE BOOL key_released(u16 mask) { return (g_input.released& mask) != 0; }

/* =========================================================
 * Frame Counter
 * ========================================================= */
extern u32 g_frame;

/* =========================================================
 * Engine Lifecycle
 * ========================================================= */

/* Initialize all engine subsystems (call once at startup) */
void engine_init(void);

/* Called every VBlank — copy shadow OAM, update input */
void engine_vblank(void) IWRAM_CODE;

/* Run one game frame */
void engine_tick(void);

/* VBlank ISR registered with BIOS */
void engine_isr(void) __attribute__((interrupt("IRQ")));

#endif /* ENGINE_H */
