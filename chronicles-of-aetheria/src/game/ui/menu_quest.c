#include "menu_quest.h"
#include "menu_types.h"
#include "quest_types.h"
#include "quest_state.h"
#include "quest_db.h"
#include "gba_types.h"

/* =========================================================
 * Quest Log Menu Implementation
 * ========================================================= */

QuestMenuState g_quest_menu;

#define KEY_A      0x0001
#define KEY_B      0x0002
#define KEY_UP     0x0040
#define KEY_DOWN   0x0080
#define KEY_L      0x0200
#define KEY_R      0x0100
#define REG_KEYINPUT (*(vu16*)(0x04000130))

static u16 s_qp;
static u16 s_qc = 0xFFFF;

static u16 _qpressed(void)
{
    u16 now = REG_KEYINPUT;
    u16 p   = (~now) & s_qp;
    s_qp    = ~now;
    s_qc    = now;
    (void)s_qc;
    return p;
}

/* ---- Tab labels ---------------------------------------- */
static const char* const s_qtab_labels[QTAB_COUNT] = {
    "ACTIVE", "AVAIL", "DONE", "SECRET"
};

/* ---- QuestStatus → tab mapping ------------------------- */
static QuestStatus _tab_to_status(QuestTab tab)
{
    switch (tab) {
    case QTAB_ACTIVE:    return QSTAT_ACTIVE;
    case QTAB_AVAILABLE: return QSTAT_AVAILABLE;
    case QTAB_COMPLETED: return QSTAT_COMPLETED;
    case QTAB_SECRET:    return QSTAT_HIDDEN;
    default:             return QSTAT_LOCKED;
    }
}

/* ---- Rebuild list -------------------------------------- */
void menu_quest_rebuild_list(void)
{
    u8 i;
    QuestStatus want = _tab_to_status(g_quest_menu.tab);
    g_quest_menu.list_count = 0;

    for (i = 0; i < QUEST_COUNT; i++) {
        QuestStatus s = quest_status(i);
        if (s == want) {
            /* For secrets tab only show revealed secrets */
            if (want == QSTAT_HIDDEN && !quest_secret_revealed(i)) continue;
            g_quest_menu.list[g_quest_menu.list_count++] = i;
        }
    }
    if (g_quest_menu.cursor >= g_quest_menu.list_count && g_quest_menu.list_count > 0)
        g_quest_menu.cursor = (u8)(g_quest_menu.list_count - 1);
    if (g_quest_menu.list_count == 0) g_quest_menu.cursor = 0;
}

/* ---- Open ---------------------------------------------- */

void menu_quest_open(void)
{
    g_quest_menu.tab    = QTAB_ACTIVE;
    g_quest_menu.cursor = 0;
    g_quest_menu.scroll = 0;
    menu_quest_rebuild_list();
    s_qp = 0;
    s_qc = 0xFFFF;
}

/* ---- Update -------------------------------------------- */

void menu_quest_update(void)
{
    u16 pressed = _qpressed();
    u8  lc      = g_quest_menu.list_count;

    if (pressed & KEY_L) {
        if (g_quest_menu.tab > 0) g_quest_menu.tab--;
        else g_quest_menu.tab = (QuestTab)(QTAB_COUNT - 1);
        g_quest_menu.cursor = 0;
        g_quest_menu.scroll = 0;
        menu_quest_rebuild_list();
        g_menu.dirty = TRUE;
    }
    if (pressed & KEY_R) {
        if (g_quest_menu.tab < QTAB_COUNT - 1) g_quest_menu.tab++;
        else g_quest_menu.tab = QTAB_ACTIVE;
        g_quest_menu.cursor = 0;
        g_quest_menu.scroll = 0;
        menu_quest_rebuild_list();
        g_menu.dirty = TRUE;
    }

    if (lc == 0) return;

    if (pressed & KEY_UP) {
        if (g_quest_menu.cursor > 0) g_quest_menu.cursor--;
        else g_quest_menu.cursor = (u8)(lc - 1);
        g_menu.dirty = TRUE;
    }
    if (pressed & KEY_DOWN) {
        if (g_quest_menu.cursor < lc - 1) g_quest_menu.cursor++;
        else g_quest_menu.cursor = 0;
        g_menu.dirty = TRUE;
    }

    /* Scroll */
    if (g_quest_menu.cursor < g_quest_menu.scroll)
        g_quest_menu.scroll = g_quest_menu.cursor;
    if (g_quest_menu.cursor >= g_quest_menu.scroll + QUEST_VISIBLE_ROWS)
        g_quest_menu.scroll = (u8)(g_quest_menu.cursor - QUEST_VISIBLE_ROWS + 1);
}

/* ---- Category icon ------------------------------------- */
static u16 _qcat_icon(QuestCategory cat)
{
    switch (cat) {
    case QCAT_MAIN:   return TILE_ICON_STAR;
    case QCAT_SIDE:   return TILE_ICON_SCROLL;
    case QCAT_SECRET: return TILE_ICON_SKULL;
    default:          return TILE_ICON_QUEST;
    }
}

/* ---- Render -------------------------------------------- */

void menu_quest_render(void)
{
    u8 i;
    u8 sel_quest_id = 0xFF;

    ui_draw_window_titled(0, 0, 30, 20, "QUEST LOG");

    /* Quest count badge */
    {
        u8 total = quest_count_by_status(QSTAT_ACTIVE, 0xFF);
        ui_write_num(24, 1, (s32)total, PAL_TEXT);
        ui_write_char(26, 1, '/', PAL_DIM);
        ui_write_num(27, 1, (s32)QUEST_COUNT, PAL_DIM);
    }

    /* Tabs row 3 */
    {
        u8 tx = 2;
        for (i = 0; i < QTAB_COUNT; i++) {
            u8 pal = (i == (u8)g_quest_menu.tab) ? PAL_HI : PAL_DIM;
            ui_write_str(tx, 3, s_qtab_labels[i], pal);
            tx += 8;
        }
    }

    /* Divider row 4 */
    for (i = 1; i < 29; i++)
        UI_SB[4 * 32 + i] = SE_ENTRY(TILE_WIN_TOP, PAL_BORDER);

    /* Quest list rows 5-12 */
    {
        u8 vis = (g_quest_menu.list_count < QUEST_VISIBLE_ROWS)
                  ? g_quest_menu.list_count : QUEST_VISIBLE_ROWS;
        if (g_quest_menu.list_count == 0) {
            ui_write_str(3, 6, "(none)", PAL_DIM);
        }
        for (i = 0; i < vis; i++) {
            u8 li     = (u8)(g_quest_menu.scroll + i);
            u8 qid    = g_quest_menu.list[li];
            u8 row    = (u8)(5 + i);
            BOOL sel  = (BOOL)(li == g_quest_menu.cursor);
            u8 pal    = sel ? PAL_HI : PAL_TEXT;
            const QuestDef* qd = quest_def_get(qid);

            if (sel) {
                ui_draw_cursor(1, row);
                sel_quest_id = qid;
            }
            if (qd) {
                UI_SB[row * 32 + 3] = SE_ENTRY(_qcat_icon(qd->category), pal);
                ui_write_str(5, row, qd->title, pal);
                /* Progress X/Y */
                ui_write_str(25, row, quest_progress_str(qid), PAL_DIM);
            }
        }
    }

    /* Divider row 13 */
    for (i = 1; i < 29; i++)
        UI_SB[13 * 32 + i] = SE_ENTRY(TILE_WIN_TOP, PAL_BORDER);

    /* Objective panel rows 14-17 */
    if (sel_quest_id != 0xFF) {
        const QuestDef* qd = quest_def_get(sel_quest_id);
        if (qd) {
            u8 obj_shown = 0;
            for (i = 0; i < QUEST_OBJ_MAX && obj_shown < QUEST_OBJ_DISPLAY; i++) {
                const QuestObjDef* obj = &qd->objectives[i];
                u8 row;
                BOOL done;
                if (obj->type == QOBJ_NONE) break;
                row  = (u8)(14 + obj_shown);
                done = quest_obj_done(sel_quest_id, i);
                /* Bullet: filled = done */
                UI_SB[row * 32 + 2] = done
                    ? SE_ENTRY(TILE_ICON_CHECKMARK, PAL_AVAIL)
                    : SE_ENTRY(TILE_ICON_QUEST,     PAL_ACTIVE);
                ui_write_str(4, row, obj->description, done ? PAL_DIM : PAL_TEXT);
                /* Progress bar (right side, 8 tiles wide) */
                if (obj->required > 1) {
                    s16 prog = (s16)quest_obj_progress(sel_quest_id, i);
                    ui_draw_bar(20, row, 8, prog, (s16)obj->required, PAL_HP);
                }
                obj_shown++;
            }
        }
    }

    /* Reward row 18 */
    if (sel_quest_id != 0xFF) {
        const QuestDef* qd = quest_def_get(sel_quest_id);
        if (qd) {
            ui_write_str(2, 18, "Reward:", PAL_DIM);
            UI_SB[18 * 32 + 10] = SE_ENTRY(TILE_ICON_COIN, PAL_HI);
            ui_write_num(11, 18, (s32)qd->reward.gold, PAL_HI);
            UI_SB[18 * 32 + 17] = SE_ENTRY(TILE_ICON_EXP, PAL_HI);
            ui_write_num(18, 18, (s32)qd->reward.xp, PAL_HI);
        }
    }

    ui_write_str(2, 19, "L/R:Tab  B:Back", PAL_DIM);
}
