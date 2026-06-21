/*
 * polish.c — Stage 15 polish / optimisation pass
 * Chronicles of Aetheria GBA RPG
 *
 * Features implemented:
 *   1. Screen flash effect   (hit / heal / level-up)
 *   2. Camera shake          (X/Y oscillation for N frames)
 *   3. Fade to/from black    (state transitions)
 *   4. Palette cycling       (water tiles — palette slots 4 and 10)
 *   5. Footstep throttle     (SFX_STEP every 8 frames while moving)
 *   6. FPS stabilizer        (skip render if update ran long)
 */

#include "../../include/engine/engine.h"
#include "../../include/gba/gba_reg.h"
#include "../../include/gba/gba_types.h"
#include "../../include/game/polish.h"

/* -----------------------------------------------------------------------
 * GBA blend / mosaic registers not already in gba_reg.h
 * ----------------------------------------------------------------------- */
#define REG_BLDCNT_P  (*(vu16*)(0x04000050))
#define REG_BLDALPHA  (*(vu16*)(0x04000052))
#define REG_BLDY_POL  (*(vu16*)(0x04000054))
#define REG_MOSAIC    (*(vu16*)(0x0400004C))

/* Palette RAM base — background palette bank 0 */
#define PAL_BG_RAM    ((vu16*)(0x05000000))

/* GBA timer / VCount register for the FPS stabilizer */
#define REG_VCOUNT    (*(vu16*)(0x04000006))

/* -----------------------------------------------------------------------
 * Internal limits
 * ----------------------------------------------------------------------- */
#define FADE_MAX        16   /* maximum blend level (full black)           */
#define SHAKE_PERIOD     4   /* frames per half-period of oscillation      */
#define PAL_CYCLE_INTERVAL 8 /* frames between water palette cycle steps   */
#define FOOTSTEP_PERIOD  8   /* frames between audible footstep SFX        */

/* VCOUNT threshold: if we are past scanline 155 when we start rendering
 * (meaning the update loop ate into the draw budget) skip this frame.     */
#define VCOUNT_SKIP_LINE 155

/* -----------------------------------------------------------------------
 * Screen-flash state
 * ----------------------------------------------------------------------- */
static u8 s_flash_timer;
static u8 s_flash_duration;
static u8 s_flash_color; /* 0=white, 1=red, 2=green */

/* -----------------------------------------------------------------------
 * Camera-shake state
 * ----------------------------------------------------------------------- */
static s8 s_shake_x;
static s8 s_shake_y;
static u8 s_shake_timer;
static u8 s_shake_intensity;

/* -----------------------------------------------------------------------
 * Fade state
 * ----------------------------------------------------------------------- */
typedef enum { FADE_NONE = 0, FADE_OUT, FADE_IN } FadeDir;

static FadeDir  s_fade_dir;
static u8       s_fade_level;   /* 0 = clear, 16 = full black */
static u8       s_fade_speed;   /* blend levels to advance per frame */
static void   (*s_fade_cb)(void); /* callback fired when fade completes */

/* -----------------------------------------------------------------------
 * Palette cycling state (water tiles)
 * ----------------------------------------------------------------------- */
static u8 s_pal_cycle_timer;
static u8 s_pal_cycle_phase;

/* Water colour table — four-phase cycle for palette slot index 4 and 10
 * in the overworld background palette (palette bank 0, slots 4 and 10).
 * Each row = {colour for slot 4, colour for slot 10}.
 * Colours are GBA 15-bit BGR (0bBBBBBGGGGGRRRRR).                        */
static const u16 k_water_colors[4][2] = {
    { 0x7B1F, 0x6EDD },   /* phase 0 — deep blue / mid blue   */
    { 0x6EDD, 0x621B },   /* phase 1 — mid  blue / light blue */
    { 0x621B, 0x55D8 },   /* phase 2 — lighter                */
    { 0x55D8, 0x7B1F },   /* phase 3 — wrap back              */
};

/* -----------------------------------------------------------------------
 * Footstep throttle state
 * ----------------------------------------------------------------------- */
static u8 s_footstep_counter;

/* -----------------------------------------------------------------------
 * FPS-stabilizer state
 * ----------------------------------------------------------------------- */
static BOOL s_skip_render_flag;

/* =======================================================================
 * polish_init
 * ======================================================================= */
void polish_init(void)
{
    s_flash_timer     = 0;
    s_flash_duration  = 0;
    s_flash_color     = FLASH_WHITE;

    s_shake_x         = 0;
    s_shake_y         = 0;
    s_shake_timer     = 0;
    s_shake_intensity = 0;

    s_fade_dir        = FADE_NONE;
    s_fade_level      = 0;
    s_fade_speed      = 1;
    s_fade_cb         = 0;

    s_pal_cycle_timer = 0;
    s_pal_cycle_phase = 0;

    s_footstep_counter = 0;
    s_skip_render_flag = FALSE;

    /*
     * Configure the GBA colour-special effect unit:
     *   REG_BLDCNT_P bits [5:0]  = first target layers (all set = 0x3F)
     *   REG_BLDCNT_P bits [13:8] = second target layers (all set = 0x3F)
     *   bits [7:6] = effect mode; we set 0b10 (brightness decrease) for
     *   the fade-to-black path.  Flash overrides via mode 0b11 (increase).
     *   Initial mode = none (0b00); each subsystem switches as needed.
     *
     *   0x00FF = all BG + OBJ layers as first target, mode = none initially.
     */
    REG_BLDCNT_P = 0x00FF;
    REG_BLDY_POL = 0;
    REG_BLDALPHA = 0;
}

/* =======================================================================
 * Screen flash
 * ======================================================================= */

void polish_flash(u8 color, u8 duration)
{
    s_flash_color    = color;
    s_flash_duration = duration;
    s_flash_timer    = duration;
}

void polish_flash_update(void)
{
    u8 blend_level;

    if (s_flash_timer == 0)
    {
        /* Ensure blend is cleared when idle */
        REG_BLDCNT_P = 0x00FF; /* mode = none; keep targets for fade */
        REG_BLDY_POL = 0;
        return;
    }

    /* Compute brightness increase from 0 (no flash) to 16 (full) and back.
     * We ramp up for the first half of the duration, then ramp down.       */
    if (s_flash_timer > (s_flash_duration >> 1))
    {
        /* Ramp up */
        u8 half = s_flash_duration >> 1;
        u8 t    = s_flash_duration - s_flash_timer; /* 0..half */
        blend_level = (u8)((t * 16) / (half ? half : 1));
    }
    else
    {
        /* Ramp down */
        blend_level = (u8)((s_flash_timer * 16) / (s_flash_duration ? s_flash_duration >> 1 : 1));
    }
    if (blend_level > 16) blend_level = 16;

    /* Flash colour determines what we blend toward:
     *   white  => brightness increase (mode 11)
     *   red    => we tint by manipulating palette directly (simplified:
     *              use brightness increase mode, let flash_color be a hint
     *              the sound/battle system uses to tint specific sprites)
     *   green  => same simplification — effect is visual enough in practice */
    (void)s_flash_color; /* colour hint used by calling code for SFX choice */

    /* Brightness INCREASE for flash — bits [7:6] = 0b11 = 3 */
    REG_BLDCNT_P = (u16)(0x00FF | (3 << 6));
    REG_BLDY_POL = (u16)blend_level;

    s_flash_timer--;
}

/* =======================================================================
 * Camera shake
 * ======================================================================= */

void polish_shake(u8 intensity, u8 duration)
{
    s_shake_intensity = intensity;
    s_shake_timer     = duration;
    s_shake_x         = 0;
    s_shake_y         = 0;
}

void polish_shake_update(void)
{
    s8 sign;
    u8 half_period;

    if (s_shake_timer == 0)
    {
        s_shake_x = 0;
        s_shake_y = 0;
        return;
    }

    /* Oscillate: sign alternates every SHAKE_PERIOD frames */
    half_period = SHAKE_PERIOD;
    sign = ((s_shake_timer / half_period) & 1) ? 1 : -1;

    /* Intensity decays linearly over the shake duration */
    s_shake_x = (s8)(sign * (s8)s_shake_intensity);
    s_shake_y = (s8)(-sign * (s8)(s_shake_intensity >> 1));

    s_shake_timer--;

    /* Fade intensity as timer winds down (simple linear decay) */
    if (s_shake_intensity > 0 && (s_shake_timer % 4) == 0)
        s_shake_intensity--;
}

s8 polish_shake_x(void)
{
    return s_shake_x;
}

s8 polish_shake_y(void)
{
    return s_shake_y;
}

/* =======================================================================
 * Fade to / from black
 * ======================================================================= */

void polish_fade_out(u8 speed, void (*on_done)(void))
{
    s_fade_dir   = FADE_OUT;
    s_fade_speed = speed ? speed : 1;
    s_fade_cb    = on_done;
    /* Do not reset s_fade_level; allow chaining from a partial fade */
}

void polish_fade_in(u8 speed, void (*on_done)(void))
{
    s_fade_dir   = FADE_IN;
    s_fade_speed = speed ? speed : 1;
    s_fade_cb    = on_done;
}

void polish_fade_update(void)
{
    if (s_fade_dir == FADE_NONE)
        return;

    /* Switch blend mode to BRIGHTNESS DECREASE (mode 0b10) for fade-to-black.
     * All layers as first target; no second target needed for this mode.    */
    REG_BLDCNT_P = (u16)(0x00FF | (2 << 6));

    if (s_fade_dir == FADE_OUT)
    {
        if (s_fade_level < FADE_MAX)
        {
            s_fade_level += s_fade_speed;
            if (s_fade_level >= FADE_MAX)
            {
                s_fade_level = FADE_MAX;
                s_fade_dir   = FADE_NONE;
                if (s_fade_cb) { s_fade_cb(); s_fade_cb = 0; }
            }
        }
    }
    else /* FADE_IN */
    {
        if (s_fade_level > 0)
        {
            if (s_fade_level >= s_fade_speed)
                s_fade_level -= s_fade_speed;
            else
                s_fade_level = 0;
        }
        if (s_fade_level == 0)
        {
            s_fade_dir = FADE_NONE;
            /* Restore blend to none once fully visible */
            REG_BLDCNT_P = 0x00FF;
            REG_BLDY_POL = 0;
            if (s_fade_cb) { s_fade_cb(); s_fade_cb = 0; }
            return;
        }
    }

    REG_BLDY_POL = (u16)s_fade_level;
}

BOOL polish_fade_done(void)
{
    return (s_fade_dir == FADE_NONE) ? TRUE : FALSE;
}

/* =======================================================================
 * Palette cycling — water tiles (overworld palette slots 4 and 10)
 * ======================================================================= */

void polish_pal_cycle_update(void)
{
    if (++s_pal_cycle_timer < PAL_CYCLE_INTERVAL)
        return;

    s_pal_cycle_timer = 0;
    s_pal_cycle_phase = (u8)((s_pal_cycle_phase + 1) & 3);

    /* Write new colours into BG palette bank 0, slots 4 and 10.
     * PAL_BG_RAM[n] is the nth 16-bit colour entry in the BG palette.     */
    PAL_BG_RAM[4]  = k_water_colors[s_pal_cycle_phase][0];
    PAL_BG_RAM[10] = k_water_colors[s_pal_cycle_phase][1];
}

/* =======================================================================
 * Footstep throttle
 * ======================================================================= */

BOOL polish_footstep_tick(void)
{
    s_footstep_counter++;
    if (s_footstep_counter >= FOOTSTEP_PERIOD)
    {
        s_footstep_counter = 0;
        return TRUE;
    }
    return FALSE;
}

/* =======================================================================
 * FPS stabilizer — basic frame-skip guard
 * ======================================================================= */

/*
 * Call polish_fps_begin() at the start of the update phase and
 * polish_should_skip_render() before the draw phase.
 *
 * If the update logic consumed so much CPU time that we are past
 * VCOUNT_SKIP_LINE by the time we would start rendering, we set the
 * skip flag so the caller can omit the draw call entirely.  This keeps
 * the game logic running at a consistent rate even on heavy frames.
 */
void polish_fps_begin(void)
{
    s_skip_render_flag = FALSE;
}

BOOL polish_should_skip_render(void)
{
    if (REG_VCOUNT > VCOUNT_SKIP_LINE)
    {
        s_skip_render_flag = TRUE;
    }
    return s_skip_render_flag;
}

/* =======================================================================
 * Master update — call once per frame before rendering
 * ======================================================================= */

void polish_update(void)
{
    polish_flash_update();
    polish_shake_update();
    polish_fade_update();
    polish_pal_cycle_update();
    /* Note: polish_footstep_tick() is called by the movement system,
     *       and polish_fps_begin() / polish_should_skip_render() are
     *       called by the main loop directly around the render call.       */
}
