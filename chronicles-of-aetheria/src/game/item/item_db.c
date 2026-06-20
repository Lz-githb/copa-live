#include "item_db.h"
#include "char_equip.h"
#include "gba_types.h"

/* ---- Rarity name strings -------------------------------- */
static const char* s_rarity_names[] = {
    "Comum", "Incomum", "Raro", "Épico", "Lendário"
};

/* ---- Item DB ROM table ---------------------------------- */
/* Shorthand macros for readability */
#define _E(n,d,eid,r,buy,sell) \
    { n, d, ICAT_EQUIPMENT, r, CEFF_NONE,0,0,0,FALSE,1,buy,sell,eid }
#define _C(n,d,r,eff,val,param,flags,all,stk,buy,sell) \
    { n, d, ICAT_CONSUMABLE, r, eff,val,param,flags,all,stk,buy,sell,0 }
#define _M(n,d,r,buy,sell) \
    { n, d, ICAT_MATERIAL, r, CEFF_NONE,0,0,0,FALSE,99,buy,sell,0 }
#define _K(n,d) \
    { n, d, ICAT_KEY_ITEM, RARITY_UNCOMMON, CEFF_NONE,0,0,0,FALSE,1,0,0,0 }

const ItemDef g_item_db[ITEM_DB_COUNT] = {
/* 0 */ { "---", "Nenhum item.", ICAT_CONSUMABLE, RARITY_COMMON,
          CEFF_NONE,0,0,0,FALSE,0,0,0,0 },

/* ---- Equipment (1-34) ---------------------------------- */
/* 1  */ _E("Espada de Ferro",    "Espada básica de ferro.",         1,  RARITY_COMMON,    150,  50),
/* 2  */ _E("Espada de Aço",      "Espada forjada em aço.",          2,  RARITY_UNCOMMON,  400, 130),
/* 3  */ _E("Lâmina do Guardião", "Espada sagrada do Guardião.",     3,  RARITY_RARE,     1200, 400),
/* 4  */ _E("Espadão de Ferro",   "Espadão pesado e contundente.",   4,  RARITY_COMMON,    300, 100),
/* 5  */ _E("Espadão do Titã",    "Destruidora de fortalezas.",      5,  RARITY_LEGENDARY,9999,3000),
/* 6  */ _E("Espada Rúnica",      "Espada encantada com runas.",     6,  RARITY_RARE,     1500, 500),
/* 7  */ _E("Lâmina Arcana",      "Lâmina que canaliza magia.",      7,  RARITY_EPIC,     4000,1300),
/* 8  */ _E("Cajado de Madeira",  "Cajado básico de aprendiz.",      8,  RARITY_COMMON,    100,  33),
/* 9  */ _E("Cajado de Fogo",     "Amplifica magia de fogo.",        9,  RARITY_UNCOMMON,  800, 260),
/* 10 */ _E("Cajado de Gelo",     "Amplifica magia de gelo.",       10,  RARITY_UNCOMMON,  800, 260),
/* 11 */ _E("Grande Orbe",        "Concentrador de magia pura.",    11,  RARITY_RARE,     2500, 830),
/* 12 */ _E("Adaga de Osso",      "Adaga rústica, mas eficaz.",     12,  RARITY_COMMON,    120,  40),
/* 13 */ _E("Adaga das Sombras",  "Adaga envenenada furtiva.",      13,  RARITY_RARE,     1800, 600),
/* 14 */ _E("Arco Curto",         "Arco simples e ágil.",           14,  RARITY_COMMON,    180,  60),
/* 15 */ _E("Arco do Caçador",    "Arco longo de alta precisão.",   15,  RARITY_RARE,     1600, 530),
/* 16 */ _E("Escudo de Ferro",    "Escudo redondo de ferro.",       16,  RARITY_COMMON,    200,  66),
/* 17 */ _E("Escudo de Torre",    "Escudo enorme, defesa máxima.",  17,  RARITY_UNCOMMON,  600, 200),
/* 18 */ _E("Cristal de Foco",    "Amplifica magias do Arcanista.", 18,  RARITY_UNCOMMON,  500, 166),
/* 19 */ _E("Carcaj",             "Offhand do Caçador. Bônus SPD.", 19,  RARITY_COMMON,    150,  50),
/* 20 */ _E("Elmo de Ferro",      "Elmo básico de proteção.",       20,  RARITY_COMMON,    250,  83),
/* 21 */ _E("Capuz de Couro",     "Capuz leve do Caçador.",         21,  RARITY_COMMON,    100,  33),
/* 22 */ _E("Chapéu de Mago",     "Chapéu pontudo do Arcanista.",   22,  RARITY_COMMON,    120,  40),
/* 23 */ _E("Armadura de Malha",  "Proteção média de correntes.",   23,  RARITY_UNCOMMON,  700, 233),
/* 24 */ _E("Armadura de Placa",  "A mais pesada proteção física.", 24,  RARITY_RARE,     2000, 666),
/* 25 */ _E("Colete de Couro",    "Armadura leve do Caçador.",      25,  RARITY_COMMON,    350, 116),
/* 26 */ _E("Robe Arcano",        "Veste mágica do Arcanista.",     26,  RARITY_UNCOMMON,  600, 200),
/* 27 */ _E("Anel do Poder",      "Aumenta força física.",          27,  RARITY_UNCOMMON,  500, 166),
/* 28 */ _E("Anel da Velocidade", "Aumenta agilidade.",             28,  RARITY_UNCOMMON,  500, 166),
/* 29 */ _E("Amuleto de Mana",    "Expande reserva de MP.",         29,  RARITY_UNCOMMON,  600, 200),
/* 30 */ _E("Pendente Crítico",   "Aumenta chance de acerto crítico.",30,RARITY_UNCOMMON,  700, 233),
/* 31 */ _E("Manto das Sombras",  "Lendário do Caçador. EVA+30.",   31,  RARITY_LEGENDARY,9999,3000),
/* 32 */ _E("Coroa Arcana",       "Lendária do Arcanista. MAG+50.", 32,  RARITY_LEGENDARY,9999,3000),
/* 33 */ _E("Crista do Guardião", "Lendária do Guardião. DEF+60.",  33,  RARITY_LEGENDARY,9999,3000),
/* 34 */ _E("Relíquia Arcana",    "Lendária do Espadachim. ATK+MAG.",34, RARITY_LEGENDARY,9999,3000),

/* ---- Common consumables (35-54) ------------------------ */
/* 35 */ _C("Poção",         "Recupera 80 HP.",          RARITY_COMMON,   CEFF_HEAL_HP,     80,  0, USE_ANYWHERE, FALSE, 99,  50,  15),
/* 36 */ _C("Hi-Poção",      "Recupera 200 HP.",         RARITY_UNCOMMON, CEFF_HEAL_HP,    200,  0, USE_ANYWHERE, FALSE, 99, 150,  45),
/* 37 */ _C("Mega-Poção",    "Recupera 500 HP.",         RARITY_RARE,     CEFF_HEAL_HP,    500,  0, USE_ANYWHERE, FALSE, 99, 400, 130),
/* 38 */ _C("Éter",          "Recupera 30 MP.",          RARITY_COMMON,   CEFF_HEAL_MP,     30,  0, USE_ANYWHERE, FALSE, 99,  80,  25),
/* 39 */ _C("Hi-Éter",       "Recupera 80 MP.",          RARITY_UNCOMMON, CEFF_HEAL_MP,     80,  0, USE_ANYWHERE, FALSE, 99, 200,  66),
/* 40 */ _C("Antídoto",      "Cura veneno.",             RARITY_COMMON,   CEFF_CURE_STATUS,  0,  2, USE_ANYWHERE, FALSE, 99,  50,  15),
/* 41 */ _C("Erva de Eco",   "Cura silêncio.",           RARITY_COMMON,   CEFF_CURE_STATUS,  0,  0, USE_ANYWHERE, FALSE, 99,  60,  20),
/* 42 */ _C("Colírio",       "Cura cegueira.",           RARITY_COMMON,   CEFF_CURE_STATUS,  0,  0, USE_ANYWHERE, FALSE, 99,  40,  13),
/* 43 */ _C("Agulha Dourada","Cura petrificação.",       RARITY_UNCOMMON, CEFF_CURE_STATUS,  0,  3, USE_ANYWHERE, FALSE, 99, 150,  50),
/* 44 */ _C("Beijo Perfeito","Cura todos status.",       RARITY_RARE,     CEFF_CURE_ALL,     0,  0, USE_ANYWHERE, FALSE, 99, 300, 100),
/* 45 */ _C("Reviver",       "Revive com 50% do HP.",   RARITY_UNCOMMON, CEFF_REVIVE,      50,  0, USE_ANYWHERE, FALSE, 99, 500, 166),
/* 46 */ _C("Barraca",       "Restaura 50% HP/MP (campo).",RARITY_UNCOMMON,CEFF_PARTY_HEAL_HP,50,5,USE_FIELD,   TRUE,  10, 400, 130),
/* 47 */ _C("Bebida de Poder","ATK+10 por 5 turnos.",   RARITY_UNCOMMON, CEFF_BUFF_ATK,    10,  5, USE_BATTLE,  FALSE, 30, 200,  66),
/* 48 */ _C("Bebida de Vel.","SPD+10 por 5 turnos.",    RARITY_UNCOMMON, CEFF_BUFF_SPD,    10,  5, USE_BATTLE,  FALSE, 30, 200,  66),
/* 49 */ _C("Bebida de Guarda","DEF+10 por 5 turnos.",  RARITY_UNCOMMON, CEFF_BUFF_DEF,    10,  5, USE_BATTLE,  FALSE, 30, 200,  66),
/* 50 */ _C("Bebida Mental","MAG+10 por 5 turnos.",     RARITY_UNCOMMON, CEFF_BUFF_MAG,    10,  5, USE_BATTLE,  FALSE, 30, 200,  66),
/* 51 */ _C("Bomba de Fumaça","Fuga garantida da batalha.",RARITY_UNCOMMON,CEFF_FLEE_BATTLE,0,  0, USE_BATTLE,  FALSE, 20, 300, 100),
/* 52 */ _C("Grimório de Fogo","Dano de fogo (150).",   RARITY_UNCOMMON, CEFF_DAMAGE_ENEMY,150, 0, USE_BATTLE,  FALSE, 20, 250,  83),
/* 53 */ _C("Grimório de Gelo","Dano de gelo (150).",   RARITY_UNCOMMON, CEFF_DAMAGE_ENEMY,150, 1, USE_BATTLE,  FALSE, 20, 250,  83),
/* 54 */ _C("Remédio",       "Cura todos os status.",   RARITY_RARE,     CEFF_CURE_ALL,     0,  0, USE_ANYWHERE, FALSE, 30, 400, 133),

/* ---- Rare consumables (55-69) -------------------------- */
/* 55 */ _C("Elixir",        "HP+MP completos (1 aliado).",  RARITY_RARE,    CEFF_FULL_HEAL,   0,  0, USE_ANYWHERE, FALSE, 10, 1500, 500),
/* 56 */ _C("Megalixir",     "HP+MP completos (todos).",     RARITY_EPIC,    CEFF_PARTY_FULL,  0,  0, USE_ANYWHERE, TRUE,   3, 5000,1666),
/* 57 */ _C("Fênix",         "Revive com HP completo.",      RARITY_RARE,    CEFF_REVIVE,     100, 0, USE_ANYWHERE, FALSE,  5, 2000, 666),
/* 58 */ _C("Soma",          "Recupera 1000 HP.",            RARITY_RARE,    CEFF_HEAL_HP,   1000, 0, USE_ANYWHERE, FALSE, 20, 1000, 333),
/* 59 */ _C("Soma+",         "Recupera 2500 HP.",            RARITY_EPIC,    CEFF_HEAL_HP,   2500, 0, USE_ANYWHERE, FALSE,  5, 3000,1000),
/* 60 */ _C("Véu Lunar",     "MDEF+50 por 5 turnos.",        RARITY_RARE,    CEFF_BUFF_DEF,    50, 5, USE_BATTLE,  FALSE, 10, 800, 266),
/* 61 */ _C("Bebida Herói",  "Todos stats +15 por 5 turnos.",RARITY_EPIC,    CEFF_BUFF_ALL,    15, 5, USE_BATTLE,  FALSE,  5,2500, 833),
/* 62 */ _C("Sangue Dragão", "Recupera 75% HP+MP.",          RARITY_RARE,    CEFF_HEAL_HP_PCT, 75, 0, USE_ANYWHERE, FALSE, 10,1800, 600),
/* 63 */ _C("Ambrosia",      "Remove maldição e restaura.",  RARITY_EPIC,    CEFF_FULL_HEAL,    0, 0, USE_ANYWHERE, FALSE,  3,4000,1333),
/* 64 */ _C("Fragmento Morte","50% de chance de KO instantâneo.",RARITY_RARE,CEFF_DAMAGE_ENEMY,9999,7,USE_BATTLE,FALSE, 5,2000, 666),
/* 65 */ _C("Água Benta",    "Dobra XP na próxima batalha.", RARITY_RARE,    CEFF_XP_BOOST,     0, 0, USE_FIELD,   FALSE,  5,1500, 500),
/* 66 */ _C("Tônico Regen",  "Regenera HP por 10 turnos.",   RARITY_RARE,    CEFF_REGEN_HP,    30,10, USE_BATTLE,  FALSE, 10, 900, 300),
/* 67 */ _C("Cristal de Mana","Recupera 200 MP.",            RARITY_RARE,    CEFF_HEAL_MP,    200, 0, USE_ANYWHERE, FALSE, 10, 700, 233),
/* 68 */ _C("Chalé",         "Restaura HP/MP completos (campo).",RARITY_EPIC,CEFF_PARTY_FULL,   0, 0, USE_FIELD,   TRUE,   5,2000, 666),
/* 69 */ _C("Grimório do Vazio","Dano de trevas em todos inimigos (400).",RARITY_EPIC,CEFF_DAMAGE_ENEMY,400,6,USE_BATTLE,FALSE,3,5000,1666),

/* ---- Common materials (70-94) -------------------------- */
/* 70 */ _M("Minério de Ferro",  "Material básico de forja.",  RARITY_COMMON,    40,  13),
/* 71 */ _M("Lingote de Aço",    "Ferro refinado e duro.",     RARITY_COMMON,   100,  33),
/* 72 */ _M("Fragmento de Mana", "Cristal de energia mágica.", RARITY_COMMON,    80,  26),
/* 73 */ _M("Essência de Fogo",  "Concentrado de chama.",      RARITY_COMMON,    60,  20),
/* 74 */ _M("Essência de Gelo",  "Concentrado de gelo.",       RARITY_COMMON,    60,  20),
/* 75 */ _M("Essência de Terra", "Concentrado de pedra.",      RARITY_COMMON,    60,  20),
/* 76 */ _M("Essência de Vento", "Concentrado de ar.",         RARITY_COMMON,    60,  20),
/* 77 */ _M("Essência de Luz",   "Concentrado de luz sagrada.",RARITY_UNCOMMON, 100,  33),
/* 78 */ _M("Essência de Trevas","Concentrado de escuridão.",  RARITY_UNCOMMON, 100,  33),
/* 79 */ _M("Couro de Monstro",  "Pele grossa e resistente.",  RARITY_COMMON,    50,  16),
/* 80 */ _M("Tecido das Sombras","Tecido mágico furtivo.",     RARITY_UNCOMMON, 120,  40),
/* 81 */ _M("Escama do Titã",    "Escama de criatura colossal.",RARITY_UNCOMMON, 200,  66),
/* 82 */ _M("Fragmento de Osso", "Osso de monstro.",           RARITY_COMMON,    30,  10),
/* 83 */ _M("Fio de Aranha",     "Fio resistente de aranha.",  RARITY_COMMON,    40,  13),
/* 84 */ _M("Cristal Bruto",     "Cristal não lapidado.",      RARITY_UNCOMMON, 120,  40),
/* 85 */ _M("Pó Arcano",         "Pó com energia mágica.",     RARITY_COMMON,    70,  23),
/* 86 */ _M("Erva",              "Planta medicinal comum.",    RARITY_COMMON,    20,   6),
/* 87 */ _M("Água Pura",         "Água cristalina.",           RARITY_COMMON,    15,   5),
/* 88 */ _M("Resina",            "Substância adesiva de árvore.",RARITY_COMMON,  25,   8),
/* 89 */ _M("Carvão",            "Combustível para forjas.",   RARITY_COMMON,    20,   6),
/* 90 */ _M("Pena",              "Pena de ave rara.",          RARITY_COMMON,    35,  11),
/* 91 */ _M("Veneno de Cobra",   "Veneno concentrado.",        RARITY_UNCOMMON,  90,  30),
/* 92 */ _M("Musgo Brilhante",   "Musgo com propriedades mágicas.",RARITY_UNCOMMON,80, 26),
/* 93 */ _M("Madeira de Espinha","Madeira dura de espinheiro.",RARITY_COMMON,    55,  18),
/* 94 */ _M("Tinta Mágica",      "Tinta usada em runas.",      RARITY_UNCOMMON, 110,  36),

/* ---- Rare/legendary materials (95-109) ----------------- */
/* 95  */ _M("Escama de Dragão",  "Drop raro de dragão.",      RARITY_RARE,    1000, 333),
/* 96  */ _M("Runa Antiga",       "Runa do período Aetheria.", RARITY_EPIC,    2000, 666),
/* 97  */ _M("Chama Eterna",      "Fogo que nunca se apaga.",  RARITY_RARE,    1500, 500),
/* 98  */ _M("Lágrima Glacial",   "Gota de gelo eterno.",      RARITY_RARE,    1500, 500),
/* 99  */ _M("Núcleo da Tempestade","Essência de relâmpago.",  RARITY_RARE,    1500, 500),
/* 100 */ _M("Cristal do Vazio",  "Cristal de energia pura.",  RARITY_EPIC,    2500, 833),
/* 101 */ _M("Fragmento Sagrado", "Luz cristalizada.",         RARITY_RARE,    1200, 400),
/* 102 */ _M("Essência das Sombras","Trevas em forma sólida.", RARITY_RARE,    1200, 400),
/* 103 */ _M("Coração do Titã",   "Órgão de titã ancião.",     RARITY_EPIC,    3000,1000),
/* 104 */ _M("Orbe do Caos",      "Contém poder caótico.",     RARITY_EPIC,    2800, 933),
/* 105 */ _M("Pedra da Lua",      "Absorve energia lunar.",    RARITY_RARE,    1800, 600),
/* 106 */ _M("Pedra do Sol",      "Absorve energia solar.",    RARITY_RARE,    1800, 600),
/* 107 */ _M("Pedra do Vazio",    "Absorve energia do vazio.", RARITY_EPIC,    3000,1000),
/* 108 */ _M("Presa de Dragão",   "Presa afiada de dragão.",   RARITY_RARE,    1000, 333),
/* 109 */ _M("Pó Astral",         "Poeira do cosmos.",         RARITY_EPIC,    2500, 833),

/* ---- Key items (110-119) ------------------------------- */
/* 110 */ _K("Chave da Masmorra I",  "Abre a primeira masmorra."),
/* 111 */ _K("Chave da Masmorra II", "Abre a segunda masmorra."),
/* 112 */ _K("Mapa do Mundo",        "Mapa dos reinos de Aetheria."),
/* 113 */ _K("Codex Antigo",         "Manuscrito com receitas lendárias."),
/* 114 */ _K("Orbe de Cristal",      "Artefato que revela segredos."),
/* 115 */ _K("Insígnia do Herói",    "Prova da bravura do grupo."),
/* 116 */ _K("Passe da Guilda",      "Acesso à Guilda dos Aventureiros."),
/* 117 */ _K("Lente Celestial",      "Revela inimigos ocultos."),
/* 118 */ _K("Fragmento Estelar",    "Fragmento de uma estrela caída."),
/* 119 */ _K("Chave de Aetheria",    "A chave para o coração de Aetheria."),

/* ---- Legendary consumables (120-127) ------------------- */
/* 120 */ _C("Pedra Filosofal",  "Concede +1 nível permanente.",    RARITY_LEGENDARY,CEFF_FULL_HEAL,  0,0,USE_FIELD, FALSE,1,0,0),
/* 121 */ _C("Essência Infinita","Restaura tudo e +5 a todos stats.",RARITY_LEGENDARY,CEFF_PARTY_FULL,0,0,USE_ANYWHERE,TRUE,1,0,0),
/* 122 */ _C("Lágrima Divina",  "Revive todos + HP/MP completos.", RARITY_LEGENDARY,CEFF_PARTY_FULL, 0,0,USE_ANYWHERE,TRUE,1,0,0),
/* 123 */ _C("Gema de Alma",    "XP permanente +10%.",             RARITY_LEGENDARY,CEFF_XP_BOOST,   0,0,USE_FIELD, FALSE,1,0,0),
/* 124 */ _C("Codex Arcano",    "Desbloqueia todas as habilidades.",RARITY_LEGENDARY,CEFF_NONE,       0,0,USE_FIELD, FALSE,1,0,0),
/* 125 */ _M("Coração da Lâmina","Material lendário para armas.",  RARITY_LEGENDARY, 0, 0),
/* 126 */ _M("Coração do Escudo","Material lendário para armaduras.",RARITY_LEGENDARY,0, 0),
/* 127 */ _M("Núcleo Astral",   "Material lendário para acessórios.",RARITY_LEGENDARY,0,0),
};

#undef _E
#undef _C
#undef _M
#undef _K

/* ---- API ------------------------------------------------ */

const ItemDef* item_get(u8 id)
{
    if (id == 0 || id >= ITEM_DB_COUNT) return &g_item_db[0];
    return &g_item_db[id];
}

const char* item_rarity_name(ItemRarity r)
{
    if (r > RARITY_LEGENDARY) return "???";
    return s_rarity_names[r];
}

BOOL item_is_sellable(u8 id)
{
    const ItemDef* def = item_get(id);
    if (def->category == ICAT_KEY_ITEM) return FALSE;
    if (def->sell_price == 0) return FALSE;
    return TRUE;
}

BOOL item_is_stackable(u8 id)
{
    const ItemDef* def = item_get(id);
    return (BOOL)(def->max_stack > 1);
}
