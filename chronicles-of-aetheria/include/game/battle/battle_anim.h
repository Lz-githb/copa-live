#ifndef BATTLE_ANIM_H
#define BATTLE_ANIM_H

#include "battle_types.h"
#include "battler.h"

/* =========================================================
 * Battle Animation Controller
 *
 * GBA Battle Screen Layout (240×160, Mode 0):
 *
 *   ┌────────────────────────────────────────┐
 *   │  [TQ indicators row 0-1]               │
 *   │  Enemy row A:  En0  En1  En2  (row 2-8)│
 *   │  Enemy row B:  En3  En4  En5  (row 8-14)│
 *   │  Ally row:  A0  A1  A2  A3   (row 14-19)│
 *   │  [HP/MP bars] [Command menu] (overlay)  │
 *   └────────────────────────────────────────┘
 *
 * Sprite positions (screen pixels):
 *   Enemy row A:  y=16,  x=20/80/140  (3 enemies)
 *   Enemy row B:  y=56,  x=20/80/140  (3 enemies)
 *   Ally row:     y=100, x=20/70/130/190 (4 allies)
 *
 * Animations use OBJ sprites from pool + palette effects.
 * Effect sprites are allocated temporarily during animation.
 * ========================================================= */

/* Screen positions for battler sprites */
#define ENEMY_Y_ROW_A    16
#define ENEMY_Y_ROW_B    56
#define ALLY_Y_ROW       100
#define ENEMY_X_POSITIONS  { 20, 80, 140 }
#define ALLY_X_POSITIONS   { 20, 70, 130, 190 }

/* Animation IDs */
typedef enum {
    ANIM_NONE = 0,
    ANIM_ATTACK,          /* slash forward */
    ANIM_FIRE,            /* fire ball */
    ANIM_WATER,           /* water wave */
    ANIM_EARTH,           /* ground shake */
    ANIM_AIR,             /* wind spiral */
    ANIM_LIGHT,           /* white flash */
    ANIM_DARK,            /* dark pulse */
    ANIM_HEAL,            /* green sparkle */
    ANIM_DRAIN,           /* red drain beam */
    ANIM_STATUS,          /* status icon pop */
    ANIM_COMBO_FIRE,      /* twin fire explosion */
    ANIM_COMBO_TIDAL,     /* tidal wave */
    ANIM_COMBO_STORM,     /* storm vortex */
    ANIM_COMBO_HOLY,      /* holy nova */
    ANIM_COMBO_SHADOW,    /* shadow bind */
    ANIM_UNLEASH_SOLEIL,
    ANIM_UNLEASH_TIDE,
    ANIM_UNLEASH_GAIA,
    ANIM_UNLEASH_STORM,
    ANIM_VICTORY,         /* win pose */
    ANIM_KO,              /* fall/dissolve */
    ANIM_HIT,             /* hit flash */
    ANIM_MISS,            /* "MISS" text */
    ANIM_CRITICAL,        /* critical flash */
    ANIM_FLEE,
    ANIM_COUNT
} AnimID;

/* Per-animation definition */
typedef struct {
    u8   frames;          /* total frames */
    BOOL shake_screen;
    BOOL flash_white;
    BOOL flash_black;
    u8   palette_shift;   /* cycles color palette for effect */
} AnimDef;

/* Animation state */
typedef enum {
    ANIM_STATE_IDLE = 0,
    ANIM_STATE_SLIDE_OUT,   /* attacker slides toward target */
    ANIM_STATE_EFFECT,      /* effect sprite plays */
    ANIM_STATE_HIT,         /* target flash */
    ANIM_STATE_SLIDE_IN,    /* attacker returns */
    ANIM_STATE_TEXT,        /* damage number floats up */
    ANIM_STATE_DONE,
} AnimStatePhase;

/* Floating damage number */
typedef struct {
    s16   x, y;
    s16   vy;           /* moves upward */
    s16   value;        /* damage value to display */
    u8    timer;
    BOOL  active;
    BOOL  is_crit;
    BOOL  is_heal;
    BOOL  is_miss;
} DamageNumber;

#define DAMAGE_NUM_MAX  10   /* max simultaneous damage numbers */

typedef struct {
    AnimID          id;
    AnimStatePhase  phase;
    u8              timer;
    u8              total_frames;
    u8              actor_idx;
    u16             target_mask;
    /* Slide animation */
    s16             actor_orig_x;
    s16             target_x;
    /* Effect sprites */
    SpriteHandle    effect_spr[4];
    u8              effect_count;
    /* Screen shake */
    u8              shake_frames;
    /* Damage numbers */
    DamageNumber    dmg_nums[DAMAGE_NUM_MAX];
} BattleAnim;

/* ---- Animation API -------------------------------------- */

void anim_init(void);

/* Start an animation for a resolved action */
void anim_start(AnimID id, u8 actor_idx, u16 target_mask);

/* Update animation (call each frame) */
void anim_update(void);

/* True when animation is complete */
BOOL anim_done(void);

/* Spawn a floating damage number */
void anim_spawn_damage_num(s16 screen_x, s16 screen_y,
                            s16 value, BOOL is_crit,
                            BOOL is_heal, BOOL is_miss);

/* Set all battler sprite positions */
void anim_set_positions(Battler battlers[BATTLE_BATTLER_MAX]);

/* Render damage numbers (call from battle render) */
void anim_render_nums(void);

/* Flash screen (brightness effect) */
void anim_flash(u8 brightness, u8 frames, BOOL white);

/* KO animation for a specific battler */
void anim_ko(Battler* b);

extern BattleAnim g_anim;

#endif /* BATTLE_ANIM_H */
