#ifndef MENU_STATUS_H
#define MENU_STATUS_H

#include "menu_types.h"
#include "character.h"

/* =========================================================
 * Character Status Screen
 *
 * Layout (30x20 tiles):
 *
 *  ┌──────────────┬──────────────┐  row 0
 *  │  STATUS      │ ◄ Kael  Mira►│  row 1
 *  ├──────────────┤──────────────┤  row 2
 *  │ [sprite 4x6] │ Name  Lv.99  │  rows 3-8
 *  │              │ Class        │
 *  │              │ HP ████ 999  │
 *  │              │ MP ████ 450  │
 *  │              │ EXP ████     │
 *  ├──────────────┼──────────────┤  row 9
 *  │ ATK   999    │ VIT   99     │  rows 10-17 (stats)
 *  │ DEF   999    │ END   99     │
 *  │ MAG   999    │ STR   99     │
 *  │ MDEF  999    │ DEX   99     │
 *  │ SPD    99    │ INT   99     │
 *  │ CRIT   99%   │ WIS   99     │
 *  │ EVA    99%   │ LCK   99     │
 *  │              │ CHA   99     │
 *  ├─────────────────────────────┤  row 18
 *  │ [status icons]              │  row 19
 *  └─────────────────────────────┘
 *
 * ========================================================= */

void menu_status_open(u8 char_idx);
void menu_status_update(void);
void menu_status_render(void);

#endif /* MENU_STATUS_H */
