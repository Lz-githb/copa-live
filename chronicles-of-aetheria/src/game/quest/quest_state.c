#include "quest_state.h"
#include "inventory.h"
#include "char_xp.h"
#include "../../engine/event.h"
#include "gba_types.h"

/* ---- Global state --------------------------------------- */
QuestSaveData g_quest_state;

/* ---- Small string buffer for progress display ----------- */
static char s_progress_buf[20];

/* ---- Lifecycle ------------------------------------------ */

void quest_system_init(void)
{
    u8 i, j;
    mem_zero(&g_quest_state, sizeof(g_quest_state));
    g_quest_state.magic = QUEST_SAVE_MAGIC;

    /* Set all quests to LOCKED initially */
    for (i = 0; i < QUEST_COUNT; i++)
        g_quest_state.status[i] = (u8)QSTAT_LOCKED;

    /* Secret quests start as HIDDEN */
    for (i = QUEST_MAIN_COUNT + QUEST_SIDE_COUNT; i < QUEST_COUNT; i++)
        g_quest_state.status[i] = (u8)QSTAT_HIDDEN;

    /* Zero all progress */
    for (i = 0; i < QUEST_COUNT; i++)
        for (j = 0; j < QUEST_OBJ_MAX; j++)
            g_quest_state.obj_progress[i][j] = 0;

    /* First main quest starts available */
    g_quest_state.status[QMAIN_AWAKENING] = (u8)QSTAT_AVAILABLE;
}

/* ---- Query ---------------------------------------------- */

QuestStatus quest_status(u8 quest_id)
{
    if (quest_id >= QUEST_COUNT) return QSTAT_LOCKED;
    return (QuestStatus)g_quest_state.status[quest_id];
}

u8 quest_obj_progress(u8 quest_id, u8 obj_idx)
{
    if (quest_id >= QUEST_COUNT || obj_idx >= QUEST_OBJ_MAX) return 0;
    return g_quest_state.obj_progress[quest_id][obj_idx];
}

BOOL quest_obj_done(u8 quest_id, u8 obj_idx)
{
    const QuestObjDef* obj;
    u8 req;
    if (quest_id >= QUEST_COUNT || obj_idx >= QUEST_OBJ_MAX) return FALSE;
    obj = quest_obj_get(quest_id, obj_idx);
    if (!obj || obj->type == QOBJ_NONE) return TRUE; /* empty slot = done */
    req = (obj->required == 0) ? 1 : obj->required;
    return (BOOL)(g_quest_state.obj_progress[quest_id][obj_idx] >= req);
}

BOOL quest_all_objectives_done(u8 quest_id)
{
    u8 i;
    for (i = 0; i < QUEST_OBJ_MAX; i++) {
        if (!quest_obj_done(quest_id, i)) return FALSE;
    }
    return TRUE;
}

BOOL quest_secret_revealed(u8 quest_id)
{
    u8 bit;
    if (!quest_is_secret(quest_id)) return FALSE;
    bit = (u8)(quest_id - (QUEST_MAIN_COUNT + QUEST_SIDE_COUNT));
    return (BOOL)((g_quest_state.secrets_found & (u32)(1u << bit)) != 0);
}

/* ---- Prerequisite check --------------------------------- */

static BOOL _prereq_met(const QuestPrereq* pre, u8 party_max_level)
{
    if (pre->req_quest_id != 0xFF &&
        quest_status(pre->req_quest_id) != QSTAT_COMPLETED)
        return FALSE;
    if (pre->req_min_level > 0 && party_max_level < pre->req_min_level)
        return FALSE;
    if (pre->req_item_id != ITEM_NONE && bag_count(pre->req_item_id) == 0)
        return FALSE;
    /* World flags checked via chain_flags (simplified: bit mask) */
    if (pre->req_world_flags != 0 &&
        (g_quest_state.chain_flags & pre->req_world_flags) != pre->req_world_flags)
        return FALSE;
    return TRUE;
}

/* ---- Mutation ------------------------------------------- */

BOOL quest_accept(u8 quest_id)
{
    if (quest_id >= QUEST_COUNT) return FALSE;
    if (g_quest_state.status[quest_id] != (u8)QSTAT_AVAILABLE) return FALSE;
    g_quest_state.status[quest_id] = (u8)QSTAT_ACTIVE;
    event_push(EVT_SFX_PLAY, 6, 0);
    return TRUE;
}

void quest_fail(u8 quest_id)
{
    if (quest_id >= QUEST_COUNT) return;
    if (g_quest_state.status[quest_id] != (u8)QSTAT_ACTIVE) return;
    g_quest_state.status[quest_id] = (u8)QSTAT_FAILED;
}

void quest_reveal_secret(u8 quest_id)
{
    u8 bit;
    if (!quest_is_secret(quest_id)) return;
    bit = (u8)(quest_id - (QUEST_MAIN_COUNT + QUEST_SIDE_COUNT));
    g_quest_state.secrets_found |= (u32)(1u << bit);
    /* Transition from HIDDEN → AVAILABLE if prereqs met */
    if (g_quest_state.status[quest_id] == (u8)QSTAT_HIDDEN) {
        const QuestDef* q = quest_def_get(quest_id);
        if (q && _prereq_met(&q->prereq, 1))
            g_quest_state.status[quest_id] = (u8)QSTAT_AVAILABLE;
    }
}

void quest_give_reward(u8 quest_id)
{
    const QuestDef* q = quest_def_get(quest_id);
    u8 i;
    u8 p;
    if (!q) return;

    /* Gold */
    gold_add(q->reward.gold);

    /* XP split equally among active party members */
    if (q->reward.xp > 0 && g_party_size > 0) {
        u32 xp_each = q->reward.xp / g_party_size;
        for (p = 0; p < g_party_size; p++)
            xp_award(&g_party[p], xp_each);
    }

    /* Items */
    for (i = 0; i < 3; i++) {
        if (q->reward.item_ids[i] != ITEM_NONE && q->reward.item_qtys[i] > 0)
            bag_add(q->reward.item_ids[i], q->reward.item_qtys[i]);
    }

    /* Set world flags */
    if (q->reward.set_world_flags)
        g_quest_state.chain_flags |= q->reward.set_world_flags;

    /* Unlock next quest in chain */
    if (q->reward.unlock_quest_id != 0xFF)
        quest_check_unlock(q->reward.unlock_quest_id, 1);

    /* Reveal secret quest */
    if (q->reward.reveal_secret_id != 0xFF)
        quest_reveal_secret(q->reward.reveal_secret_id);
}

BOOL quest_complete(u8 quest_id)
{
    if (quest_id >= QUEST_COUNT) return FALSE;
    if (g_quest_state.status[quest_id] != (u8)QSTAT_ACTIVE) return FALSE;
    if (!quest_all_objectives_done(quest_id)) return FALSE;

    g_quest_state.status[quest_id] = (u8)QSTAT_COMPLETED;
    g_quest_state.total_completed++;
    quest_give_reward(quest_id);
    quest_update_availability();
    event_push(EVT_SFX_PLAY, 7, 0); /* victory jingle */
    return TRUE;
}

void quest_advance_obj(u8 quest_id, u8 obj_idx, u8 delta)
{
    const QuestObjDef* obj;
    u8 req;
    u8* prog;

    if (quest_id >= QUEST_COUNT || obj_idx >= QUEST_OBJ_MAX) return;
    if (g_quest_state.status[quest_id] != (u8)QSTAT_ACTIVE) return;

    obj  = quest_obj_get(quest_id, obj_idx);
    if (!obj || obj->type == QOBJ_NONE) return;

    req  = (obj->required == 0) ? 1 : obj->required;
    prog = &g_quest_state.obj_progress[quest_id][obj_idx];

    if (*prog < req) {
        *prog = (u8)MIN((u32)(*prog + delta), (u32)req);
    }

    /* Auto-complete quest when all objectives are done */
    if (quest_all_objectives_done(quest_id))
        quest_complete(quest_id);
}

BOOL quest_check_unlock(u8 quest_id, u8 party_max_level)
{
    const QuestDef* q;
    QuestStatus cur;
    if (quest_id >= QUEST_COUNT) return FALSE;

    q   = quest_def_get(quest_id);
    cur = quest_status(quest_id);

    if (cur == QSTAT_COMPLETED || cur == QSTAT_ACTIVE) return FALSE;
    if (cur == QSTAT_HIDDEN) return FALSE; /* need explicit reveal */

    if (_prereq_met(&q->prereq, party_max_level)) {
        g_quest_state.status[quest_id] = (u8)QSTAT_AVAILABLE;
        return TRUE;
    }
    return FALSE;
}

void quest_update_availability(void)
{
    u8 max_level = 1;
    u8 i;

    /* Find highest party level */
    for (i = 0; i < g_party_size; i++) {
        if (g_party[i].level > max_level)
            max_level = g_party[i].level;
    }

    for (i = 0; i < QUEST_COUNT; i++) {
        if (g_quest_state.status[i] == (u8)QSTAT_LOCKED)
            quest_check_unlock(i, max_level);
    }
}

/* ---- Event-driven progress ------------------------------ */

static void _try_advance(u8 qid, QuestEventType evt, u32 p0, u32 p1)
{
    const QuestDef* q;
    u8 i;
    if (g_quest_state.status[qid] != (u8)QSTAT_ACTIVE) return;
    q = quest_def_get(qid);

    for (i = 0; i < QUEST_OBJ_MAX; i++) {
        const QuestObjDef* obj = &q->objectives[i];
        if (obj->type == QOBJ_NONE) break;
        if (quest_obj_done(qid, i)) continue;

        switch (evt) {
        case QEVT_ENEMY_KILLED:
            if (obj->type == QOBJ_KILL && (u8)p0 == obj->target_id)
                quest_advance_obj(qid, i, 1);
            else if (obj->type == QOBJ_KILL_ANY)
                quest_advance_obj(qid, i, 1);
            break;

        case QEVT_BATTLE_WON:
            if (obj->type == QOBJ_WIN_BATTLE)
                quest_advance_obj(qid, i, 1);
            break;

        case QEVT_ITEM_OBTAINED:
            if (obj->type == QOBJ_COLLECT && (u8)p0 == obj->target_id) {
                /* Re-evaluate based on current bag count */
                u8 have = bag_count(obj->target_id);
                u8 req  = (obj->required == 0) ? 1 : obj->required;
                u8 cur  = g_quest_state.obj_progress[qid][i];
                if (have > cur && cur < req)
                    g_quest_state.obj_progress[qid][i] = MIN(have, req);
                if (quest_all_objectives_done(qid))
                    quest_complete(qid);
            }
            break;

        case QEVT_NPC_TALKED:
            if (obj->type == QOBJ_TALK && (u8)p0 == obj->target_id)
                quest_advance_obj(qid, i, 1);
            break;

        case QEVT_MAP_ENTERED:
            if (obj->type == QOBJ_REACH_MAP && (u8)p0 == obj->target_id)
                quest_advance_obj(qid, i, 1);
            break;

        case QEVT_ITEM_CRAFTED:
            if (obj->type == QOBJ_CRAFT && (u8)p0 == obj->target_id)
                quest_advance_obj(qid, i, 1);
            break;

        case QEVT_ITEM_BOUGHT:
            if (obj->type == QOBJ_BUY && (u8)p0 == obj->target_id)
                quest_advance_obj(qid, i, 1);
            break;

        case QEVT_ITEM_EQUIPPED:
            if (obj->type == QOBJ_EQUIP_ITEM && (u8)p0 == obj->target_id)
                quest_advance_obj(qid, i, 1);
            break;

        case QEVT_SCRIPT_FLAG:
            if (obj->type == QOBJ_SCRIPTED &&
                (u8)p0 == qid &&
                (u8)p1 == obj->required)
                quest_advance_obj(qid, i, 1);
            break;

        case QEVT_ESCORT_SUCCESS:
            if (obj->type == QOBJ_ESCORT_SAFE && (u8)p0 == qid)
                quest_advance_obj(qid, i, 1);
            break;

        default:
            break;
        }
    }
}

void quest_notify(QuestEventType evt, u32 param0, u32 param1)
{
    u8 i;
    for (i = 0; i < QUEST_COUNT; i++)
        _try_advance(i, evt, param0, param1);
    quest_update_availability();
}

/* ---- Save/Load ------------------------------------------ */

void quest_save(SaveSlot* slot)
{
    u8 i;
    u8* dst = &slot->extra[QUEST_SAVE_OFFSET];
    const u8* src = (const u8*)&g_quest_state;
    for (i = 0; i < QUEST_SAVE_SIZE; i++)
        dst[i] = src[i];
}

BOOL quest_load(const SaveSlot* slot)
{
    const u8* src = &slot->extra[QUEST_SAVE_OFFSET];
    u8* dst = (u8*)&g_quest_state;
    u8 i;
    if (src[0] != QUEST_SAVE_MAGIC) return FALSE;
    for (i = 0; i < QUEST_SAVE_SIZE; i++)
        dst[i] = src[i];
    return TRUE;
}

/* ---- UI helpers ----------------------------------------- */

u8 quest_count_by_status(QuestStatus stat, u8 cat)
{
    u8 count = 0;
    u8 i;
    for (i = 0; i < QUEST_COUNT; i++) {
        if ((QuestStatus)g_quest_state.status[i] != stat) continue;
        if (cat != 0xFF && g_quest_db[i].category != (QuestCategory)cat) continue;
        count++;
    }
    return count;
}

u8 quest_list(QuestStatus stat, u8 cat, u8* out)
{
    u8 count = 0;
    u8 i;
    for (i = 0; i < QUEST_COUNT; i++) {
        if ((QuestStatus)g_quest_state.status[i] != stat) continue;
        if (cat != 0xFF && g_quest_db[i].category != (QuestCategory)cat) continue;
        out[count++] = i;
    }
    return count;
}

/* Simple integer-to-string into buffer */
static void _itoa2(char* buf, u8 val)
{
    buf[0] = (char)('0' + val / 10);
    buf[1] = (char)('0' + val % 10);
    buf[2] = '\0';
    /* Trim leading zero for values < 10 */
    if (buf[0] == '0') { buf[0] = buf[1]; buf[1] = buf[2]; }
}

const char* quest_progress_str(u8 quest_id)
{
    u8 done = 0, total = 0;
    u8 i;
    char tmp[4];

    if (quest_id >= QUEST_COUNT) return "???";

    for (i = 0; i < QUEST_OBJ_MAX; i++) {
        const QuestObjDef* obj = quest_obj_get(quest_id, i);
        if (!obj) break;
        total++;
        if (quest_obj_done(quest_id, i)) done++;
    }

    _itoa2(s_progress_buf, done);
    i = (u8)(s_progress_buf[0] ? (s_progress_buf[1] ? 2 : 1) : 0);
    s_progress_buf[i++] = '/';
    _itoa2(tmp, total);
    {
        u8 j = 0;
        while (tmp[j]) s_progress_buf[i++] = tmp[j++];
    }
    s_progress_buf[i] = '\0';
    return s_progress_buf;
}
