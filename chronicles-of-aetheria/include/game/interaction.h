#ifndef GAME_INTERACTION_H
#define GAME_INTERACTION_H

#include "../engine/engine.h"
#include "world.h"
#include "npc.h"

/* =========================================================
 * Interaction System
 *
 * On A button press, we check (in priority order):
 *   1. NPC directly in front of player
 *   2. MapObject at tile in front of player
 *   3. Special tile flag (sign, bookshelf, etc.)
 *
 * Scripts are simple linear command arrays stored in ROM.
 * Commands: DIALOGUE, CHOICE, FLAG_SET, FLAG_CHECK,
 *           WARP, GIVE_ITEM, BATTLE, etc.
 *
 * The interpreter runs one command per frame unless
 * a blocking command is active (DIALOGUE waits for input).
 * ========================================================= */

/* Script command opcodes */
typedef enum {
    SCMD_END = 0,        /* end of script */
    SCMD_DIALOGUE,       /* show text; param = text_id */
    SCMD_FACE_PLAYER,    /* NPC faces the player */
    SCMD_FACE_DIR,       /* NPC faces param direction */
    SCMD_MOVE_NPC,       /* move NPC to tile (param0=dx, param1=dy) */
    SCMD_FLAG_SET,       /* set world flag param */
    SCMD_FLAG_CLEAR,
    SCMD_FLAG_BRANCH,    /* if flag set, jump to label */
    SCMD_GIVE_ITEM,      /* add item to inventory; param = item_id */
    SCMD_BATTLE,         /* start battle; param = enemy_group_id */
    SCMD_WARP,           /* warp to map; param0=map, param1=spawn */
    SCMD_WAIT,           /* wait param frames */
    SCMD_SHAKE,          /* camera shake; param0=intensity, param1=frames */
    SCMD_WEATHER,        /* change weather; param = WeatherType */
    SCMD_SOUND,          /* play SFX; param = sfx_id */
    SCMD_HEAL,           /* restore HP/MP */
    SCMD_LABEL,          /* branch target; param = label_id */
    SCMD_JUMP,           /* unconditional jump to label */
    SCMD_CLOSE,          /* close dialogue box */
} ScriptOpcode;

typedef struct {
    ScriptOpcode op;
    u16          param0;
    u16          param1;
} ScriptCmd;

/* ---- Dialogue text ------------------------------------- */
#define DIALOGUE_MAX_LINES  4
#define DIALOGUE_LINE_LEN   32

typedef struct {
    const char* lines[DIALOGUE_MAX_LINES];
    u8          line_count;
} DialogueText;

/* ---- Script interpreter state -------------------------- */
typedef enum {
    INTERP_IDLE = 0,
    INTERP_RUNNING,
    INTERP_WAIT_INPUT,    /* waiting for A press */
    INTERP_WAIT_FRAMES,   /* waiting N frames */
    INTERP_DONE,
} InterpState;

typedef struct {
    InterpState       state;
    const ScriptCmd*  script;    /* current script in ROM */
    u16               pc;        /* program counter */
    Npc*              npc;       /* NPC executing this script (or NULL) */
    u32               wait_timer;
    /* Dialogue box state */
    BOOL              dialogue_open;
    u8                dialogue_line;
    u8                dialogue_char;  /* character reveal timer */
    u32               dialogue_text_id;
    char              dialogue_buf[DIALOGUE_LINE_LEN];
} ScriptInterp;

/* ---- Interaction API ----------------------------------- */

void interact_init(const ScriptCmd** script_table,
                   const DialogueText* text_table,
                   u16 script_count, u16 text_count);

/* Called when player presses A */
void interact_try(u8 player_tx, u8 player_ty, u8 player_dir);

/* Run one frame of script interpreter */
void interact_update(void);

/* Render dialogue box (call from render) */
void interact_render(void);

/* Is an interaction (dialogue/script) currently active? */
BOOL interact_busy(void);

/* Force-close any active dialogue */
void interact_close(void);

/* Directly run a script by ID */
void interact_run_script(u16 script_id, Npc* npc);

/* Check if a world flag is set */
BOOL interact_flag_get(u16 flag_id);
void interact_flag_set(u16 flag_id);
void interact_flag_clear(u16 flag_id);

/* Script table (ROM pointers, indexed by script_id) */
extern const ScriptCmd** g_script_table;
extern const DialogueText* g_text_table;

#endif /* GAME_INTERACTION_H */
