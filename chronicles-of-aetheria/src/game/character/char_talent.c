#include "char_talent.h"
#include "char_skill.h"
#include "gba_types.h"

/* ---- ROM talent tree data -------------------------------- */

static const ClassTalentTree s_guardian_tree = {
    .branches = {
        /* Branch 0: Fortress */
        {
            .branch_name = "Fortaleza",
            .nodes = {
                { "Pele de Pedra",    "+8 DEF",                   TALENT_STAT_BONUS,    TEFF_DSTAT_BONUS,   8,   DSTAT_DEF,       1, 1,  0xFF },
                { "Muralha Viva",     "+15 HP max",               TALENT_STAT_BONUS,    TEFF_DSTAT_BONUS,   15,  DSTAT_HP_MAX,    1, 1,  0    },
                { "Escudo Reforçado", "DEF +12 adicional",        TALENT_STAT_BONUS,    TEFF_DSTAT_BONUS,   12,  DSTAT_DEF,       1, 20, 1    },
                { "Barreira Divina",  "Dano -10% (passivo)",      TALENT_PASSIVE_EFF,   TEFF_GUARD_AURA,    10,  0,               1, 40, 2    },
                { "Fortaleza Eterna", "HP+50, DEF+20 capstone",   TALENT_CAPSTONE,      TEFF_DSTAT_BONUS,   50,  DSTAT_HP_MAX,    1, 60, 3    },
            },
        },
        /* Branch 1: Warlord */
        {
            .branch_name = "Senhor da Guerra",
            .nodes = {
                { "Golpe Pesado",     "+6 ATK",                   TALENT_STAT_BONUS,    TEFF_DSTAT_BONUS,   6,   DSTAT_ATK,       1, 1,  0xFF },
                { "Vigor de Batalha", "+4 STR",                   TALENT_STAT_BONUS,    TEFF_ATTR_BONUS,    4,   ATTR_STR,        1, 1,  0    },
                { "Contra-Ataque",    "15% chance counter",       TALENT_PASSIVE_EFF,   TEFF_COUNTER_CHANCE,15,  0,               1, 20, 1    },
                { "Fúria do Guardião","ATK+16",                   TALENT_STAT_BONUS,    TEFF_DSTAT_BONUS,   16,  DSTAT_ATK,       1, 40, 2    },
                { "Juramento de Ferro","Unlock Guardian Strike+", TALENT_UNLOCK_SKILL,  TEFF_UNLOCK_SKILL,  0,   CSKILL_GUARDIAN_STRIKE, 1, 60, 3 },
            },
        },
        /* Branch 2: Paladin */
        {
            .branch_name = "Paladino",
            .nodes = {
                { "Toque Sagrado",    "HP Regen 3%/turno",        TALENT_PASSIVE_EFF,   TEFF_HP_REGEN_PCT,  3,   0,               1, 1,  0xFF },
                { "Luz Curativa",     "+10 MDEF",                 TALENT_STAT_BONUS,    TEFF_DSTAT_BONUS,   10,  DSTAT_MDEF,      1, 1,  0    },
                { "Aura Protetora",   "Party DEF +8",             TALENT_PASSIVE_EFF,   TEFF_GUARD_AURA,    8,   0,               1, 20, 1    },
                { "Escudo Sagrado",   "Dano mágico -15%",         TALENT_PASSIVE_EFF,   TEFF_MANA_SHIELD_PCT,15, 0,               1, 40, 2    },
                { "Juramento do Guardião","Cura 20% HP ao taunt", TALENT_CAPSTONE,      TEFF_HP_REGEN_PCT,  20,  0,               1, 60, 3    },
            },
        },
    },
};

static const ClassTalentTree s_arcsword_tree = {
    .branches = {
        /* Branch 0: Runic Edge */
        {
            .branch_name = "Lâmina Rúnica",
            .nodes = {
                { "Aço Infundido",    "+7 ATK",                   TALENT_STAT_BONUS,    TEFF_DSTAT_BONUS,   7,   DSTAT_ATK,       1, 1,  0xFF },
                { "Fio Elemental",    "Infusão +2 turnos",        TALENT_PASSIVE_EFF,   TEFF_INFUSE_EXTEND, 2,   0,               1, 1,  0    },
                { "Corte Duplo",      "+12% dano físico",         TALENT_PASSIVE_EFF,   TEFF_DMG_BONUS_PCT, 12,  0,               1, 20, 1    },
                { "Lâmina Mestra",    "+14 ATK",                  TALENT_STAT_BONUS,    TEFF_DSTAT_BONUS,   14,  DSTAT_ATK,       1, 40, 2    },
                { "Espada dos Rúnas", "Unlock Arcane Unleash",    TALENT_UNLOCK_SKILL,  TEFF_UNLOCK_SKILL,  0,   CSKILL_ARCANE_UNLEASH, 1, 60, 3 },
            },
        },
        /* Branch 1: Arcane Art */
        {
            .branch_name = "Arte Arcana",
            .nodes = {
                { "Canalização",      "+6 MAG",                   TALENT_STAT_BONUS,    TEFF_DSTAT_BONUS,   6,   DSTAT_MAG,       1, 1,  0xFF },
                { "Mente Aguçada",    "+4 INT",                   TALENT_STAT_BONUS,    TEFF_ATTR_BONUS,    4,   ATTR_INT,        1, 1,  0    },
                { "Amplificação",     "+15% dano mágico",         TALENT_PASSIVE_EFF,   TEFF_MAGIC_AMP_PCT, 15,  0,               1, 20, 1    },
                { "Maestria Arcana",  "MP Regen 4%/turno",        TALENT_PASSIVE_EFF,   TEFF_MP_REGEN_PCT,  4,   0,               1, 40, 2    },
                { "Resonância Total", "+20 MAG, +20 ATK",         TALENT_CAPSTONE,      TEFF_DSTAT_BONUS,   20,  DSTAT_MAG,       1, 60, 3    },
            },
        },
        /* Branch 2: Resonance */
        {
            .branch_name = "Ressonância",
            .nodes = {
                { "Sintonia",         "+5 DEX",                   TALENT_STAT_BONUS,    TEFF_ATTR_BONUS,    5,   ATTR_DEX,        1, 1,  0xFF },
                { "Elo Arcano",       "+8 SPD",                   TALENT_STAT_BONUS,    TEFF_DSTAT_BONUS,   8,   DSTAT_SPD,       1, 1,  0    },
                { "Impulso Dual",     "Dual strike +20%",         TALENT_PASSIVE_EFF,   TEFF_DUAL_STRIKE,   20,  0,               1, 20, 1    },
                { "Harmonia Rúnica",  "Infusão: +25% dano mágico",TALENT_PASSIVE_EFF,   TEFF_INFUSE_EXTEND, 1,   0,               1, 40, 2    },
                { "Ressonância Dual", "Flag: dual resonance cap", TALENT_CAPSTONE,      TEFF_UNLOCK_SKILL,  0,   CSKILL_DUAL_RESONANCE, 1, 60, 3 },
            },
        },
    },
};

static const ClassTalentTree s_arcanist_tree = {
    .branches = {
        /* Branch 0: Pyromancy */
        {
            .branch_name = "Piromancia",
            .nodes = {
                { "Chama Interna",    "+8 MAG",                   TALENT_STAT_BONUS,    TEFF_DSTAT_BONUS,   8,   DSTAT_MAG,       1, 1,  0xFF },
                { "Afinidade do Fogo","Fogo +30%",                TALENT_PASSIVE_EFF,   TEFF_ELEM_MASTERY,  30,  0,               1, 1,  0    },
                { "Mente em Chamas",  "+12% dano mágico",         TALENT_PASSIVE_EFF,   TEFF_MAGIC_AMP_PCT, 12,  0,               1, 20, 1    },
                { "Inferno Interior", "+16 MAG",                  TALENT_STAT_BONUS,    TEFF_DSTAT_BONUS,   16,  DSTAT_MAG,       1, 40, 2    },
                { "Arcana Suprema",   "Unlock Grand Arcana",      TALENT_UNLOCK_SKILL,  TEFF_UNLOCK_SKILL,  0,   CSKILL_GRAND_ARCANA, 1, 60, 3 },
            },
        },
        /* Branch 1: Chronomancy */
        {
            .branch_name = "Cronomancia",
            .nodes = {
                { "Reflexo Temporal", "+8 SPD",                   TALENT_STAT_BONUS,    TEFF_DSTAT_BONUS,   8,   DSTAT_SPD,       1, 1,  0xFF },
                { "Mente Acelerada",  "+4 DEX",                   TALENT_STAT_BONUS,    TEFF_ATTR_BONUS,    4,   ATTR_DEX,        1, 1,  0    },
                { "Carregamento +",   "Charge mult +20%",         TALENT_PASSIVE_EFF,   TEFF_CHARGE_MULT,   20,  0,               1, 20, 1    },
                { "Haste Arcana",     "SPD+14, EVA+8",            TALENT_STAT_BONUS,    TEFF_DSTAT_BONUS,   14,  DSTAT_SPD,       1, 40, 2    },
                { "Paradoxo Temporal","MP Regen 8%/turno",        TALENT_CAPSTONE,      TEFF_MP_REGEN_PCT,  8,   0,               1, 60, 3    },
            },
        },
        /* Branch 2: Void Walker */
        {
            .branch_name = "Andarilho do Vazio",
            .nodes = {
                { "Toque do Vazio",   "+6 INT",                   TALENT_STAT_BONUS,    TEFF_ATTR_BONUS,    6,   ATTR_INT,        1, 1,  0xFF },
                { "Escudo de Mana",   "Mana shield 10%",          TALENT_PASSIVE_EFF,   TEFF_MANA_SHIELD_PCT,10, 0,               1, 1,  0    },
                { "Drenagem Arcana",  "Lifesteal mágico 8%",      TALENT_PASSIVE_EFF,   TEFF_LIFESTEAL_PCT, 8,   0,               1, 20, 1    },
                { "Forma do Vazio",   "+20% dano trevas",         TALENT_PASSIVE_EFF,   TEFF_ELEM_MASTERY,  20,  0,               1, 40, 2    },
                { "Forma Void",       "Flag: void form activo",   TALENT_CAPSTONE,      TEFF_UNLOCK_SKILL,  0,   CSKILL_ARC_VOID, 1, 60, 3    },
            },
        },
    },
};

static const ClassTalentTree s_hunter_tree = {
    .branches = {
        /* Branch 0: Predator */
        {
            .branch_name = "Predador",
            .nodes = {
                { "Olho Afiado",      "+8 CRIT",                  TALENT_STAT_BONUS,    TEFF_DSTAT_BONUS,   8,   DSTAT_CRIT,      1, 1,  0xFF },
                { "Presa Marcada",    "Marked bonus +15%",        TALENT_PASSIVE_EFF,   TEFF_MARK_BONUS,    15,  0,               1, 1,  0    },
                { "Golpe Mortal",     "Crit mult +50%",           TALENT_PASSIVE_EFF,   TEFF_CRIT_MULT,     50,  0,               1, 20, 1    },
                { "Instinto",         "+12 DEX",                  TALENT_STAT_BONUS,    TEFF_ATTR_BONUS,    12,  ATTR_DEX,        1, 40, 2    },
                { "Instinto Predatório","Flag: hunter instinct",  TALENT_CAPSTONE,      TEFF_UNLOCK_SKILL,  0,   CSKILL_DEATHMARK, 1, 60, 3  },
            },
        },
        /* Branch 1: Shadowblade */
        {
            .branch_name = "Lâmina das Sombras",
            .nodes = {
                { "Passo Silencioso", "+8 EVA",                   TALENT_STAT_BONUS,    TEFF_DSTAT_BONUS,   8,   DSTAT_EVASION,   1, 1,  0xFF },
                { "Dual Wield",       "Dual strike +25%",         TALENT_PASSIVE_EFF,   TEFF_DUAL_STRIKE,   25,  0,               1, 1,  0    },
                { "Veneno Potente",   "+12% dano físico",         TALENT_PASSIVE_EFF,   TEFF_DMG_BONUS_PCT, 12,  0,               1, 20, 1    },
                { "Roubo de Vida",    "Lifesteal 10%",            TALENT_PASSIVE_EFF,   TEFF_LIFESTEAL_PCT, 10,  0,               1, 40, 2    },
                { "Sombra Absoluta",  "EVA+20, SPD+15",           TALENT_CAPSTONE,      TEFF_DSTAT_BONUS,   20,  DSTAT_EVASION,   1, 60, 3    },
            },
        },
        /* Branch 2: Ranger */
        {
            .branch_name = "Arqueiro",
            .nodes = {
                { "Tiro Preciso",     "+6 ATK",                   TALENT_STAT_BONUS,    TEFF_DSTAT_BONUS,   6,   DSTAT_ATK,       1, 1,  0xFF },
                { "Carcaj Cheio",     "Trap power +20",           TALENT_PASSIVE_EFF,   TEFF_TRAP_POWER,    20,  0,               1, 1,  0    },
                { "Chuva de Flechas+","Unlock Rain of Arrows",    TALENT_UNLOCK_SKILL,  TEFF_UNLOCK_SKILL,  0,   CSKILL_RAIN_OF_ARROWS, 1, 20, 1 },
                { "Mira Perfeita",    "+16 ATK",                  TALENT_STAT_BONUS,    TEFF_DSTAT_BONUS,   16,  DSTAT_ATK,       1, 40, 2    },
                { "Morte Marcada",    "Unlock Deathmark",         TALENT_CAPSTONE,      TEFF_UNLOCK_SKILL,  0,   CSKILL_DEATHMARK, 1, 60, 3   },
            },
        },
    },
};

const ClassTalentTree g_talent_trees[CLASS_COUNT] = {
    [CLASS_GUARDIAN] = s_guardian_tree,
    [CLASS_ARCSWORD] = s_arcsword_tree,
    [CLASS_ARCANIST] = s_arcanist_tree,
    [CLASS_HUNTER]   = s_hunter_tree,
};

/* ---- API ------------------------------------------------ */

void talent_state_init(TalentState* ts)
{
    u8 i;
    ts->unlocked     = 0;
    ts->points_spent = 0;
    ts->points_avail = 0;
    ts->extra_skill_count = 0;
    ts->flags        = 0;
    for (i = 0; i < DSTAT_COUNT; i++) ts->dstat_bonus[i] = 0;
    for (i = 0; i < ATTR_COUNT;  i++) ts->attr_bonus[i]  = 0;
    for (i = 0; i < 4;           i++) ts->extra_skills[i] = 0xFF;
}

void talent_gain_point(TalentState* ts)
{
    if (ts->points_avail < 99) ts->points_avail++;
}

BOOL talent_is_unlocked(const TalentState* ts, u8 branch, u8 node)
{
    u8 bit = (u8)(branch * TALENT_NODES + node);
    return (BOOL)((ts->unlocked & (u16)(1u << bit)) != 0);
}

BOOL talent_can_unlock(const TalentState* ts, CharClass cls,
                        u8 branch, u8 node, u8 char_level)
{
    const TalentNode* tn;
    if (branch >= TALENT_BRANCHES || node >= TALENT_NODES) return FALSE;
    tn = &g_talent_trees[cls].branches[branch].nodes[node];

    if (talent_is_unlocked(ts, branch, node))      return FALSE;
    if (ts->points_avail < tn->cost)               return FALSE;
    if (char_level < tn->req_level)                return FALSE;
    if (tn->req_node != 0xFF &&
        !talent_is_unlocked(ts, branch, tn->req_node)) return FALSE;
    return TRUE;
}

static void _apply_node(TalentState* ts, const TalentNode* tn)
{
    switch (tn->effect) {
    case TEFF_ATTR_BONUS:
        if (tn->effect_param < ATTR_COUNT)
            ts->attr_bonus[tn->effect_param] += tn->effect_val;
        break;
    case TEFF_DSTAT_BONUS:
        if (tn->effect_param < DSTAT_COUNT)
            ts->dstat_bonus[tn->effect_param] += tn->effect_val;
        break;
    case TEFF_UNLOCK_SKILL:
        if (ts->extra_skill_count < 4) {
            ts->extra_skills[ts->extra_skill_count++] = tn->effect_param;
        }
        break;
    case TEFF_HP_REGEN_PCT:
        ts->dstat_bonus[DSTAT_HP_MAX] += tn->effect_val;
        break;
    case TEFF_MP_REGEN_PCT:
        ts->dstat_bonus[DSTAT_MP_MAX] += tn->effect_val;
        break;
    case TEFF_DMG_BONUS_PCT:
        ts->dstat_bonus[DSTAT_ATK] += tn->effect_val;
        break;
    case TEFF_MAGIC_AMP_PCT:
        ts->dstat_bonus[DSTAT_MAG] += tn->effect_val;
        break;
    case TEFF_CRIT_MULT:
        ts->dstat_bonus[DSTAT_CRIT] += tn->effect_val;
        break;
    case TEFF_GUARD_AURA:
        ts->dstat_bonus[DSTAT_DEF] += tn->effect_val;
        break;
    case TEFF_COUNTER_CHANCE:
        /* encoded in flags/crit as game mechanic */
        ts->flags |= TFLG_GUARDIAN_OATH;
        break;
    case TEFF_LIFESTEAL_PCT:
        ts->flags |= TFLG_ARCSWORD_RESONANCE;
        break;
    case TEFF_MANA_SHIELD_PCT:
        ts->flags |= TFLG_ARCANIST_VOIDFORM;
        break;
    case TEFF_MARK_BONUS:
        ts->flags |= TFLG_HUNTER_INSTINCT;
        break;
    default:
        break;
    }
}

void talent_recompute_bonuses(TalentState* ts, CharClass cls)
{
    u8 b, n, i;
    for (i = 0; i < DSTAT_COUNT; i++) ts->dstat_bonus[i] = 0;
    for (i = 0; i < ATTR_COUNT;  i++) ts->attr_bonus[i]  = 0;
    ts->extra_skill_count = 0;
    ts->flags = 0;
    for (b = 0; b < TALENT_BRANCHES; b++) {
        for (n = 0; n < TALENT_NODES; n++) {
            if (talent_is_unlocked(ts, b, n)) {
                _apply_node(ts, &g_talent_trees[cls].branches[b].nodes[n]);
            }
        }
    }
}

BOOL talent_unlock(TalentState* ts, CharClass cls, u8 branch, u8 node,
                   u8 char_level)
{
    const TalentNode* tn;
    u8 bit;
    if (!talent_can_unlock(ts, cls, branch, node, char_level)) return FALSE;
    tn  = &g_talent_trees[cls].branches[branch].nodes[node];
    bit = (u8)(branch * TALENT_NODES + node);
    ts->unlocked     |= (u16)(1u << bit);
    ts->points_spent += tn->cost;
    ts->points_avail -= tn->cost;
    _apply_node(ts, tn);
    return TRUE;
}

const TalentNode* talent_get_node(CharClass cls, u8 branch, u8 node)
{
    if (cls >= CLASS_COUNT || branch >= TALENT_BRANCHES || node >= TALENT_NODES)
        return NULL;
    return &g_talent_trees[cls].branches[branch].nodes[node];
}

const TalentBranch* talent_get_branch(CharClass cls, u8 branch)
{
    if (cls >= CLASS_COUNT || branch >= TALENT_BRANCHES) return NULL;
    return &g_talent_trees[cls].branches[branch];
}

void talent_reset(TalentState* ts)
{
    u8 spent = ts->points_spent;
    talent_state_init(ts);
    ts->points_avail = spent; /* refund all points */
}
