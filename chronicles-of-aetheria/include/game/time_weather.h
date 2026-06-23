#ifndef GAME_TIME_WEATHER_H
#define GAME_TIME_WEATHER_H

#include "../engine/engine.h"

/* =========================================================
 * Day/Night Cycle + Weather System
 *
 * TIME SYSTEM
 * -----------
 * Game uses a 24-hour clock. Real time → game time ratio
 * is configurable (TIME_SCALE). Default: 1 real minute = 1 game hour.
 *
 * Time phases affect:
 *   - Palette brightness via REG_BLDY (fade to black at night)
 *   - Palette tint via BG palette blending
 *   - NPC schedules (not yet implemented)
 *   - Random encounter tables
 *
 * GBA brightness hardware:
 *   REG_BLDCNT  (0x04000050) — which layers + direction
 *   REG_BLDY    (0x04000054) — 0..16 fade amount
 *   Direction: BLEND_BLACK (fade to black) for night
 *
 * WEATHER SYSTEM
 * --------------
 * Weather types: CLEAR, OVERCAST, RAIN, STORM, SNOW,
 *                SANDSTORM, FOG, ASH (volcanic area)
 *
 * Each weather has:
 *   - Particle type (rain drops / snow flakes / etc.)
 *   - Particle count (uses OBJ sprites)
 *   - Palette tint modifier
 *   - Brightness modifier
 *   - Lightning chance (STORM only)
 *
 * Weather transitions smoothly over WEATHER_TRANS_FRAMES frames.
 * ========================================================= */

/* ---- Blend registers ------------------------------------- */
#define REG_BLDCNT    (*(vu16*)(0x04000050))
#define REG_BLDALPHA  (*(vu16*)(0x04000052))
#define REG_BLDY      (*(vu16*)(0x04000054))

/* BLDCNT flags */
#define BLD_BG0        0x0001
#define BLD_BG1        0x0002
#define BLD_BG2        0x0004
#define BLD_BG3        0x0008
#define BLD_OBJ        0x0010
#define BLD_BACKDROP   0x0020
#define BLD_OFF        0x0000
#define BLD_STD        0x0040   /* alpha blend */
#define BLD_WHITE      0x0080   /* fade to white */
#define BLD_BLACK      0x00C0   /* fade to black */

/* All layers as blend source */
#define BLD_ALL_LAYERS (BLD_BG0|BLD_BG1|BLD_BG2|BLD_BG3|BLD_OBJ)

/* ---- Time ------------------------------------------------ */

/* Frames per game minute (tune for desired day length)
 * 60 frames/sec × 2 frames/game-minute = 1 real sec = 30 game minutes
 * → full day = 48 real minutes
 * Set to 3 for faster days during testing (1 real min = 1 game hour) */
#define TIME_FRAMES_PER_MINUTE  3
#define TIME_MINUTES_PER_HOUR   60
#define TIME_HOURS_PER_DAY      24
#define TIME_FRAMES_PER_HOUR    (TIME_FRAMES_PER_MINUTE * TIME_MINUTES_PER_HOUR)
#define TIME_FRAMES_PER_DAY     (TIME_FRAMES_PER_HOUR * TIME_HOURS_PER_DAY)

typedef enum {
    PHASE_DAWN = 0,    /*  5:00 -  7:00 */
    PHASE_MORNING,     /*  7:00 - 11:00 */
    PHASE_NOON,        /* 11:00 - 14:00 */
    PHASE_AFTERNOON,   /* 14:00 - 17:00 */
    PHASE_DUSK,        /* 17:00 - 19:00 */
    PHASE_NIGHT,       /* 19:00 -  5:00 */
    PHASE_COUNT
} TimePhase;

typedef struct {
    u32       total_frames;     /* total game frames elapsed */
    u8        hour;             /* 0-23 */
    u8        minute;           /* 0-59 */
    TimePhase phase;
    u8        brightness;       /* 0-16 (REG_BLDY value, 0=no fade) */
    /* Tint: we add a small color offset to sky-layer palette */
    COLOR     tint;             /* BGR555 tint color */
    u8        tint_blend;       /* 0-16 blend amount */
} GameTime;

/* ---- Weather -------------------------------------------- */

typedef enum {
    WEATHER_CLEAR = 0,
    WEATHER_OVERCAST,
    WEATHER_RAIN,
    WEATHER_HEAVY_RAIN,
    WEATHER_STORM,        /* rain + lightning */
    WEATHER_SNOW,
    WEATHER_SANDSTORM,
    WEATHER_FOG,
    WEATHER_ASH,          /* volcanic ash (downward) */
    WEATHER_COUNT
} WeatherType;

#define WEATHER_PARTICLE_MAX   24   /* max OBJ sprites for weather */
#define WEATHER_TRANS_FRAMES   60   /* frames for weather transition */

/* Per-particle state */
typedef struct {
    s16          x, y;      /* screen position */
    s16          vx, vy;    /* velocity (fixed 4.4) */
    SpriteHandle spr;
    BOOL         active;
} WeatherParticle;

typedef struct {
    WeatherType     current;
    WeatherType     target;         /* transitioning toward */
    u8              trans_timer;    /* 0..WEATHER_TRANS_FRAMES */
    u8              particle_count;
    WeatherParticle particles[WEATHER_PARTICLE_MAX];

    /* Lightning (STORM) */
    u32 lightning_timer;
    u8  lightning_phase;    /* 0=idle, 1=flash, 2=fade */
    u8  lightning_bright;

    /* Fog/sandstorm overlay */
    u8  overlay_alpha;
} WeatherState;

/* ---- Time/Weather API ----------------------------------- */

void time_weather_init(u8 start_hour, u8 start_minute,
                       WeatherType start_weather);

/* Advance time and weather (call each frame, only for outdoor maps) */
void time_weather_update(BOOL outdoor);

/* Apply day/night brightness to hardware blend registers */
void time_weather_apply_hw(void);

/* Transition to a new weather type */
void weather_set(WeatherType type);

/* Instant set (no transition, e.g., entering a building) */
void weather_set_instant(WeatherType type);

/* Accessors */
TimePhase   time_get_phase(void);
u8          time_get_hour(void);
u8          time_get_minute(void);
WeatherType weather_get_current(void);

/* Screen flash for lightning (call from render) */
void weather_render_flash(void);

/* Debug: advance time by N hours */
void time_advance_hours(u8 hours);

/* External state access */
extern GameTime    g_time;
extern WeatherState g_weather;

#endif /* GAME_TIME_WEATHER_H */
