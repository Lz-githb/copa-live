/* =========================================================
 * audio_data.c — Chronicles of Aetheria
 *
 * Music tracks and SFX tables.
 *
 * GBA audio approach:
 *   DirectSound channels A & B feed 8-bit signed PCM.
 *   Timer 0 controls the sample rate (16364 Hz typical).
 *   Music is stored as minimal pattern data; in production
 *   replace s_empty_pattern with a converted MOD/XM export.
 *   SFX are short square-wave bursts at varying pitches.
 * ========================================================= */

#include "../../include/gba/gba_types.h"

/* ---- Music track IDs ------------------------------------ */
#define MUS_TITLE       0
#define MUS_OVERWORLD   1
#define MUS_TOWN        2
#define MUS_DUNGEON     3
#define MUS_BATTLE      4
#define MUS_BOSS        5
#define MUS_VICTORY     6
#define MUS_GAMEOVER    7
#define MUS_CASTLE      8
#define MUS_SHADOW      9
#define MUS_COUNT      10

/* ---- SFX IDs ------------------------------------------- */
#define SFX_MENU_CURSOR   0
#define SFX_MENU_CONFIRM  1
#define SFX_MENU_CANCEL   2
#define SFX_ATTACK        3
#define SFX_MAGIC         4
#define SFX_HIT           5
#define SFX_HEAL          6
#define SFX_LEVELUP       7
#define SFX_CHEST         8
#define SFX_DOOR          9
#define SFX_STEP         10
#define SFX_WARP         11
#define SFX_BOSS_ROAR    12
#define SFX_VICTORY      13
#define SFX_GAMEOVER     14
#define SFX_COUNT        15

/* ---- PCM sample data (signed 8-bit, 8 kHz) ------------- */

/* Square wave at ~1 kHz (8 samples on + 8 off) */
static const s8 s_sq_hi[32] = {
     80, 80, 80, 80, 80, 80, 80, 80,
    -80,-80,-80,-80,-80,-80,-80,-80,
     80, 80, 80, 80, 80, 80, 80, 80,
    -80,-80,-80,-80,-80,-80,-80,-80
};
/* Square wave at ~500 Hz (16 on + 16 off) */
static const s8 s_sq_mid[64] = {
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
    -60,-60,-60,-60,-60,-60,-60,-60,-60,-60,-60,-60,-60,-60,-60,-60,
     60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
    -60,-60,-60,-60,-60,-60,-60,-60,-60,-60,-60,-60,-60,-60,-60,-60
};
/* Square wave at ~250 Hz (32 on + 32 off) — low thud */
static const s8 s_sq_lo[64] = {
     40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
     40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
    -40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,
    -40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40
};
/* Rising arpeggio (3 pitch bursts chained) — level-up / open chest */
static const s8 s_arpeggio[96] = {
     70, 70, 70, 70, 70, 70, 70, 70,-70,-70,-70,-70,-70,-70,-70,-70, /* lo */
     80, 80, 80, 80, 80, 80, 80, 80,-80,-80,-80,-80,-80,-80,-80,-80, /* mid */
     90, 90, 90, 90, 90, 90, 90, 90,-90,-90,-90,-90,-90,-90,-90,-90, /* hi */
     70, 70, 70, 70, 70, 70, 70, 70,-70,-70,-70,-70,-70,-70,-70,-70,
     80, 80, 80, 80, 80, 80, 80, 80,-80,-80,-80,-80,-80,-80,-80,-80,
     90, 90, 90, 90, 90, 90, 90, 90,-90,-90,-90,-90,-90,-90,-90,-90
};
/* Descending tone — game over */
static const s8 s_descend[96] = {
     60, 60, 60, 60, 60, 60, 60, 60,-60,-60,-60,-60,-60,-60,-60,-60,
     50, 50, 50, 50, 50, 50, 50, 50,-50,-50,-50,-50,-50,-50,-50,-50,
     40, 40, 40, 40, 40, 40, 40, 40,-40,-40,-40,-40,-40,-40,-40,-40,
     30, 30, 30, 30, 30, 30, 30, 30,-30,-30,-30,-30,-30,-30,-30,-30,
     20, 20, 20, 20, 20, 20, 20, 20,-20,-20,-20,-20,-20,-20,-20,-20,
     10, 10, 10, 10, 10, 10, 10, 10,-10,-10,-10,-10,-10,-10,-10,-10
};
/* Warp shimmer (rapid alternating) */
static const s8 s_shimmer[32] = {
     70,-70, 70,-70, 70,-70, 70,-70,
     80,-80, 80,-80, 80,-80, 80,-80,
     90,-90, 90,-90, 90,-90, 90,-90,
    100,-100,100,-100,100,-100,100,-100
};
/* Boss roar: loud low rumble */
static const s8 s_roar[64] = {
    100,100,100,100, 80, 80, 80, 80, 60, 60, 60, 60, 40, 40, 40, 40,
     20, 20, 20, 20,  0,  0,  0,  0,-20,-20,-20,-20,-40,-40,-40,-40,
    -60,-60,-60,-60,-80,-80,-80,-80,-100,-100,-100,-100,-80,-80,-80,-80,
    -60,-60,-60,-60,-40,-40,-40,-40,-20,-20,-20,-20,  0,  0,  0,  0
};
/* Footstep: very short click */
static const s8 s_step[8] = { 30, 20, 10, 0, -10, -20, -30, 0 };
/* Heal chime: gentle high tone */
static const s8 s_chime[32] = {
     50, 50, 50, 50, 50, 50, 50, 50,-50,-50,-50,-50,-50,-50,-50,-50,
     30, 30, 30, 30, 30, 30, 30, 30,-30,-30,-30,-30,-30,-30,-30,-30
};

/* ---- SFX descriptor ------------------------------------ */
typedef struct {
    const s8* data;
    u16       len;
    u16       loop_start;   /* 0xFFFF = no loop (one-shot) */
    u8        volume;       /* 0-255 */
    u16       timer_reload; /* REG_TM0CNT_L reload for sample rate */
} SfxDef;

/* Timer reload for 8 kHz @ 16.78 MHz GBA clock: 65536 - (16777216/8000) = 63440 */
#define TIMER_8KHZ  63440u
/* Timer reload for 16 kHz: 65536 - (16777216/16000) = 64487 */
#define TIMER_16KHZ 64487u

const SfxDef g_sfx_table[SFX_COUNT] = {
    /* MENU_CURSOR  */ { s_sq_hi,   32, 0xFFFF, 160, TIMER_16KHZ },
    /* MENU_CONFIRM */ { s_arpeggio,96, 0xFFFF, 200, TIMER_16KHZ },
    /* MENU_CANCEL  */ { s_sq_lo,   64, 0xFFFF, 140, TIMER_8KHZ  },
    /* ATTACK       */ { s_sq_mid,  64, 0xFFFF, 220, TIMER_8KHZ  },
    /* MAGIC        */ { s_shimmer, 32, 0xFFFF, 200, TIMER_16KHZ },
    /* HIT          */ { s_sq_lo,   64, 0xFFFF, 255, TIMER_8KHZ  },
    /* HEAL         */ { s_chime,   32, 0xFFFF, 180, TIMER_16KHZ },
    /* LEVELUP      */ { s_arpeggio,96, 0xFFFF, 240, TIMER_16KHZ },
    /* CHEST        */ { s_arpeggio,96, 0xFFFF, 200, TIMER_16KHZ },
    /* DOOR         */ { s_sq_lo,   64, 0xFFFF, 160, TIMER_8KHZ  },
    /* STEP         */ { s_step,     8, 0xFFFF,  80, TIMER_8KHZ  },
    /* WARP         */ { s_shimmer, 32, 0xFFFF, 220, TIMER_16KHZ },
    /* BOSS_ROAR    */ { s_roar,    64, 0xFFFF, 255, TIMER_8KHZ  },
    /* VICTORY      */ { s_arpeggio,96, 0xFFFF, 255, TIMER_16KHZ },
    /* GAMEOVER     */ { s_descend, 96, 0xFFFF, 200, TIMER_8KHZ  },
};

/* ---- Music pattern data -------------------------------- */
/* Stub pattern: 64 bytes of silence.
 * Replace with output from a tracker → GBA converter
 * (e.g. gsfopt, mmutil, or custom Python script).       */
static const u8 s_empty_pattern[64] = { 0 };

/* Simple title jingle: short repeating melodic phrase */
static const u8 s_title_pattern[64] = {
    /* ch, note, vol, effect — simplified stub encoding */
    0x01,0x3C,0x7F,0x00,  0x01,0x40,0x7F,0x00,
    0x01,0x43,0x7F,0x00,  0x01,0x48,0x7F,0x00,
    0x01,0x43,0x60,0x00,  0x01,0x40,0x60,0x00,
    0x01,0x3C,0x60,0x00,  0x00,0x00,0x00,0x00,
    0x01,0x3C,0x7F,0x00,  0x01,0x40,0x7F,0x00,
    0x01,0x43,0x7F,0x00,  0x01,0x4C,0x7F,0x00,
    0x01,0x48,0x60,0x00,  0x01,0x43,0x60,0x00,
    0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00,
};

/* Battle theme: faster tempo stub */
static const u8 s_battle_pattern[64] = {
    0x01,0x30,0x7F,0x0C,  0x01,0x30,0x40,0x00,
    0x01,0x33,0x7F,0x00,  0x01,0x30,0x40,0x00,
    0x01,0x30,0x7F,0x0C,  0x01,0x30,0x40,0x00,
    0x01,0x37,0x7F,0x00,  0x01,0x35,0x60,0x00,
    0x01,0x38,0x7F,0x0C,  0x01,0x38,0x40,0x00,
    0x01,0x35,0x7F,0x00,  0x01,0x38,0x40,0x00,
    0x01,0x3A,0x7F,0x0C,  0x01,0x3A,0x40,0x00,
    0x01,0x38,0x7F,0x00,  0x01,0x35,0x60,0x00,
};

/* ---- Music track descriptor ----------------------------- */
typedef struct {
    u8          bpm;
    u8          pattern_count;
    const u8*   pattern_data;
    u16         pattern_len;
    const char* title;
} MusicTrack;

const MusicTrack g_music_table[MUS_COUNT] = {
    /* TITLE     */ { 100, 1, s_title_pattern,  64, "Chronicles of Aetheria" },
    /* OVERWORLD */ { 120, 1, s_empty_pattern,  64, "Fields of Aetheria"     },
    /* TOWN      */ { 110, 1, s_empty_pattern,  64, "Village Life"           },
    /* DUNGEON   */ {  85, 1, s_empty_pattern,  64, "Depths Below"           },
    /* BATTLE    */ { 160, 1, s_battle_pattern, 64, "Battle Cry"             },
    /* BOSS      */ { 175, 1, s_battle_pattern, 64, "Dread Encounter"        },
    /* VICTORY   */ { 140, 1, s_title_pattern,  64, "Triumph"                },
    /* GAMEOVER  */ {  65, 1, s_empty_pattern,  64, "Fallen Hero"            },
    /* CASTLE    */ {  95, 1, s_empty_pattern,  64, "Royal Halls"            },
    /* SHADOW    */ {  75, 1, s_empty_pattern,  64, "Shadow Realm"           },
};

/* Expose counts for audio system init */
const u8 g_sfx_count   = SFX_COUNT;
const u8 g_music_count = MUS_COUNT;
