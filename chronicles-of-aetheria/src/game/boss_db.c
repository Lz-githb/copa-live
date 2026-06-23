/*
 * boss_db.c — Chronicles of Aetheria
 *
 * ROM-resident definitions for all 30 bosses.
 * No dynamic allocation; all data is const and placed in ROM by the linker.
 *
 * Element resist array layout (index == ElementType):
 *   [0]=NONE [1]=FIRE [2]=WATER [3]=EARTH [4]=AIR [5]=LIGHT [6]=DARK
 */

#include "../../include/game/boss_db.h"
#include "../../include/game/monster_db.h"
#include "../../include/game/item/item_db.h"
#include "../../include/game/battle/battle_types.h"

/* Shorthand resist macros --------------------------------- */
#define N  ELEM_NEUTRAL   /* 100 — normal damage            */
#define R  ELEM_RESIST    /*  50 — half damage              */
#define W  ELEM_WEAK      /* 150 — extra damage             */
#define A  ELEM_ABSORB    /* 255 — absorbs as HP            */
#define IM ELEM_NULL      /*   0 — immune                   */

/* =========================================================
 * Skill ID placeholders.
 * The real IDs come from skill_db.h; use symbolic names here
 * so the file remains readable without that header present.
 * Replace with concrete enum values once skill_db.h is final.
 * ========================================================= */
/* Generic boss skills */
#define SK_NONE          0
#define SK_SLASH         1
#define SK_HEAVY_BLOW    2
#define SK_ROAR          3
#define SK_VINE_LASH     4
#define SK_THORN_WALL    5
#define SK_SPORE_CLOUD   6
#define SK_NATURE_CALL   7
#define SK_ROCK_THROW    8
#define SK_CAVE_CRUSH    9
#define SK_TREMOR        10
#define SK_BOULDER_RAIN  11
#define SK_WATER_CANNON  12
#define SK_TIDAL_WAVE    13
#define SK_WHIRLPOOL     14
#define SK_DEEP_CURRENT  15
#define SK_SAND_BLAST    16
#define SK_BURROW        17
#define SK_SAND_QUAKE    18
#define SK_DUNE_SURGE    19
#define SK_THUNDER_BOLT  20
#define SK_WING_SLASH    21
#define SK_STORM_CALL    22
#define SK_THUNDER_STORM 23
#define SK_TOXIC_MIST    24
#define SK_CURSED_GRASP  25
#define SK_PLAGUE_AURA   26
#define SK_DEATH_CURSE   27
#define SK_IRON_FIST     28
#define SK_METAL_CRUSH   29
#define SK_OVERLOAD      30
#define SK_MELTDOWN      31
#define SK_SHADOW_BITE   32
#define SK_SHADOW_HOWL   33
#define SK_DARK_PACK     34
#define SK_VOID_FANG     35
#define SK_LAVA_SLAM     36
#define SK_ERUPTION      37
#define SK_MAGMA_WAVE    38
#define SK_LAVA_CORE     39
#define SK_ICE_BREATH    40
#define SK_FROST_CLAW    41
#define SK_BLIZZARD      42
#define SK_PERMAFROST    43
#define SK_MIASMA        44
#define SK_BLIGHT        45
#define SK_DEATH_FIELD   46
#define SK_UNDEAD_CALL   47
#define SK_GALE_SLASH    48
#define SK_CYCLONE       49
#define SK_THUNDER_DIVE  50
#define SK_SKY_RUIN      51
#define SK_SEISMIC       52
#define SK_EARTH_PRISON  53
#define SK_TITAN_STOMP   54
#define SK_ROCK_AVALANCHE 55
#define SK_DARK_BOLT     56
#define SK_MANA_DRAIN    57
#define SK_ARCANE_NOVA   58
#define SK_VOID_REND     59
#define SK_SOUL_REAP     60
#define SK_BONE_LANCE    61
#define SK_LICH_AURA     62
#define SK_CRYPT_WAVE    63
#define SK_VOID_STRIKE   64
#define SK_ABYSSAL_MAW   65
#define SK_DIMENSION_CUT 66
#define SK_VOID_COLLAPSE 67
#define SK_DRAGON_CLAW   68
#define SK_DRAGON_BREATH 69
#define SK_ANCIENT_ROAR  70
#define SK_ELDER_NOVA    71
#define SK_HELLFIRE      72
#define SK_DEMON_CRUSH   73
#define SK_INFERNAL_GATE 74
#define SK_APOCALYPSE    75
#define SK_VOID_PULSE    76
#define SK_COSMIC_TEAR   77
#define SK_OBLIVION      78
#define SK_TRUE_VOID     79
/* Secret boss skills */
#define SK_HOLY_SLASH    80
#define SK_DIVINE_EDGE   81
#define SK_SWORD_TEMPEST 82
#define SK_FINAL_VERDICT 83
#define SK_ACID_BLOB     84
#define SK_SPLIT         85
#define SK_PRISM_BURST   86
#define SK_RAINBOW_FLOOD 87
#define SK_PRICE_TAG     88
#define SK_GOLD_CURSE    89
#define SK_DEATHBLOW     90
#define SK_MARKET_CRASH  91
#define SK_ANCIENT_SMASH 92
#define SK_RUNE_BARRIER  93
#define SK_GOLEM_LASER   94
#define SK_CREATION_BEAM 95
#define SK_FALLEN_LANCE  96
#define SK_ANGEL_WRATH   97
#define SK_DARK_HALO     98
#define SK_DIVINE_FALL   99
#define SK_PHANTOM_SLASH 100
#define SK_SPECTRAL_WAVE 101
#define SK_ROYAL_CURSE   102
#define SK_GHOST_ARMY    103
#define SK_HERALD_CRY    104
#define SK_RIFT_BEAM     105
#define SK_VOID_STORM    106
#define SK_SINGULARITY   107
#define SK_PINCER        108
#define SK_TIDAL_CRUSH   109
#define SK_SHELL_CANNON  110
#define SK_ABYSSAL_TIDE  111
#define SK_MIRROR_COPY   112
#define SK_REFLECTED_DOOM 113
#define SK_INVERSE_FIELD 114
#define SK_SHATTER_MIRROR 115
#define SK_TRUE_OBLIVION 116
#define SK_REALITY_ERASE 117
#define SK_VOID_ABSOLUTE 118
#define SK_ANNIHILATION  119

/* =========================================================
 * Recipe placeholder IDs (until recipe_db.h is written)
 * ========================================================= */
#define RCP_NONE         0xFF
#define RCP_VERDANT_TEA    1
#define RCP_TROLL_BREW     2
#define RCP_SEA_POTION     3
#define RCP_DESERT_BALM    4
#define RCP_THUNDER_SHARD  5
#define RCP_HAG_CAULDRON   6
#define RCP_IRON_TONIC     7
#define RCP_SHADOW_INK     8
#define RCP_LAVA_FLASK     9
#define RCP_FROST_ELIXIR  10
#define RCP_PLAGUE_VIAL   11
#define RCP_STORM_ESSENCE 12
#define RCP_STONE_DUST    13
#define RCP_DARK_GRIMOIRE 14
#define RCP_BONE_STAFF    15
#define RCP_VOID_CRYSTAL  16
#define RCP_ELDER_SCALE   17
#define RCP_LICH_POWDER   18
#define RCP_DEMON_FORGE   19
#define RCP_VOID_CROWN    20
#define RCP_KNIGHT_OIL    21
#define RCP_SLIME_JAM     22
#define RCP_DEATH_COIN    23
#define RCP_ANCIENT_CLAY  24
#define RCP_FEATHER_BLADE 25
#define RCP_GHOST_THREAD  26
#define RCP_HERALD_RUNE   27
#define RCP_CRAB_SOUP     28
#define RCP_MIRROR_SHARD  29
#define RCP_TRUE_VOID_INK 30

/* =========================================================
 * Sprite / Palette IDs  (assigned by the art team)
 * ========================================================= */
#define SPR_FOREST_GUARDIAN    0
#define SPR_CAVE_TROLL         1
#define SPR_SEA_SERPENT        2
#define SPR_SANDWORM           3
#define SPR_THUNDER_ROOK       4
#define SPR_SWAMP_HAG          5
#define SPR_IRON_GOLEM         6
#define SPR_SHADOW_WOLF        7
#define SPR_LAVA_TITAN         8
#define SPR_FROST_WYRM         9
#define SPR_PLAGUE_WRAITH     10
#define SPR_STORM_EAGLE       11
#define SPR_STONE_COLOSSUS    12
#define SPR_DARK_SORCERER     13
#define SPR_UNDEAD_KING       14
#define SPR_VOID_SERPENT      15
#define SPR_ELDER_DRAGON      16
#define SPR_LICH_KING         17
#define SPR_DEMON_LORD        18
#define SPR_VOID_GOD          19
#define SPR_SECRET_KNIGHT     20
#define SPR_PRISMATIC_SLIME   21
#define SPR_DEATH_MERCHANT    22
#define SPR_ANCIENT_GOLEM     23
#define SPR_FALLEN_ANGEL      24
#define SPR_PHANTOM_KING      25
#define SPR_VOID_HERALD       26
#define SPR_TITAN_CRAB        27
#define SPR_MIRROR_DEMON      28
#define SPR_TRUE_VOID_GOD     29

#define PAL_FOREST_GUARDIAN    0
#define PAL_CAVE_TROLL         1
#define PAL_SEA_SERPENT        2
#define PAL_SANDWORM           3
#define PAL_THUNDER_ROOK       4
#define PAL_SWAMP_HAG          5
#define PAL_IRON_GOLEM         6
#define PAL_SHADOW_WOLF        7
#define PAL_LAVA_TITAN         8
#define PAL_FROST_WYRM         9
#define PAL_PLAGUE_WRAITH     10
#define PAL_STORM_EAGLE       11
#define PAL_STONE_COLOSSUS    12
#define PAL_DARK_SORCERER     13
#define PAL_UNDEAD_KING       14
#define PAL_VOID_SERPENT      15
#define PAL_ELDER_DRAGON      16
#define PAL_LICH_KING         17
#define PAL_DEMON_LORD        18
#define PAL_VOID_GOD          19
#define PAL_SECRET_KNIGHT     20
#define PAL_PRISMATIC_SLIME   21
#define PAL_DEATH_MERCHANT    22
#define PAL_ANCIENT_GOLEM     23
#define PAL_FALLEN_ANGEL      24
#define PAL_PHANTOM_KING      25
#define PAL_VOID_HERALD       26
#define PAL_TITAN_CRAB        27
#define PAL_MIRROR_DEMON      28
#define PAL_TRUE_VOID_GOD     29

/* =========================================================
 * g_boss_db — the complete boss table
 *
 * Resist array:  { NONE, FIRE, WATER, EARTH, AIR, LIGHT, DARK }
 * Thresholds:    percentage HP at which the NEXT phase triggers.
 *                e.g. {50, 25, 0} = phase2 at 50 %, phase3 at 25 %.
 *                Unused slots must be 0.
 * ========================================================= */
const BossDef g_boss_db[BOSS_COUNT] = {

    /* ======================================================
     * [0] BOSS_FOREST_GUARDIAN
     * Early-game earth/nature boss. Two phases.
     * Area: Verdant Forest.
     * ===================================================== */
    {
        "Forest Guardian",
        "Warden of the Ancient Forest",
        2,
        {
            /* Phase 1 — Rooted form */
            {
                "Forest Guardian",
                /*hp   mp */ 900,  80,
                /*atk  def*/ 28,   22,
                /*mag  mdf*/ 15,   18,
                /*spd     */ 8,
                ELEM_EARTH,
                /* N    F    W    E    A    L    D  */
                { N,   W,   N,   A,   N,   N,   N  },
                { SK_VINE_LASH, SK_THORN_WALL, SK_ROAR, SK_SPORE_CLOUD },
                4,
                SPR_FOREST_GUARDIAN, PAL_FOREST_GUARDIAN,
                AI_BOSS_PHASE,
                "Ancient power awakens!"
            },
            /* Phase 2 — Awakened form */
            {
                "Guardian Awakened",
                /*hp   mp */ 1100, 120,
                /*atk  def*/ 38,   28,
                /*mag  mdf*/ 22,   24,
                /*spd     */ 12,
                ELEM_EARTH,
                { N,   W,   N,   A,   N,   N,   R  },
                { SK_VINE_LASH, SK_THORN_WALL, SK_NATURE_CALL, SK_ROCK_THROW },
                4,
                SPR_FOREST_GUARDIAN, PAL_FOREST_GUARDIAN,
                AI_BOSS_PHASE,
                "The forest fights back!"
            },
            /* Phase 3 — unused */
            { "", 0, 0, 0,0,0,0,0, 0, {N,N,N,N,N,N,N}, {0,0,0,0}, 0, 0,0, 0, "" }
        },
        { 50, 0, 0 },   /* phase2 triggers at 50 % HP */
        320,            /* exp  */
        150,            /* gold */
        RCP_VERDANT_TEA,
        ITEM_ID_NONE,
        REGION_VERDANT,
        FALSE,
        "An ancient spirit bound to the oldest tree in Verdant Forest. "
        "It attacks anything that disturbs the roots."
    },

    /* ======================================================
     * [1] BOSS_CAVE_TROLL
     * Earth brute. Two phases.
     * ===================================================== */
    {
        "Cave Troll",
        "Lord of the Crystal Caves",
        2,
        {
            {
                "Cave Troll",
                1000, 40,
                32,  30,
                8,   20,
                6,
                ELEM_EARTH,
                { N,  W,   N,   A,   N,   N,   N  },
                { SK_ROCK_THROW, SK_HEAVY_BLOW, SK_TREMOR, SK_ROAR },
                4,
                SPR_CAVE_TROLL, PAL_CAVE_TROLL,
                AI_BOSS_PHASE,
                "The cave shakes!"
            },
            {
                "Enraged Troll",
                1200, 40,
                44,  34,
                10,  22,
                9,
                ELEM_EARTH,
                { N,  W,   N,   A,   N,   N,   N  },
                { SK_ROCK_THROW, SK_BOULDER_RAIN, SK_TREMOR, SK_CAVE_CRUSH },
                4,
                SPR_CAVE_TROLL, PAL_CAVE_TROLL,
                AI_BOSS_PHASE,
                "Troll goes berserk!"
            },
            { "", 0, 0, 0,0,0,0,0, 0, {N,N,N,N,N,N,N}, {0,0,0,0}, 0, 0,0, 0, "" }
        },
        { 40, 0, 0 },
        380,
        180,
        RCP_TROLL_BREW,
        ITEM_ID_NONE,
        REGION_CAVES,
        FALSE,
        "A massive troll that has claimed the Crystal Caves as its den. "
        "Its hide has grown hard as rock over centuries underground."
    },

    /* ======================================================
     * [2] BOSS_SEA_SERPENT
     * Water boss. Three phases.
     * ===================================================== */
    {
        "Sea Serpent",
        "Sovereign of the Sunken Isles",
        3,
        {
            {
                "Sea Serpent",
                1100, 100,
                30,  24,
                28,  26,
                14,
                ELEM_WATER,
                { N,  N,   A,   N,   N,   N,   N  },
                { SK_WATER_CANNON, SK_HEAVY_BLOW, SK_WHIRLPOOL, SK_ROAR },
                4,
                SPR_SEA_SERPENT, PAL_SEA_SERPENT,
                AI_BOSS_PHASE,
                "The deep awakens!"
            },
            {
                "Sea Serpent Enraged",
                1300, 120,
                40,  28,
                36,  30,
                18,
                ELEM_WATER,
                { N,  N,   A,   N,   R,   N,   N  },
                { SK_WATER_CANNON, SK_TIDAL_WAVE, SK_WHIRLPOOL, SK_DEEP_CURRENT },
                4,
                SPR_SEA_SERPENT, PAL_SEA_SERPENT,
                AI_BOSS_PHASE,
                "Waves crash without end!"
            },
            {
                "Ancient Sea Serpent",
                1500, 150,
                50,  32,
                44,  36,
                22,
                ELEM_WATER,
                { N,  N,   A,   W,   N,   N,   N  },
                { SK_TIDAL_WAVE, SK_WHIRLPOOL, SK_DEEP_CURRENT, SK_WATER_CANNON },
                4,
                SPR_SEA_SERPENT, PAL_SEA_SERPENT,
                AI_BOSS_PHASE,
                "The ocean itself attacks!"
            }
        },
        { 60, 30, 0 },
        500,
        220,
        RCP_SEA_POTION,
        ITEM_ID_NONE,
        REGION_OCEAN,
        FALSE,
        "A colossal serpent that rules the waters around the Sunken Isles. "
        "Sailors pray they never meet its gaze beneath the waves."
    },

    /* ======================================================
     * [3] BOSS_SANDWORM
     * Earth/burrow boss. Two phases.
     * ===================================================== */
    {
        "Sandworm",
        "Devourer of the Dunes",
        2,
        {
            {
                "Sandworm",
                1200, 60,
                36,  26,
                12,  20,
                10,
                ELEM_EARTH,
                { N,  W,   N,   A,   N,   N,   N  },
                { SK_SAND_BLAST, SK_BURROW, SK_HEAVY_BLOW, SK_ROAR },
                4,
                SPR_SANDWORM, PAL_SANDWORM,
                AI_BOSS_PHASE,
                "It vanishes underground!"
            },
            {
                "Sandworm Titan",
                1500, 80,
                50,  30,
                18,  24,
                14,
                ELEM_EARTH,
                { N,  W,   N,   A,   N,   N,   N  },
                { SK_SAND_BLAST, SK_SAND_QUAKE, SK_DUNE_SURGE, SK_BOULDER_RAIN },
                4,
                SPR_SANDWORM, PAL_SANDWORM,
                AI_BOSS_PHASE,
                "The desert swallows you!"
            },
            { "", 0, 0, 0,0,0,0,0, 0, {N,N,N,N,N,N,N}, {0,0,0,0}, 0, 0,0, 0, "" }
        },
        { 50, 0, 0 },
        540,
        200,
        RCP_DESERT_BALM,
        ITEM_ID_NONE,
        REGION_AELWYN,  /* desert region closest to starter zone */
        FALSE,
        "A leviathan that tunnels beneath the desert sands, swallowing "
        "caravans whole. Its arrival is heralded only by trembling ground."
    },

    /* ======================================================
     * [4] BOSS_THUNDER_ROOK
     * Air/lightning. Two phases.
     * ===================================================== */
    {
        "Thunder Rook",
        "Stormcaller of Ironpeak",
        2,
        {
            {
                "Thunder Rook",
                1100, 110,
                34,  20,
                40,  28,
                20,
                ELEM_AIR,
                { N,  N,   N,   N,   A,   N,   N  },
                { SK_THUNDER_BOLT, SK_WING_SLASH, SK_STORM_CALL, SK_ROAR },
                4,
                SPR_THUNDER_ROOK, PAL_THUNDER_ROOK,
                AI_BOSS_PHASE,
                "Lightning fills the sky!"
            },
            {
                "Thunder Rook Prime",
                1350, 140,
                46,  24,
                56,  34,
                26,
                ELEM_AIR,
                { N,  N,   N,   N,   A,   N,   N  },
                { SK_THUNDER_BOLT, SK_WING_SLASH, SK_THUNDER_STORM, SK_STORM_CALL },
                4,
                SPR_THUNDER_ROOK, PAL_THUNDER_ROOK,
                AI_BOSS_PHASE,
                "A bolt from the heavens!"
            },
            { "", 0, 0, 0,0,0,0,0, 0, {N,N,N,N,N,N,N}, {0,0,0,0}, 0, 0,0, 0, "" }
        },
        { 45, 0, 0 },
        600,
        240,
        RCP_THUNDER_SHARD,
        ITEM_ID_NONE,
        REGION_MOUNTAINS,
        FALSE,
        "A great rook whose feathers conduct lightning. It nests atop "
        "Ironpeak, and storms follow wherever it flies."
    },

    /* ======================================================
     * [5] BOSS_SWAMP_HAG
     * Dark/poison. Two phases.
     * ===================================================== */
    {
        "Swamp Hag",
        "Crone of Murkhaven",
        2,
        {
            {
                "Swamp Hag",
                1050, 140,
                26,  22,
                44,  30,
                16,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   W,   A  },
                { SK_TOXIC_MIST, SK_CURSED_GRASP, SK_DARK_BOLT, SK_ROAR },
                4,
                SPR_SWAMP_HAG, PAL_SWAMP_HAG,
                AI_BOSS_PHASE,
                "She cackles with dark glee!"
            },
            {
                "Hag Ascendant",
                1300, 180,
                34,  26,
                58,  36,
                18,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   W,   A  },
                { SK_TOXIC_MIST, SK_PLAGUE_AURA, SK_CURSED_GRASP, SK_DARK_BOLT },
                4,
                SPR_SWAMP_HAG, PAL_SWAMP_HAG,
                AI_BOSS_PHASE,
                "The swamp rises against you!"
            },
            { "", 0, 0, 0,0,0,0,0, 0, {N,N,N,N,N,N,N}, {0,0,0,0}, 0, 0,0, 0, "" }
        },
        { 40, 0, 0 },
        650,
        260,
        RCP_HAG_CAULDRON,
        ITEM_ID_NONE,
        REGION_SWAMP,
        FALSE,
        "An ancient witch who has merged with the swamp itself. Those who "
        "enter Murkhaven rarely find their way back out."
    },

    /* ======================================================
     * [6] BOSS_IRON_GOLEM
     * Earth/metal. Two phases.
     * ===================================================== */
    {
        "Iron Golem",
        "Relic Guardian of the Ruins",
        2,
        {
            {
                "Iron Golem",
                1800, 40,
                52,  48,
                10,  40,
                6,
                ELEM_EARTH,
                { N,  R,   R,   A,   N,   N,   N  },
                { SK_IRON_FIST, SK_HEAVY_BLOW, SK_METAL_CRUSH, SK_ROAR },
                4,
                SPR_IRON_GOLEM, PAL_IRON_GOLEM,
                AI_BOSS_PHASE,
                "Gears grind; steam vents!"
            },
            {
                "Overloaded Golem",
                2000, 60,
                66,  44,
                14,  38,
                10,
                ELEM_FIRE,
                { R,  A,   N,   R,   N,   N,   N  },
                { SK_IRON_FIST, SK_OVERLOAD, SK_MELTDOWN, SK_METAL_CRUSH },
                4,
                SPR_IRON_GOLEM, PAL_IRON_GOLEM,
                AI_BOSS_PHASE,
                "Systems critical — overload!"
            },
            { "", 0, 0, 0,0,0,0,0, 0, {N,N,N,N,N,N,N}, {0,0,0,0}, 0, 0,0, 0, "" }
        },
        { 50, 0, 0 },
        800,
        350,
        RCP_IRON_TONIC,
        ITEM_ID_NONE,
        REGION_RUINS,
        FALSE,
        "A construct of arcane iron built to guard the Ancient Ruins. "
        "When damaged enough its core overheats and it becomes erratic."
    },

    /* ======================================================
     * [7] BOSS_SHADOW_WOLF
     * Dark. Three phases.
     * ===================================================== */
    {
        "Shadow Wolf",
        "Alpha of the Void Pack",
        3,
        {
            {
                "Shadow Wolf",
                1600, 100,
                54,  30,
                30,  32,
                28,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   W,   A  },
                { SK_SHADOW_BITE, SK_SHADOW_HOWL, SK_SLASH, SK_ROAR },
                4,
                SPR_SHADOW_WOLF, PAL_SHADOW_WOLF,
                AI_BOSS_PHASE,
                "Shadows converge!"
            },
            {
                "Shadow Wolf Berserk",
                1800, 120,
                70,  34,
                38,  36,
                36,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   W,   A  },
                { SK_SHADOW_BITE, SK_SHADOW_HOWL, SK_DARK_PACK, SK_VOID_FANG },
                4,
                SPR_SHADOW_WOLF, PAL_SHADOW_WOLF,
                AI_BOSS_PHASE,
                "The pack descends!"
            },
            {
                "Void Alpha",
                2100, 160,
                86,  38,
                50,  42,
                44,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   W,   A  },
                { SK_VOID_FANG, SK_SHADOW_HOWL, SK_DARK_PACK, SK_SHADOW_BITE },
                4,
                SPR_SHADOW_WOLF, PAL_SHADOW_WOLF,
                AI_BOSS_PHASE,
                "Void itself hungers!"
            }
        },
        { 60, 30, 0 },
        900,
        380,
        RCP_SHADOW_INK,
        ITEM_ID_NONE,
        REGION_SHADOWREALM,
        FALSE,
        "The alpha of a pack of wolves that have been consumed by shadow. "
        "It can summon its pack from the darkness between worlds."
    },

    /* ======================================================
     * [8] BOSS_LAVA_TITAN
     * Fire. Three phases.
     * ===================================================== */
    {
        "Lava Titan",
        "Incandescent Lord of Ignarath",
        3,
        {
            {
                "Lava Titan",
                2000, 80,
                62,  44,
                50,  38,
                10,
                ELEM_FIRE,
                { N,  A,   W,   N,   N,   N,   N  },
                { SK_LAVA_SLAM, SK_HEAVY_BLOW, SK_ERUPTION, SK_ROAR },
                4,
                SPR_LAVA_TITAN, PAL_LAVA_TITAN,
                AI_BOSS_PHASE,
                "Magma flows from his fists!"
            },
            {
                "Lava Titan Molten",
                2400, 100,
                78,  48,
                64,  44,
                14,
                ELEM_FIRE,
                { N,  A,   W,   N,   N,   N,   N  },
                { SK_LAVA_SLAM, SK_ERUPTION, SK_MAGMA_WAVE, SK_HEAVY_BLOW },
                4,
                SPR_LAVA_TITAN, PAL_LAVA_TITAN,
                AI_BOSS_PHASE,
                "The volcano trembles!"
            },
            {
                "Lava Titan Core",
                2800, 120,
                92,  52,
                80,  50,
                18,
                ELEM_FIRE,
                { N,  A,   W,   N,   N,   N,   N  },
                { SK_LAVA_CORE, SK_MAGMA_WAVE, SK_ERUPTION, SK_LAVA_SLAM },
                4,
                SPR_LAVA_TITAN, PAL_LAVA_TITAN,
                AI_BOSS_PHASE,
                "His heart is a volcano!"
            }
        },
        { 60, 25, 0 },
        1100,
        480,
        RCP_LAVA_FLASK,
        ITEM_ID_NONE,
        REGION_VOLCANO,
        FALSE,
        "A colossus born from magma and will. It slumbered inside Ignarath "
        "Volcano for an aeon before the heroes disturbed its rest."
    },

    /* ======================================================
     * [9] BOSS_FROST_WYRM
     * Water/ice. Three phases.
     * ===================================================== */
    {
        "Frost Wyrm",
        "Glacier Serpent of Frostpeak",
        3,
        {
            {
                "Frost Wyrm",
                2100, 100,
                58,  42,
                60,  46,
                16,
                ELEM_WATER,
                { N,  W,   A,   N,   N,   N,   N  },
                { SK_ICE_BREATH, SK_FROST_CLAW, SK_HEAVY_BLOW, SK_ROAR },
                4,
                SPR_FROST_WYRM, PAL_FROST_WYRM,
                AI_BOSS_PHASE,
                "Frozen breath fills the air!"
            },
            {
                "Frost Wyrm Raging",
                2500, 130,
                74,  48,
                76,  54,
                20,
                ELEM_WATER,
                { N,  W,   A,   N,   N,   N,   N  },
                { SK_ICE_BREATH, SK_BLIZZARD, SK_FROST_CLAW, SK_HEAVY_BLOW },
                4,
                SPR_FROST_WYRM, PAL_FROST_WYRM,
                AI_BOSS_PHASE,
                "Snowstorm without end!"
            },
            {
                "Frost Wyrm Ancient",
                3000, 160,
                90,  54,
                94,  60,
                24,
                ELEM_WATER,
                { N,  W,   A,   N,   N,   N,   N  },
                { SK_PERMAFROST, SK_BLIZZARD, SK_ICE_BREATH, SK_FROST_CLAW },
                4,
                SPR_FROST_WYRM, PAL_FROST_WYRM,
                AI_BOSS_PHASE,
                "The world turns to ice!"
            }
        },
        { 60, 25, 0 },
        1250,
        520,
        RCP_FROST_ELIXIR,
        ITEM_ID_NONE,
        REGION_SNOWLANDS,
        FALSE,
        "An ice dragon whose body is older than the Snowlands themselves. "
        "Its bones are pure glacial crystal and its breath freezes time."
    },

    /* ======================================================
     * [10] BOSS_PLAGUE_WRAITH
     * Dark/poison. Two phases.
     * ===================================================== */
    {
        "Plague Wraith",
        "Herald of a Thousand Diseases",
        2,
        {
            {
                "Plague Wraith",
                2000, 180,
                50,  32,
                88,  54,
                22,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   W,   A  },
                { SK_MIASMA, SK_BLIGHT, SK_DARK_BOLT, SK_TOXIC_MIST },
                4,
                SPR_PLAGUE_WRAITH, PAL_PLAGUE_WRAITH,
                AI_BOSS_PHASE,
                "Plague clouds billow forth!"
            },
            {
                "Plague Wraith Ascended",
                2500, 220,
                64,  38,
                110, 62,
                28,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   W,   A  },
                { SK_DEATH_FIELD, SK_MIASMA, SK_BLIGHT, SK_UNDEAD_CALL },
                4,
                SPR_PLAGUE_WRAITH, PAL_PLAGUE_WRAITH,
                AI_BOSS_PHASE,
                "Death spreads from its touch!"
            },
            { "", 0, 0, 0,0,0,0,0, 0, {N,N,N,N,N,N,N}, {0,0,0,0}, 0, 0,0, 0, "" }
        },
        { 45, 0, 0 },
        1350,
        560,
        RCP_PLAGUE_VIAL,
        ITEM_ID_NONE,
        REGION_RUINS,
        FALSE,
        "The undying remnant of a court sorcerer who tried to transcend death "
        "using forbidden plague rites. It succeeded — horribly."
    },

    /* ======================================================
     * [11] BOSS_STORM_EAGLE
     * Air/lightning. Three phases.
     * ===================================================== */
    {
        "Storm Eagle",
        "Eye of the Endless Tempest",
        3,
        {
            {
                "Storm Eagle",
                2200, 140,
                68,  34,
                72,  48,
                34,
                ELEM_AIR,
                { N,  N,   N,   N,   A,   N,   N  },
                { SK_GALE_SLASH, SK_THUNDER_BOLT, SK_WING_SLASH, SK_STORM_CALL },
                4,
                SPR_STORM_EAGLE, PAL_STORM_EAGLE,
                AI_BOSS_PHASE,
                "Winds howl and darken!"
            },
            {
                "Storm Eagle Tempest",
                2700, 170,
                86,  40,
                90,  56,
                42,
                ELEM_AIR,
                { N,  N,   N,   N,   A,   N,   N  },
                { SK_CYCLONE, SK_THUNDER_BOLT, SK_THUNDER_DIVE, SK_STORM_CALL },
                4,
                SPR_STORM_EAGLE, PAL_STORM_EAGLE,
                AI_BOSS_PHASE,
                "The storm answers its call!"
            },
            {
                "Storm Eagle Apex",
                3100, 200,
                104, 48,
                112, 64,
                52,
                ELEM_AIR,
                { N,  N,   N,   N,   A,   N,   N  },
                { SK_SKY_RUIN, SK_CYCLONE, SK_THUNDER_STORM, SK_THUNDER_DIVE },
                4,
                SPR_STORM_EAGLE, PAL_STORM_EAGLE,
                AI_BOSS_PHASE,
                "The heavens themselves fall!"
            }
        },
        { 60, 30, 0 },
        1500,
        620,
        RCP_STORM_ESSENCE,
        ITEM_ID_NONE,
        REGION_MOUNTAINS,
        FALSE,
        "A legendary eagle born inside a permanent hurricane. Its wingbeats "
        "can level mountains and its cry summons typhoons."
    },

    /* ======================================================
     * [12] BOSS_STONE_COLOSSUS
     * Earth. Three phases.
     * ===================================================== */
    {
        "Stone Colossus",
        "Edifice of the Mountain God",
        3,
        {
            {
                "Stone Colossus",
                2800, 60,
                90,  80,
                20,  70,
                5,
                ELEM_EARTH,
                { N,  W,   N,   A,   N,   N,   N  },
                { SK_SEISMIC, SK_HEAVY_BLOW, SK_ROCK_THROW, SK_ROAR },
                4,
                SPR_STONE_COLOSSUS, PAL_STONE_COLOSSUS,
                AI_BOSS_PHASE,
                "The earth cracks beneath it!"
            },
            {
                "Colossus Cracked",
                3000, 80,
                106, 70,
                26,  64,
                8,
                ELEM_EARTH,
                { N,  W,   N,   A,   N,   N,   N  },
                { SK_SEISMIC, SK_EARTH_PRISON, SK_TITAN_STOMP, SK_ROCK_THROW },
                4,
                SPR_STONE_COLOSSUS, PAL_STONE_COLOSSUS,
                AI_BOSS_PHASE,
                "Cracks reveal molten core!"
            },
            {
                "Colossus Unleashed",
                3400, 100,
                126, 60,
                34,  58,
                12,
                ELEM_FIRE,
                { N,  A,   W,   R,   N,   N,   N  },
                { SK_ROCK_AVALANCHE, SK_TITAN_STOMP, SK_SEISMIC, SK_EARTH_PRISON },
                4,
                SPR_STONE_COLOSSUS, PAL_STONE_COLOSSUS,
                AI_BOSS_PHASE,
                "Magma flows from within!"
            }
        },
        { 60, 30, 0 },
        1800,
        700,
        RCP_STONE_DUST,
        ITEM_ID_NONE,
        REGION_MOUNTAINS,
        FALSE,
        "A walking mountain raised by a forgotten pantheon. Its very step "
        "reshapes the landscape, and its eyes glow with ancient flame."
    },

    /* ======================================================
     * [13] BOSS_DARK_SORCERER
     * Dark/magic. Three phases.
     * ===================================================== */
    {
        "Dark Sorcerer",
        "Keeper of Forbidden Knowledge",
        3,
        {
            {
                "Dark Sorcerer",
                3000, 300,
                44,  38,
                130, 80,
                30,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   W,   A  },
                { SK_DARK_BOLT, SK_MANA_DRAIN, SK_ARCANE_NOVA, SK_VOID_REND },
                4,
                SPR_DARK_SORCERER, PAL_DARK_SORCERER,
                AI_BOSS_PHASE,
                "His grimoire opens itself!"
            },
            {
                "Dark Sorcerer Empowered",
                3500, 360,
                54,  44,
                160, 90,
                36,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   W,   A  },
                { SK_ARCANE_NOVA, SK_VOID_REND, SK_MANA_DRAIN, SK_DARK_BOLT },
                4,
                SPR_DARK_SORCERER, PAL_DARK_SORCERER,
                AI_BOSS_PHASE,
                "Dark knowledge consumes him!"
            },
            {
                "Dark Sorcerer Transcended",
                4000, 420,
                66,  50,
                196, 100,
                42,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   W,   A  },
                { SK_VOID_REND, SK_ARCANE_NOVA, SK_MANA_DRAIN, SK_VOID_COLLAPSE },
                4,
                SPR_DARK_SORCERER, PAL_DARK_SORCERER,
                AI_BOSS_PHASE,
                "Reality bends to his will!"
            }
        },
        { 60, 30, 0 },
        2200,
        900,
        RCP_DARK_GRIMOIRE,
        ITEM_ID_NONE,
        REGION_RUINS,
        FALSE,
        "Once a court scholar, he delved too deep into forbidden magic. "
        "Now he is more spell than man, and his hunger is infinite."
    },

    /* ======================================================
     * [14] BOSS_UNDEAD_KING
     * Dark/undead. Three phases.
     * ===================================================== */
    {
        "Undead King",
        "Sovereign of the Restless Dead",
        3,
        {
            {
                "Undead King",
                3500, 200,
                100, 70,
                80,  90,
                22,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   W,   A  },
                { SK_SOUL_REAP, SK_BONE_LANCE, SK_HEAVY_BLOW, SK_UNDEAD_CALL },
                4,
                SPR_UNDEAD_KING, PAL_UNDEAD_KING,
                AI_BOSS_PHASE,
                "Graves open at his command!"
            },
            {
                "Undead King Wrathful",
                4000, 250,
                122, 78,
                100, 100,
                28,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   W,   A  },
                { SK_SOUL_REAP, SK_LICH_AURA, SK_BONE_LANCE, SK_UNDEAD_CALL },
                4,
                SPR_UNDEAD_KING, PAL_UNDEAD_KING,
                AI_BOSS_PHASE,
                "An army of bones rises!"
            },
            {
                "Undead King Eternal",
                4500, 300,
                148, 86,
                124, 112,
                34,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   W,   A  },
                { SK_CRYPT_WAVE, SK_LICH_AURA, SK_SOUL_REAP, SK_BONE_LANCE },
                4,
                SPR_UNDEAD_KING, PAL_UNDEAD_KING,
                AI_BOSS_PHASE,
                "Death itself obeys him!"
            }
        },
        { 60, 30, 0 },
        2800,
        1100,
        RCP_BONE_STAFF,
        ITEM_ID_NONE,
        REGION_SHADOWREALM,
        FALSE,
        "A king who refused to die and enslaved the spirit of death itself. "
        "He rules an endless domain of ash and bone beneath the Shadow Realm."
    },

    /* ======================================================
     * [15] BOSS_VOID_SERPENT
     * Dark/void. Three phases.
     * ===================================================== */
    {
        "Void Serpent",
        "Harbinger of the Endless Dark",
        3,
        {
            {
                "Void Serpent",
                4000, 200,
                110, 60,
                110, 80,
                30,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   N,   A  },
                { SK_VOID_STRIKE, SK_ABYSSAL_MAW, SK_DARK_BOLT, SK_HEAVY_BLOW },
                4,
                SPR_VOID_SERPENT, PAL_VOID_SERPENT,
                AI_BOSS_PHASE,
                "The void opens its eye!"
            },
            {
                "Void Serpent Unbound",
                4600, 260,
                136, 68,
                136, 92,
                38,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   N,   A  },
                { SK_VOID_STRIKE, SK_DIMENSION_CUT, SK_ABYSSAL_MAW, SK_VOID_REND },
                4,
                SPR_VOID_SERPENT, PAL_VOID_SERPENT,
                AI_BOSS_PHASE,
                "Space tears like paper!"
            },
            {
                "Void Serpent Eternal",
                5200, 320,
                168, 78,
                166, 104,
                48,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   N,   A  },
                { SK_VOID_COLLAPSE, SK_DIMENSION_CUT, SK_ABYSSAL_MAW, SK_VOID_STRIKE },
                4,
                SPR_VOID_SERPENT, PAL_VOID_SERPENT,
                AI_BOSS_PHASE,
                "Stars wink out around it!"
            }
        },
        { 60, 30, 0 },
        3500,
        1400,
        RCP_VOID_CRYSTAL,
        ITEM_ID_NONE,
        REGION_AETHERIA,
        FALSE,
        "A cosmic serpent born from the void between worlds. It predates "
        "Aetheria itself and regards all light as an offense to be devoured."
    },

    /* ======================================================
     * [16] BOSS_ELDER_DRAGON
     * Fire/dragon. Three phases.
     * ===================================================== */
    {
        "Elder Dragon",
        "The Last of the First Flame",
        3,
        {
            {
                "Elder Dragon",
                5000, 280,
                140, 90,
                120, 100,
                20,
                ELEM_FIRE,
                { N,  A,   W,   N,   N,   N,   N  },
                { SK_DRAGON_BREATH, SK_DRAGON_CLAW, SK_ERUPTION, SK_HEAVY_BLOW },
                4,
                SPR_ELDER_DRAGON, PAL_ELDER_DRAGON,
                AI_BOSS_PHASE,
                "Ancient fire scorches all!"
            },
            {
                "Elder Dragon Wrathful",
                6000, 340,
                174, 100,
                150, 112,
                26,
                ELEM_FIRE,
                { N,  A,   W,   N,   N,   N,   N  },
                { SK_DRAGON_BREATH, SK_ANCIENT_ROAR, SK_DRAGON_CLAW, SK_MAGMA_WAVE },
                4,
                SPR_ELDER_DRAGON, PAL_ELDER_DRAGON,
                AI_BOSS_PHASE,
                "Dragonfire consumes the world!"
            },
            {
                "Elder Dragon Ascended",
                7000, 400,
                210, 110,
                186, 126,
                32,
                ELEM_FIRE,
                { N,  A,   W,   N,   N,   R,   N  },
                { SK_ELDER_NOVA, SK_ANCIENT_ROAR, SK_DRAGON_BREATH, SK_DRAGON_CLAW },
                4,
                SPR_ELDER_DRAGON, PAL_ELDER_DRAGON,
                AI_BOSS_PHASE,
                "A nova of primal flame!"
            }
        },
        { 60, 30, 0 },
        5000,
        2000,
        RCP_ELDER_SCALE,
        ITEM_ID_NONE,
        REGION_VOLCANO,
        FALSE,
        "The oldest living creature in Aetheria. Its lineage traces to the "
        "first fire kindled at creation. It will not let that fire die."
    },

    /* ======================================================
     * [17] BOSS_LICH_KING
     * Dark/undead penultimate. Three phases.
     * ===================================================== */
    {
        "Lich King",
        "Master of the Undying Throne",
        3,
        {
            {
                "Lich King",
                6000, 400,
                130, 80,
                180, 130,
                26,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   W,   A  },
                { SK_SOUL_REAP, SK_LICH_AURA, SK_ARCANE_NOVA, SK_BONE_LANCE },
                4,
                SPR_LICH_KING, PAL_LICH_KING,
                AI_BOSS_PHASE,
                "His phylactery glows red!"
            },
            {
                "Lich King Unshackled",
                7000, 480,
                162, 90,
                220, 148,
                32,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   W,   A  },
                { SK_LICH_AURA, SK_CRYPT_WAVE, SK_SOUL_REAP, SK_ARCANE_NOVA },
                4,
                SPR_LICH_KING, PAL_LICH_KING,
                AI_BOSS_PHASE,
                "Death magic floods the room!"
            },
            {
                "Lich King Eternal",
                8500, 560,
                200, 100,
                270, 168,
                40,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   W,   A  },
                { SK_CRYPT_WAVE, SK_DEATH_FIELD, SK_LICH_AURA, SK_SOUL_REAP },
                4,
                SPR_LICH_KING, PAL_LICH_KING,
                AI_BOSS_PHASE,
                "Eternal night descends!"
            }
        },
        { 60, 25, 0 },
        7000,
        2800,
        RCP_LICH_POWDER,
        ITEM_ID_NONE,
        REGION_SHADOWREALM,
        FALSE,
        "An archmage who sacrificed his mortality for ultimate power. "
        "He orchestrates events from his Undying Throne across centuries."
    },

    /* ======================================================
     * [18] BOSS_DEMON_LORD
     * Dark/fire. Three phases.
     * ===================================================== */
    {
        "Demon Lord",
        "Conqueror of the Mortal Realm",
        3,
        {
            {
                "Demon Lord",
                7000, 360,
                190, 110,
                190, 140,
                24,
                ELEM_DARK,
                { N,  A,   N,   N,   N,   W,   A  },
                { SK_HELLFIRE, SK_DEMON_CRUSH, SK_SOUL_REAP, SK_DARK_BOLT },
                4,
                SPR_DEMON_LORD, PAL_DEMON_LORD,
                AI_BOSS_PHASE,
                "Hellfire erupts from below!"
            },
            {
                "Demon Lord Unleashed",
                8000, 430,
                234, 124,
                236, 160,
                30,
                ELEM_DARK,
                { N,  A,   N,   N,   N,   W,   A  },
                { SK_HELLFIRE, SK_INFERNAL_GATE, SK_DEMON_CRUSH, SK_SOUL_REAP },
                4,
                SPR_DEMON_LORD, PAL_DEMON_LORD,
                AI_BOSS_PHASE,
                "Demons pour through the gate!"
            },
            {
                "Demon Lord Transcendent",
                9000, 500,
                280, 140,
                286, 180,
                38,
                ELEM_DARK,
                { N,  A,   N,   N,   N,   W,   A  },
                { SK_APOCALYPSE, SK_INFERNAL_GATE, SK_HELLFIRE, SK_DEMON_CRUSH },
                4,
                SPR_DEMON_LORD, PAL_DEMON_LORD,
                AI_BOSS_PHASE,
                "The apocalypse begins now!"
            }
        },
        { 60, 25, 0 },
        9000,
        3600,
        RCP_DEMON_FORGE,
        ITEM_ID_NONE,
        REGION_AETHERIA,
        FALSE,
        "The ruler of the demon realm, who seeks to dissolve the boundary "
        "between worlds and consume Aetheria in infernal fire."
    },

    /* ======================================================
     * [19] BOSS_VOID_GOD
     * Final boss — void. Three phases.
     * ===================================================== */
    {
        "Void God",
        "The Unmaking — End of All Things",
        3,
        {
            {
                "Void God",
                8000, 500,
                200, 120,
                220, 160,
                30,
                ELEM_DARK,
                { R,  N,   N,   N,   N,   W,   A  },
                { SK_VOID_PULSE, SK_VOID_STRIKE, SK_COSMIC_TEAR, SK_VOID_REND },
                4,
                SPR_VOID_GOD, PAL_VOID_GOD,
                AI_BOSS_PHASE,
                "The void speaks your name!"
            },
            {
                "Void God Manifest",
                9000, 600,
                250, 140,
                270, 180,
                38,
                ELEM_DARK,
                { R,  N,   N,   N,   N,   W,   A  },
                { SK_COSMIC_TEAR, SK_VOID_COLLAPSE, SK_VOID_PULSE, SK_OBLIVION },
                4,
                SPR_VOID_GOD, PAL_VOID_GOD,
                AI_BOSS_PHASE,
                "Creation itself unravels!"
            },
            {
                "Void God Absolute",
                9999, 700,
                300, 160,
                330, 200,
                48,
                ELEM_DARK,
                { R,  R,   R,   R,   R,   W,   A  },
                { SK_OBLIVION, SK_COSMIC_TEAR, SK_VOID_COLLAPSE, SK_TRUE_VOID },
                4,
                SPR_VOID_GOD, PAL_VOID_GOD,
                AI_BOSS_PHASE,
                "Nothing remains but void!"
            }
        },
        { 60, 25, 0 },
        15000,
        9999,
        RCP_VOID_CROWN,
        ITEM_ID_NONE,
        REGION_AETHERIA,
        FALSE,
        "The embodiment of un-existence. Before the first world was created "
        "there was only void, and the Void God hungers to restore that state."
    },

    /* ======================================================
     * [20] BOSS_SECRET_KNIGHT  (optional)
     * Light/holy superboss. Three phases.
     * ===================================================== */
    {
        "Secret Knight",
        "The Nameless Paladin",
        3,
        {
            {
                "Secret Knight",
                5500, 200,
                200, 160,
                80,  140,
                36,
                ELEM_LIGHT,
                { N,  N,   N,   N,   N,   A,   W  },
                { SK_HOLY_SLASH, SK_DIVINE_EDGE, SK_HEAVY_BLOW, SK_ROAR },
                4,
                SPR_SECRET_KNIGHT, PAL_SECRET_KNIGHT,
                AI_BOSS_PHASE,
                "A holy aura blinds you!"
            },
            {
                "Knight Radiant",
                7000, 260,
                250, 180,
                100, 160,
                46,
                ELEM_LIGHT,
                { N,  N,   N,   N,   N,   A,   W  },
                { SK_SWORD_TEMPEST, SK_HOLY_SLASH, SK_DIVINE_EDGE, SK_HEAVY_BLOW },
                4,
                SPR_SECRET_KNIGHT, PAL_SECRET_KNIGHT,
                AI_BOSS_PHASE,
                "Light shreds the darkness!"
            },
            {
                "Knight of Final Verdict",
                9000, 320,
                310, 200,
                130, 180,
                58,
                ELEM_LIGHT,
                { N,  N,   N,   N,   N,   A,   W  },
                { SK_FINAL_VERDICT, SK_SWORD_TEMPEST, SK_HOLY_SLASH, SK_DIVINE_EDGE },
                4,
                SPR_SECRET_KNIGHT, PAL_SECRET_KNIGHT,
                AI_BOSS_PHASE,
                "Judgment falls upon you!"
            }
        },
        { 60, 25, 0 },
        20000,
        5000,
        RCP_KNIGHT_OIL,
        ITEM_ID_NONE,
        REGION_BOSS,
        TRUE,
        "A wandering paladin who has challenged every hero who ever lived. "
        "He has never lost. His name has been erased from all records."
    },

    /* ======================================================
     * [21] BOSS_PRISMATIC_SLIME  (optional)
     * Absorbs all elements — gimmick boss. Three phases.
     * ===================================================== */
    {
        "Prismatic Slime",
        "The Rainbow Terror",
        3,
        {
            {
                "Prismatic Slime",
                6000, 100,
                100, 80,
                100, 80,
                20,
                ELEM_NONE,
                { N,  A,   A,   A,   A,   A,   A  },
                { SK_ACID_BLOB, SK_SPLIT, SK_HEAVY_BLOW, SK_ROAR },
                4,
                SPR_PRISMATIC_SLIME, PAL_PRISMATIC_SLIME,
                AI_BOSS_PHASE,
                "The slime splits apart!"
            },
            {
                "Prismatic Slime Alpha",
                7500, 130,
                130, 100,
                130, 100,
                28,
                ELEM_NONE,
                { N,  A,   A,   A,   A,   A,   A  },
                { SK_PRISM_BURST, SK_ACID_BLOB, SK_SPLIT, SK_HEAVY_BLOW },
                4,
                SPR_PRISMATIC_SLIME, PAL_PRISMATIC_SLIME,
                AI_BOSS_PHASE,
                "It absorbs your magic!"
            },
            {
                "Prismatic Slime Core",
                9000, 160,
                170, 120,
                170, 120,
                36,
                ELEM_NONE,
                { N,  A,   A,   A,   A,   A,   A  },
                { SK_RAINBOW_FLOOD, SK_PRISM_BURST, SK_ACID_BLOB, SK_SPLIT },
                4,
                SPR_PRISMATIC_SLIME, PAL_PRISMATIC_SLIME,
                AI_BOSS_PHASE,
                "Pure rainbow carnage!"
            }
        },
        { 60, 30, 0 },
        18000,
        4500,
        RCP_SLIME_JAM,
        ITEM_ID_NONE,
        REGION_CAVES,
        TRUE,
        "A blob of living prism that absorbs every type of magic it encounters. "
        "It evolved in a ley-line nexus and now feeds on pure mana."
    },

    /* ======================================================
     * [22] BOSS_DEATH_MERCHANT  (optional)
     * Dark commerce-themed superboss. Two phases.
     * ===================================================== */
    {
        "Death Merchant",
        "Broker of Final Transactions",
        2,
        {
            {
                "Death Merchant",
                7000, 300,
                160, 100,
                200, 140,
                40,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   W,   A  },
                { SK_PRICE_TAG, SK_GOLD_CURSE, SK_DARK_BOLT, SK_MANA_DRAIN },
                4,
                SPR_DEATH_MERCHANT, PAL_DEATH_MERCHANT,
                AI_BOSS_PHASE,
                "The price is your life!"
            },
            {
                "Death Merchant Closing",
                9500, 380,
                210, 120,
                256, 170,
                50,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   W,   A  },
                { SK_DEATHBLOW, SK_MARKET_CRASH, SK_PRICE_TAG, SK_GOLD_CURSE },
                4,
                SPR_DEATH_MERCHANT, PAL_DEATH_MERCHANT,
                AI_BOSS_PHASE,
                "Final sale — everything must go!"
            },
            { "", 0, 0, 0,0,0,0,0, 0, {N,N,N,N,N,N,N}, {0,0,0,0}, 0, 0,0, 0, "" }
        },
        { 40, 0, 0 },
        22000,
        9999,
        RCP_DEATH_COIN,
        ITEM_ID_NONE,
        REGION_BOSS,
        TRUE,
        "A spectral entity that trades in life and death. It will sell you "
        "power — at a price. Those who can't pay become its merchandise."
    },

    /* ======================================================
     * [23] BOSS_ANCIENT_GOLEM  (optional)
     * Earth superboss — unstoppable. Three phases.
     * ===================================================== */
    {
        "Ancient Golem",
        "The First Construct",
        3,
        {
            {
                "Ancient Golem",
                8000, 80,
                240, 220,
                40,  200,
                4,
                ELEM_EARTH,
                { N,  R,   R,   A,   N,   N,   N  },
                { SK_ANCIENT_SMASH, SK_SEISMIC, SK_HEAVY_BLOW, SK_ROAR },
                4,
                SPR_ANCIENT_GOLEM, PAL_ANCIENT_GOLEM,
                AI_BOSS_PHASE,
                "The first golem awakens!"
            },
            {
                "Ancient Golem Runic",
                9000, 100,
                290, 240,
                60,  220,
                7,
                ELEM_EARTH,
                { N,  R,   R,   A,   N,   N,   N  },
                { SK_ANCIENT_SMASH, SK_RUNE_BARRIER, SK_SEISMIC, SK_EARTH_PRISON },
                4,
                SPR_ANCIENT_GOLEM, PAL_ANCIENT_GOLEM,
                AI_BOSS_PHASE,
                "Ancient runes blaze to life!"
            },
            {
                "Ancient Golem Perfect",
                9999, 120,
                350, 260,
                80,  240,
                10,
                ELEM_EARTH,
                { N,  R,   R,   A,   N,   R,   R  },
                { SK_GOLEM_LASER, SK_CREATION_BEAM, SK_RUNE_BARRIER, SK_ANCIENT_SMASH },
                4,
                SPR_ANCIENT_GOLEM, PAL_ANCIENT_GOLEM,
                AI_BOSS_PHASE,
                "Perfection — the first form!"
            }
        },
        { 60, 25, 0 },
        25000,
        6000,
        RCP_ANCIENT_CLAY,
        ITEM_ID_NONE,
        REGION_RUINS,
        TRUE,
        "The very first golem ever constructed, sealed away when its creators "
        "feared it would surpass them. It has not forgotten being abandoned."
    },

    /* ======================================================
     * [24] BOSS_FALLEN_ANGEL  (optional)
     * Light turned Dark. Three phases.
     * ===================================================== */
    {
        "Fallen Angel",
        "Grace That Was Shattered",
        3,
        {
            {
                "Fallen Angel",
                7500, 350,
                180, 130,
                220, 160,
                50,
                ELEM_LIGHT,
                { N,  N,   N,   N,   N,   A,   W  },
                { SK_FALLEN_LANCE, SK_ANGEL_WRATH, SK_HOLY_SLASH, SK_ROAR },
                4,
                SPR_FALLEN_ANGEL, PAL_FALLEN_ANGEL,
                AI_BOSS_PHASE,
                "Broken wings still carry!"
            },
            {
                "Fallen Angel Corrupted",
                8500, 420,
                226, 150,
                270, 180,
                62,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   W,   A  },
                { SK_DARK_HALO, SK_FALLEN_LANCE, SK_ANGEL_WRATH, SK_SOUL_REAP },
                4,
                SPR_FALLEN_ANGEL, PAL_FALLEN_ANGEL,
                AI_BOSS_PHASE,
                "Corruption overwhelms grace!"
            },
            {
                "Fallen Angel Absolute",
                9999, 500,
                280, 170,
                340, 210,
                76,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   W,   A  },
                { SK_DIVINE_FALL, SK_DARK_HALO, SK_FALLEN_LANCE, SK_ANGEL_WRATH },
                4,
                SPR_FALLEN_ANGEL, PAL_FALLEN_ANGEL,
                AI_BOSS_PHASE,
                "Heaven mourns its champion!"
            }
        },
        { 60, 25, 0 },
        28000,
        7000,
        RCP_FEATHER_BLADE,
        ITEM_ID_NONE,
        REGION_AETHERIA,
        TRUE,
        "A celestial being cast out for questioning the divine order. "
        "It has embraced its fall and now uses the power of both light and dark."
    },

    /* ======================================================
     * [25] BOSS_PHANTOM_KING  (optional)
     * Dark undead. Two phases.
     * ===================================================== */
    {
        "Phantom King",
        "Ruler of the Hollow Court",
        2,
        {
            {
                "Phantom King",
                7000, 280,
                170, 110,
                230, 170,
                44,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   W,   A  },
                { SK_PHANTOM_SLASH, SK_SPECTRAL_WAVE, SK_ROYAL_CURSE, SK_SOUL_REAP },
                4,
                SPR_PHANTOM_KING, PAL_PHANTOM_KING,
                AI_BOSS_PHASE,
                "Spectral court assembles!"
            },
            {
                "Phantom King Wrathful",
                9500, 360,
                224, 134,
                294, 200,
                58,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   W,   A  },
                { SK_GHOST_ARMY, SK_ROYAL_CURSE, SK_PHANTOM_SLASH, SK_SPECTRAL_WAVE },
                4,
                SPR_PHANTOM_KING, PAL_PHANTOM_KING,
                AI_BOSS_PHASE,
                "The dead obey their king!"
            },
            { "", 0, 0, 0,0,0,0,0, 0, {N,N,N,N,N,N,N}, {0,0,0,0}, 0, 0,0, 0, "" }
        },
        { 40, 0, 0 },
        20000,
        5500,
        RCP_GHOST_THREAD,
        ITEM_ID_NONE,
        REGION_SHADOWREALM,
        TRUE,
        "A king who died but refused to release his court from service. "
        "He rules a hollow reflection of his mortal kingdom in the Shadow Realm."
    },

    /* ======================================================
     * [26] BOSS_VOID_HERALD  (optional)
     * Void harbinger — prelude to True Void God. Three phases.
     * ===================================================== */
    {
        "Void Herald",
        "Voice of the Absolute Silence",
        3,
        {
            {
                "Void Herald",
                7000, 400,
                190, 110,
                240, 170,
                46,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   N,   A  },
                { SK_HERALD_CRY, SK_VOID_PULSE, SK_VOID_STRIKE, SK_DARK_BOLT },
                4,
                SPR_VOID_HERALD, PAL_VOID_HERALD,
                AI_BOSS_PHASE,
                "The herald announces doom!"
            },
            {
                "Void Herald Ascended",
                8500, 490,
                242, 132,
                300, 200,
                58,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   N,   A  },
                { SK_RIFT_BEAM, SK_HERALD_CRY, SK_VOID_PULSE, SK_VOID_STORM },
                4,
                SPR_VOID_HERALD, PAL_VOID_HERALD,
                AI_BOSS_PHASE,
                "Reality fractures wide!"
            },
            {
                "Void Herald Eternal",
                9999, 580,
                300, 160,
                370, 240,
                72,
                ELEM_DARK,
                { N,  N,   N,   N,   N,   N,   A  },
                { SK_SINGULARITY, SK_RIFT_BEAM, SK_VOID_STORM, SK_HERALD_CRY },
                4,
                SPR_VOID_HERALD, PAL_VOID_HERALD,
                AI_BOSS_PHASE,
                "A singularity opens here!"
            }
        },
        { 60, 25, 0 },
        30000,
        7500,
        RCP_HERALD_RUNE,
        ITEM_ID_NONE,
        REGION_AETHERIA,
        TRUE,
        "An emissary sent ahead of the True Void God to weaken the world. "
        "It speaks only in whispers that erase the memory of those who hear them."
    },

    /* ======================================================
     * [27] BOSS_TITAN_CRAB  (optional)
     * Water/earth ocean superboss. Three phases.
     * ===================================================== */
    {
        "Titan Crab",
        "The Island That Moves",
        3,
        {
            {
                "Titan Crab",
                9000, 100,
                260, 240,
                60,  210,
                8,
                ELEM_WATER,
                { N,  W,   A,   R,   N,   N,   N  },
                { SK_PINCER, SK_TIDAL_CRUSH, SK_HEAVY_BLOW, SK_ROAR },
                4,
                SPR_TITAN_CRAB, PAL_TITAN_CRAB,
                AI_BOSS_PHASE,
                "The sea floor rises up!"
            },
            {
                "Titan Crab Enraged",
                9500, 130,
                316, 260,
                78,  230,
                12,
                ELEM_WATER,
                { N,  W,   A,   R,   N,   N,   N  },
                { SK_SHELL_CANNON, SK_PINCER, SK_TIDAL_CRUSH, SK_TIDAL_WAVE },
                4,
                SPR_TITAN_CRAB, PAL_TITAN_CRAB,
                AI_BOSS_PHASE,
                "Cannon shells rain down!"
            },
            {
                "Titan Crab Leviathan",
                9999, 160,
                380, 280,
                100, 250,
                16,
                ELEM_WATER,
                { N,  W,   A,   R,   N,   N,   N  },
                { SK_ABYSSAL_TIDE, SK_SHELL_CANNON, SK_PINCER, SK_TIDAL_CRUSH },
                4,
                SPR_TITAN_CRAB, PAL_TITAN_CRAB,
                AI_BOSS_PHASE,
                "The abyss rises to devour!"
            }
        },
        { 60, 25, 0 },
        26000,
        6500,
        RCP_CRAB_SOUP,
        ITEM_ID_NONE,
        REGION_OCEAN,
        TRUE,
        "A crab so enormous that sailors have built docks on its shell, "
        "mistaking it for an island. It is very, very unhappy about that."
    },

    /* ======================================================
     * [28] BOSS_MIRROR_DEMON  (optional)
     * Copies player's element. Three phases.
     * ===================================================== */
    {
        "Mirror Demon",
        "Reflection of Your Worst Fear",
        3,
        {
            {
                "Mirror Demon",
                7500, 300,
                200, 140,
                200, 140,
                40,
                ELEM_NONE,
                { N,  N,   N,   N,   N,   N,   N  },
                { SK_MIRROR_COPY, SK_SLASH, SK_HEAVY_BLOW, SK_ROAR },
                4,
                SPR_MIRROR_DEMON, PAL_MIRROR_DEMON,
                AI_BOSS_PHASE,
                "It copies your every move!"
            },
            {
                "Mirror Demon Warped",
                8500, 380,
                256, 170,
                256, 170,
                52,
                ELEM_NONE,
                { N,  N,   N,   N,   N,   N,   N  },
                { SK_REFLECTED_DOOM, SK_MIRROR_COPY, SK_INVERSE_FIELD, SK_SLASH },
                4,
                SPR_MIRROR_DEMON, PAL_MIRROR_DEMON,
                AI_BOSS_PHASE,
                "Your attacks return tenfold!"
            },
            {
                "Mirror Demon True",
                9999, 460,
                320, 210,
                320, 210,
                66,
                ELEM_NONE,
                { N,  N,   N,   N,   N,   N,   N  },
                { SK_SHATTER_MIRROR, SK_REFLECTED_DOOM, SK_INVERSE_FIELD, SK_MIRROR_COPY },
                4,
                SPR_MIRROR_DEMON, PAL_MIRROR_DEMON,
                AI_BOSS_PHASE,
                "The mirror shatters — unleashed!"
            }
        },
        { 60, 25, 0 },
        24000,
        6000,
        RCP_MIRROR_SHARD,
        ITEM_ID_NONE,
        REGION_BOSS,
        TRUE,
        "A demon made of living mirror-glass. It reflects your own power "
        "and adapts, growing stronger with each attack it copies."
    },

    /* ======================================================
     * [29] BOSS_TRUE_VOID_GOD  (optional)
     * Ultimate secret final boss. Three phases. Max HP.
     * ===================================================== */
    {
        "True Void God",
        "The Absolute — Beyond Existence",
        3,
        {
            {
                "True Void God",
                9000, 700,
                280, 200,
                310, 240,
                60,
                ELEM_DARK,
                { R,  R,   R,   R,   R,   W,   A  },
                { SK_VOID_ABSOLUTE, SK_COSMIC_TEAR, SK_OBLIVION, SK_VOID_COLLAPSE },
                4,
                SPR_TRUE_VOID_GOD, PAL_TRUE_VOID_GOD,
                AI_BOSS_PHASE,
                "True void has no limits!"
            },
            {
                "True Void God Unveiled",
                9999, 860,
                360, 240,
                390, 290,
                76,
                ELEM_DARK,
                { R,  R,   R,   R,   R,   W,   A  },
                { SK_REALITY_ERASE, SK_VOID_ABSOLUTE, SK_COSMIC_TEAR, SK_OBLIVION },
                4,
                SPR_TRUE_VOID_GOD, PAL_TRUE_VOID_GOD,
                AI_BOSS_PHASE,
                "Existence is a mistake!"
            },
            {
                "True Void God Final Form",
                9999, 9999,
                460, 280,
                490, 350,
                99,
                ELEM_DARK,
                { A,  R,   R,   R,   R,   W,   A  },
                { SK_TRUE_OBLIVION, SK_REALITY_ERASE, SK_VOID_ABSOLUTE, SK_ANNIHILATION },
                4,
                SPR_TRUE_VOID_GOD, PAL_TRUE_VOID_GOD,
                AI_BOSS_PHASE,
                "This is the end of all!"
            }
        },
        { 60, 25, 0 },
        99999,
        9999,
        RCP_TRUE_VOID_INK,
        ITEM_ID_NONE,
        REGION_AETHERIA,
        TRUE,
        "The true face of the Void God, hidden behind the form that was "
        "defeated. It has existed since before the first atom and will exist "
        "long after the last star dies. There is no escape."
    }

}; /* end g_boss_db */

/* =========================================================
 * API implementation
 * ========================================================= */

const BossDef* boss_get(u8 id)
{
    if (id >= BOSS_COUNT) return NULL;
    return &g_boss_db[id];
}

void boss_db_init(void)
{
    /* All data is const ROM — nothing to initialise at runtime. */
}
