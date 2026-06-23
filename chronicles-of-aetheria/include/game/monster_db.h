#ifndef MONSTER_DB_H
#define MONSTER_DB_H

#include "gba_types.h"
#include "battle_types.h"
#include "battler.h"

/* =========================================================
 * Monster Database — 150 monsters organized by region
 *
 * Each MonsterDef extends EnemyDef with:
 *   - Region tag (which area it spawns in)
 *   - Loot table (up to 3 drops with probability)
 *   - Behavior pattern (AI archetype)
 *   - Elemental weaknesses / resistances (inherit from EnemyDef.resist)
 *   - Boss flag (has unique music and phases if TRUE)
 *
 * ID space: 0-149 (fits in u8 up to 255 — safe)
 * ========================================================= */

/* ---- Region IDs ----------------------------------------- */
#define REGION_NONE         0xFF
#define REGION_AELWYN       0    /* Starter village surroundings */
#define REGION_VERDANT      1    /* Verdant Forest */
#define REGION_CAVES        2    /* Crystal Caves */
#define REGION_SWAMP        3    /* Murkhaven Swamp */
#define REGION_MOUNTAINS    4    /* Ironpeak Mountains */
#define REGION_RUINS        5    /* Ancient Ruins */
#define REGION_VOLCANO      6    /* Ignarath Volcano */
#define REGION_SNOWLANDS    7    /* Frostpeak Snowlands */
#define REGION_OCEAN        8    /* Sunken Isles / Ocean */
#define REGION_SHADOWREALM  9    /* Shadow Realm */
#define REGION_AETHERIA     10   /* Aetheria (final area) */
#define REGION_BOSS         11   /* Boss-only (not random encounter) */
#define REGION_COUNT        12

/* ---- AI archetypes -------------------------------------- */
#define AI_RANDOM       0   /* attack randomly */
#define AI_AGGRESSIVE   1   /* always hit hardest target */
#define AI_HEALER       2   /* heals allies below 50% HP */
#define AI_PATTERN      3   /* fixed skill rotation */
#define AI_REACT        4   /* reacts to player element */
#define AI_BOSS_PHASE   5   /* multi-phase boss AI */

/* ---- Loot entry ----------------------------------------- */
typedef struct {
    u8  item_id;     /* ITEM_NONE = no drop */
    u8  quantity;
    u8  chance;      /* 0-100 percent */
} LootEntry;

/* ---- Full monster definition ---------------------------- */
typedef struct {
    /* Inherited combat stats (mirrors EnemyDef layout) */
    const char*  name;
    u8           sprite_id;
    u8           palette_id;
    ElementType  element;
    s16          hp,  mp;
    s16          atk, def;
    s16          mag, mdef;
    s16          spd;
    u8           resist[ELEM_COUNT]; /* ELEM_RESIST_* per element */
    u32          exp_reward;
    u16          gold_reward;
    u8           skill_ids[4];
    u8           skill_count;
    u8           ai_type;

    /* Extended fields */
    u8           region;
    u8           level_min;    /* min level to encounter */
    u8           level_max;    /* max level (for scaling) */
    LootEntry    loot[3];
    BOOL         is_boss;
    const char*  description;  /* bestiary flavour text */
} MonsterDef;

/* ---- Monster IDs (0-149) -------------------------------- */

/* Aelwyn Surroundings (0-12) */
#define MON_SLIME           0
#define MON_FOREST_RAT      1
#define MON_GOBLIN_SCOUT    2
#define MON_GOBLIN_WARRIOR  3
#define MON_WILD_BOAR       4
#define MON_BROWN_WOLF      5
#define MON_GIANT_BEE       6
#define MON_MUSHROOM_MAN    7
#define MON_STONE_CRAB      8
#define MON_BANDIT          9
#define MON_BANDIT_ARCHER   10
#define MON_BANDIT_MAGE     11
#define MON_PLAGUE_RAT      12

/* Verdant Forest (13-27) */
#define MON_TREANT          13
#define MON_VINE_SNAKE      14
#define MON_FOREST_SPIDER   15
#define MON_GIANT_SPIDER    16
#define MON_DRYAD           17
#define MON_ELF_ROGUE       18
#define MON_ELF_ARCHER      19
#define MON_PIXIE           20
#define MON_HORNET_SWARM    21
#define MON_THORN_WOLF      22
#define MON_CORRUPTED_BEAR  23
#define MON_MANDRAKE        24
#define MON_SPORE_BAT       25
#define MON_ELDER_TREANT    26
#define MON_FOREST_WITCH    27

/* Crystal Caves (28-41) */
#define MON_CRYSTAL_BAT     28
#define MON_CAVE_TROLL      29
#define MON_ROCK_GOLEM      30
#define MON_CAVE_SLIME      31
#define MON_BLIND_WORM      32
#define MON_CRYSTAL_SERPENT 33
#define MON_EARTH_WISP      34
#define MON_STALACTITE_CRAB 35
#define MON_GEM_BEETLE      36
#define MON_KOBOLD          37
#define MON_KOBOLD_SHAMAN   38
#define MON_STONE_GUARDIAN  39
#define MON_CRYSTAL_GOLEM   40
#define MON_DEEP_LURKER     41

/* Murkhaven Swamp (42-54) */
#define MON_BOG_SLIME       42
#define MON_SWAMP_CROC      43
#define MON_PLAGUE_FROG     44
#define MON_WILL_O_WISP     45
#define MON_SWAMP_WRAITH    46
#define MON_GIANT_LEECH     47
#define MON_TOXIC_SNAKE     48
#define MON_MUD_GOLEM       49
#define MON_SPORE_CLOUD     50
#define MON_SWAMP_HAG       51
#define MON_DEATH_FLOWER    52
#define MON_BASILISK        53
#define MON_SWAMP_TROLL     54

/* Ironpeak Mountains (55-69) */
#define MON_MOUNTAIN_WOLF   55
#define MON_HARPY           56
#define MON_WYVERN          57
#define MON_GRANITE_TROLL   58
#define MON_WIND_ELEMENTAL  59
#define MON_ICE_ELEMENTAL   60
#define MON_YETI            61
#define MON_STORM_EAGLE     62
#define MON_CLIFF_SPIDER    63
#define MON_ROCK_TITAN      64
#define MON_THUNDER_LIZARD  65
#define MON_FROST_WOLF      66
#define MON_MOUNTAIN_ORC    67
#define MON_ORC_CHAMPION    68
#define MON_PEAK_WYVERN     69

/* Ancient Ruins (70-83) */
#define MON_SKELETON        70
#define MON_ZOMBIE          71
#define MON_GHOST           72
#define MON_WRAITH          73
#define MON_MUMMY           74
#define MON_ANCIENT_GOLEM   75
#define MON_RUNE_GUARDIAN   76
#define MON_CURSE_WISP      77
#define MON_SHADOW_KNIGHT   78
#define MON_LICH_APPRENTICE 79
#define MON_BONE_DRAGON     80
#define MON_VOID_SHADE      81
#define MON_RUIN_COLOSSUS   82
#define MON_DARK_PRIEST     83

/* Ignarath Volcano (84-96) */
#define MON_FIRE_LIZARD     84
#define MON_LAVA_SLIME      85
#define MON_ASH_GOLEM       86
#define MON_FIRE_IMP        87
#define MON_MAGMA_CRAB      88
#define MON_FLAME_BAT       89
#define MON_FIRE_ELEMENTAL  90
#define MON_VOLCANO_TROLL   91
#define MON_EMBER_SNAKE     92
#define MON_HEAT_WRAITH     93
#define MON_MOLTEN_GOLEM    94
#define MON_PYROMANCER      95
#define MON_INFERNO_WYRM    96

/* Frostpeak Snowlands (97-109) */
#define MON_SNOW_WOLF       97
#define MON_FROST_BAT       98
#define MON_ICE_TROLL       99
#define MON_SNOW_GIANT      100
#define MON_BLIZZARD_WISP   101
#define MON_FROST_GOLEM     102
#define MON_WENDIGO         103
#define MON_ICE_SERPENT     104
#define MON_POLAR_BEAR      105
#define MON_CRYSTAL_IMP     106
#define MON_GLACIER_CRAB    107
#define MON_FROST_DRAGON    108
#define MON_SNOW_WITCH      109

/* Sunken Isles / Ocean (110-121) */
#define MON_SEA_SLIME       110
#define MON_SHARK           111
#define MON_JELLYFISH       112
#define MON_WATER_ELEMENTAL 113
#define MON_SEA_SERPENT     114
#define MON_MERROW          115
#define MON_DEEP_ANGLER     116
#define MON_TIDE_CRAB       117
#define MON_CORAL_GOLEM     118
#define MON_SIREN           119
#define MON_LEVIATHAN_PAWN  120
#define MON_OCEAN_DRAGON    121

/* Shadow Realm (122-134) */
#define MON_SHADOW_SLIME    122
#define MON_NIGHTMARE       123
#define MON_VOID_SPRITE     124
#define MON_DARK_ELF        125
#define MON_SHADE           126
#define MON_SHADOW_BEAST    127
#define MON_DARK_KNIGHT_SR  128
#define MON_SOUL_EATER      129
#define MON_VOID_KNIGHT     130
#define MON_PHANTOM_MAGE    131
#define MON_ABYSSAL_WRAITH  132
#define MON_DARK_COLOSSUS   133
#define MON_VOID_WALKER_MON 134

/* Aetheria Final Area (135-144) */
#define MON_AETHER_WISP     135
#define MON_CRYSTAL_ANGEL   136
#define MON_DIVINE_GOLEM    137
#define MON_AETHER_KNIGHT   138
#define MON_CORRUPTED_ANGEL 139
#define MON_VOID_TITAN      140
#define MON_AETHER_DRAGON   141
#define MON_FINAL_SHADE     142
#define MON_HERALD_OF_VOID  143
#define MON_AETHER_COLOSSUS 144

/* Bosses (145-149, is_boss = TRUE) */
#define MON_BOSS_SERPENTIS   145  /* Boss: Great Serpent */
#define MON_BOSS_ELDER_TREANT 146 /* Boss: Elder Treant */
#define MON_BOSS_LICH        147  /* Boss: Lich King */
#define MON_BOSS_INFERNO     148  /* Boss: Inferno Dragon */
#define MON_BOSS_VOID_LORD   149  /* Boss: Lord of the Void */

#define MONSTER_COUNT        150

/* ---- ROM table ------------------------------------------ */
extern const MonsterDef g_monster_db[MONSTER_COUNT];

/* ---- Monster group (encounter table) -------------------- */
typedef struct {
    u8  monster_ids[6];   /* up to 6 monsters per encounter */
    u8  count;
    u8  region;
    u8  background_id;
    u8  music_id;
    u8  min_steps;        /* min player steps between encounters */
} MonsterGroup;

#define MONSTER_GROUP_COUNT  60
extern const MonsterGroup g_monster_groups[MONSTER_GROUP_COUNT];

/* ---- API ------------------------------------------------ */
void                  monster_db_init(void);
const MonsterDef*     monster_get(u8 id);
const MonsterGroup*   monster_group_get(u8 id);

/* Pick a random encounter for current region (returns group id) */
u8 monster_random_encounter(u8 region, u8 player_level);

/* Populate EnemyDef-compatible struct for battle system */
void monster_to_enemy_def(const MonsterDef* mon, EnemyDef* out);

/* Roll loot drops after kill; adds items to g_bag */
void monster_roll_loot(const MonsterDef* mon);

#endif /* MONSTER_DB_H */
