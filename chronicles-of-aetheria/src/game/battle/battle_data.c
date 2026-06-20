#include "battle_data.h"
#include "battle_types.h"
#include "battler.h"
#include "battle_combo.h"
#include "gba_types.h"

/* ---- Skill table ---------------------------------------- */
const SkillDef g_skill_table[SKILL_COUNT] = {
[SKILL_ATTACK]    = { "Attack",    0,  60, ELEM_NONE,  TGT_SINGLE_ENEMY, EFF_DAMAGE,     TRUE,  0,              0,   MOD_ATK, 0, ANIM_ATTACK,  "Basic attack" },
[SKILL_SLASH]     = { "Slash",     4,  80, ELEM_NONE,  TGT_SINGLE_ENEMY, EFF_DAMAGE,     TRUE,  0,              0,   MOD_ATK, 0, ANIM_ATTACK,  "Heavy slash" },
[SKILL_DOUBLE_CUT]= { "DblCut",   8,  55, ELEM_NONE,  TGT_SINGLE_ENEMY, EFF_DAMAGE,     TRUE,  0,              0,   MOD_ATK, 0, ANIM_ATTACK,  "Two quick cuts" },
[SKILL_BREAK]     = { "Break",     6,  40, ELEM_NONE,  TGT_SINGLE_ENEMY, EFF_STAT_MOD,  TRUE,  0,              0,   MOD_ATK,-1, ANIM_ATTACK,  "Lower ATK" },
[SKILL_SHATTER]   = { "Shatter",  6,  40, ELEM_NONE,  TGT_SINGLE_ENEMY, EFF_STAT_MOD,  TRUE,  0,              0,   MOD_DEF,-1, ANIM_ATTACK,  "Lower DEF" },

[SKILL_FIRE]      = { "Fire",      5,  60, ELEM_FIRE,  TGT_SINGLE_ENEMY, EFF_DAMAGE,    FALSE, 0,              0,   MOD_ATK, 0, ANIM_FIRE,    "Fire bolt" },
[SKILL_FIRA]      = { "Fira",     12,  90, ELEM_FIRE,  TGT_SINGLE_ENEMY, EFF_DAMAGE,    FALSE, 0,              0,   MOD_ATK, 0, ANIM_FIRE,    "Bigger fire bolt" },
[SKILL_FIRAGA]    = { "Firaga",   24, 130, ELEM_FIRE,  TGT_SINGLE_ENEMY, EFF_DAMAGE,    FALSE, 0,              0,   MOD_ATK, 0, ANIM_FIRE,    "Ultimate fire" },
[SKILL_METEOR]    = { "Meteor",   30, 100, ELEM_FIRE,  TGT_ALL_ENEMIES,  EFF_DAMAGE,    FALSE, 0,              0,   MOD_ATK, 0, ANIM_FIRE,    "Multi-hit fire" },

[SKILL_WATER]     = { "Water",     5,  60, ELEM_WATER, TGT_SINGLE_ENEMY, EFF_DAMAGE,    FALSE, 0,              0,   MOD_ATK, 0, ANIM_WATER,   "Water shot" },
[SKILL_AQUA]      = { "Aqua",     12,  90, ELEM_WATER, TGT_SINGLE_ENEMY, EFF_DAMAGE,    FALSE, 0,              0,   MOD_ATK, 0, ANIM_WATER,   "Aqua blast" },
[SKILL_TIDAL]     = { "Tidal",    20,  80, ELEM_WATER, TGT_ALL_ENEMIES,  EFF_DAMAGE,    FALSE, 0,              0,   MOD_ATK, 0, ANIM_WATER,   "AOE water" },
[SKILL_TSUNAMI]   = { "Tsunami",  30, 140, ELEM_WATER, TGT_ALL_ENEMIES,  EFF_DAMAGE,    FALSE, 0,              0,   MOD_ATK, 0, ANIM_WATER,   "Massive wave" },

[SKILL_QUAKE]     = { "Quake",    10,  80, ELEM_EARTH, TGT_ALL_ENEMIES,  EFF_DAMAGE,    FALSE, 0,              0,   MOD_ATK, 0, ANIM_EARTH,   "Ground shake" },
[SKILL_STONE_EDGE]= { "StoneEdge",14, 110, ELEM_EARTH, TGT_SINGLE_ENEMY, EFF_DAMAGE,    FALSE, 0,              0,   MOD_ATK, 0, ANIM_EARTH,   "Rock spike" },
[SKILL_PETRIFY]   = { "Petrify",  18,   0, ELEM_EARTH, TGT_SINGLE_ENEMY, EFF_STATUS_ADD,FALSE, STATUS_STONE,  60,   MOD_ATK, 0, ANIM_EARTH,   "Inflict stone" },

[SKILL_GUST]      = { "Gust",      5,  55, ELEM_AIR,   TGT_SINGLE_ENEMY, EFF_DAMAGE,    FALSE, 0,              0,   MOD_ATK, 0, ANIM_AIR,     "Wind slice" },
[SKILL_CYCLONE]   = { "Cyclone",  12,  85, ELEM_AIR,   TGT_ALL_ENEMIES,  EFF_DAMAGE,    FALSE, 0,              0,   MOD_ATK, 0, ANIM_AIR,     "Wind sweep" },
[SKILL_TORNADO]   = { "Tornado",  22, 120, ELEM_AIR,   TGT_SINGLE_ENEMY, EFF_DAMAGE,    FALSE, 0,              0,   MOD_ATK, 0, ANIM_AIR,     "Tornado" },

[SKILL_FLASH]     = { "Flash",     8,   0, ELEM_LIGHT, TGT_ALL_ENEMIES,  EFF_STATUS_ADD,FALSE, STATUS_BLIND,  80,   MOD_ATK, 0, ANIM_LIGHT,   "Blind enemies" },
[SKILL_HOLY]      = { "Holy",     20, 120, ELEM_LIGHT, TGT_SINGLE_ENEMY, EFF_DAMAGE,    FALSE, 0,              0,   MOD_ATK, 0, ANIM_LIGHT,   "Holy light" },
[SKILL_RADIANCE]  = { "Radiance", 28, 100, ELEM_LIGHT, TGT_ALL_ENEMIES,  EFF_DAMAGE,    FALSE, 0,              0,   MOD_ATK, 0, ANIM_LIGHT,   "Radiant burst" },
[SKILL_REVIVE]    = { "Revive",   30,  50, ELEM_LIGHT, TGT_SINGLE_ALLY,  EFF_REVIVE,    FALSE, 0,              0,   MOD_ATK, 0, ANIM_HEAL,    "Revive at 50% HP" },

[SKILL_SHADOW]    = { "Shadow",    8,  70, ELEM_DARK,  TGT_SINGLE_ENEMY, EFF_DAMAGE,    FALSE, 0,              0,   MOD_ATK, 0, ANIM_DARK,    "Dark bolt" },
[SKILL_DRAIN]     = { "Drain",    14,  80, ELEM_DARK,  TGT_SINGLE_ENEMY, EFF_DRAIN,     FALSE, 0,              0,   MOD_ATK, 0, ANIM_DRAIN,   "HP drain" },
[SKILL_DOOM]      = { "Doom",     20,   0, ELEM_DARK,  TGT_SINGLE_ENEMY, EFF_STATUS_ADD,FALSE, STATUS_DOOM,   70,   MOD_ATK, 0, ANIM_DARK,    "Countdown KO" },
[SKILL_ECLIPSE]   = { "Eclipse",  26,  90, ELEM_DARK,  TGT_ALL_ENEMIES,  EFF_STATUS_ADD,FALSE, STATUS_SILENCE,50,   MOD_ATK, 0, ANIM_DARK,    "Dark+silence" },

[SKILL_HEAL]      = { "Heal",      5,  60, ELEM_LIGHT, TGT_SINGLE_ALLY,  EFF_HEAL,      FALSE, 0,              0,   MOD_ATK, 0, ANIM_HEAL,    "Restore HP" },
[SKILL_CURA]      = { "Cura",     14,  80, ELEM_LIGHT, TGT_ALL_ALLIES,   EFF_HEAL,      FALSE, 0,              0,   MOD_ATK, 0, ANIM_HEAL,    "AOE heal" },
[SKILL_REGEN_CAST]= { "Regen",   10,   0, ELEM_LIGHT, TGT_SINGLE_ALLY,  EFF_STATUS_ADD,FALSE, STATUS_REGEN, 100,   MOD_ATK, 0, ANIM_HEAL,    "Apply regen" },
[SKILL_HASTE_CAST]= { "Haste",   12,   0, ELEM_NONE,  TGT_SINGLE_ALLY,  EFF_STATUS_ADD,FALSE, STATUS_HASTE, 100,   MOD_ATK, 0, ANIM_HEAL,    "Apply haste" },
[SKILL_SLOW_CAST] = { "Slow",    10,   0, ELEM_NONE,  TGT_SINGLE_ENEMY, EFF_STATUS_ADD,FALSE, STATUS_SLOW,   85,   MOD_ATK, 0, ANIM_DARK,    "Apply slow" },
[SKILL_BARRIER_CAST]={ "Barrier",12,  0, ELEM_LIGHT, TGT_SINGLE_ALLY,  EFF_STATUS_ADD,FALSE, STATUS_BARRIER,100,  MOD_ATK, 0, ANIM_HEAL,    "Magic shield" },
[SKILL_SILENCE_CAST]={ "Silence", 8,  0, ELEM_DARK,  TGT_SINGLE_ENEMY, EFF_STATUS_ADD,FALSE, STATUS_SILENCE,75,   MOD_ATK, 0, ANIM_DARK,    "Silence" },
[SKILL_DISPEL]    = { "Dispel",   14,  0, ELEM_LIGHT, TGT_SINGLE_ENEMY, EFF_STATUS_REMOVE,FALSE,0,            0,   MOD_ATK, 0, ANIM_LIGHT,   "Remove buffs" },
[SKILL_FULL_CURE] = { "FullCure", 20,  0, ELEM_LIGHT, TGT_SINGLE_ALLY,  EFF_STATUS_REMOVE,FALSE,0,            0,   MOD_ATK, 0, ANIM_HEAL,    "Cure all status" },

[SKILL_UNLEASH_SOLEIL]={ "Soleil",40,180,ELEM_FIRE, TGT_ALL_ENEMIES, EFF_DAMAGE,       FALSE, 0,              0,   MOD_ATK, 0, ANIM_UNLEASH_SOLEIL, "Fire unleash" },
[SKILL_UNLEASH_TIDE]  ={ "Tide",  40,180,ELEM_WATER,TGT_ALL_ENEMIES, EFF_DAMAGE,       FALSE, 0,              0,   MOD_ATK, 0, ANIM_UNLEASH_TIDE,   "Water unleash" },
[SKILL_UNLEASH_GAIA]  ={ "Gaia",  40,180,ELEM_EARTH,TGT_ALL_ENEMIES, EFF_DAMAGE,       FALSE, 0,              0,   MOD_ATK, 0, ANIM_UNLEASH_GAIA,   "Earth unleash" },
[SKILL_UNLEASH_STORM] ={ "Storm", 40,180,ELEM_AIR,  TGT_ALL_ENEMIES, EFF_DAMAGE,       FALSE, 0,              0,   MOD_ATK, 0, ANIM_UNLEASH_STORM,  "Storm unleash" },
};

/* ---- Enemy table ---------------------------------------- */
static const u8 s_res_norm[7]   = { 100,100,100,100,100,100,100 };
static const u8 s_res_fire[7]   = { 100, 50,150,100,100,100, 50 };
static const u8 s_res_water[7]  = { 100,150, 50,100,100,100,100 };
static const u8 s_res_earth[7]  = { 100,100,100, 50,150,100,100 };
static const u8 s_res_undead[7] = { 100,150,100,100,100,200, 50 };

static const u8 s_slime_sk[]    = { SKILL_ATTACK };
static const u8 s_goblin_sk[]   = { SKILL_ATTACK, SKILL_SLASH };
static const u8 s_wolf_sk[]     = { SKILL_ATTACK, SKILL_BREAK };
static const u8 s_orc_sk[]      = { SKILL_ATTACK, SKILL_SLASH, SKILL_SHATTER };
static const u8 s_mage_sk[]     = { SKILL_FIRE, SKILL_WATER, SKILL_SLOW_CAST };
static const u8 s_dragon_sk[]   = { SKILL_FIRAGA, SKILL_SLASH, SKILL_PETRIFY };
static const u8 s_undead_sk[]   = { SKILL_ATTACK, SKILL_DRAIN, SKILL_SHADOW };
static const u8 s_eleF_sk[]     = { SKILL_FIRE, SKILL_FIRA };
static const u8 s_eleW_sk[]     = { SKILL_WATER, SKILL_AQUA };
static const u8 s_golem_sk[]    = { SKILL_ATTACK, SKILL_QUAKE, SKILL_SHATTER };
static const u8 s_dknight_sk[]  = { SKILL_SLASH, SKILL_SHADOW, SKILL_DOOM };
static const u8 s_serpent_sk[]  = { SKILL_SLASH, SKILL_PETRIFY, SKILL_TIDAL, SKILL_ECLIPSE };

const EnemyDef g_enemy_table[ENEMY_COUNT] = {
/*  name         spr pal elem       resist         hp  mp atk def mag mdf spd  ai  exp  gold skills   cnt */
  { "Slime",      0,  0, ELEM_WATER,{100,150, 50,100,100,100,100}, 30, 0, 12,  8,  4,  4, 10, 0,  10,  5, s_slime_sk,  1 },
  { "Goblin",     1,  1, ELEM_NONE, s_res_norm,                    55, 0, 20, 14,  6,  6, 18, 0,  18, 10, s_goblin_sk, 2 },
  { "Wolf",       2,  2, ELEM_NONE, s_res_norm,                    70, 0, 28, 12,  6,  6, 25, 0,  22, 12, s_wolf_sk,   2 },
  { "Orc",        3,  3, ELEM_EARTH,s_res_earth,                  100, 0, 35, 22, 10, 10, 14, 0,  35, 18, s_orc_sk,    3 },
  { "Mage",       4,  4, ELEM_FIRE, s_res_fire,                    60,40, 12, 10, 30, 20, 20, 1,  40, 22, s_mage_sk,   3 },
  { "Dragon",     5,  5, ELEM_FIRE, s_res_fire,                   280,40, 55, 40, 50, 35, 22, 1,  90, 50, s_dragon_sk, 3 },
  { "Undead",     6,  6, ELEM_DARK, s_res_undead,                  80, 0, 25, 18, 15, 10, 12, 0,  38, 20, s_undead_sk, 3 },
  { "FireEle",    7,  7, ELEM_FIRE, s_res_fire,                    90,30, 20, 15, 40, 10, 20, 0,  42, 24, s_eleF_sk,   2 },
  { "WaterEle",   8,  8, ELEM_WATER,s_res_water,                   90,30, 20, 15, 40, 10, 18, 0,  42, 24, s_eleW_sk,   2 },
  { "Golem",      9,  9, ELEM_EARTH,s_res_earth,                  200, 0, 50, 60,  8, 30,  8, 0,  70, 40, s_golem_sk,  3 },
  { "DkKnight",  10, 10, ELEM_DARK, s_res_undead,                 180,20, 50, 35, 30, 25, 18, 1,  85, 45, s_dknight_sk,3 },
  { "Serpent",   11, 11, ELEM_WATER,s_res_water,                  500,80, 60, 50, 60, 45, 20, 2, 200,100, s_serpent_sk,4 },
};

/* ---- Enemy groups (no name field per battler.h) ---------- */
const EnemyGroup g_enemy_groups[GROUP_COUNT] = {
  { { ENEMY_SLIME,ENEMY_SLIME,ENEMY_SLIME,0,0,0 },        3, 0, 1, 1 },
  { { ENEMY_GOBLIN,ENEMY_GOBLIN,ENEMY_GOBLIN,ENEMY_GOBLIN,0,0 }, 4, 0, 1, 1 },
  { { ENEMY_WOLF,ENEMY_GOBLIN,ENEMY_GOBLIN,0,0,0 },        3, 0, 1, 2 },
  { { ENEMY_ELEMENTAL_F,ENEMY_ELEMENTAL_F,ENEMY_ELEMENTAL_W,0,0,0 }, 3, 1, 2, 2 },
  { { ENEMY_BOSS_SERPENT,ENEMY_ORC,ENEMY_ORC,0,0,0 },      3, 2, 3, 4 },
  { { ENEMY_UNDEAD,ENEMY_UNDEAD,ENEMY_UNDEAD,ENEMY_MAGE,0,0 }, 4, 1, 2, 2 },
};

/* ---- Combo table ---------------------------------------- */
const ComboDef g_combo_table[BATTLE_COMBO_MAX] = {
  {
    "FireStorm", { 0,1,0 }, 2, 160, ELEM_FIRE, TGT_ALL_ENEMIES,
    EFF_DAMAGE, EFF_STATUS_ADD, STATUS_BURN, 40, ANIM_COMBO_FIRE, "Twin fire tornado"
  },
  {
    "TidalWave", { 1,2,0 }, 2, 160, ELEM_WATER, TGT_ALL_ENEMIES,
    EFF_DAMAGE, EFF_STATUS_ADD, STATUS_FREEZE, 30, ANIM_COMBO_TIDAL, "Massive tidal surge"
  },
  {
    "StormBlade", { 0,3,0 }, 2, 140, ELEM_AIR, TGT_SINGLE_ENEMY,
    EFF_DAMAGE, EFF_STAT_MOD, 0, 0, ANIM_COMBO_STORM, "Wind-charged slash"
  },
  {
    "HolyNova", { 2,3,0 }, 2, 150, ELEM_LIGHT, TGT_ALL_ENEMIES,
    EFF_DAMAGE, EFF_STATUS_ADD, STATUS_BLIND, 60, ANIM_COMBO_HOLY, "Radiant explosion"
  },
  {
    "ShadowBind", { 1,3,0 }, 2, 130, ELEM_DARK, TGT_ALL_ENEMIES,
    EFF_DAMAGE, EFF_STATUS_ADD, STATUS_CONFUSE, 50, ANIM_COMBO_SHADOW, "Dark tentacle bind"
  },
  {
    "TriNova", { 0,1,2 }, 3, 220, ELEM_LIGHT, TGT_ALL_ENEMIES,
    EFF_DAMAGE, EFF_STATUS_REMOVE, 0, 0, ANIM_COMBO_HOLY, "Ultimate triple tech"
  },
  /* Remaining slots empty */
  { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 },
  { 0 }, { 0 }, { 0 }, { 0 },
};

const u8 g_combo_count = 6;

/* ---- Character roster ----------------------------------- */
static const u8 s_c0_sk[] = { SKILL_ATTACK, SKILL_SLASH, SKILL_FIRE, SKILL_FIRA, SKILL_UNLEASH_SOLEIL };
static const u8 s_c1_sk[] = { SKILL_ATTACK, SKILL_AQUA, SKILL_TIDAL, SKILL_HEAL, SKILL_UNLEASH_TIDE };
static const u8 s_c2_sk[] = { SKILL_ATTACK, SKILL_QUAKE, SKILL_STONE_EDGE, SKILL_CURA, SKILL_UNLEASH_GAIA };
static const u8 s_c3_sk[] = { SKILL_ATTACK, SKILL_GUST, SKILL_CYCLONE, SKILL_HASTE_CAST, SKILL_UNLEASH_STORM };

const CharDef g_char_table[BATTLE_ALLY_MAX] = {
  { "Kael",  0, 0, ELEM_FIRE,  220, 60, 45, 30, 35, 28, 22, {100,50,150,100,100,100,100}, s_c0_sk, 5 },
  { "Lyra",  1, 1, ELEM_WATER, 180, 90, 28, 22, 55, 40, 26, {100,150,50,100,100,100,100}, s_c1_sk, 5 },
  { "Gorin", 2, 2, ELEM_EARTH, 260, 40, 55, 50, 20, 35, 16, {100,100,100,50,150,100,100}, s_c2_sk, 5 },
  { "Aria",  3, 3, ELEM_AIR,   200, 75, 32, 28, 48, 35, 30, {100,100,100,100,100,100,100},s_c3_sk, 5 },
};

/* ---- Data access ---------------------------------------- */

void battle_data_init(void) { /* ROM const — nothing to do */ }

const SkillDef*   skill_get(u8 id)    { if (id >= SKILL_COUNT)  return &g_skill_table[0]; return &g_skill_table[id]; }
const EnemyDef*   enemy_get(u8 id)    { if (id >= ENEMY_COUNT)  return &g_enemy_table[0]; return &g_enemy_table[id]; }
const EnemyGroup* group_get(u8 id)    { if (id >= GROUP_COUNT)  return &g_enemy_groups[0]; return &g_enemy_groups[id]; }
const ComboDef*   combo_get_by_id(u8 id) { if (id >= g_combo_count) return &g_combo_table[0]; return &g_combo_table[id]; }
