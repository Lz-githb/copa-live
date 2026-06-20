#include "../../include/game/interaction.h"
#include "../../include/engine/engine.h"

/* =========================================================
 * Interaction System Implementation
 * ========================================================= */

/* World flags: 256 bits = 32 bytes (covers flag IDs 0-255) */
#define FLAG_BYTES 32
static u8 s_flags[FLAG_BYTES];

const ScriptCmd**   g_script_table = NULL;
const DialogueText* g_text_table   = NULL;
static u16          s_script_count = 0;
static u16          s_text_count   = 0;

static ScriptInterp s_interp;

/* Direction tile offset (same as npc.c DIR_DX/DY) */
static const s8 IDIR_DX[4] = {  0, -1,  1,  0 };
static const s8 IDIR_DY[4] = {  1,  0,  0, -1 };

/* ---- Flag management ------------------------------------ */

BOOL interact_flag_get(u16 flag_id) {
    if (flag_id >= FLAG_BYTES * 8) return FALSE;
    return (s_flags[flag_id >> 3] & (1 << (flag_id & 7))) != 0;
}

void interact_flag_set(u16 flag_id) {
    if (flag_id < FLAG_BYTES * 8)
        s_flags[flag_id >> 3] |= (u8)(1 << (flag_id & 7));
}

void interact_flag_clear(u16 flag_id) {
    if (flag_id < FLAG_BYTES * 8)
        s_flags[flag_id >> 3] &= (u8)~(1 << (flag_id & 7));
}

/* ---- Script interpreter --------------------------------- */

static void _interp_reset(void) {
    s_interp.state          = INTERP_IDLE;
    s_interp.script         = NULL;
    s_interp.pc             = 0;
    s_interp.npc            = NULL;
    s_interp.dialogue_open  = FALSE;
    s_interp.dialogue_line  = 0;
    s_interp.dialogue_char  = 0;
    s_interp.wait_timer     = 0;
}

/* Find label in current script */
static u16 _find_label(const ScriptCmd* script, u16 label_id) {
    u16 i = 0;
    while (script[i].op != SCMD_END) {
        if (script[i].op == SCMD_LABEL && script[i].param0 == label_id)
            return i + 1;
        i++;
    }
    return 0;
}

static void _exec_step(void) {
    if (!s_interp.script || s_interp.state != INTERP_RUNNING) return;

    const ScriptCmd* cmd = &s_interp.script[s_interp.pc];

    switch (cmd->op) {
    case SCMD_END:
        s_interp.state = INTERP_DONE;
        break;

    case SCMD_DIALOGUE:
        if (cmd->param0 < s_text_count) {
            const DialogueText* txt = &g_text_table[cmd->param0];
            (void)txt;
            s_interp.dialogue_text_id = cmd->param0;
            s_interp.dialogue_open    = TRUE;
            s_interp.dialogue_line    = 0;
            s_interp.dialogue_char    = 0;
            s_interp.state = INTERP_WAIT_INPUT;
        } else {
            s_interp.pc++;
        }
        break;

    case SCMD_FACE_PLAYER:
        /* NPC faces the player — handled by npc_face_player at trigger */
        s_interp.pc++;
        break;

    case SCMD_FACE_DIR:
        if (s_interp.npc) {
            s_interp.npc->dir = (u8)cmd->param0;
        }
        s_interp.pc++;
        break;

    case SCMD_FLAG_SET:
        interact_flag_set(cmd->param0);
        s_interp.pc++;
        break;

    case SCMD_FLAG_CLEAR:
        interact_flag_clear(cmd->param0);
        s_interp.pc++;
        break;

    case SCMD_FLAG_BRANCH:
        if (interact_flag_get(cmd->param0)) {
            s_interp.pc = _find_label(s_interp.script, cmd->param1);
        } else {
            s_interp.pc++;
        }
        break;

    case SCMD_GIVE_ITEM:
        /* TODO: add to inventory */
        s_interp.pc++;
        break;

    case SCMD_BATTLE:
        event_push(EVT_BATTLE_START, cmd->param0, 0);
        s_interp.pc++;
        break;

    case SCMD_WARP: {
        WarpEntry tmp;
        tmp.dest_map   = (u8)cmd->param0;
        tmp.dest_spawn = (u8)cmd->param1;
        tmp.type       = WARP_TELEPORT;
        world_start_warp(&tmp);
        s_interp.pc++;
        break;
    }

    case SCMD_WAIT:
        s_interp.wait_timer = cmd->param0;
        s_interp.state      = INTERP_WAIT_FRAMES;
        s_interp.pc++;
        break;

    case SCMD_SHAKE:
        camera_shake((u8)cmd->param0, (u8)cmd->param1);
        s_interp.pc++;
        break;

    case SCMD_WEATHER:
        event_push(EVT_SFX_PLAY, 0, 0);   /* placeholder: weather change sfx */
        s_interp.pc++;
        break;

    case SCMD_SOUND:
        event_push(EVT_SFX_PLAY, cmd->param0, 0);
        s_interp.pc++;
        break;

    case SCMD_LABEL:
        s_interp.pc++;
        break;

    case SCMD_JUMP:
        s_interp.pc = _find_label(s_interp.script, cmd->param0);
        break;

    case SCMD_CLOSE:
        s_interp.dialogue_open = FALSE;
        s_interp.pc++;
        break;

    default:
        s_interp.pc++;
        break;
    }
}

/* ---- Public API ----------------------------------------- */

void interact_init(const ScriptCmd** script_table,
                   const DialogueText* text_table,
                   u16 script_count, u16 text_count) {
    g_script_table = script_table;
    g_text_table   = text_table;
    s_script_count = script_count;
    s_text_count   = text_count;
    mem_zero(s_flags, sizeof(s_flags));
    _interp_reset();
}

void interact_run_script(u16 script_id, Npc* npc) {
    if (!g_script_table || script_id >= s_script_count) return;
    s_interp.script    = g_script_table[script_id];
    s_interp.npc       = npc;
    s_interp.pc        = 0;
    s_interp.state     = INTERP_RUNNING;
    s_interp.dialogue_open = FALSE;
    if (npc) npc_freeze_all();
}

void interact_try(u8 player_tx, u8 player_ty, u8 player_dir) {
    if (interact_busy()) return;

    /* 1. NPC in front */
    Npc* npc = npc_facing_player(player_tx, player_ty, player_dir);
    if (npc && npc->active) {
        npc_face_player(npc, player_tx, player_ty);
        interact_run_script(npc->script_id, npc);
        return;
    }

    /* 2. Map object in front */
    s16 ftx = player_tx + IDIR_DX[player_dir];
    s16 fty = player_ty + IDIR_DY[player_dir];
    MapObject* obj = world_get_object(ftx, fty);
    if (obj) {
        switch (obj->type) {
        case OBJ_SIGN:
        case OBJ_NOTICE_BOARD:
        case OBJ_BOOKSHELF:
            interact_run_script(obj->script_id, NULL);
            break;
        case OBJ_CHEST:
            if (!obj->opened) {
                obj->opened = TRUE;
                interact_run_script(obj->script_id, NULL);
            }
            break;
        case OBJ_DOOR_LOCKED:
            /* TODO: check key item */
            interact_run_script(obj->script_id, NULL);
            break;
        }
        return;
    }

    /* 3. Tile flag (signs embedded in map via event tile) */
    u8 flags = world_get_flags(ftx, fty);
    if (flags & TF_EVENT) {
        /* Tile-embedded event: look up object at that position */
        /* (falls through if no object registered there) */
    }
}

void interact_update(void) {
    switch (s_interp.state) {

    case INTERP_IDLE:
    case INTERP_DONE:
        if (s_interp.dialogue_open) {
            s_interp.dialogue_open = FALSE;
            npc_unfreeze_all();
        }
        if (s_interp.state == INTERP_DONE) {
            _interp_reset();
            npc_unfreeze_all();
        }
        break;

    case INTERP_RUNNING:
        /* Execute one command per frame */
        _exec_step();
        break;

    case INTERP_WAIT_INPUT:
        /* Advance dialogue text character by character */
        if (s_interp.dialogue_open) {
            if (s_interp.dialogue_char < DIALOGUE_LINE_LEN) {
                s_interp.dialogue_char += 2;  /* 2 chars per frame */
            }
        }
        /* A or B press: advance or close dialogue */
        if (key_pressed(KEY_A) || key_pressed(KEY_B)) {
            if (s_interp.dialogue_char < DIALOGUE_LINE_LEN &&
                key_pressed(KEY_B)) {
                /* Speed up text */
                s_interp.dialogue_char = DIALOGUE_LINE_LEN;
            } else {
                /* Next line or close */
                const DialogueText* txt = &g_text_table[s_interp.dialogue_text_id];
                s_interp.dialogue_line++;
                if (s_interp.dialogue_line >= txt->line_count) {
                    s_interp.dialogue_open = FALSE;
                    s_interp.pc++;
                    s_interp.state = INTERP_RUNNING;
                } else {
                    s_interp.dialogue_char = 0;
                }
            }
        }
        break;

    case INTERP_WAIT_FRAMES:
        if (s_interp.wait_timer > 0) {
            s_interp.wait_timer--;
        } else {
            s_interp.state = INTERP_RUNNING;
        }
        break;
    }
}

void interact_render(void) {
    if (!s_interp.dialogue_open) return;
    /* TODO: render dialogue box using tile-based text renderer */
    /* The dialogue box is drawn on BG0 (HUD layer) at rows 14-19 */
    /* Character-by-character reveal uses s_interp.dialogue_char */
}

BOOL interact_busy(void) {
    return s_interp.state == INTERP_RUNNING ||
           s_interp.state == INTERP_WAIT_INPUT ||
           s_interp.state == INTERP_WAIT_FRAMES;
}

void interact_close(void) {
    _interp_reset();
    npc_unfreeze_all();
}
