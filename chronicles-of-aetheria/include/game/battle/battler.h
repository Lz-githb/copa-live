#ifndef BATTLER_H
#define BATTLER_H

#include "battle_types.h"

/* =========================================================
 * Battler — Runtime State of One Combat Unit
 *
 * Both player characters and enemies use the same struct.
 * ROM templates (CharDef / EnemyDef) are copied into
 * these runtime slots at battle start.
 * ========================================================= */

#define BATTLER_NAME_LEN  12

typedef struct Battler {
    /* Identity */
    char         name[BATTLER_NAME_LEN];
    u8           idx;          /* 0-9 (0-3 allies, 4-9 enemies) */
    BOOL         is_enemy;
    u8           sprite_id;    /* sprite tile base for combat */
    u8           palette_id;

    /* Core stats (raw, before modifiers) */
    s16   hp,     hp_max;
    s16   mp,     mp_max;
    s16   atk,    def;        /* physical */
    s16   mag,    mdef;       /* magical */
    s16   spd;
    u32   exp_reward;
    u16   gold_reward;

    /* Element affinity */
    ElementType element;           /* own element (used for combo affinity) */
    u8          resist[ELEM_COUNT];/* affinity per element (ELEM_RESIST etc.) */

    /* Status effects */
    u32   status_bits;         /* bitmask of active StatusFlags */
    u8    status_dur[STATUS_COUNT]; /* remaining turns per status */
    u8    doom_timer;          /* turns until DOOM triggers */

    /* Stat modifiers (stages -4..+4) */
    s8    atk_stage;
    s8    def_stage;
    s8    mag_stage;
    s8    mdef_stage;
    s8    spd_stage;

    /* Combo system */
    u8    combo_pts;           /* 0-100 */

    /* Skill loadout (skill IDs into g_skill_table) */
    u8    skill_ids[BATTLE_SKILL_MAX];
    u8    skill_count;

    /* Battle flags */
    BOOL  alive;
    BOOL  defending;           /* guarding this turn */
    BOOL  acted;               /* already acted this round */

    /* Turn order */
    s16   turn_wait;           /* lower = acts sooner */

    /* Sprite handle (from sprite system) */
    SpriteHandle spr;

    /* Screen position for animations */
    s16   screen_x;
    s16   screen_y;
} Battler;

/* ---- Character template (ROM) --------------------------- */
typedef struct {
    const char*  name;
    u8           sprite_id;
    u8           palette_id;
    ElementType  element;
    s16          hp_base, mp_base;
    s16          atk_base, def_base;
    s16          mag_base, mdef_base;
    s16          spd_base;
    u8           resist[ELEM_COUNT];
    u8           skill_ids[BATTLE_SKILL_MAX];
    u8           skill_count;
} CharDef;

/* ---- Enemy template (ROM) ------------------------------- */
typedef struct {
    const char*  name;
    u8           sprite_id;
    u8           palette_id;
    ElementType  element;
    s16          hp, mp;
    s16          atk, def;
    s16          mag, mdef;
    s16          spd;
    u8           resist[ELEM_COUNT];
    u32          exp_reward;
    u16          gold_reward;
    /* AI behavior */
    u8           skill_ids[4];
    u8           skill_count;
    u8           ai_type;      /* 0=random, 1=pattern, 2=react */
} EnemyDef;

/* ---- Enemy group (defines one battle encounter) --------- */
typedef struct {
    u8  enemy_ids[BATTLE_ENEMY_MAX];
    u8  count;
    u8  background_id;   /* which battle BG to use */
    u8  music_id;
    u8  flee_multiplier; /* 1-4: how hard to flee */
} EnemyGroup;

/* ---- Battler API ---------------------------------------- */

/* Initialize a battler slot from a character template */
void battler_from_char(Battler* b, u8 idx, const CharDef* ch);

/* Initialize a battler slot from an enemy template */
void battler_from_enemy(Battler* b, u8 idx, const EnemyDef* en);

/* Compute effective stat value after stage modifiers */
s16  battler_eff_atk(const Battler* b);
s16  battler_eff_def(const Battler* b);
s16  battler_eff_mag(const Battler* b);
s16  battler_eff_mdef(const Battler* b);
s16  battler_eff_spd(const Battler* b);

/* Apply damage/healing (clamps HP, marks dead if HP<=0) */
void battler_apply_damage(Battler* b, s16 amount);

/* Add/remove combo points */
void battler_add_cp(Battler* b, u8 amount);

/* Compute turn_wait value (lower = acts sooner) */
void battler_compute_wait(Battler* b);

/* Check if battler can act (alive, not skip-turn status) */
BOOL battler_can_act(const Battler* b);

/* Check if battler can use magic */
BOOL battler_can_magic(const Battler* b);

/* Reset per-round flags (acted, defending) */
void battler_new_round(Battler* b);

#endif /* BATTLER_H */
