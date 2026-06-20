#include "../../include/game/time_weather.h"
#include "../../include/engine/engine.h"

/* =========================================================
 * Day/Night + Weather System Implementation
 * ========================================================= */

GameTime    g_time;
WeatherState g_weather;

/* ---- Time phase boundaries (in game hours) -------------- */
typedef struct { u8 start; u8 end; } HourRange;
static const HourRange PHASE_HOURS[PHASE_COUNT] = {
    { 5,  7  },   /* DAWN      */
    { 7,  11 },   /* MORNING   */
    { 11, 14 },   /* NOON      */
    { 14, 17 },   /* AFTERNOON */
    { 17, 19 },   /* DUSK      */
    { 19, 5  },   /* NIGHT (wraps midnight) */
};

/* Brightness level (REG_BLDY, 0=full bright, 16=black) per phase */
static const u8 PHASE_BRIGHTNESS[PHASE_COUNT] = {
    4,   /* DAWN      — slightly dark */
    0,   /* MORNING   — full bright */
    0,   /* NOON      — full bright */
    0,   /* AFTERNOON — full bright */
    3,   /* DUSK      — slight dimming */
    10,  /* NIGHT     — quite dark */
};

/* Tint colors per phase (used to shift sky layer palette) */
static const COLOR PHASE_TINT[PHASE_COUNT] = {
    RGB15(28, 18,  8),   /* DAWN:      warm orange */
    RGB15(31, 31, 31),   /* MORNING:   white/neutral */
    RGB15(31, 29, 22),   /* NOON:      slight warm */
    RGB15(30, 28, 24),   /* AFTERNOON: neutral warm */
    RGB15(28, 14,  6),   /* DUSK:      orange-red */
    RGB15( 4,  6, 16),   /* NIGHT:     deep blue */
};

/* Tint blend strength per phase (0-16) */
static const u8 PHASE_TINT_BLEND[PHASE_COUNT] = {
    6, 0, 2, 2, 8, 12
};

/* ---- Weather particle tile base -------------------------
 * These tile IDs in OBJ VRAM:
 *   RAIN:  single 1x4 vertical line sprite (1x8 pixels)
 *   SNOW:  single 2x2 dot sprite
 *   ASH:   diagonal dot
 * (loaded by asset system when weather is enabled) */
#define WEATHER_TILE_RAIN   508
#define WEATHER_TILE_SNOW   509
#define WEATHER_TILE_ASH    510
#define WEATHER_TILE_SAND   511

/* Particle counts per weather type */
static const u8 WEATHER_PARTICLE_COUNTS[WEATHER_COUNT] = {
    0,   /* CLEAR */
    0,   /* OVERCAST */
    12,  /* RAIN */
    20,  /* HEAVY_RAIN */
    20,  /* STORM */
    10,  /* SNOW */
    16,  /* SANDSTORM */
    0,   /* FOG (palette-only) */
    8,   /* ASH */
};

/* Particle velocities (vx, vy in pixels per frame, signed) */
static const s8 WEATHER_VX[WEATHER_COUNT] = {
    0, 0, -1, -2, -3, -1, -4, 0, -1
};
static const s8 WEATHER_VY[WEATHER_COUNT] = {
    0, 0,  4,  5,  5,  1,  0, 0,  2
};

/* ---- LCG for particle spawn positions ------------------- */
static u32 s_prng = 0xDEAD1234;
static u8 _rng8(void) {
    s_prng = s_prng * 22695477u + 1;
    return (u8)(s_prng >> 16);
}

/* ---- Helpers -------------------------------------------- */

static TimePhase _calc_phase(u8 hour) {
    u8 p;
    for (p = 0; p < PHASE_COUNT - 1; p++) {
        if (hour >= PHASE_HOURS[p].start && hour < PHASE_HOURS[p].end)
            return (TimePhase)p;
    }
    return PHASE_NIGHT;  /* wraps: 19-23 and 0-4 */
}

/* Lerp between two brightness values using sub-phase position */
static u8 _lerp8(u8 a, u8 b, u8 t, u8 max) {
    return (u8)(a + ((s32)(b - a) * t) / max);
}

static void _particle_init(WeatherParticle* p, WeatherType wt) {
    p->x   = (s16)(_rng8() % SCREEN_W);
    p->y   = (s16)(_rng8() % SCREEN_H);
    p->vx  = WEATHER_VX[wt];
    p->vy  = WEATHER_VY[wt];
    /* Scatter initial y so rain doesn't all start at once */
    if (wt == WEATHER_RAIN || wt == WEATHER_HEAVY_RAIN ||
        wt == WEATHER_STORM) {
        p->y = (s16)(_rng8() % SCREEN_H);
    }
    p->active = TRUE;
}

static void _init_particles(WeatherType wt) {
    u8 count = WEATHER_PARTICLE_COUNTS[wt];
    if (count > WEATHER_PARTICLE_MAX) count = WEATHER_PARTICLE_MAX;

    /* Free any existing particle sprites */
    u8 i;
    for (i = 0; i < WEATHER_PARTICLE_MAX; i++) {
        if (g_weather.particles[i].spr != INVALID_SPRITE) {
            sprite_free(g_weather.particles[i].spr);
            g_weather.particles[i].spr    = INVALID_SPRITE;
            g_weather.particles[i].active = FALSE;
        }
    }

    g_weather.particle_count = count;

    for (i = 0; i < count; i++) {
        WeatherParticle* p = &g_weather.particles[i];
        _particle_init(p, wt);
        p->spr = sprite_alloc();

        /* Choose tile based on weather */
        u16 tile = WEATHER_TILE_RAIN;
        if (wt == WEATHER_SNOW)                        tile = WEATHER_TILE_SNOW;
        else if (wt == WEATHER_ASH)                    tile = WEATHER_TILE_ASH;
        else if (wt == WEATHER_SANDSTORM)              tile = WEATHER_TILE_SAND;

        sprite_set_gfx(p->spr, tile, 15, SPR_SIZE_8x8);
        sprite_set_priority(p->spr, 0);
        sprite_set_pos(p->spr, p->x, p->y);
    }
}

/* ---- Public API ----------------------------------------- */

void time_weather_init(u8 start_hour, u8 start_minute,
                       WeatherType start_weather) {
    mem_zero(&g_time,    sizeof(GameTime));
    mem_zero(&g_weather, sizeof(WeatherState));

    g_time.hour   = start_hour;
    g_time.minute = start_minute;
    g_time.total_frames = ((u32)start_hour * TIME_FRAMES_PER_HOUR) +
                          ((u32)start_minute * TIME_FRAMES_PER_MINUTE);
    g_time.phase  = _calc_phase(start_hour);

    /* Pre-compute initial brightness */
    g_time.brightness = PHASE_BRIGHTNESS[g_time.phase];
    g_time.tint       = PHASE_TINT[g_time.phase];
    g_time.tint_blend = PHASE_TINT_BLEND[g_time.phase];

    /* Init particle handles to invalid */
    u8 i;
    for (i = 0; i < WEATHER_PARTICLE_MAX; i++) {
        g_weather.particles[i].spr    = INVALID_SPRITE;
        g_weather.particles[i].active = FALSE;
    }

    g_weather.current = start_weather;
    g_weather.target  = start_weather;
    g_weather.trans_timer = 0;

    if (start_weather != WEATHER_CLEAR && start_weather != WEATHER_FOG &&
        start_weather != WEATHER_OVERCAST) {
        _init_particles(start_weather);
    }
}

void time_weather_update(BOOL outdoor) {
    /* ---- Advance time ---- */
    g_time.total_frames++;
    if (g_time.total_frames >= (u32)TIME_FRAMES_PER_DAY)
        g_time.total_frames = 0;

    /* Derive hour and minute from frame counter */
    u32 f        = g_time.total_frames;
    g_time.hour  = (u8)((f / TIME_FRAMES_PER_HOUR) % 24);
    g_time.minute= (u8)((f % TIME_FRAMES_PER_HOUR) / TIME_FRAMES_PER_MINUTE);

    /* Update phase */
    TimePhase new_phase = _calc_phase(g_time.hour);
    if (new_phase != g_time.phase) {
        g_time.phase = new_phase;
    }

    /* Smoothly interpolate brightness within phase */
    if (outdoor) {
        /* Lerp toward target brightness over 64 frames */
        u8 target_br = PHASE_BRIGHTNESS[g_time.phase];
        if (g_time.brightness < target_br) g_time.brightness++;
        else if (g_time.brightness > target_br) g_time.brightness--;

        g_time.tint       = PHASE_TINT[g_time.phase];
        g_time.tint_blend = PHASE_TINT_BLEND[g_time.phase];
    } else {
        /* Indoor: no day/night effect */
        g_time.brightness = 0;
        g_time.tint_blend = 0;
    }

    /* ---- Weather transition ---- */
    if (g_weather.current != g_weather.target) {
        g_weather.trans_timer++;
        if (g_weather.trans_timer >= WEATHER_TRANS_FRAMES) {
            g_weather.current = g_weather.target;
            g_weather.trans_timer = 0;
            _init_particles(g_weather.current);
        }
    }

    /* ---- Update weather particles ---- */
    WeatherType wt = g_weather.current;
    u8 i;
    for (i = 0; i < g_weather.particle_count; i++) {
        WeatherParticle* p = &g_weather.particles[i];
        if (!p->active) continue;

        p->x += p->vx;
        p->y += p->vy;

        /* Wrap particles around screen */
        if (p->y >= SCREEN_H) p->y = -8;
        if (p->y < -8)        p->y = (s16)(SCREEN_H - 1);
        if (p->x < -8)        p->x = (s16)(SCREEN_W + 7);
        if (p->x >= SCREEN_W + 8) p->x = 0;

        /* Add slight wobble to snow/ash */
        if (wt == WEATHER_SNOW || wt == WEATHER_ASH) {
            /* Slow oscillation using frame counter */
            s16 wobble = (s16)((g_frame + i * 17) & 0x3F);
            if (wobble >= 32) wobble = 64 - wobble;
            p->x += (wobble - 16) >> 4;
        }

        if (p->spr != INVALID_SPRITE) {
            sprite_set_pos(p->spr, p->x, p->y);
        }
    }

    /* ---- Lightning (STORM) ---- */
    if (wt == WEATHER_STORM) {
        g_weather.lightning_timer++;
        switch (g_weather.lightning_phase) {
        case 0: /* idle: random interval */
            if (g_weather.lightning_timer >= 180 + (_rng8() & 0xFF)) {
                g_weather.lightning_timer  = 0;
                g_weather.lightning_phase  = 1;
                g_weather.lightning_bright = 16;
            }
            break;
        case 1: /* flash: fade from white quickly */
            if (g_weather.lightning_bright > 0)
                g_weather.lightning_bright -= 2;
            else {
                g_weather.lightning_phase = 0;
                g_weather.lightning_timer = 0;
            }
            break;
        }
    } else {
        g_weather.lightning_phase  = 0;
        g_weather.lightning_bright = 0;
    }
}

void time_weather_apply_hw(void) {
    /* Apply day/night brightness via hardware fade-to-black */
    if (g_time.brightness > 0) {
        REG_BLDCNT = BLD_ALL_LAYERS | BLD_BLACK;
        REG_BLDY   = g_time.brightness;
    } else {
        /* No time-based fade; let weather control blending if needed */
        WeatherType wt = g_weather.current;
        if (wt == WEATHER_FOG || wt == WEATHER_OVERCAST) {
            /* Slight brightness reduction */
            REG_BLDCNT = BLD_ALL_LAYERS | BLD_BLACK;
            REG_BLDY   = (wt == WEATHER_FOG) ? 3 : 2;
        } else if (g_weather.lightning_bright > 0) {
            /* Lightning: flash to white */
            REG_BLDCNT = BLD_ALL_LAYERS | BLD_WHITE;
            REG_BLDY   = g_weather.lightning_bright;
        } else {
            REG_BLDCNT = 0;
            REG_BLDY   = 0;
        }
    }
}

void weather_set(WeatherType type) {
    if (type == g_weather.current && type == g_weather.target) return;
    g_weather.target      = type;
    g_weather.trans_timer = 0;
}

void weather_set_instant(WeatherType type) {
    g_weather.target  = type;
    g_weather.current = type;
    g_weather.trans_timer = 0;
    _init_particles(type);
}

TimePhase   time_get_phase(void)         { return g_time.phase; }
u8          time_get_hour(void)          { return g_time.hour;  }
u8          time_get_minute(void)        { return g_time.minute; }
WeatherType weather_get_current(void)    { return g_weather.current; }

void weather_render_flash(void) {
    /* Lightning flash: we handle this in apply_hw */
    (void)0;
}

void time_advance_hours(u8 hours) {
    g_time.total_frames += (u32)hours * TIME_FRAMES_PER_HOUR;
    if (g_time.total_frames >= (u32)TIME_FRAMES_PER_DAY)
        g_time.total_frames -= (u32)TIME_FRAMES_PER_DAY;
}
