#include "quest_db.h"
#include "item_db.h"
#include "battle_data.h"
#include "gba_types.h"

/* =========================================================
 * Shorthand macros for readability
 * ========================================================= */

#define OBJ(desc, type, tid, req) { desc, type, tid, req }
#define OBJ0                      { NULL, QOBJ_NONE, 0, 0 }

#define REW(gold, xp, i0,q0, i1,q1, i2,q2, unlock, secret, wflags, iflags) \
    { gold, xp, {i0,i1,i2}, {q0,q1,q2}, unlock, secret, wflags, iflags }

#define PRE(quest, level, flags, item) \
    { quest, level, flags, item }
#define PRE_NONE PRE(0xFF, 0, 0, ITEM_NONE)

/* World flag bits (expand as story content grows) */
#define WF_VILLAGE_SAVED    (1u<<0)
#define WF_GUILD_MEMBER     (1u<<1)
#define WF_DUNGEON1_CLEAR   (1u<<2)
#define WF_CRYSTALS_FOUND   (1u<<3)
#define WF_BETRAYAL_KNOWN   (1u<<4)
#define WF_RUINS_EXPLORED   (1u<<5)
#define WF_DRAGON_SLAIN     (1u<<6)
#define WF_GATE_OPENED      (1u<<7)
#define WF_TRIALS_DONE      (1u<<8)
#define WF_AETHERIA_SAVED   (1u<<9)
#define WF_SHOP_BLACK_OPEN  (1u<<10)
#define WF_FORGE_LEGEND_OPEN (1u<<11)
#define WF_TRUE_KING_KNOWN  (1u<<12)
#define WF_VOID_WALKER      (1u<<13)

/* NPC script IDs referenced in objectives */
#define NSCR_ELDER         1
#define NSCR_GUILDMASTER   2
#define NSCR_BLACKSMITH    3
#define NSCR_ALCHEMIST     4
#define NSCR_INNKEEPER     5
#define NSCR_MERCHANT      6
#define NSCR_ARENA_HOST    7
#define NSCR_MYSTERIOUS_MAN 8
#define NSCR_OLD_SAGE      9
#define NSCR_PRINCESS      10
#define NSCR_BETRAYER      11

/* Map IDs referenced in objectives */
#define MAP_VILLAGE         0
#define MAP_TOWN            1
#define MAP_GUILD_HALL      2
#define MAP_DUNGEON_1       3
#define MAP_RUINS           4
#define MAP_DRAGON_LAIR     5
#define MAP_AETHERIA_GATE   6
#define MAP_VOID_REALM      7
#define MAP_ARENA           8

/* =========================================================
 * MAIN QUESTS (0-9)
 * ========================================================= */

/* ---- QMAIN_AWAKENING (0) ------------------------------- */
static const QuestDef q_awakening = {
    "O Despertar",
    "Algo sombrio acorda nas terras de Aetheria. Saia da vila e "
    "alcance a cidade de Korrath para reportar os eventos ao Conselho.",
    "Ancião da Vila",
    QCAT_MAIN, QMAIN_AWAKENING, 1,
    {
        OBJ("Converse com o Ancião da Vila",   QOBJ_TALK,     NSCR_ELDER,   1),
        OBJ("Alcance a Cidade de Korrath",     QOBJ_REACH_MAP, MAP_TOWN,    1),
        OBJ("Fale com o Mestre da Guilda",     QOBJ_TALK,     NSCR_GUILDMASTER, 1),
        OBJ0,
    },
    REW(200, 300, ITEM_POTION,3, ITEM_ETHER,2, ITEM_NONE,0,
        QMAIN_GUILD_JOIN, 0xFF, WF_VILLAGE_SAVED, 0),
    PRE_NONE,
    TRUE, FALSE, 0
};

/* ---- QMAIN_GUILD_JOIN (1) ------------------------------ */
static const QuestDef q_guild_join = {
    "Ingresso na Guilda",
    "O Mestre da Guilda aceita testar suas habilidades. "
    "Vença 3 batalhas no campo e retorne para a inscrição oficial.",
    "Mestre Aldric",
    QCAT_MAIN, QMAIN_GUILD_JOIN, 1,
    {
        OBJ("Vença 3 batalhas no campo",      QOBJ_WIN_BATTLE, 0,           3),
        OBJ("Retorne ao Mestre da Guilda",    QOBJ_TALK,       NSCR_GUILDMASTER, 1),
        OBJ0, OBJ0,
    },
    REW(500, 600, ITEM_HI_POTION,2, ITEM_GUILD_PASS,1, ITEM_NONE,0,
        QMAIN_FIRST_DUNGEON, 0xFF, WF_GUILD_MEMBER, 0),
    PRE(QMAIN_AWAKENING, 1, WF_VILLAGE_SAVED, ITEM_NONE),
    FALSE, FALSE, 0
};

/* ---- QMAIN_FIRST_DUNGEON (2) --------------------------- */
static const QuestDef q_first_dungeon = {
    "A Primeira Masmorra",
    "Relatos de monstros saindo das ruínas subterrâneas ao norte "
    "assolam os camponeses. Investigue e elimine a ameaça.",
    "Mestre Aldric",
    QCAT_MAIN, QMAIN_FIRST_DUNGEON, 2,
    {
        OBJ("Entre na Primeira Masmorra",     QOBJ_REACH_MAP,  MAP_DUNGEON_1, 1),
        OBJ("Elimine 5 inimigos subterrâneos",QOBJ_KILL_ANY,   0,             5),
        OBJ("Derrote o Chefe da Masmorra",    QOBJ_KILL,       ENEMY_BOSS_SERPENT, 1),
        OBJ("Retorne ao Mestre da Guilda",    QOBJ_TALK,       NSCR_GUILDMASTER, 1),
    },
    REW(1000, 1200, ITEM_IRON_ORE,5, ITEM_STEEL_INGOT,2, ITEM_HI_POTION,3,
        QMAIN_CRYSTAL_HUNT, QSECRET_ARCANE_TOME, WF_DUNGEON1_CLEAR, 0),
    PRE(QMAIN_GUILD_JOIN, 5, WF_GUILD_MEMBER, ITEM_GUILD_PASS),
    FALSE, FALSE, 0
};

/* ---- QMAIN_CRYSTAL_HUNT (3) ---------------------------- */
static const QuestDef q_crystal_hunt = {
    "Caça aos Cristais",
    "O Conselho de Aetheria revela que três Cristais Elementais "
    "foram roubados por facções rivais. Recupere-os antes que "
    "sejam usados para despertar o Dragão Negro.",
    "Conselheira Lyra",
    QCAT_MAIN, QMAIN_CRYSTAL_HUNT, 3,
    {
        OBJ("Colete 1 Cristal de Fogo",   QOBJ_COLLECT, ITEM_FIRE_ESSENCE,  3),
        OBJ("Colete 1 Cristal de Gelo",   QOBJ_COLLECT, ITEM_ICE_ESSENCE,   3),
        OBJ("Colete 1 Cristal de Terra",  QOBJ_COLLECT, ITEM_EARTH_ESSENCE, 3),
        OBJ("Entregue ao Conselho",       QOBJ_TALK,    NSCR_PRINCESS,      1),
    },
    REW(2000, 2000, ITEM_SOMA,2, ITEM_MANA_CRYSTAL,2, ITEM_NONE,0,
        QMAIN_BETRAYAL, QSECRET_TRUE_KING, WF_CRYSTALS_FOUND, 0),
    PRE(QMAIN_FIRST_DUNGEON, 10, WF_DUNGEON1_CLEAR, ITEM_NONE),
    FALSE, FALSE, 0
};

/* ---- QMAIN_BETRAYAL (4) -------------------------------- */
static const QuestDef q_betrayal = {
    "A Traição",
    "Um dos conselheiros de Aetheria traiu o reino, entregando "
    "os cristais ao culto do Vazio. Confronte o traidor e recupere "
    "os cristais antes que o ritual seja concluído.",
    "Princesa Selene",
    QCAT_MAIN, QMAIN_BETRAYAL, 4,
    {
        OBJ("Investigue o Conselheiro Vorn", QOBJ_TALK,       NSCR_MYSTERIOUS_MAN, 1),
        OBJ("Derrote os Cavaleiros do Vazio",QOBJ_KILL,       ENEMY_DARK_KNIGHT,   3),
        OBJ("Confronte o Traidor",           QOBJ_SCRIPTED,   4,                   1),
        OBJ0,
    },
    REW(3000, 3000, ITEM_ELIXIR,2, ITEM_ANCIENT_RUNE,1, ITEM_NONE,0,
        QMAIN_ANCIENT_RUIN, QSECRET_BETRAYER, WF_BETRAYAL_KNOWN, 0),
    PRE(QMAIN_CRYSTAL_HUNT, 15, WF_CRYSTALS_FOUND, ITEM_NONE),
    FALSE, TRUE, 0
};

/* ---- QMAIN_ANCIENT_RUIN (5) ---------------------------- */
static const QuestDef q_ancient_ruin = {
    "Ruínas Antigas",
    "O sábio Orion revelou que as Ruínas de Aethos guardam o segredo "
    "para selar o Dragão Negro. Explore as ruínas e encontre o Codex.",
    "Sábio Orion",
    QCAT_MAIN, QMAIN_ANCIENT_RUIN, 3,
    {
        OBJ("Alcance as Ruínas de Aethos",   QOBJ_REACH_MAP,  MAP_RUINS,    1),
        OBJ("Derrote 4 Guardiões Antigos",   QOBJ_KILL,       ENEMY_GOLEM,  4),
        OBJ("Encontre o Codex Antigo",       QOBJ_COLLECT,    ITEM_ANCIENT_CODEX, 1),
        OBJ("Retorne ao Sábio Orion",        QOBJ_TALK,       NSCR_OLD_SAGE,1),
    },
    REW(4000, 4000, ITEM_ANCIENT_RUNE,2, ITEM_MOONSTONE,1, ITEM_HERO_DRINK,3,
        QMAIN_DRAGON_HUNT, 0xFF, WF_RUINS_EXPLORED, 0),
    PRE(QMAIN_BETRAYAL, 20, WF_BETRAYAL_KNOWN, ITEM_NONE),
    FALSE, FALSE, 0
};

/* ---- QMAIN_DRAGON_HUNT (6) ----------------------------- */
static const QuestDef q_dragon_hunt = {
    "O Dragão Negro",
    "O Dragão Negro Vareth foi completamente despertado e ameaça "
    "destruir Korrath. Enfrente a besta em seu covil e use o poder "
    "do Codex para enfraquecê-la.",
    "Mestre Aldric",
    QCAT_MAIN, QMAIN_DRAGON_HUNT, 5,
    {
        OBJ("Alcance o Covil do Dragão",     QOBJ_REACH_MAP, MAP_DRAGON_LAIR, 1),
        OBJ("Derrote 2 Dragonetes",          QOBJ_KILL,      ENEMY_DRAGON,    2),
        OBJ("Derrote Vareth, o Dragão Negro",QOBJ_SCRIPTED,  QMAIN_DRAGON_HUNT, 1),
        OBJ0,
    },
    REW(8000, 8000, ITEM_DRAGON_SCALE,3, ITEM_TITAN_HEART,1, ITEM_SOMA_PLUS,2,
        QMAIN_AETHERIA_GATE, QSECRET_GENESIS, WF_DRAGON_SLAIN, 0),
    PRE(QMAIN_ANCIENT_RUIN, 25, WF_RUINS_EXPLORED, ITEM_ANCIENT_CODEX),
    FALSE, FALSE, 0
};

/* ---- QMAIN_AETHERIA_GATE (7) --------------------------- */
static const QuestDef q_aetheria_gate = {
    "Portal de Aetheria",
    "Com o Dragão selado, o Portal de Aetheria pode ser reaberto. "
    "Carregue os três Cristais ao Portal e atravesse para o reino "
    "do Vazio onde o verdadeiro inimigo aguarda.",
    "Princesa Selene",
    QCAT_MAIN, QMAIN_AETHERIA_GATE, 4,
    {
        OBJ("Colete as 3 Essências Elementais", QOBJ_SCRIPTED, QMAIN_AETHERIA_GATE, 1),
        OBJ("Alcance o Portal de Aetheria",     QOBJ_REACH_MAP, MAP_AETHERIA_GATE,  1),
        OBJ("Ative o Portal",                   QOBJ_SCRIPTED,  QMAIN_AETHERIA_GATE, 2),
        OBJ0,
    },
    REW(10000, 10000, ITEM_VOID_CRYSTAL,2, ITEM_HOLY_FRAGMENT,2, ITEM_MEGALIXIR,1,
        QMAIN_FINAL_TRIALS, QSECRET_VOID_WALKER, WF_GATE_OPENED, 0),
    PRE(QMAIN_DRAGON_HUNT, 30, WF_DRAGON_SLAIN, ITEM_NONE),
    FALSE, FALSE, 0
};

/* ---- QMAIN_FINAL_TRIALS (8) ---------------------------- */
static const QuestDef q_final_trials = {
    "Provas Finais",
    "No coração do reino do Vazio, três provações testam o valor "
    "dos heróis: a Prova da Força, a Prova da Sabedoria e a Prova "
    "do Sacrifício. Supere-as e ganhe poder para o confronto final.",
    "Guardião do Vazio",
    QCAT_MAIN, QMAIN_FINAL_TRIALS, 5,
    {
        OBJ("Prova da Força: vença 3 batalhas", QOBJ_WIN_BATTLE,  0,                 3),
        OBJ("Prova da Sabedoria",               QOBJ_SCRIPTED,    QMAIN_FINAL_TRIALS, 1),
        OBJ("Prova do Sacrifício",              QOBJ_SCRIPTED,    QMAIN_FINAL_TRIALS, 2),
        OBJ("Chegue à Câmara Final",            QOBJ_SCRIPTED,    QMAIN_FINAL_TRIALS, 3),
    },
    REW(15000, 15000, ITEM_ASTRAL_CORE,1, ITEM_DIVINE_TEAR,1, ITEM_PHILOSOPHERS_STONE,1,
        QMAIN_AETHERIA_END, 0xFF, WF_TRIALS_DONE, 0),
    PRE(QMAIN_AETHERIA_GATE, 40, WF_GATE_OPENED, ITEM_NONE),
    FALSE, FALSE, 0
};

/* ---- QMAIN_AETHERIA_END (9) ---------------------------- */
static const QuestDef q_aetheria_end = {
    "O Fim de Aetheria",
    "O Senhor do Vazio, Malachar, aguarda no trono do caos. "
    "Esta é a última batalha — o destino de Aetheria está em suas mãos.",
    "Destino",
    QCAT_MAIN, QMAIN_AETHERIA_END, 5,
    {
        OBJ("Derrote Malachar, Senhor do Vazio", QOBJ_SCRIPTED, QMAIN_AETHERIA_END, 1),
        OBJ0, OBJ0, OBJ0,
    },
    REW(99999, 99999, ITEM_AETHERIA_KEY,1, ITEM_ESSENCE_INFINITY,1, ITEM_NONE,0,
        0xFF, 0xFF, WF_AETHERIA_SAVED, 0),
    PRE(QMAIN_FINAL_TRIALS, 50, WF_TRIALS_DONE, ITEM_NONE),
    FALSE, FALSE, 0
};

/* =========================================================
 * SIDE QUESTS (10-21)
 * ========================================================= */

/* ---- QSIDE_LOST_CAT (10) ------------------------------- */
static const QuestDef q_lost_cat = {
    "O Gato Perdido",
    "A filha do Ancião perdeu seu gato, Mochi, nas cavernas ao sul "
    "da vila. Ele provavelmente assustou algum monstro por lá...",
    "Minha da Vila",
    QCAT_SIDE, QSIDE_LOST_CAT, 1,
    {
        OBJ("Encontre o Gato Mochi",  QOBJ_SCRIPTED, QSIDE_LOST_CAT, 1),
        OBJ("Retorne com o gato",     QOBJ_TALK,     NSCR_ELDER,     1),
        OBJ0, OBJ0,
    },
    REW(100, 150, ITEM_POTION,3, ITEM_NONE,0, ITEM_NONE,0,
        0xFF, 0xFF, 0, 0),
    PRE_NONE,
    FALSE, FALSE, 0
};

/* ---- QSIDE_HERB_GATHER (11) ---------------------------- */
static const QuestDef q_herb_gather = {
    "Coleta de Ervas",
    "O alquimista da cidade precisa de ervas medicinais "
    "para preparar poções para os feridos. Colete 5 ervas e "
    "traga água pura do rio.",
    "Alquimista Brenn",
    QCAT_SIDE, QSIDE_HERB_GATHER, 1,
    {
        OBJ("Colete 5 Ervas",            QOBJ_COLLECT, ITEM_HERB,       5),
        OBJ("Colete 3 unidades de Água Pura", QOBJ_COLLECT, ITEM_PURE_WATER, 3),
        OBJ("Entregue ao Alquimista",    QOBJ_TALK,    NSCR_ALCHEMIST,  1),
        OBJ0,
    },
    REW(200, 200, ITEM_HI_POTION,3, ITEM_ETHER,2, ITEM_ANTIDOTE,5,
        RECIPE_HI_POTION, 0xFF, 0, 0),
    PRE_NONE,
    FALSE, FALSE, 0
};

/* ---- QSIDE_BANDIT_HUNT (12) ---------------------------- */
static const QuestDef q_bandit_hunt = {
    "Caçada aos Bandidos",
    "Bandidos Goblin estão saqueando viajantes na estrada norte. "
    "O Mestre da Guilda oferece recompensa pela eliminação do bando.",
    "Guilda dos Aventureiros",
    QCAT_SIDE, QSIDE_BANDIT_HUNT, 2,
    {
        OBJ("Elimine 5 Goblins",         QOBJ_KILL,     ENEMY_GOBLIN,       5),
        OBJ("Elimine o Líder Orc",       QOBJ_KILL,     ENEMY_ORC,          1),
        OBJ("Relate ao Mestre da Guilda",QOBJ_TALK,     NSCR_GUILDMASTER,   1),
        OBJ0,
    },
    REW(600, 500, ITEM_STEEL_INGOT,2, ITEM_HI_POTION,2, ITEM_NONE,0,
        0xFF, 0xFF, 0, 0),
    PRE(QMAIN_GUILD_JOIN, 3, WF_GUILD_MEMBER, ITEM_GUILD_PASS),
    FALSE, FALSE, 0
};

/* ---- QSIDE_SMITH_DELIVERY (13) ------------------------- */
static const QuestDef q_smith_delivery = {
    "Entrega do Ferreiro",
    "O ferreiro Torak encomendou Lingotes de Aço mas seu assistente "
    "foi atacado por lobos. Entregue os lingotes e garanta um desconto.",
    "Ferreiro Torak",
    QCAT_SIDE, QSIDE_SMITH_DELIVERY, 1,
    {
        OBJ("Obtenha 3 Lingotes de Aço",  QOBJ_COLLECT, ITEM_STEEL_INGOT,  3),
        OBJ("Entregue ao Ferreiro Torak", QOBJ_TALK,    NSCR_BLACKSMITH,   1),
        OBJ0, OBJ0,
    },
    REW(400, 300, IITEM_RING_POWER,1, ITEM_COAL,5, ITEM_NONE,0,
        0xFF, 0xFF, 0, 0),
    PRE_NONE,
    FALSE, FALSE, 0
};

/* ---- QSIDE_HAUNTED_INN (14) ---------------------------- */
static const QuestDef q_haunted_inn = {
    "Estalagem Amaldiçoada",
    "Hóspedes desaparecem toda noite na Estalagem do Corvo Sombrio. "
    "O estalajadeiro Gorm pede ajuda desesperadamente. Investigue.",
    "Estalajadeiro Gorm",
    QCAT_SIDE, QSIDE_HAUNTED_INN, 3,
    {
        OBJ("Investigue a estalagem à noite",     QOBJ_SCRIPTED, QSIDE_HAUNTED_INN, 1),
        OBJ("Elimine os Mortos-Vivos no porão",   QOBJ_KILL,     ENEMY_UNDEAD,       4),
        OBJ("Derrote o Espectro do Porão",        QOBJ_SCRIPTED, QSIDE_HAUNTED_INN,  2),
        OBJ("Reporte ao Estalajadeiro",           QOBJ_TALK,     NSCR_INNKEEPER,     1),
    },
    REW(1200, 1000, ITEM_CLOAK_SHADOW,0, ITEM_SHADOW_CLOTH,3, ITEM_REMEDY,3,
        0xFF, QSECRET_SHADOW_BLADE, 0, 0),
    PRE(QMAIN_GUILD_JOIN, 8, WF_GUILD_MEMBER, ITEM_NONE),
    FALSE, FALSE, 0
};

/* ---- QSIDE_GUILD_TRIALS (15) --------------------------- */
static const QuestDef q_guild_trials = {
    "Provas da Guilda",
    "Para subir de patente na Guilda, prove sua valentia: complete "
    "três contratos de eliminação e retorne com as provas.",
    "Mestre Aldric",
    QCAT_SIDE, QSIDE_GUILD_TRIALS, 3,
    {
        OBJ("Elimine 3 Elementais de Fogo", QOBJ_KILL, ENEMY_ELEMENTAL_F, 3),
        OBJ("Elimine 2 Cavaleiros das Trevas",QOBJ_KILL, ENEMY_DARK_KNIGHT, 2),
        OBJ("Elimine 1 Golem Ancião",       QOBJ_KILL, ENEMY_GOLEM,        1),
        OBJ("Apresente as provas à Guilda", QOBJ_TALK, NSCR_GUILDMASTER,   1),
    },
    REW(3000, 2500, ITEM_HERO_BADGE,1, ITEM_HERO_DRINK,2, ITEM_SOMA,2,
        0xFF, 0xFF, WF_SHOP_BLACK_OPEN, 0),
    PRE(QMAIN_FIRST_DUNGEON, 15, WF_DUNGEON1_CLEAR | WF_GUILD_MEMBER, ITEM_GUILD_PASS),
    FALSE, FALSE, 0
};

/* ---- QSIDE_DRAGON_SCALE (16) --------------------------- */
static const QuestDef q_dragon_scale = {
    "Escamas do Dragão",
    "O ferreiro lendário Korgg precisa de 3 Escamas de Dragão "
    "para forjar uma armadura épica. Elas só existem nos antros "
    "de dragões menores nas montanhas Ash.",
    "Ferreiro Korgg",
    QCAT_SIDE, QSIDE_DRAGON_SCALE, 4,
    {
        OBJ("Colete 3 Escamas de Dragão",    QOBJ_COLLECT, ITEM_DRAGON_SCALE, 3),
        OBJ("Entregue ao Ferreiro Korgg",    QOBJ_TALK,    NSCR_BLACKSMITH,   1),
        OBJ0, OBJ0,
    },
    REW(5000, 4000, IITEM_PLATE_ARMOR,1, ITEM_TITAN_SCALE,3, ITEM_NONE,0,
        RECIPE_TITAN_GS, 0xFF, WF_FORGE_LEGEND_OPEN, 0),
    PRE(QMAIN_CRYSTAL_HUNT, 20, WF_CRYSTALS_FOUND, ITEM_NONE),
    FALSE, FALSE, 0
};

/* ---- QSIDE_ANCIENT_CODEX (17) -------------------------- */
static const QuestDef q_ancient_codex = {
    "Codex Perdido",
    "Um estudioso encontrou referências a um Codex Arcano enterrado "
    "nas ruínas de Aethos. Ele contém segredos de poder imensurável. "
    "Encontre-o antes que o culto do Vazio o faça.",
    "Estudioso Errol",
    QCAT_SIDE, QSIDE_ANCIENT_CODEX, 3,
    {
        OBJ("Alcance as Ruínas de Aethos",   QOBJ_REACH_MAP, MAP_RUINS,         1),
        OBJ("Encontre o Codex Arcano",       QOBJ_COLLECT,   ITEM_ANCIENT_CODEX, 1),
        OBJ("Entregue ao Estudioso Errol",   QOBJ_TALK,      NSCR_OLD_SAGE,      1),
        OBJ0,
    },
    REW(2500, 2000, ITEM_ARCANE_CODEX,1, ITEM_ARCANE_DUST,5, ITEM_MAGIC_INK,5,
        0xFF, QSECRET_ARCANE_TOME, 0, 0),
    PRE(QMAIN_ANCIENT_RUIN, 20, WF_RUINS_EXPLORED, ITEM_NONE),
    FALSE, FALSE, 0
};

/* ---- QSIDE_ESCORT (18) --------------------------------- */
static const QuestDef q_escort = {
    "Escolta do Mercador",
    "O mercador Aldus precisa transportar uma carga valiosa de "
    "Korrath até o porto de Salvamar. A rota é perigosa. Escolte-o.",
    "Mercador Aldus",
    QCAT_SIDE, QSIDE_ESCORT, 2,
    {
        OBJ("Aceite a missão de escolta",     QOBJ_TALK,        NSCR_MERCHANT, 1),
        OBJ("Proteja o mercador (3 ataques)", QOBJ_ESCORT_SAFE, QSIDE_ESCORT,  1),
        OBJ("Chegue ao porto de Salvamar",    QOBJ_SCRIPTED,    QSIDE_ESCORT,  1),
        OBJ0,
    },
    REW(800, 700, ITEM_HI_POTION,5, ITEM_HI_ETHER,3, ITEM_RING_SPEED,0,
        0xFF, 0xFF, 0, 0),
    PRE(QMAIN_GUILD_JOIN, 5, WF_GUILD_MEMBER, ITEM_NONE),
    FALSE, TRUE, 0
};

/* ---- QSIDE_MONSTER_HUNT (19) --------------------------- */
static const QuestDef q_monster_hunt = {
    "Pesquisa de Monstros",
    "O naturalista Piro documenta as criaturas de Aetheria. "
    "Ajude-o eliminando um espécimen de cada tipo para análise.",
    "Naturalista Piro",
    QCAT_SIDE, QSIDE_MONSTER_HUNT, 3,
    {
        OBJ("Elimine ao menos 1 de cada tipo (12)",QOBJ_SCRIPTED, QSIDE_MONSTER_HUNT, 1),
        OBJ("Reporte ao Naturalista Piro",         QOBJ_TALK,     NSCR_MYSTERIOUS_MAN, 1),
        OBJ0, OBJ0,
    },
    REW(4000, 3500, ITEM_CELESTIAL_LENS,1, ITEM_HERO_DRINK,3, ITEM_SOMA,3,
        0xFF, 0xFF, 0, 0),
    PRE(QMAIN_FIRST_DUNGEON, 15, WF_DUNGEON1_CLEAR, ITEM_NONE),
    FALSE, FALSE, 0
};

/* ---- QSIDE_LEGENDARY_FORGE (20) ------------------------ */
static const QuestDef q_legendary_forge = {
    "A Forja Lendária",
    "A Forja Lendária de Aetheria foi descoberta nas profundezas das "
    "montanhas. Para ativá-la, forje ao menos um item lendário "
    "usando os materiais mais raros do mundo.",
    "Sábio Orion",
    QCAT_SIDE, QSIDE_LEGENDARY_FORGE, 5,
    {
        OBJ("Obtenha 1 Runa Antiga",         QOBJ_COLLECT, ITEM_ANCIENT_RUNE,    1),
        OBJ("Obtenha o Coração do Titã",     QOBJ_COLLECT, ITEM_TITAN_HEART,     1),
        OBJ("Forje um item lendário",        QOBJ_CRAFT,   RECIPE_TITAN_GS,      1),
        OBJ("Mostre ao Sábio Orion",         QOBJ_TALK,    NSCR_OLD_SAGE,        1),
    },
    REW(10000, 8000, ITEM_ASTRAL_DUST,3, ITEM_CHAOS_ORB,1, ITEM_NONE,0,
        0xFF, 0xFF, WF_FORGE_LEGEND_OPEN, 0),
    PRE(QSIDE_DRAGON_SCALE, 30, WF_RUINS_EXPLORED, ITEM_NONE),
    FALSE, FALSE, 0
};

/* ---- QSIDE_CHAMPIONS (21) ------------------------------ */
static const QuestDef q_champions = {
    "Torneio dos Campeões",
    "A Arena de Korrath realiza o Grande Torneio anual. "
    "Vença 5 batalhas consecutivas no torneio e conquiste o título "
    "de Campeão de Aetheria.",
    "Anfiteatro Real",
    QCAT_SIDE, QSIDE_CHAMPIONS, 4,
    {
        OBJ("Entre na Arena",               QOBJ_REACH_MAP, MAP_ARENA,          1),
        OBJ("Vença 5 batalhas de torneio",  QOBJ_WIN_BATTLE, 0,                 5),
        OBJ("Receba o título do Anfitrião", QOBJ_TALK,      NSCR_ARENA_HOST,    1),
        OBJ0,
    },
    REW(8000, 6000, IITEM_PENDANT_CRIT,1, ITEM_HERO_DRINK,5, ITEM_MEGALIXIR,2,
        0xFF, 0xFF, 0, 0),
    PRE(QMAIN_GUILD_JOIN, 20, WF_GUILD_MEMBER, ITEM_HERO_BADGE),
    FALSE, FALSE, 0
};

/* =========================================================
 * SECRET QUESTS (22-27)
 * ========================================================= */

/* ---- QSECRET_SHADOW_BLADE (22) ------------------------- */
static const QuestDef q_shadow_blade = {
    "[SECRETA] Lâmina das Sombras",
    "Uma lâmina antiga jaz nas sombras da Estalagem do Corvo. "
    "Ela pertenceu a um assassino lendário que desapareceu há "
    "cem anos. Descubra o mistério por trás da lâmina amaldiçoada.",
    "???",
    QCAT_SECRET, QSECRET_SHADOW_BLADE, 4,
    {
        OBJ("Encontre a lâmina amaldiçoada",  QOBJ_COLLECT,  IITEM_SHADOW_DAGGER, 1),
        OBJ("Consulte o fantasma do assassino",QOBJ_SCRIPTED, QSECRET_SHADOW_BLADE,1),
        OBJ("Purifique a lâmina (Forja Sagrada)",QOBJ_SCRIPTED,QSECRET_SHADOW_BLADE,2),
        OBJ0,
    },
    REW(5000, 5000, IITEM_CLOAK_SHADOW,1, ITEM_SHADOW_ESSENCE,3, ITEM_NONE,0,
        0xFF, 0xFF, 0, 0),
    PRE(QSIDE_HAUNTED_INN, 15, 0, ITEM_NONE),
    FALSE, FALSE, 0
};

/* ---- QSECRET_ARCANE_TOME (23) -------------------------- */
static const QuestDef q_arcane_tome = {
    "[SECRETA] Tomo Arcano",
    "Um tomo selado foi descoberto numa câmara oculta das ruínas. "
    "Contém feitiços proibidos do período pré-Aetheria. Um Arcanista "
    "de alto nível pode decifrá-lo e ganhar poder imenso.",
    "???",
    QCAT_SECRET, QSECRET_ARCANE_TOME, 5,
    {
        OBJ("Encontre o Tomo Arcano Selado", QOBJ_SCRIPTED, QSECRET_ARCANE_TOME, 1),
        OBJ("Quebre o selo do tomo",         QOBJ_SCRIPTED, QSECRET_ARCANE_TOME, 2),
        OBJ("Aprenda o feitiço do Vazio",    QOBJ_SCRIPTED, QSECRET_ARCANE_TOME, 3),
        OBJ0,
    },
    REW(6000, 6000, IITEM_CROWN_ARCANE,1, ITEM_ASTRAL_DUST,5, ITEM_VOID_CRYSTAL,2,
        0xFF, 0xFF, WF_VOID_WALKER, 0),
    PRE(QMAIN_FIRST_DUNGEON, 25, WF_DUNGEON1_CLEAR, ITEM_NONE),
    FALSE, FALSE, 0
};

/* ---- QSECRET_BETRAYER (24) ----------------------------- */
static const QuestDef q_betrayer_past = {
    "[SECRETA] Passado do Traidor",
    "O conselheiro Vorn não nasceu como traidor. Documentos secretos "
    "revelam que ele foi manipulado pelo culto do Vazio. Descubra "
    "a verdade e talvez possa redimi-lo.",
    "???",
    QCAT_SECRET, QSECRET_BETRAYER, 3,
    {
        OBJ("Encontre o diário de Vorn",      QOBJ_SCRIPTED, QSECRET_BETRAYER, 1),
        OBJ("Confronte Vorn com a verdade",   QOBJ_TALK,     NSCR_BETRAYER,    1),
        OBJ("Decida o destino de Vorn",       QOBJ_SCRIPTED, QSECRET_BETRAYER, 2),
        OBJ0,
    },
    REW(4000, 4000, ITEM_HOLY_FRAGMENT,2, ITEM_STAR_SHARD,1, ITEM_NONE,0,
        0xFF, 0xFF, 0, 0),
    PRE(QMAIN_BETRAYAL, 20, WF_BETRAYAL_KNOWN, ITEM_NONE),
    FALSE, FALSE, 0
};

/* ---- QSECRET_VOID_WALKER (25) -------------------------- */
static const QuestDef q_void_walker = {
    "[SECRETA] Andarilho do Vazio",
    "Uma fissura no tecido da realidade leva a uma dimensão de puro "
    "caos. Apenas os mais corajosos aventureiros ousam entrar — "
    "e os mais fortes conseguem sair.",
    "???",
    QCAT_SECRET, QSECRET_VOID_WALKER, 5,
    {
        OBJ("Entre na Dimensão do Vazio",    QOBJ_REACH_MAP, MAP_VOID_REALM,   1),
        OBJ("Sobreviva às 3 ondas do Vazio", QOBJ_WIN_BATTLE, 0,               3),
        OBJ("Derrote o Guardião do Vazio",   QOBJ_SCRIPTED,  QSECRET_VOID_WALKER, 1),
        OBJ("Escape da Dimensão",            QOBJ_SCRIPTED,  QSECRET_VOID_WALKER, 2),
    },
    REW(20000, 20000, ITEM_VOID_CRYSTAL,3, ITEM_CHAOS_ORB,2, ITEM_ESSENCE_INFINITY,1,
        0xFF, 0xFF, WF_VOID_WALKER, 0),
    PRE(QMAIN_AETHERIA_GATE, 40, WF_GATE_OPENED, ITEM_NONE),
    FALSE, TRUE, 0
};

/* ---- QSECRET_TRUE_KING (26) ---------------------------- */
static const QuestDef q_true_king = {
    "[SECRETA] O Verdadeiro Rei",
    "Documentos encontrados nas Ruínas de Aethos revelam que a "
    "linhagem real de Korrath foi usurpada há gerações. O verdadeiro "
    "herdeiro vive em anonimato. Descubra quem é.",
    "???",
    QCAT_SECRET, QSECRET_TRUE_KING, 4,
    {
        OBJ("Encontre os documentos reais",  QOBJ_SCRIPTED, QSECRET_TRUE_KING, 1),
        OBJ("Investigue o Misterioso Homem", QOBJ_TALK,     NSCR_MYSTERIOUS_MAN, 1),
        OBJ("Revele a verdade à Princesa",   QOBJ_TALK,     NSCR_PRINCESS,      1),
        OBJ("Resolva a questão de sucessão", QOBJ_SCRIPTED, QSECRET_TRUE_KING,  2),
    },
    REW(15000, 12000, ITEM_HERO_BADGE,1, ITEM_SUNSTONE,2, ITEM_NONE,0,
        0xFF, 0xFF, WF_TRUE_KING_KNOWN, 0),
    PRE(QMAIN_CRYSTAL_HUNT, 25, WF_RUINS_EXPLORED, ITEM_NONE),
    FALSE, FALSE, 0
};

/* ---- QSECRET_GENESIS (27) ------------------------------ */
static const QuestDef q_genesis = {
    "[SECRETA] Gênesis de Aetheria",
    "Um fragmento astral guardado pelo Dragão Negro revela a origem "
    "de Aetheria: não um reino humano, mas uma criação divina com "
    "um propósito oculto. Descubra a verdade final.",
    "???",
    QCAT_SECRET, QSECRET_GENESIS, 5,
    {
        OBJ("Obtenha o Fragmento Astral",    QOBJ_COLLECT, ITEM_STAR_SHARD,    1),
        OBJ("Decodifique o fragmento",       QOBJ_SCRIPTED, QSECRET_GENESIS,   1),
        OBJ("Visite os 3 santuários antigos",QOBJ_SCRIPTED, QSECRET_GENESIS,   2),
        OBJ("Descubra a verdade final",      QOBJ_SCRIPTED, QSECRET_GENESIS,   3),
    },
    REW(30000, 30000, ITEM_SOUL_GEM,1, ITEM_ASTRAL_CORE,1, ITEM_DIVINE_TEAR,1,
        0xFF, 0xFF, 0, 0),
    PRE(QMAIN_DRAGON_HUNT, 45, WF_DRAGON_SLAIN, ITEM_NONE),
    FALSE, FALSE, 0
};

/* =========================================================
 * Master table
 * ========================================================= */
const QuestDef g_quest_db[QUEST_COUNT] = {
    [QMAIN_AWAKENING]      = q_awakening,
    [QMAIN_GUILD_JOIN]     = q_guild_join,
    [QMAIN_FIRST_DUNGEON]  = q_first_dungeon,
    [QMAIN_CRYSTAL_HUNT]   = q_crystal_hunt,
    [QMAIN_BETRAYAL]       = q_betrayal,
    [QMAIN_ANCIENT_RUIN]   = q_ancient_ruin,
    [QMAIN_DRAGON_HUNT]    = q_dragon_hunt,
    [QMAIN_AETHERIA_GATE]  = q_aetheria_gate,
    [QMAIN_FINAL_TRIALS]   = q_final_trials,
    [QMAIN_AETHERIA_END]   = q_aetheria_end,
    [QSIDE_LOST_CAT]       = q_lost_cat,
    [QSIDE_HERB_GATHER]    = q_herb_gather,
    [QSIDE_BANDIT_HUNT]    = q_bandit_hunt,
    [QSIDE_SMITH_DELIVERY] = q_smith_delivery,
    [QSIDE_HAUNTED_INN]    = q_haunted_inn,
    [QSIDE_GUILD_TRIALS]   = q_guild_trials,
    [QSIDE_DRAGON_SCALE]   = q_dragon_scale,
    [QSIDE_ANCIENT_CODEX]  = q_ancient_codex,
    [QSIDE_ESCORT]         = q_escort,
    [QSIDE_MONSTER_HUNT]   = q_monster_hunt,
    [QSIDE_LEGENDARY_FORGE]= q_legendary_forge,
    [QSIDE_CHAMPIONS]      = q_champions,
    [QSECRET_SHADOW_BLADE] = q_shadow_blade,
    [QSECRET_ARCANE_TOME]  = q_arcane_tome,
    [QSECRET_BETRAYER]     = q_betrayer_past,
    [QSECRET_VOID_WALKER]  = q_void_walker,
    [QSECRET_TRUE_KING]    = q_true_king,
    [QSECRET_GENESIS]      = q_genesis,
};

#undef OBJ
#undef OBJ0
#undef REW
#undef PRE
#undef PRE_NONE

/* ---- Lookup API ----------------------------------------- */

const QuestDef* quest_def_get(u8 quest_id)
{
    if (quest_id >= QUEST_COUNT) return NULL;
    return &g_quest_db[quest_id];
}

BOOL quest_is_main(u8 quest_id)
{
    return (BOOL)(quest_id < QUEST_MAIN_COUNT);
}

BOOL quest_is_secret(u8 quest_id)
{
    return (BOOL)(quest_id >= (QUEST_MAIN_COUNT + QUEST_SIDE_COUNT));
}

const QuestObjDef* quest_obj_get(u8 quest_id, u8 obj_idx)
{
    const QuestDef* q = quest_def_get(quest_id);
    if (!q || obj_idx >= QUEST_OBJ_MAX) return NULL;
    if (q->objectives[obj_idx].type == QOBJ_NONE) return NULL;
    return &q->objectives[obj_idx];
}
