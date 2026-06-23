#ifndef QUEST_TYPES_H
#define QUEST_TYPES_H

#include "../../gba/gba_types.h"

/* =========================================================
 * Quest System — Shared Types
 *
 * 28 quests total:
 *   IDs  0- 9  Main quests      (história principal)
 *   IDs 10-21  Side quests      (missões opcionais)
 *   IDs 22-27  Secret quests    (missões ocultas)
 *
 * Each quest has up to QUEST_OBJ_MAX objectives.
 * Progress is tracked per-objective (u8 counter).
 * State fits in SaveSlot.extra[] (156 bytes used).
 * ========================================================= */

#define QUEST_COUNT        28
#define QUEST_MAIN_COUNT   10
#define QUEST_SIDE_COUNT   12
#define QUEST_SECRET_COUNT  6
#define QUEST_OBJ_MAX       4

/* ---- Quest IDs ------------------------------------------ */

/* Main quests */
#define QMAIN_AWAKENING      0   /* O Despertar          */
#define QMAIN_GUILD_JOIN     1   /* Ingresso na Guilda   */
#define QMAIN_FIRST_DUNGEON  2   /* A Primeira Masmorra  */
#define QMAIN_CRYSTAL_HUNT   3   /* Caça aos Cristais    */
#define QMAIN_BETRAYAL       4   /* A Traição            */
#define QMAIN_ANCIENT_RUIN   5   /* Ruínas Antigas       */
#define QMAIN_DRAGON_HUNT    6   /* O Dragão Negro       */
#define QMAIN_AETHERIA_GATE  7   /* Portal de Aetheria   */
#define QMAIN_FINAL_TRIALS   8   /* Provas Finais        */
#define QMAIN_AETHERIA_END   9   /* O Fim de Aetheria    */

/* Side quests */
#define QSIDE_LOST_CAT       10  /* O Gato Perdido       */
#define QSIDE_HERB_GATHER    11  /* Coleta de Ervas      */
#define QSIDE_BANDIT_HUNT    12  /* Caçada aos Bandidos  */
#define QSIDE_SMITH_DELIVERY 13  /* Entrega do Ferreiro  */
#define QSIDE_HAUNTED_INN    14  /* Estalagem Amaldiçoada*/
#define QSIDE_GUILD_TRIALS   15  /* Provas da Guilda     */
#define QSIDE_DRAGON_SCALE   16  /* Escamas do Dragão    */
#define QSIDE_ANCIENT_CODEX  17  /* Codex Perdido        */
#define QSIDE_ESCORT         18  /* Escolta do Mercador  */
#define QSIDE_MONSTER_HUNT   19  /* Pesquisa de Monstros */
#define QSIDE_LEGENDARY_FORGE 20 /* A Forja Lendária     */
#define QSIDE_CHAMPIONS      21  /* Torneio de Campeões  */

/* Secret quests */
#define QSECRET_SHADOW_BLADE 22  /* Lâmina das Sombras   */
#define QSECRET_ARCANE_TOME  23  /* Tomo Arcano          */
#define QSECRET_BETRAYER     24  /* Passado do Traidor   */
#define QSECRET_VOID_WALKER  25  /* Andarilho do Vazio   */
#define QSECRET_TRUE_KING    26  /* O Verdadeiro Rei     */
#define QSECRET_GENESIS      27  /* Gênesis de Aetheria  */

/* ---- Quest status ---------------------------------------- */
typedef enum {
    QSTAT_LOCKED      = 0, /* prerequisites not met */
    QSTAT_HIDDEN      = 1, /* secret: exists but undiscovered */
    QSTAT_AVAILABLE   = 2, /* can be accepted */
    QSTAT_ACTIVE      = 3, /* accepted, in progress */
    QSTAT_COMPLETED   = 4, /* all objectives done + reward given */
    QSTAT_FAILED      = 5, /* failed (some quests can fail) */
} QuestStatus;

/* ---- Quest category ------------------------------------- */
typedef enum {
    QCAT_MAIN   = 0,
    QCAT_SIDE   = 1,
    QCAT_SECRET = 2,
} QuestCategory;

/* ---- Objective types ------------------------------------ */
typedef enum {
    QOBJ_NONE        = 0,   /* unused slot */
    QOBJ_KILL,              /* kill N of enemy type X */
    QOBJ_KILL_ANY,          /* kill N enemies (any type) */
    QOBJ_COLLECT,           /* have N of item X in bag */
    QOBJ_TALK,              /* talk to NPC with script_id X */
    QOBJ_REACH_MAP,         /* step into map X */
    QOBJ_WIN_BATTLE,        /* win N battles (any) */
    QOBJ_CRAFT,             /* craft recipe X at least once */
    QOBJ_BUY,              /* buy item X at least once */
    QOBJ_EQUIP_ITEM,        /* equip item X on any character */
    QOBJ_SCRIPTED,          /* manually ticked by quest_notify() */
    QOBJ_ESCORT_SAFE,       /* escort flag set (scripted) */
} QuestObjType;

/* ---- Objective descriptor (ROM) ------------------------- */
typedef struct {
    const char*  description;
    QuestObjType type;
    u8           target_id;  /* enemy_id / item_id / npc_script / map_id / recipe_id */
    u8           required;   /* quantity needed (0 = trigger once = 1 effective) */
} QuestObjDef;

/* ---- Reward (ROM) --------------------------------------- */
typedef struct {
    u32 gold;
    u32 xp;                  /* split equally among party */
    u8  item_ids[3];         /* ITEM_NONE = no item */
    u8  item_qtys[3];
    u8  unlock_quest_id;     /* quest to unlock on completion (0xFF=none) */
    u8  reveal_secret_id;    /* secret quest to reveal (0xFF=none) */
    u32 set_world_flags;     /* WorldFlags.switches bits to set */
    u32 set_items_obtained;  /* WorldFlags.items_obtained bits to set */
} QuestReward;

/* ---- Prerequisite (ROM) --------------------------------- */
typedef struct {
    u8   req_quest_id;      /* 0xFF = no quest requirement */
    u8   req_min_level;     /* 0 = no level requirement */
    u32  req_world_flags;   /* all these flags must be set */
    u8   req_item_id;       /* ITEM_NONE = no item requirement */
} QuestPrereq;

/* ---- Full quest definition (ROM) ------------------------ */
typedef struct {
    const char*   title;
    const char*   description;    /* shown in quest log */
    const char*   giver_name;     /* NPC/entity that gives it */
    QuestCategory category;
    u8            id;
    u8            difficulty;     /* 1-5 stars */
    QuestObjDef   objectives[QUEST_OBJ_MAX];
    QuestReward   reward;
    QuestPrereq   prereq;
    BOOL          auto_accept;    /* TRUE = no dialogue needed */
    BOOL          can_fail;       /* TRUE = can enter FAILED state */
    u8            time_limit;     /* 0 = no limit, else in "days" */
} QuestDef;

/* ---- Notification event types (for quest_notify) -------- */
typedef enum {
    QEVT_ENEMY_KILLED,     /* param0=enemy_id */
    QEVT_BATTLE_WON,       /* param0=group_id */
    QEVT_ITEM_OBTAINED,    /* param0=item_id, param1=qty */
    QEVT_NPC_TALKED,       /* param0=script_id */
    QEVT_MAP_ENTERED,      /* param0=map_id */
    QEVT_ITEM_CRAFTED,     /* param0=recipe_id */
    QEVT_ITEM_BOUGHT,      /* param0=item_id */
    QEVT_ITEM_EQUIPPED,    /* param0=item_id */
    QEVT_SCRIPT_FLAG,      /* param0=quest_id, param1=obj_idx */
    QEVT_ESCORT_SUCCESS,   /* param0=quest_id */
} QuestEventType;

#endif /* QUEST_TYPES_H */
