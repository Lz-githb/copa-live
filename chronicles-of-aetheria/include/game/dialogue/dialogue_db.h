#ifndef DIALOGUE_DB_H
#define DIALOGUE_DB_H

#include "../interaction.h"

/* =========================================================
 * Dialogue Database
 *
 * Text IDs — DialogueText entries (up to 4 lines × 32 chars)
 * Script IDs — ScriptCmd arrays in ROM
 *
 * Naming convention:
 *   TXT_*   — text_id constants
 *   SCR_*   — script_id constants
 * ========================================================= */

/* ---- Text IDs ------------------------------------------- */

/* Generic / town */
#define TXT_INNKEEPER_GREET     0
#define TXT_INNKEEPER_REST      1
#define TXT_INNKEEPER_FAREWELL  2
#define TXT_SHOPKEEPER_GREET    3
#define TXT_SHOPKEEPER_BYE      4
#define TXT_GUARD_BLOCK         5
#define TXT_TOWNFOLK_1          6
#define TXT_TOWNFOLK_2          7
#define TXT_TOWNFOLK_3          8
#define TXT_TOWNFOLK_4          9
#define TXT_SIGN_AELWYN        10
#define TXT_SIGN_FOREST        11
#define TXT_SIGN_CAVE          12
#define TXT_SIGN_SWAMP         13
#define TXT_SIGN_MOUNTAINS     14

/* Main story NPCs */
#define TXT_ELDER_INTRO        15
#define TXT_ELDER_QUEST        16
#define TXT_ELDER_AFTER        17
#define TXT_MIRA_JOIN          18
#define TXT_THERON_JOIN        19
#define TXT_SYLA_JOIN          20
#define TXT_SAGE_RUINS         21
#define TXT_SAGE_CLUE          22
#define TXT_PROPHET_VISION     23
#define TXT_PROPHET_WARNING    24

/* Boss pre-battle */
#define TXT_BOSS_SERPENTIS     25
#define TXT_BOSS_TREANT        26
#define TXT_BOSS_LICH          27
#define TXT_BOSS_INFERNO       28
#define TXT_BOSS_VOID_LORD     29

/* Item / reward */
#define TXT_FOUND_ITEM         30
#define TXT_CHEST_EMPTY        31
#define TXT_DUNGEON_KEY        32
#define TXT_HEAL_SPRING        33
#define TXT_SAVE_POINT         34

/* Quest givers */
#define TXT_QUEST_HERB_GIVE    35
#define TXT_QUEST_HERB_DONE    36
#define TXT_QUEST_BANDIT_GIVE  37
#define TXT_QUEST_BANDIT_DONE  38
#define TXT_QUEST_RELIC_GIVE   39
#define TXT_QUEST_RELIC_DONE   40

#define DIALOGUE_TEXT_COUNT    41

/* ---- Script IDs ----------------------------------------- */

#define SCR_NONE               0xFF

#define SCR_INNKEEPER          0
#define SCR_SHOPKEEPER_AELWYN  1
#define SCR_ELDER_INTRO        2
#define SCR_ELDER_QUEST        3
#define SCR_MIRA_JOIN          4
#define SCR_THERON_JOIN        5
#define SCR_SYLA_JOIN          6
#define SCR_GUARD_BLOCK        7
#define SCR_TOWNFOLK_A         8
#define SCR_TOWNFOLK_B         9
#define SCR_SIGN_AELWYN       10
#define SCR_SIGN_FOREST       11
#define SCR_SIGN_CAVE         12
#define SCR_CHEST_IRON_KEY    13
#define SCR_HEAL_SPRING       14
#define SCR_SAGE_RUINS        15
#define SCR_PROPHET           16
#define SCR_BOSS_SERPENTIS    17
#define SCR_BOSS_TREANT       18
#define SCR_BOSS_LICH         19
#define SCR_BOSS_INFERNO      20
#define SCR_BOSS_VOID_LORD    21
#define SCR_QUEST_HERB        22
#define SCR_QUEST_BANDIT      23
#define SCR_QUEST_RELIC       24
#define SCR_SAVE_STATUE       25

#define SCRIPT_COUNT          26

/* ---- ROM tables ----------------------------------------- */
extern const DialogueText  g_text_table[DIALOGUE_TEXT_COUNT];
extern const ScriptCmd* const g_script_table[SCRIPT_COUNT];

void dialogue_db_init(void);

#endif /* DIALOGUE_DB_H */
