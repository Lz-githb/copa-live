#ifndef MENU_SAVE_H
#define MENU_SAVE_H

#include "menu_types.h"
#include "save.h"

/* =========================================================
 * Save / Load Screen
 *
 * Layout (30x20 tiles):
 *
 *  ┌─────────────────────────────┐  row 0
 *  │        SAVE / LOAD          │  row 1
 *  ├──────────────┬──────────────┤  row 2
 *  │    SAVE      │    LOAD      │  row 3  (mode tabs)
 *  ├──────────────┴──────────────┤  row 4
 *  │  ┌──────────────────────┐   │
 *  │  │ Slot 1               │   │  rows 5-8  (slot card)
 *  │  │ [charname]  Lv.XX    │   │
 *  │  │ [location]  HH:MM:SS │   │
 *  │  └──────────────────────┘   │
 *  │  ┌──────────────────────┐   │  rows 9-12
 *  │  │ Slot 2               │   │
 *  │  │ ...                  │   │
 *  │  └──────────────────────┘   │
 *  │  ┌──────────────────────┐   │  rows 13-16
 *  │  │ Slot 3               │   │
 *  │  │ ...                  │   │
 *  │  └──────────────────────┘   │
 *  ├─────────────────────────────┤  row 17
 *  │  [A] Select  [B] Cancel     │  rows 18-19
 *  └─────────────────────────────┘
 *
 * ========================================================= */

typedef enum {
    SAVE_SCREEN_SAVE = 0,
    SAVE_SCREEN_LOAD
} SaveScreenMode;

typedef struct {
    SaveScreenMode  mode;
    u8              cursor;         /* 0-2 slot selection */
    SaveSlot        previews[SAVE_SLOT_COUNT];
    BOOL            slot_valid[SAVE_SLOT_COUNT];
    BOOL            confirming;     /* show overwrite/delete confirm? */
} SaveMenuState;

extern SaveMenuState g_save_menu;

void menu_save_open(SaveScreenMode mode);
void menu_save_update(void);
void menu_save_render(void);

/* Returns TRUE if a slot was acted upon */
BOOL menu_save_is_done(void);

#endif /* MENU_SAVE_H */
