#ifndef MENU_QUEST_H
#define MENU_QUEST_H

#include "menu_types.h"
#include "quest_types.h"
#include "quest_state.h"

/* =========================================================
 * Quest Log Menu
 *
 * Layout (30x20 tiles):
 *
 *  ┌─────────────────────────────┐  row 0
 *  │  QUEST LOG        [n/N]     │  row 1  (title + count)
 *  ├─────────────────────────────┤  row 2
 *  │ ACTIVE │ AVAIL │ DONE │ ?  │  row 3  (tabs)
 *  ├──────┬──────────────────────┤  row 4
 *  │ >Quest Name                 │  rows 5-12  (quest list)
 *  │  Quest Name                 │
 *  │  ...                        │
 *  ├──────────────────────────────┤  row 13
 *  │ ● Obj 1     [progress bar]  │  rows 14-17 (objectives)
 *  │ ○ Obj 2     [progress bar]  │
 *  │ ○ Obj 3     [progress bar]  │
 *  ├──────────────────────────────┤  row 18
 *  │ Reward: [icon] gold / items │  row 19
 *  └─────────────────────────────┘
 *
 * Tabs: ACTIVE | AVAILABLE | COMPLETED | SECRET
 * ========================================================= */

#define QUEST_VISIBLE_ROWS  8
#define QUEST_OBJ_DISPLAY   3

typedef enum {
    QTAB_ACTIVE = 0,
    QTAB_AVAILABLE,
    QTAB_COMPLETED,
    QTAB_SECRET,
    QTAB_COUNT
} QuestTab;

typedef struct {
    QuestTab  tab;
    u8        list[QUEST_COUNT];    /* quest ids matching current tab */
    u8        list_count;
    u8        cursor;
    u8        scroll;
} QuestMenuState;

extern QuestMenuState g_quest_menu;

void menu_quest_open(void);
void menu_quest_update(void);
void menu_quest_render(void);
void menu_quest_rebuild_list(void);

#endif /* MENU_QUEST_H */
