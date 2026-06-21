/* Chronicles of Aetheria — Dialogue & Script Database (ROM) */

#include "../../include/game/dialogue/dialogue_db.h"
#include "../../include/game/item/item_db.h"

/* =========================================================
 * Text table — DialogueText[DIALOGUE_TEXT_COUNT]
 * Each entry has up to 4 lines of 32 chars.
 * ========================================================= */
const DialogueText g_text_table[DIALOGUE_TEXT_COUNT] = {

[TXT_INNKEEPER_GREET] = {
    { "Welcome, traveller!",
      "A warm bed costs 50 gold.",
      "Shall I prepare your room?",
      NULL },
    3
},
[TXT_INNKEEPER_REST] = {
    { "Sweet dreams!",
      "Your party rests well.",
      "HP and MP restored.",
      NULL },
    3
},
[TXT_INNKEEPER_FAREWELL] = {
    { "Safe travels!",
      "Come back anytime.",
      NULL, NULL },
    2
},
[TXT_SHOPKEEPER_GREET] = {
    { "Welcome to my shop!",
      "Browse freely.",
      NULL, NULL },
    2
},
[TXT_SHOPKEEPER_BYE] = {
    { "Come again!",
      NULL, NULL, NULL },
    1
},
[TXT_GUARD_BLOCK] = {
    { "Halt! This road is closed",
      "by order of the Elder.",
      "Turn back, stranger.",
      NULL },
    3
},
[TXT_TOWNFOLK_1] = {
    { "I hear monsters have been",
      "getting bolder lately...",
      "Be careful out there.",
      NULL },
    3
},
[TXT_TOWNFOLK_2] = {
    { "The Verdant Forest hasn't",
      "felt right since the dark",
      "presence appeared.",
      NULL },
    3
},
[TXT_TOWNFOLK_3] = {
    { "A sage lives in the ruins",
      "to the east. They say he",
      "knows of the Void Lord.",
      NULL },
    3
},
[TXT_TOWNFOLK_4] = {
    { "My husband went to the",
      "Crystal Caves three days",
      "ago. He hasn't returned.",
      NULL },
    3
},
[TXT_SIGN_AELWYN] = {
    { "AELWYN VILLAGE",
      "Population: 247",
      "Founded 412 A.E.",
      NULL },
    3
},
[TXT_SIGN_FOREST] = {
    { "VERDANT FOREST",
      "Danger: MODERATE",
      "Beware of spiders.",
      NULL },
    3
},
[TXT_SIGN_CAVE] = {
    { "CRYSTAL CAVES",
      "Danger: HIGH",
      "Light sources recommended.",
      NULL },
    3
},
[TXT_SIGN_SWAMP] = {
    { "MURKHAVEN SWAMP",
      "Danger: HIGH",
      "Antidotes advised.",
      NULL },
    3
},
[TXT_SIGN_MOUNTAINS] = {
    { "IRONPEAK MOUNTAINS",
      "Danger: VERY HIGH",
      "Access at own risk.",
      NULL },
    3
},

/* Main story NPCs */
[TXT_ELDER_INTRO] = {
    { "Kael... you've returned.",
      "A dark wind blows from the",
      "east. Aetheria is in peril.",
      NULL },
    3
},
[TXT_ELDER_QUEST] = {
    { "The five Aether Shards must",
      "be gathered before the Void",
      "Lord completes the ritual.",
      "Will you undertake this?" },
    4
},
[TXT_ELDER_AFTER] = {
    { "The fate of Aetheria rests",
      "in your hands. Go swiftly,",
      "and may the light guide you.",
      NULL },
    3
},
[TXT_MIRA_JOIN] = {
    { "Kael! I knew you'd come.",
      "Father wants me to stay but",
      "I won't sit by while Aetheria",
      "burns. I'm coming with you." },
    4
},
[TXT_THERON_JOIN] = {
    { "I've been hunting bandits",
      "on this road for a week.",
      "Sounds like you need a",
      "shield. I'm Theron." },
    4
},
[TXT_SYLA_JOIN] = {
    { "A hunter travels lighter",
      "alone... but this prey is",
      "beyond any one hunter.",
      "Count me in, Kael." },
    4
},
[TXT_SAGE_RUINS] = {
    { "The Void Lord was sealed",
      "here a thousand years ago.",
      "Five shards hold the seal.",
      "Each shard... was scattered." },
    4
},
[TXT_SAGE_CLUE] = {
    { "The first shard lies within",
      "the Serpentis, deep in the",
      "swamp. The creature guards",
      "it without knowing why." },
    4
},
[TXT_PROPHET_VISION] = {
    { "I see it... five lights",
      "extinguished one by one...",
      "and then a darkness that",
      "swallows everything." },
    4
},
[TXT_PROPHET_WARNING] = {
    { "The Void Lord grows strong",
      "each day the shards remain",
      "scattered. You must hurry,",
      "chosen one. Time is short." },
    4
},

/* Boss pre-battle */
[TXT_BOSS_SERPENTIS] = {
    { "SSSSSS... interlopersss...",
      "The shard is MINE.",
      "I will crush you beneath",
      "a thousand coils!" },
    4
},
[TXT_BOSS_TREANT] = {
    { "INTRUDERS... the forest",
      "WEEPS at your presence.",
      "I am its WRATH.",
      "Turn back or be consumed!" },
    4
},
[TXT_BOSS_LICH] = {
    { "Fools! You dare disturb",
      "the eternal rest of Mordael?",
      "I have cheated death three",
      "centuries — I'll cheat it more!" },
    4
},
[TXT_BOSS_INFERNO] = {
    { "*ROOOAARRR*",
      "Tiny things that burn...",
      "I will show you TRUE fire.",
      "THIS MOUNTAIN IS MY THRONE!" },
    4
},
[TXT_BOSS_VOID_LORD] = {
    { "At last. The insects arrive.",
      "I have waited a millennium",
      "for this moment. Kneel...",
      "or be unmade like all else." },
    4
},

/* Misc */
[TXT_FOUND_ITEM] = {
    { "You found something!",
      NULL, NULL, NULL },
    1
},
[TXT_CHEST_EMPTY] = {
    { "The chest is empty.",
      NULL, NULL, NULL },
    1
},
[TXT_DUNGEON_KEY] = {
    { "You found a dungeon key!",
      "Doors in this area can",
      "now be unlocked.",
      NULL },
    3
},
[TXT_HEAL_SPRING] = {
    { "A healing spring!",
      "The party's HP and MP",
      "are fully restored.",
      NULL },
    3
},
[TXT_SAVE_POINT] = {
    { "A save statue.",
      "Your journey is recorded",
      "in stone.",
      NULL },
    3
},

/* Quests */
[TXT_QUEST_HERB_GIVE] = {
    { "My daughter is ill.",
      "I need 5 Herbs from the",
      "forest. Please help us!",
      NULL },
    3
},
[TXT_QUEST_HERB_DONE] = {
    { "You found them! Thank you!",
      "Take this as reward.",
      NULL, NULL },
    2
},
[TXT_QUEST_BANDIT_GIVE] = {
    { "Bandits took our shipment",
      "near the forest road.",
      "Can you recover it?",
      NULL },
    3
},
[TXT_QUEST_BANDIT_DONE] = {
    { "You recovered everything!",
      "The merchant's guild thanks",
      "you. Here's your reward.",
      NULL },
    3
},
[TXT_QUEST_RELIC_GIVE] = {
    { "An ancient relic was stolen",
      "from the ruins museum.",
      "Bring it back to me,",
      "and I'll reward you well." },
    4
},
[TXT_QUEST_RELIC_DONE] = {
    { "The Aether Relic! You found",
      "it! This belongs in the",
      "museum, not a thief's bag.",
      "Accept this reward." },
    4
},

}; /* end g_text_table */

/* =========================================================
 * Script definitions — ScriptCmd arrays
 * ========================================================= */

/* Innkeeper: greet → (A press) rest → farewell */
static const ScriptCmd s_scr_innkeeper[] = {
    { SCMD_FACE_PLAYER, 0, 0 },
    { SCMD_DIALOGUE, TXT_INNKEEPER_GREET, 0 },
    { SCMD_HEAL, 0, 0 },
    { SCMD_DIALOGUE, TXT_INNKEEPER_REST, 0 },
    { SCMD_DIALOGUE, TXT_INNKEEPER_FAREWELL, 0 },
    { SCMD_END, 0, 0 },
};

/* Shopkeeper Aelwyn */
static const ScriptCmd s_scr_shopkeeper_aelwyn[] = {
    { SCMD_FACE_PLAYER, 0, 0 },
    { SCMD_DIALOGUE, TXT_SHOPKEEPER_GREET, 0 },
    { SCMD_END, 0, 0 },
};

/* Elder — checks flag 0 (have we met the elder?) */
static const ScriptCmd s_scr_elder_intro[] = {
    { SCMD_FACE_PLAYER, 0, 0 },
    { SCMD_FLAG_BRANCH, 0, 4 },    /* if flag 0 set, jump to label 4 */
    { SCMD_DIALOGUE, TXT_ELDER_INTRO, 0 },
    { SCMD_DIALOGUE, TXT_ELDER_QUEST, 0 },
    { SCMD_FLAG_SET, 0, 0 },
    { SCMD_JUMP, 0, 5 },
    { SCMD_LABEL, 4, 0 },
    { SCMD_DIALOGUE, TXT_ELDER_AFTER, 0 },
    { SCMD_LABEL, 5, 0 },
    { SCMD_END, 0, 0 },
};

/* Elder quest follow-up */
static const ScriptCmd s_scr_elder_quest[] = {
    { SCMD_FACE_PLAYER, 0, 0 },
    { SCMD_DIALOGUE, TXT_ELDER_AFTER, 0 },
    { SCMD_END, 0, 0 },
};

/* Mira joins — flag 1 */
static const ScriptCmd s_scr_mira_join[] = {
    { SCMD_FACE_PLAYER, 0, 0 },
    { SCMD_FLAG_BRANCH, 1, 2 },
    { SCMD_DIALOGUE, TXT_MIRA_JOIN, 0 },
    { SCMD_FLAG_SET, 1, 0 },
    { SCMD_LABEL, 2, 0 },
    { SCMD_END, 0, 0 },
};

/* Theron joins — flag 2 */
static const ScriptCmd s_scr_theron_join[] = {
    { SCMD_FACE_PLAYER, 0, 0 },
    { SCMD_FLAG_BRANCH, 2, 2 },
    { SCMD_DIALOGUE, TXT_THERON_JOIN, 0 },
    { SCMD_FLAG_SET, 2, 0 },
    { SCMD_LABEL, 2, 0 },
    { SCMD_END, 0, 0 },
};

/* Syla joins — flag 3 */
static const ScriptCmd s_scr_syla_join[] = {
    { SCMD_FACE_PLAYER, 0, 0 },
    { SCMD_FLAG_BRANCH, 3, 2 },
    { SCMD_DIALOGUE, TXT_SYLA_JOIN, 0 },
    { SCMD_FLAG_SET, 3, 0 },
    { SCMD_LABEL, 2, 0 },
    { SCMD_END, 0, 0 },
};

/* Guard block */
static const ScriptCmd s_scr_guard_block[] = {
    { SCMD_FACE_PLAYER, 0, 0 },
    { SCMD_DIALOGUE, TXT_GUARD_BLOCK, 0 },
    { SCMD_END, 0, 0 },
};

/* Generic townfolk A */
static const ScriptCmd s_scr_townfolk_a[] = {
    { SCMD_FACE_PLAYER, 0, 0 },
    { SCMD_DIALOGUE, TXT_TOWNFOLK_1, 0 },
    { SCMD_END, 0, 0 },
};

/* Generic townfolk B */
static const ScriptCmd s_scr_townfolk_b[] = {
    { SCMD_FACE_PLAYER, 0, 0 },
    { SCMD_DIALOGUE, TXT_TOWNFOLK_2, 0 },
    { SCMD_END, 0, 0 },
};

/* Signs */
static const ScriptCmd s_scr_sign_aelwyn[] = {
    { SCMD_DIALOGUE, TXT_SIGN_AELWYN, 0 },
    { SCMD_END, 0, 0 },
};
static const ScriptCmd s_scr_sign_forest[] = {
    { SCMD_DIALOGUE, TXT_SIGN_FOREST, 0 },
    { SCMD_END, 0, 0 },
};
static const ScriptCmd s_scr_sign_cave[] = {
    { SCMD_DIALOGUE, TXT_SIGN_CAVE, 0 },
    { SCMD_END, 0, 0 },
};

/* Chest: Dungeon Key 1 — flag 10 (opened) */
static const ScriptCmd s_scr_chest_iron_key[] = {
    { SCMD_FLAG_BRANCH, 10, 3 },
    { SCMD_DIALOGUE, TXT_DUNGEON_KEY, 0 },
    { SCMD_GIVE_ITEM, ITEM_DUNGEON_KEY_1, 1 },
    { SCMD_FLAG_SET, 10, 0 },
    { SCMD_JUMP, 0, 4 },
    { SCMD_LABEL, 3, 0 },
    { SCMD_DIALOGUE, TXT_CHEST_EMPTY, 0 },
    { SCMD_LABEL, 4, 0 },
    { SCMD_END, 0, 0 },
};

/* Healing spring — always heals */
static const ScriptCmd s_scr_heal_spring[] = {
    { SCMD_DIALOGUE, TXT_HEAL_SPRING, 0 },
    { SCMD_HEAL, 0, 0 },
    { SCMD_END, 0, 0 },
};

/* Sage in ruins */
static const ScriptCmd s_scr_sage_ruins[] = {
    { SCMD_FACE_PLAYER, 0, 0 },
    { SCMD_DIALOGUE, TXT_SAGE_RUINS, 0 },
    { SCMD_DIALOGUE, TXT_SAGE_CLUE, 0 },
    { SCMD_END, 0, 0 },
};

/* Prophet */
static const ScriptCmd s_scr_prophet[] = {
    { SCMD_FACE_PLAYER, 0, 0 },
    { SCMD_SHAKE, 2, 20 },
    { SCMD_DIALOGUE, TXT_PROPHET_VISION, 0 },
    { SCMD_DIALOGUE, TXT_PROPHET_WARNING, 0 },
    { SCMD_END, 0, 0 },
};

/* Boss pre-battle scripts */
static const ScriptCmd s_scr_boss_serpentis[] = {
    { SCMD_SHAKE, 3, 30 },
    { SCMD_DIALOGUE, TXT_BOSS_SERPENTIS, 0 },
    { SCMD_BATTLE, 0, 3 },   /* boss group id = 3 (placeholder) */
    { SCMD_END, 0, 0 },
};
static const ScriptCmd s_scr_boss_treant[] = {
    { SCMD_SHAKE, 3, 30 },
    { SCMD_DIALOGUE, TXT_BOSS_TREANT, 0 },
    { SCMD_BATTLE, 1, 3 },
    { SCMD_END, 0, 0 },
};
static const ScriptCmd s_scr_boss_lich[] = {
    { SCMD_SHAKE, 4, 40 },
    { SCMD_DIALOGUE, TXT_BOSS_LICH, 0 },
    { SCMD_BATTLE, 2, 3 },
    { SCMD_END, 0, 0 },
};
static const ScriptCmd s_scr_boss_inferno[] = {
    { SCMD_SHAKE, 5, 60 },
    { SCMD_SOUND, 10, 0 },
    { SCMD_DIALOGUE, TXT_BOSS_INFERNO, 0 },
    { SCMD_BATTLE, 3, 3 },
    { SCMD_END, 0, 0 },
};
static const ScriptCmd s_scr_boss_void_lord[] = {
    { SCMD_WEATHER, 3, 0 },
    { SCMD_SHAKE, 6, 90 },
    { SCMD_DIALOGUE, TXT_BOSS_VOID_LORD, 0 },
    { SCMD_BATTLE, 4, 3 },
    { SCMD_END, 0, 0 },
};

/* Quest: herb gathering — flag 20 (accepted/done) */
static const ScriptCmd s_scr_quest_herb[] = {
    { SCMD_FACE_PLAYER, 0, 0 },
    { SCMD_FLAG_BRANCH, 20, 2 },
    { SCMD_DIALOGUE, TXT_QUEST_HERB_GIVE, 0 },
    { SCMD_FLAG_SET, 20, 0 },
    { SCMD_LABEL, 2, 0 },
    { SCMD_END, 0, 0 },
};

/* Quest: bandit cargo */
static const ScriptCmd s_scr_quest_bandit[] = {
    { SCMD_FACE_PLAYER, 0, 0 },
    { SCMD_FLAG_BRANCH, 21, 2 },
    { SCMD_DIALOGUE, TXT_QUEST_BANDIT_GIVE, 0 },
    { SCMD_FLAG_SET, 21, 0 },
    { SCMD_LABEL, 2, 0 },
    { SCMD_END, 0, 0 },
};

/* Quest: relic recovery */
static const ScriptCmd s_scr_quest_relic[] = {
    { SCMD_FACE_PLAYER, 0, 0 },
    { SCMD_FLAG_BRANCH, 22, 2 },
    { SCMD_DIALOGUE, TXT_QUEST_RELIC_GIVE, 0 },
    { SCMD_FLAG_SET, 22, 0 },
    { SCMD_LABEL, 2, 0 },
    { SCMD_END, 0, 0 },
};

/* Save statue */
static const ScriptCmd s_scr_save_statue[] = {
    { SCMD_DIALOGUE, TXT_SAVE_POINT, 0 },
    { SCMD_END, 0, 0 },
};

/* ---- Master script pointer table ------------------------ */
const ScriptCmd* const g_script_table[SCRIPT_COUNT] = {
    [SCR_INNKEEPER]         = s_scr_innkeeper,
    [SCR_SHOPKEEPER_AELWYN] = s_scr_shopkeeper_aelwyn,
    [SCR_ELDER_INTRO]       = s_scr_elder_intro,
    [SCR_ELDER_QUEST]       = s_scr_elder_quest,
    [SCR_MIRA_JOIN]         = s_scr_mira_join,
    [SCR_THERON_JOIN]       = s_scr_theron_join,
    [SCR_SYLA_JOIN]         = s_scr_syla_join,
    [SCR_GUARD_BLOCK]       = s_scr_guard_block,
    [SCR_TOWNFOLK_A]        = s_scr_townfolk_a,
    [SCR_TOWNFOLK_B]        = s_scr_townfolk_b,
    [SCR_SIGN_AELWYN]       = s_scr_sign_aelwyn,
    [SCR_SIGN_FOREST]       = s_scr_sign_forest,
    [SCR_SIGN_CAVE]         = s_scr_sign_cave,
    [SCR_CHEST_IRON_KEY]    = s_scr_chest_iron_key,
    [SCR_HEAL_SPRING]       = s_scr_heal_spring,
    [SCR_SAGE_RUINS]        = s_scr_sage_ruins,
    [SCR_PROPHET]           = s_scr_prophet,
    [SCR_BOSS_SERPENTIS]    = s_scr_boss_serpentis,
    [SCR_BOSS_TREANT]       = s_scr_boss_treant,
    [SCR_BOSS_LICH]         = s_scr_boss_lich,
    [SCR_BOSS_INFERNO]      = s_scr_boss_inferno,
    [SCR_BOSS_VOID_LORD]    = s_scr_boss_void_lord,
    [SCR_QUEST_HERB]        = s_scr_quest_herb,
    [SCR_QUEST_BANDIT]      = s_scr_quest_bandit,
    [SCR_QUEST_RELIC]       = s_scr_quest_relic,
    [SCR_SAVE_STATUE]       = s_scr_save_statue,
};

/* ---- Init (wires tables into interaction system) -------- */
void dialogue_db_init(void)
{
    interact_init(g_script_table, g_text_table, SCRIPT_COUNT, DIALOGUE_TEXT_COUNT);
}
