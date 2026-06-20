#include "../include/engine/engine.h"
#include "../include/game/game_states.h"

/* =========================================================
 * Chronicles of Aetheria — Entry Point & Engine Core
 *
 * Execution flow:
 *   main()
 *     engine_init()       — hardware + subsystems
 *     game_register_states()
 *     state_change(TITLE) — queue initial state
 *     loop:
 *       VBlankIntrWait()  — sync to 60 Hz
 *       engine_vblank()   — OAM flush, input snapshot
 *       engine_tick()     — state update + render
 * ========================================================= */

/* ---- Global variables ------------------------------------ */
InputState g_input  = {0, 0, 0};
u32        g_frame  = 0;

/* ---- VBlank ISR ----------------------------------------- */

/* Placed in IWRAM for fastest interrupt response */
void IWRAM_CODE engine_vblank(void) {
    /* 1. Flush shadow OAM to hardware */
    sprite_flush();

    /* 2. Audio: refill DMA buffer */
    audio_vblank_handler();

    /* 3. Snapshot input (stable for this frame) */
    input_update();
}

/* IRQ handler — registered in BIOS interrupt vector */
void engine_isr(void) {
    u16 fired = REG_IF & REG_IE;

    if (fired & IRQ_VBLANK) {
        engine_vblank();
        REG_IF = IRQ_VBLANK;   /* acknowledge */
    }

    /* Acknowledge all handled IRQs */
    REG_IF = fired;
}

/* ---- Engine Init ---------------------------------------- */

void engine_init(void) {
    /* Disable display while initializing */
    REG_DISPCNT = DCNT_BLANK;

    /* Disable all IRQs during setup */
    REG_IME = 0;

    /* Initialize subsystems (order matters) */
    mem_init();
    event_init();
    tile_init();
    sprite_init();
    audio_init();
    save_init();
    scene_init();

    /* Register VBlank ISR */
    IRQ_HANDLER = engine_isr;
    REG_DISPSTAT = DSTAT_VBL_IRQ;
    REG_IE  = IRQ_VBLANK;
    REG_IME = 1;

    /* Clear screen */
    REG_DISPCNT = DCNT_MODE0 | DCNT_OBJ_MAP_1D;
}

/* ---- Engine Tick ---------------------------------------- */

void engine_tick(void) {
    /* Update active game state (logic) */
    state_tick();

    /* Dispatch queued events */
    event_dispatch_all();

    /* Render active state */
    state_render();

    /* Reset frame pool (temporary allocations expire) */
    mem_frame_reset();

    g_frame++;
}

/* ---- Entry Point ---------------------------------------- */

int main(void) {
    engine_init();

    /* Register game content */
    game_register_states();

    /* Start at title screen */
    state_change(STATE_TITLE);

    /* Main loop: runs forever at 60 Hz */
    while (1) {
        VBlankIntrWait();   /* suspend CPU until VBlank */
        engine_tick();
    }

    /* Unreachable */
    return 0;
}
