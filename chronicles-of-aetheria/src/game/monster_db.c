/* Chronicles of Aetheria — Monster Database (ROM data) */

#include "../../include/game/monster_db.h"
#include "../../include/game/item/item_db.h"

/* resist[] order: NONE, FIRE, WATER, EARTH, WIND, LIGHT, DARK */
#define N  ELEM_NEUTRAL
#define R  ELEM_RESIST
#define W  ELEM_WEAK
#define A  ELEM_ABSORB
#define I  0   /* immune */

/* =========================================================
 * g_monster_db[150]
 * ========================================================= */
const MonsterDef g_monster_db[MONSTER_COUNT] = {

/* ---- Aelwyn Surroundings (0-12) -------------------------------- */
[MON_SLIME] = {
    "Slime", 0, 0, ELEM_WATER,
    /*hp  mp  atk def mag mdef spd*/
    30,  0,  6,  2,  0,  2,  4,
    {N,W,A,N,N,W,R},
    8, 2, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_AELWYN, 1, 5,
    {{ITEM_MONSTER_HIDE,1,40},{ITEM_PURE_WATER,1,20},{ITEM_NONE,0,0}},
    FALSE, "A gelatinous blob that wobbles aimlessly. Weak to fire."
},
[MON_FOREST_RAT] = {
    "Forest Rat", 1, 0, ELEM_NONE,
    25,  0,  8,  3,  0,  2,  8,
    {N,N,N,N,N,N,N},
    6, 2, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_AELWYN, 1, 4,
    {{ITEM_BONE_FRAGMENT,1,30},{ITEM_NONE,0,0},{ITEM_NONE,0,0}},
    FALSE, "A rodent that scurries through the underbrush near Aelwyn."
},
[MON_GOBLIN_SCOUT] = {
    "Goblin Scout", 2, 0, ELEM_NONE,
    35,  0, 10,  4,  0,  3,  9,
    {N,N,N,N,N,N,N},
    10, 4, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_AELWYN, 1, 5,
    {{ITEM_IRON_ORE,1,25},{ITEM_HERB,1,30},{ITEM_NONE,0,0}},
    FALSE, "A small, cowardly goblin sent ahead to scout for ambushes."
},
[MON_GOBLIN_WARRIOR] = {
    "Goblin Warrior", 3, 0, ELEM_NONE,
    55,  0, 15,  8,  0,  5,  7,
    {N,N,N,N,N,N,N},
    18, 6, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_AELWYN, 2, 6,
    {{ITEM_IRON_ORE,1,35},{ITEM_BONE_FRAGMENT,1,20},{ITEM_NONE,0,0}},
    FALSE, "A tougher goblin wielding a crude iron blade."
},
[MON_WILD_BOAR] = {
    "Wild Boar", 4, 0, ELEM_NONE,
    65,  0, 14,  6,  0,  4,  6,
    {N,N,N,N,N,N,N},
    14, 5, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_AGGRESSIVE,
    REGION_AELWYN, 1, 5,
    {{ITEM_MONSTER_HIDE,1,50},{ITEM_BONE_FRAGMENT,1,20},{ITEM_NONE,0,0}},
    FALSE, "A territorial boar that charges anything that approaches."
},
[MON_BROWN_WOLF] = {
    "Brown Wolf", 5, 0, ELEM_NONE,
    50,  0, 13,  5,  0,  4, 11,
    {N,N,N,N,N,N,N},
    12, 4, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_AGGRESSIVE,
    REGION_AELWYN, 1, 5,
    {{ITEM_MONSTER_HIDE,1,45},{ITEM_FEATHER,1,15},{ITEM_NONE,0,0}},
    FALSE, "A pack predator that hunts the edges of Aelwyn forest."
},
[MON_GIANT_BEE] = {
    "Giant Bee", 6, 0, ELEM_WIND,
    30,  0, 11,  3,  0,  4, 13,
    {N,N,N,W,R,N,N},
    10, 3, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_AELWYN, 1, 4,
    {{ITEM_RESIN,1,40},{ITEM_FEATHER,1,25},{ITEM_NONE,0,0}},
    FALSE, "An oversized bee with a venomous stinger."
},
[MON_MUSHROOM_MAN] = {
    "Mushroom Man", 7, 0, ELEM_EARTH,
    70,  10,  8, 10,  8,  6,  3,
    {N,N,W,R,W,N,N},
    20, 6, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 2, AI_RANDOM,
    REGION_AELWYN, 2, 6,
    {{ITEM_HERB,2,50},{ITEM_GLOWMOSS,1,10},{ITEM_NONE,0,0}},
    FALSE, "A sentient fungal creature that releases sleep spores."
},
[MON_STONE_CRAB] = {
    "Stone Crab", 8, 0, ELEM_EARTH,
    80,  0,  9, 16,  0,  8,  2,
    {N,N,W,R,N,N,N},
    16, 5, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_AELWYN, 2, 6,
    {{ITEM_IRON_ORE,1,30},{ITEM_CRYSTAL_SHARD,1,15},{ITEM_NONE,0,0}},
    FALSE, "A crab with a shell as hard as stone. Slow but resilient."
},
[MON_BANDIT] = {
    "Bandit", 9, 0, ELEM_NONE,
    60,  0, 16,  7,  0,  5, 10,
    {N,N,N,N,N,N,N},
    20, 8, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_AELWYN, 2, 7,
    {{ITEM_IRON_ORE,1,20},{ITEM_HERB,1,25},{ITEM_NONE,0,0}},
    FALSE, "A road thief who preys on travellers near Aelwyn."
},
[MON_BANDIT_ARCHER] = {
    "Bandit Archer", 10, 0, ELEM_NONE,
    50,  0, 14,  5,  0,  5, 12,
    {N,N,N,N,N,N,N},
    18, 7, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_AELWYN, 2, 7,
    {{ITEM_FEATHER,2,35},{ITEM_SNAKE_VENOM,1,15},{ITEM_NONE,0,0}},
    FALSE, "A bandit who harasses from range with a shortbow."
},
[MON_BANDIT_MAGE] = {
    "Bandit Mage", 11, 5, ELEM_NONE,
    40,  25, 10,  4, 14,  8,  9,
    {N,N,N,N,N,N,N},
    25, 9, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_AELWYN, 3, 8,
    {{ITEM_MANA_SHARD,1,30},{ITEM_ARCANE_DUST,1,20},{ITEM_NONE,0,0}},
    FALSE, "A rogue spellcaster who flings fire at unsuspecting travellers."
},
[MON_PLAGUE_RAT] = {
    "Plague Rat", 12, 0, ELEM_NONE,
    40,  0, 12,  4,  0,  3, 12,
    {N,N,N,N,N,N,N},
    15, 5, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_AELWYN, 3, 8,
    {{ITEM_SNAKE_VENOM,1,40},{ITEM_BONE_FRAGMENT,1,20},{ITEM_NONE,0,0}},
    FALSE, "A diseased rat whose bite inflicts poison. Handle with care."
},

/* ---- Verdant Forest (13-27) ------------------------------------ */
[MON_TREANT] = {
    "Treant", 13, 1, ELEM_EARTH,
    120,  0, 18, 14,  5, 10,  2,
    {N,W,N,R,N,N,N},
    35, 10, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 2, AI_RANDOM,
    REGION_VERDANT, 4, 10,
    {{ITEM_THORN_WOOD,2,50},{ITEM_RESIN,1,30},{ITEM_EARTH_ESSENCE,1,15}},
    FALSE, "An ancient tree awakened by dark magic. Burns quickly."
},
[MON_VINE_SNAKE] = {
    "Vine Snake", 14, 0, ELEM_EARTH,
    55,  0, 16,  6,  0,  5, 13,
    {N,N,N,R,N,N,N},
    22, 7, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_VERDANT, 4, 9,
    {{ITEM_SNAKE_VENOM,2,45},{ITEM_MONSTER_HIDE,1,30},{ITEM_NONE,0,0}},
    FALSE, "Camouflaged among the vines, this serpent strikes without warning."
},
[MON_FOREST_SPIDER] = {
    "Forest Spider", 15, 0, ELEM_NONE,
    45,  0, 14,  5,  0,  4, 11,
    {N,N,N,N,N,N,N},
    18, 6, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_VERDANT, 3, 8,
    {{ITEM_SPIDER_SILK,2,55},{ITEM_SNAKE_VENOM,1,20},{ITEM_NONE,0,0}},
    FALSE, "Weaves sticky webs across forest paths to trap prey."
},
[MON_GIANT_SPIDER] = {
    "Giant Spider", 16, 0, ELEM_NONE,
    90,  0, 20,  9,  0,  6, 10,
    {N,N,N,N,N,N,N},
    40, 12, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_VERDANT, 5, 11,
    {{ITEM_SPIDER_SILK,3,60},{ITEM_SNAKE_VENOM,2,30},{ITEM_MONSTER_HIDE,1,20}},
    FALSE, "A massive arachnid that can web the entire party."
},
[MON_DRYAD] = {
    "Dryad", 17, 2, ELEM_EARTH,
    75,  40, 12, 10, 18, 12,  8,
    {N,W,N,R,N,N,N},
    45, 14, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 2, AI_HEALER,
    REGION_VERDANT, 5, 11,
    {{ITEM_EARTH_ESSENCE,1,35},{ITEM_HERB,3,50},{ITEM_GLOWMOSS,2,25}},
    FALSE, "A forest spirit that heals its allies and summons thorns."
},
[MON_ELF_ROGUE] = {
    "Elf Rogue", 18, 0, ELEM_NONE,
    65,  0, 20,  7,  0,  7, 15,
    {N,N,N,N,N,N,N},
    38, 13, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_VERDANT, 5, 12,
    {{ITEM_FEATHER,2,30},{ITEM_ARCANE_DUST,1,20},{ITEM_NONE,0,0}},
    FALSE, "A banished elf who ambushes intruders from the treetops."
},
[MON_ELF_ARCHER] = {
    "Elf Archer", 19, 0, ELEM_NONE,
    60,  0, 22,  6,  0,  8, 14,
    {N,N,N,N,N,N,N},
    40, 13, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_VERDANT, 5, 12,
    {{ITEM_FEATHER,3,40},{ITEM_WIND_ESSENCE,1,15},{ITEM_NONE,0,0}},
    FALSE, "Fires enchanted arrows from long range with deadly accuracy."
},
[MON_PIXIE] = {
    "Pixie", 20, 3, ELEM_WIND,
    35,  50,  8,  4, 22, 14, 16,
    {N,N,N,W,R,N,N},
    30, 10, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_VERDANT, 4, 10,
    {{ITEM_WIND_ESSENCE,1,40},{ITEM_ARCANE_DUST,2,35},{ITEM_MANA_SHARD,1,20}},
    FALSE, "A mischievous fairy that casts confusion and sleep spells."
},
[MON_HORNET_SWARM] = {
    "Hornet Swarm", 21, 0, ELEM_WIND,
    60,  0, 17,  3,  0,  5, 17,
    {N,N,N,W,R,N,N},
    28, 9, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_AGGRESSIVE,
    REGION_VERDANT, 4, 10,
    {{ITEM_RESIN,1,30},{ITEM_SNAKE_VENOM,1,20},{ITEM_NONE,0,0}},
    FALSE, "Hundreds of venomous hornets acting as a single entity."
},
[MON_THORN_WOLF] = {
    "Thorn Wolf", 22, 0, ELEM_EARTH,
    80,  0, 22, 10,  0,  8, 12,
    {N,N,N,R,N,N,N},
    42, 13, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_VERDANT, 6, 12,
    {{ITEM_MONSTER_HIDE,2,45},{ITEM_THORN_WOOD,2,35},{ITEM_EARTH_ESSENCE,1,15}},
    FALSE, "A wolf with thorny bark-like hide. Its bite tears and poisons."
},
[MON_CORRUPTED_BEAR] = {
    "Corrupted Bear", 23, 0, ELEM_DARK,
    140,  0, 26, 14,  0,  8,  6,
    {N,N,N,N,N,W,R},
    60, 16, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_VERDANT, 7, 14,
    {{ITEM_MONSTER_HIDE,3,50},{ITEM_DARK_ESSENCE,1,25},{ITEM_BONE_FRAGMENT,2,30}},
    FALSE, "A bear twisted by dark energy seeping from the forest depths."
},
[MON_MANDRAKE] = {
    "Mandrake", 24, 4, ELEM_EARTH,
    65,  30, 10, 12, 16, 14,  4,
    {N,W,N,R,W,N,N},
    35, 11, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_VERDANT, 5, 11,
    {{ITEM_HERB,3,60},{ITEM_EARTH_ESSENCE,1,30},{ITEM_GLOWMOSS,1,20}},
    FALSE, "Its scream causes paralysis. Harvesting its root is dangerous."
},
[MON_SPORE_BAT] = {
    "Spore Bat", 25, 0, ELEM_WIND,
    50,  0, 13,  4,  0,  5, 14,
    {N,N,N,W,R,N,N},
    24, 8, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_VERDANT, 4, 9,
    {{ITEM_FEATHER,2,35},{ITEM_ARCANE_DUST,1,15},{ITEM_NONE,0,0}},
    FALSE, "A bat that releases toxic spores in a cloud around itself."
},
[MON_ELDER_TREANT] = {
    "Elder Treant", 26, 5, ELEM_EARTH,
    200,  20, 28, 22,  8, 16,  2,
    {N,W,N,R,N,N,N},
    90, 25, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_VERDANT, 9, 16,
    {{ITEM_THORN_WOOD,4,70},{ITEM_EARTH_ESSENCE,2,40},{ITEM_ANCIENT_RUNE,1,10}},
    FALSE, "A towering ancient tree. Can summon saplings and cause earthquakes."
},
[MON_FOREST_WITCH] = {
    "Forest Witch", 27, 6, ELEM_DARK,
    90,  60, 14,  8, 24, 16, 10,
    {N,N,W,N,N,W,R},
    80, 22, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 4, AI_REACT,
    REGION_VERDANT, 10, 17,
    {{ITEM_DARK_ESSENCE,2,40},{ITEM_ARCANE_DUST,3,35},{ITEM_SHADOW_CLOTH,1,15}},
    FALSE, "An exiled witch who tends the forest's dark heart."
},

/* ---- Crystal Caves (28-41) ------------------------------------- */
[MON_CRYSTAL_BAT] = {
    "Crystal Bat", 28, 7, ELEM_EARTH,
    60,  0, 16,  6,  0,  6, 13,
    {N,N,W,R,N,N,N},
    30, 9, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_CAVES, 6, 12,
    {{ITEM_CRYSTAL_SHARD,1,40},{ITEM_FEATHER,1,20},{ITEM_NONE,0,0}},
    FALSE, "A bat encrusted with crystal formations that deflect attacks."
},
[MON_CAVE_TROLL] = {
    "Cave Troll", 29, 8, ELEM_EARTH,
    180,  0, 28, 18,  0, 10,  3,
    {N,W,N,R,N,N,N},
    65, 18, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_CAVES, 7, 14,
    {{ITEM_IRON_ORE,2,40},{ITEM_BONE_FRAGMENT,2,30},{ITEM_MONSTER_HIDE,1,20}},
    FALSE, "A massive troll that regenerates health each turn."
},
[MON_ROCK_GOLEM] = {
    "Rock Golem", 30, 9, ELEM_EARTH,
    200,  0, 24, 25,  0, 12,  2,
    {N,N,N,A,N,N,N},
    70, 18, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_CAVES, 8, 15,
    {{ITEM_IRON_ORE,3,50},{ITEM_EARTH_ESSENCE,2,30},{ITEM_CRYSTAL_SHARD,2,25}},
    FALSE, "An animated rock construct. Absorbs earth damage."
},
[MON_CAVE_SLIME] = {
    "Cave Slime", 31, 10, ELEM_WATER,
    55,  0,  8,  5,  0,  5,  3,
    {N,W,A,N,N,W,R},
    18, 6, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_CAVES, 6, 11,
    {{ITEM_PURE_WATER,1,40},{ITEM_GLOWMOSS,1,30},{ITEM_NONE,0,0}},
    FALSE, "A bioluminescent slime that drips from stalactites."
},
[MON_BLIND_WORM] = {
    "Blind Worm", 32, 11, ELEM_EARTH,
    100,  0, 20, 12,  0,  8,  5,
    {N,N,W,R,N,N,N},
    45, 13, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_AGGRESSIVE,
    REGION_CAVES, 7, 13,
    {{ITEM_MONSTER_HIDE,2,40},{ITEM_BONE_FRAGMENT,1,25},{ITEM_NONE,0,0}},
    FALSE, "A massive eyeless worm that senses vibrations through the rock."
},
[MON_CRYSTAL_SERPENT] = {
    "Crystal Serpent", 33, 12, ELEM_EARTH,
    110,  0, 24, 14,  0,  9, 11,
    {N,N,N,R,N,N,N},
    55, 16, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_CAVES, 8, 15,
    {{ITEM_CRYSTAL_SHARD,3,50},{ITEM_SNAKE_VENOM,2,30},{ITEM_EARTH_ESSENCE,1,15}},
    FALSE, "A serpent with crystalline scales sharp enough to pierce armor."
},
[MON_EARTH_WISP] = {
    "Earth Wisp", 34, 13, ELEM_EARTH,
    65,  40, 12,  8, 20, 14,  9,
    {N,N,W,A,N,N,N},
    38, 11, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_CAVES, 7, 13,
    {{ITEM_EARTH_ESSENCE,2,45},{ITEM_MANA_SHARD,1,25},{ITEM_NONE,0,0}},
    FALSE, "A glowing orb of condensed earth magic that haunts deep caves."
},
[MON_STALACTITE_CRAB] = {
    "Stalactite Crab", 35, 14, ELEM_EARTH,
    130,  0, 18, 20,  0, 10,  2,
    {N,N,N,R,N,N,N},
    50, 14, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_CAVES, 7, 13,
    {{ITEM_IRON_ORE,2,35},{ITEM_CRYSTAL_SHARD,1,25},{ITEM_NONE,0,0}},
    FALSE, "Clings to cave ceilings and drops on unwary adventurers."
},
[MON_GEM_BEETLE] = {
    "Gem Beetle", 36, 15, ELEM_EARTH,
    75,  0, 14, 18,  0,  9,  4,
    {N,N,N,R,N,N,N},
    40, 12, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_CAVES, 7, 13,
    {{ITEM_CRYSTAL_SHARD,2,50},{ITEM_IRON_ORE,1,20},{ITEM_NONE,0,0}},
    FALSE, "A beetle with gemstone shells that refract light into beams."
},
[MON_KOBOLD] = {
    "Kobold", 37, 16, ELEM_NONE,
    70,  0, 16,  8,  0,  6, 10,
    {N,N,N,N,N,N,N},
    35, 11, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_CAVES, 7, 13,
    {{ITEM_IRON_ORE,1,30},{ITEM_COAL,2,40},{ITEM_NONE,0,0}},
    FALSE, "A small reptilian miner who guards cave resources aggressively."
},
[MON_KOBOLD_SHAMAN] = {
    "Kobold Shaman", 38, 17, ELEM_EARTH,
    60,  45, 10,  6, 18, 12,  8,
    {N,N,N,R,N,N,N},
    45, 14, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 2, AI_HEALER,
    REGION_CAVES, 8, 14,
    {{ITEM_EARTH_ESSENCE,1,35},{ITEM_MANA_SHARD,1,30},{ITEM_ARCANE_DUST,1,20}},
    FALSE, "Heals kobold allies and calls stone spikes from the cave floor."
},
[MON_STONE_GUARDIAN] = {
    "Stone Guardian", 39, 18, ELEM_EARTH,
    220,  0, 26, 28,  0, 14,  3,
    {N,N,N,A,N,N,N},
    85, 22, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_CAVES, 10, 16,
    {{ITEM_IRON_ORE,4,60},{ITEM_EARTH_ESSENCE,3,35},{ITEM_STEEL_INGOT,1,10}},
    FALSE, "An ancient construct set to guard the cave's inner sanctum."
},
[MON_CRYSTAL_GOLEM] = {
    "Crystal Golem", 40, 19, ELEM_EARTH,
    240,  0, 28, 24,  0, 16,  2,
    {N,N,N,A,N,N,N},
    95, 25, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_CAVES, 11, 17,
    {{ITEM_CRYSTAL_SHARD,5,70},{ITEM_EARTH_ESSENCE,3,40},{ITEM_ANCIENT_RUNE,1,12}},
    FALSE, "A golem forged entirely from enchanted crystal. Shards its body as a weapon."
},
[MON_DEEP_LURKER] = {
    "Deep Lurker", 41, 20, ELEM_DARK,
    160,  20, 30, 15,  8, 12, 10,
    {N,N,N,N,N,W,R},
    75, 20, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 4, AI_REACT,
    REGION_CAVES, 10, 16,
    {{ITEM_DARK_ESSENCE,2,35},{ITEM_MONSTER_HIDE,2,40},{ITEM_SHADOW_CLOTH,1,15}},
    FALSE, "Something ancient and eyeless. It feeds on fear."
},

/* ---- Murkhaven Swamp (42-54) ----------------------------------- */
[MON_BOG_SLIME] = {
    "Bog Slime", 42, 21, ELEM_WATER,
    80,  0, 10,  6,  0,  6,  2,
    {N,W,A,N,N,N,N},
    28, 9, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_SWAMP, 8, 14,
    {{ITEM_PURE_WATER,1,30},{ITEM_SNAKE_VENOM,1,25},{ITEM_NONE,0,0}},
    FALSE, "A toxic slime lurking in the murky waters of the swamp."
},
[MON_SWAMP_CROC] = {
    "Swamp Croc", 43, 22, ELEM_WATER,
    160,  0, 28, 16,  0, 10,  6,
    {N,N,R,N,N,N,N},
    65, 18, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_SWAMP, 9, 15,
    {{ITEM_MONSTER_HIDE,3,55},{ITEM_BONE_FRAGMENT,2,30},{ITEM_NONE,0,0}},
    FALSE, "A massive crocodilian that camouflages perfectly in murky water."
},
[MON_PLAGUE_FROG] = {
    "Plague Frog", 44, 23, ELEM_WATER,
    65,  0, 14,  6,  0,  8, 10,
    {N,W,R,N,N,N,N},
    30, 10, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_SWAMP, 8, 13,
    {{ITEM_SNAKE_VENOM,2,50},{ITEM_PURE_WATER,1,20},{ITEM_NONE,0,0}},
    FALSE, "Coated in a paralytic toxin. Its tongue can reach ten feet."
},
[MON_WILL_O_WISP] = {
    "Will-o'-Wisp", 45, 24, ELEM_DARK,
    50,  60, 10,  4, 22, 18, 14,
    {N,N,N,N,N,W,A},
    40, 12, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_SWAMP, 9, 15,
    {{ITEM_DARK_ESSENCE,1,40},{ITEM_MANA_SHARD,1,30},{ITEM_NONE,0,0}},
    FALSE, "A spectral light that lures wanderers to their doom."
},
[MON_SWAMP_WRAITH] = {
    "Swamp Wraith", 46, 25, ELEM_DARK,
    95,  30, 22, 10, 16, 15, 11,
    {N,N,N,N,N,W,R},
    55, 16, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_SWAMP, 10, 16,
    {{ITEM_DARK_ESSENCE,2,40},{ITEM_SHADOW_CLOTH,1,20},{ITEM_NONE,0,0}},
    FALSE, "A restless spirit of one who drowned in the swamp long ago."
},
[MON_GIANT_LEECH] = {
    "Giant Leech", 47, 26, ELEM_WATER,
    110,  0, 18, 10,  0,  8,  5,
    {N,N,R,N,N,N,N},
    42, 13, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_AGGRESSIVE,
    REGION_SWAMP, 9, 14,
    {{ITEM_MONSTER_HIDE,2,45},{ITEM_PURE_WATER,1,20},{ITEM_NONE,0,0}},
    FALSE, "Drains HP each turn it remains attached. Thrives in stagnant pools."
},
[MON_TOXIC_SNAKE] = {
    "Toxic Snake", 48, 27, ELEM_WATER,
    70,  0, 18,  6,  0,  7, 14,
    {N,N,R,N,N,N,N},
    35, 11, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_SWAMP, 8, 14,
    {{ITEM_SNAKE_VENOM,3,55},{ITEM_MONSTER_HIDE,1,30},{ITEM_NONE,0,0}},
    FALSE, "Its venom can kill a grown man in minutes."
},
[MON_MUD_GOLEM] = {
    "Mud Golem", 49, 28, ELEM_EARTH,
    180,  0, 22, 20,  0, 10,  2,
    {N,W,R,R,N,N,N},
    60, 17, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_SWAMP, 10, 16,
    {{ITEM_EARTH_ESSENCE,2,40},{ITEM_IRON_ORE,1,20},{ITEM_NONE,0,0}},
    FALSE, "A construct born from the swamp's toxic mud. Resists water and earth."
},
[MON_SPORE_CLOUD] = {
    "Spore Cloud", 50, 29, ELEM_WIND,
    45,  20,  8,  2, 16, 12, 12,
    {N,N,N,N,R,N,N},
    25, 8, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_SWAMP, 8, 13,
    {{ITEM_HERB,2,35},{ITEM_GLOWMOSS,2,40},{ITEM_ARCANE_DUST,1,15}},
    FALSE, "A drifting mass of hallucinogenic spores. Inflicts confusion."
},
[MON_SWAMP_HAG] = {
    "Swamp Hag", 51, 30, ELEM_DARK,
    105,  55, 16, 10, 22, 14,  8,
    {N,N,W,N,N,W,R},
    70, 20, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 4, AI_REACT,
    REGION_SWAMP, 11, 17,
    {{ITEM_DARK_ESSENCE,2,40},{ITEM_SNAKE_VENOM,2,35},{ITEM_SHADOW_CLOTH,1,20}},
    FALSE, "An ancient crone who commands the swamp's denizens."
},
[MON_DEATH_FLOWER] = {
    "Death Flower", 52, 31, ELEM_DARK,
    80,  30, 14, 12, 18, 10,  1,
    {N,W,R,N,N,W,R},
    45, 14, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_SWAMP, 10, 16,
    {{ITEM_HERB,2,40},{ITEM_DARK_ESSENCE,1,30},{ITEM_SNAKE_VENOM,2,25}},
    FALSE, "A carnivorous plant that exhales a death-dealing pollen."
},
[MON_BASILISK] = {
    "Basilisk", 53, 32, ELEM_EARTH,
    200,  0, 30, 20,  0, 14,  7,
    {N,N,N,R,N,N,N},
    100, 28, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_SWAMP, 13, 19,
    {{ITEM_MONSTER_HIDE,4,60},{ITEM_EARTH_ESSENCE,2,35},{ITEM_ANCIENT_RUNE,1,8}},
    FALSE, "Its gaze petrifies. Avert your eyes or be turned to stone."
},
[MON_SWAMP_TROLL] = {
    "Swamp Troll", 54, 33, ELEM_WATER,
    250,  0, 32, 22,  0, 12,  4,
    {N,W,R,N,N,N,N},
    110, 30, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_SWAMP, 13, 20,
    {{ITEM_MONSTER_HIDE,4,55},{ITEM_BONE_FRAGMENT,3,35},{ITEM_IRON_ORE,2,20}},
    FALSE, "A regenerating monstrosity that dominates the swamp's murky depths."
},

/* ---- Ironpeak Mountains (55-69) -------------------------------- */
[MON_MOUNTAIN_WOLF] = {
    "Mountain Wolf", 55, 34, ELEM_WIND,
    100,  0, 26,  8,  0,  8, 14,
    {N,N,N,N,R,N,N},
    50, 15, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_MOUNTAINS, 12, 18,
    {{ITEM_MONSTER_HIDE,2,45},{ITEM_FEATHER,1,20},{ITEM_NONE,0,0}},
    FALSE, "A pack hunter adapted to mountain altitudes. Hunts in packs."
},
[MON_HARPY] = {
    "Harpy", 56, 35, ELEM_WIND,
    85,  0, 24,  8,  0,  9, 16,
    {N,N,N,W,R,N,N},
    55, 16, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_MOUNTAINS, 12, 18,
    {{ITEM_FEATHER,3,55},{ITEM_WIND_ESSENCE,1,25},{ITEM_NONE,0,0}},
    FALSE, "A winged woman-beast that dives from mountain peaks to attack."
},
[MON_WYVERN] = {
    "Wyvern", 57, 36, ELEM_WIND,
    160,  0, 32, 14,  0, 12, 13,
    {N,N,N,W,R,N,N},
    90, 25, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_MOUNTAINS, 14, 20,
    {{ITEM_DRAGON_SCALE,1,30},{ITEM_FEATHER,3,45},{ITEM_WIND_ESSENCE,2,25}},
    FALSE, "A two-legged dragon that rides the mountain thermals."
},
[MON_GRANITE_TROLL] = {
    "Granite Troll", 58, 37, ELEM_EARTH,
    280,  0, 34, 26,  0, 14,  3,
    {N,N,N,R,N,N,N},
    100, 27, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_MOUNTAINS, 13, 19,
    {{ITEM_IRON_ORE,4,55},{ITEM_STEEL_INGOT,1,20},{ITEM_BONE_FRAGMENT,3,30}},
    FALSE, "Nearly indistinguishable from the boulders it sleeps among."
},
[MON_WIND_ELEMENTAL] = {
    "Wind Elemental", 59, 38, ELEM_WIND,
    120,  50, 22, 10, 28, 18, 18,
    {N,N,N,W,A,N,N},
    75, 21, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_MOUNTAINS, 13, 19,
    {{ITEM_WIND_ESSENCE,3,55},{ITEM_MANA_SHARD,2,30},{ITEM_STORM_CORE,1,10}},
    FALSE, "A living whirlwind that tears through mountain passes."
},
[MON_ICE_ELEMENTAL] = {
    "Ice Elemental", 60, 39, ELEM_WATER,
    130,  50, 24, 12, 26, 16, 10,
    {N,W,A,W,N,N,N},
    80, 22, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_MOUNTAINS, 14, 20,
    {{ITEM_ICE_ESSENCE,3,55},{ITEM_FROZEN_TEAR,1,20},{ITEM_MANA_SHARD,2,25}},
    FALSE, "A crystalline being of pure ice that freezes everything it touches."
},
[MON_YETI] = {
    "Yeti", 61, 40, ELEM_WATER,
    220,  0, 36, 20,  0, 14,  6,
    {N,W,R,N,N,N,N},
    95, 26, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_MOUNTAINS, 14, 21,
    {{ITEM_MONSTER_HIDE,4,55},{ITEM_ICE_ESSENCE,2,30},{ITEM_BONE_FRAGMENT,2,20}},
    FALSE, "A massive, fur-covered beast of the high peaks. Roars cause fear."
},
[MON_STORM_EAGLE] = {
    "Storm Eagle", 62, 41, ELEM_WIND,
    100,  0, 28, 10,  0, 10, 17,
    {N,N,N,N,R,N,N},
    60, 17, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_AGGRESSIVE,
    REGION_MOUNTAINS, 12, 18,
    {{ITEM_FEATHER,4,60},{ITEM_WIND_ESSENCE,2,30},{ITEM_STORM_CORE,1,8}},
    FALSE, "An eagle wreathed in lightning that nests at the mountain summit."
},
[MON_CLIFF_SPIDER] = {
    "Cliff Spider", 63, 42, ELEM_NONE,
    90,  0, 22, 14,  0, 10, 11,
    {N,N,N,N,N,N,N},
    45, 14, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_MOUNTAINS, 12, 18,
    {{ITEM_SPIDER_SILK,3,50},{ITEM_SNAKE_VENOM,2,30},{ITEM_NONE,0,0}},
    FALSE, "Scales sheer cliff faces and ambushes from above."
},
[MON_ROCK_TITAN] = {
    "Rock Titan", 64, 43, ELEM_EARTH,
    350,  0, 38, 30,  0, 16,  2,
    {N,N,N,A,N,N,N},
    130, 35, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_MOUNTAINS, 16, 22,
    {{ITEM_IRON_ORE,5,65},{ITEM_STEEL_INGOT,2,30},{ITEM_EARTH_ESSENCE,3,25}},
    FALSE, "A creature of living stone as tall as a mountain fortress tower."
},
[MON_THUNDER_LIZARD] = {
    "Thunder Lizard", 65, 44, ELEM_WIND,
    150,  0, 30, 16,  0, 12, 12,
    {N,N,N,W,R,N,N},
    80, 22, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_MOUNTAINS, 14, 20,
    {{ITEM_STORM_CORE,1,25},{ITEM_MONSTER_HIDE,3,45},{ITEM_DRAGON_SCALE,1,15}},
    FALSE, "Breathes lightning and shakes the mountainside with its footsteps."
},
[MON_FROST_WOLF] = {
    "Frost Wolf", 66, 45, ELEM_WATER,
    115,  0, 28, 12,  0, 10, 13,
    {N,W,R,N,N,N,N},
    60, 17, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_MOUNTAINS, 13, 19,
    {{ITEM_MONSTER_HIDE,2,45},{ITEM_ICE_ESSENCE,1,25},{ITEM_NONE,0,0}},
    FALSE, "Its breath freezes prey solid. Hunts exclusively in packs."
},
[MON_MOUNTAIN_ORC] = {
    "Mountain Orc", 67, 46, ELEM_NONE,
    170,  0, 32, 18,  0, 12,  8,
    {N,N,N,N,N,N,N},
    85, 24, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_MOUNTAINS, 14, 20,
    {{ITEM_IRON_ORE,3,40},{ITEM_STEEL_INGOT,1,15},{ITEM_BONE_FRAGMENT,2,25}},
    FALSE, "Hardened by life at high altitude. Wears crude iron plate."
},
[MON_ORC_CHAMPION] = {
    "Orc Champion", 68, 47, ELEM_NONE,
    240,  0, 38, 22,  0, 14,  9,
    120, 32, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_MOUNTAINS, 16, 22,
    {{ITEM_STEEL_INGOT,2,35},{ITEM_IRON_ORE,4,45},{ITEM_ANCIENT_RUNE,1,8}},
    FALSE, "A warchief who leads the mountain orc clans in battle."
},
[MON_PEAK_WYVERN] = {
    "Peak Wyvern", 69, 48, ELEM_WIND,
    280,  0, 40, 20,  0, 18, 14,
    140, 38, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_MOUNTAINS, 17, 23,
    {{ITEM_DRAGON_SCALE,3,45},{ITEM_WIND_ESSENCE,3,35},{ITEM_STORM_CORE,2,20}},
    FALSE, "The apex predator of Ironpeak. Its wingspan darkens the sun."
},

/* ---- Ancient Ruins (70-83) ------------------------------------- */
[MON_SKELETON] = {
    "Skeleton", 70, 49, ELEM_DARK,
    90,  0, 22, 14,  0,  8,  7,
    {N,N,N,N,N,W,R},
    40, 12, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_RUINS, 14, 20,
    {{ITEM_BONE_FRAGMENT,2,60},{ITEM_IRON_ORE,1,15},{ITEM_NONE,0,0}},
    FALSE, "An animated skeleton standing guard over the ancient ruins."
},
[MON_ZOMBIE] = {
    "Zombie", 71, 50, ELEM_DARK,
    130,  0, 20, 16,  0,  6,  3,
    {N,N,N,N,N,W,R},
    45, 13, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_AGGRESSIVE,
    REGION_RUINS, 14, 20,
    {{ITEM_BONE_FRAGMENT,2,55},{ITEM_DARK_ESSENCE,1,20},{ITEM_NONE,0,0}},
    FALSE, "A shambling corpse. Its bite spreads infection."
},
[MON_GHOST] = {
    "Ghost", 72, 51, ELEM_DARK,
    80,  40, 18,  6, 20, 20, 12,
    {N,N,N,N,N,W,R},
    50, 15, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_RUINS, 14, 20,
    {{ITEM_DARK_ESSENCE,1,45},{ITEM_MANA_SHARD,1,25},{ITEM_NONE,0,0}},
    FALSE, "Physical attacks barely harm it. Use magic or light."
},
[MON_WRAITH] = {
    "Wraith", 73, 52, ELEM_DARK,
    110,  30, 26,  8, 22, 18, 14,
    {N,N,N,N,N,W,R},
    65, 18, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_RUINS, 16, 22,
    {{ITEM_DARK_ESSENCE,2,45},{ITEM_SHADOW_CLOTH,1,20},{ITEM_NONE,0,0}},
    FALSE, "Drains life with each touch. Seeks warmth to sustain itself."
},
[MON_MUMMY] = {
    "Mummy", 74, 53, ELEM_DARK,
    160,  0, 24, 20,  0, 10,  4,
    {N,W,N,N,N,W,R},
    70, 20, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_RUINS, 15, 21,
    {{ITEM_BONE_FRAGMENT,3,50},{ITEM_ANCIENT_RUNE,1,15},{ITEM_SHADOW_CLOTH,1,20}},
    FALSE, "An embalmed ancient warrior cursed to guard its pharaoh's tomb."
},
[MON_ANCIENT_GOLEM] = {
    "Ancient Golem", 75, 54, ELEM_NONE,
    280,  0, 30, 28,  0, 16,  3,
    {N,N,N,N,N,N,N},
    100, 28, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_RUINS, 16, 22,
    {{ITEM_STEEL_INGOT,2,35},{ITEM_ANCIENT_RUNE,2,25},{ITEM_IRON_ORE,4,40}},
    FALSE, "A war machine built by a lost civilization. Still operational."
},
[MON_RUNE_GUARDIAN] = {
    "Rune Guardian", 76, 55, ELEM_LIGHT,
    200,  60, 28, 22, 24, 20,  7,
    110, 30, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_RUINS, 16, 22,
    {{ITEM_ANCIENT_RUNE,3,45},{ITEM_LIGHT_ESSENCE,2,30},{ITEM_MANA_SHARD,2,25}},
    FALSE, "A construct inscribed with ancient sealing runes. Holy and terrible."
},
[MON_CURSE_WISP] = {
    "Curse Wisp", 77, 56, ELEM_DARK,
    70,  50, 14,  6, 24, 20, 13,
    55, 16, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_RUINS, 15, 21,
    {{ITEM_DARK_ESSENCE,2,50},{ITEM_MANA_SHARD,1,30},{ITEM_NONE,0,0}},
    FALSE, "A wisp born from a curse. Inflicts random status ailments."
},
[MON_SHADOW_KNIGHT] = {
    "Shadow Knight", 78, 57, ELEM_DARK,
    220,  0, 36, 24,  0, 16, 10,
    120, 33, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_RUINS, 17, 23,
    {{ITEM_DARK_ESSENCE,3,40},{ITEM_SHADOW_CLOTH,2,25},{ITEM_STEEL_INGOT,1,15}},
    FALSE, "A knightly spirit bound to protect a long-fallen kingdom."
},
[MON_LICH_APPRENTICE] = {
    "Lich Apprentice", 79, 58, ELEM_DARK,
    120,  80, 16, 10, 32, 22, 11,
    100, 28, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 4, AI_REACT,
    REGION_RUINS, 17, 23,
    {{ITEM_DARK_ESSENCE,3,45},{ITEM_ARCANE_DUST,3,35},{ITEM_ANCIENT_RUNE,1,15}},
    FALSE, "A student of necromancy who achieved partial undeath."
},
[MON_BONE_DRAGON] = {
    "Bone Dragon", 80, 59, ELEM_DARK,
    350,  0, 40, 28,  0, 20,  8,
    160, 42, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_RUINS, 19, 25,
    {{ITEM_DRAGON_FANG,2,30},{ITEM_BONE_FRAGMENT,5,60},{ITEM_DARK_ESSENCE,3,35}},
    FALSE, "A dragon long dead, reanimated by the ruins' lingering curse."
},
[MON_VOID_SHADE] = {
    "Void Shade", 81, 60, ELEM_DARK,
    140,  40, 28, 12, 26, 22, 15,
    95, 26, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 4, AI_REACT,
    REGION_RUINS, 18, 24,
    {{ITEM_VOID_CRYSTAL,1,20},{ITEM_DARK_ESSENCE,3,40},{ITEM_SHADOW_ESSENCE,1,15}},
    FALSE, "A shade from the void that slipped through a crack in the ruins."
},
[MON_RUIN_COLOSSUS] = {
    "Ruin Colossus", 82, 61, ELEM_NONE,
    400,  0, 42, 32,  0, 18,  2,
    170, 45, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_RUINS, 19, 25,
    {{ITEM_STEEL_INGOT,4,40},{ITEM_ANCIENT_RUNE,3,30},{ITEM_IRON_ORE,6,50}},
    FALSE, "A colossal war machine activated by intruders' footsteps."
},
[MON_DARK_PRIEST] = {
    "Dark Priest", 83, 62, ELEM_DARK,
    130,  90, 18, 12, 34, 24, 10,
    135, 36, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 4, AI_REACT,
    REGION_RUINS, 18, 24,
    {{ITEM_DARK_ESSENCE,4,45},{ITEM_ARCANE_DUST,3,35},{ITEM_ANCIENT_RUNE,2,20}},
    FALSE, "Conducts rituals to awaken the ruins' darkest powers."
},

/* ---- Ignarath Volcano (84-96) ---------------------------------- */
[MON_FIRE_LIZARD] = {
    "Fire Lizard", 84, 63, ELEM_FIRE,
    110,  0, 24, 14,  0, 10, 11,
    {N,A,W,N,N,N,N},
    50, 15, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_VOLCANO, 18, 24,
    {{ITEM_FIRE_ESSENCE,2,45},{ITEM_MONSTER_HIDE,2,35},{ITEM_NONE,0,0}},
    FALSE, "A lizard that bathes in lava. Its skin burns on contact."
},
[MON_LAVA_SLIME] = {
    "Lava Slime", 85, 64, ELEM_FIRE,
    95,  0, 14, 10,  0,  8,  3,
    {N,A,W,N,N,N,N},
    40, 12, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_VOLCANO, 17, 23,
    {{ITEM_FIRE_ESSENCE,1,50},{ITEM_COAL,2,40},{ITEM_NONE,0,0}},
    FALSE, "Molten slime that sets the floor ablaze wherever it moves."
},
[MON_ASH_GOLEM] = {
    "Ash Golem", 86, 65, ELEM_FIRE,
    230,  0, 30, 22,  0, 14,  3,
    90, 25, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_VOLCANO, 18, 24,
    {{ITEM_FIRE_ESSENCE,3,45},{ITEM_COAL,4,50},{ITEM_EARTH_ESSENCE,1,15}},
    FALSE, "A golem formed from volcanic ash and heat-fused minerals."
},
[MON_FIRE_IMP] = {
    "Fire Imp", 87, 66, ELEM_FIRE,
    70,  30, 18,  8, 22, 12, 15,
    45, 13, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_VOLCANO, 17, 23,
    {{ITEM_FIRE_ESSENCE,2,45},{ITEM_ARCANE_DUST,2,30},{ITEM_NONE,0,0}},
    FALSE, "A mischievous demon that hurls fireballs and cackles in glee."
},
[MON_MAGMA_CRAB] = {
    "Magma Crab", 88, 67, ELEM_FIRE,
    160,  0, 20, 28,  0, 14,  2,
    65, 18, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_VOLCANO, 18, 24,
    {{ITEM_FIRE_ESSENCE,2,40},{ITEM_IRON_ORE,3,35},{ITEM_STEEL_INGOT,1,10}},
    FALSE, "Shell of solidified magma. Scalding to the touch."
},
[MON_FLAME_BAT] = {
    "Flame Bat", 89, 68, ELEM_FIRE,
    80,  0, 22, 10,  0, 10, 16,
    42, 12, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_AGGRESSIVE,
    REGION_VOLCANO, 17, 23,
    {{ITEM_FIRE_ESSENCE,1,40},{ITEM_FEATHER,2,30},{ITEM_NONE,0,0}},
    FALSE, "Coated in flaming oil that ignites anything it brushes against."
},
[MON_FIRE_ELEMENTAL] = {
    "Fire Elemental", 90, 69, ELEM_FIRE,
    150,  60, 28, 14, 34, 18, 13,
    90, 25, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_VOLCANO, 19, 25,
    {{ITEM_FIRE_ESSENCE,4,60},{ITEM_ETERNAL_FLAME,1,10},{ITEM_MANA_SHARD,2,25}},
    FALSE, "A being of pure fire. Ice attacks are its only weakness."
},
[MON_VOLCANO_TROLL] = {
    "Volcano Troll", 91, 70, ELEM_FIRE,
    300,  0, 36, 24,  0, 16,  4,
    115, 31, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_VOLCANO, 20, 26,
    {{ITEM_FIRE_ESSENCE,3,45},{ITEM_IRON_ORE,5,50},{ITEM_MONSTER_HIDE,3,30}},
    FALSE, "Bathed in lava from birth. Regenerates in the heat."
},
[MON_EMBER_SNAKE] = {
    "Ember Snake", 92, 71, ELEM_FIRE,
    120,  0, 26, 14,  0, 12, 14,
    60, 17, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_VOLCANO, 19, 25,
    {{ITEM_SNAKE_VENOM,2,35},{ITEM_FIRE_ESSENCE,2,40},{ITEM_DRAGON_SCALE,1,10}},
    FALSE, "Hides in lava flows and strikes with burning precision."
},
[MON_HEAT_WRAITH] = {
    "Heat Wraith", 93, 72, ELEM_FIRE,
    110,  40, 24, 10, 26, 18, 14,
    70, 20, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 4, AI_REACT,
    REGION_VOLCANO, 19, 25,
    {{ITEM_FIRE_ESSENCE,2,45},{ITEM_DARK_ESSENCE,1,20},{ITEM_ETERNAL_FLAME,1,8}},
    FALSE, "The spirit of one consumed by volcanic eruption. Burns forever."
},
[MON_MOLTEN_GOLEM] = {
    "Molten Golem", 94, 73, ELEM_FIRE,
    380,  0, 40, 30,  0, 18,  2,
    150, 40, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_VOLCANO, 21, 27,
    {{ITEM_FIRE_ESSENCE,5,65},{ITEM_STEEL_INGOT,3,35},{ITEM_ETERNAL_FLAME,1,12}},
    FALSE, "A colossal golem of living magma. Leaves a trail of fire."
},
[MON_PYROMANCER] = {
    "Pyromancer", 95, 74, ELEM_FIRE,
    140,  100, 20, 14, 40, 22, 11,
    130, 35, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 4, AI_REACT,
    REGION_VOLCANO, 21, 27,
    {{ITEM_FIRE_ESSENCE,4,50},{ITEM_ARCANE_DUST,4,40},{ITEM_ETERNAL_FLAME,1,15}},
    FALSE, "A mage who sought power in the volcano and found it — at a cost."
},
[MON_INFERNO_WYRM] = {
    "Inferno Wyrm", 96, 75, ELEM_FIRE,
    450,  0, 44, 26,  0, 22, 10,
    180, 48, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_VOLCANO, 22, 28,
    {{ITEM_DRAGON_SCALE,4,50},{ITEM_ETERNAL_FLAME,2,20},{ITEM_DRAGON_FANG,2,25}},
    FALSE, "A wingless fire dragon that burrows through lava like water."
},

/* ---- Frostpeak Snowlands (97-109) ------------------------------ */
[MON_SNOW_WOLF] = {
    "Snow Wolf", 97, 76, ELEM_WATER,
    130,  0, 28, 12,  0, 10, 15,
    60, 17, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_SNOWLANDS, 20, 26,
    {{ITEM_MONSTER_HIDE,3,50},{ITEM_ICE_ESSENCE,1,25},{ITEM_NONE,0,0}},
    FALSE, "An apex predator of the frozen tundra. Travels in packs."
},
[MON_FROST_BAT] = {
    "Frost Bat", 98, 77, ELEM_WATER,
    85,  0, 22, 10,  0, 10, 16,
    42, 13, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_SNOWLANDS, 19, 25,
    {{ITEM_ICE_ESSENCE,1,40},{ITEM_FEATHER,2,30},{ITEM_NONE,0,0}},
    FALSE, "A bat whose ultrasonic screams create shockwaves of frost."
},
[MON_ICE_TROLL] = {
    "Ice Troll", 99, 78, ELEM_WATER,
    290,  0, 34, 22,  0, 14,  4,
    105, 29, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_SNOWLANDS, 20, 26,
    {{ITEM_ICE_ESSENCE,3,45},{ITEM_BONE_FRAGMENT,3,35},{ITEM_MONSTER_HIDE,3,30}},
    FALSE, "Larger and meaner than a mountain troll. Regenerates in cold."
},
[MON_SNOW_GIANT] = {
    "Snow Giant", 100, 79, ELEM_WATER,
    380,  0, 40, 26,  0, 16,  3,
    140, 38, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_SNOWLANDS, 21, 27,
    {{ITEM_ICE_ESSENCE,4,50},{ITEM_STEEL_INGOT,2,25},{ITEM_TITAN_HEART,1,5}},
    FALSE, "A giant that has lived in the ice for centuries. Hurls glaciers."
},
[MON_BLIZZARD_WISP] = {
    "Blizzard Wisp", 101, 80, ELEM_WATER,
    90,  60, 20, 10, 28, 20, 14,
    70, 20, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_SNOWLANDS, 20, 26,
    {{ITEM_ICE_ESSENCE,2,50},{ITEM_MANA_SHARD,2,30},{ITEM_FROZEN_TEAR,1,15}},
    FALSE, "A spirit born from a deadly blizzard. Inflicts freeze status."
},
[MON_FROST_GOLEM] = {
    "Frost Golem", 102, 81, ELEM_WATER,
    300,  0, 32, 26,  0, 16,  2,
    110, 30, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_SNOWLANDS, 21, 27,
    {{ITEM_ICE_ESSENCE,4,55},{ITEM_FROZEN_TEAR,2,25},{ITEM_STEEL_INGOT,2,20}},
    FALSE, "A golem sculpted from a single block of ancient glacier ice."
},
[MON_WENDIGO] = {
    "Wendigo", 103, 82, ELEM_DARK,
    250,  20, 38, 18, 12, 16, 13,
    130, 35, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 4, AI_REACT,
    REGION_SNOWLANDS, 21, 27,
    {{ITEM_DARK_ESSENCE,3,40},{ITEM_ICE_ESSENCE,2,30},{ITEM_MONSTER_HIDE,4,45}},
    FALSE, "A spirit of cold and hunger. Cannibalistic origins fuel its curse."
},
[MON_ICE_SERPENT] = {
    "Ice Serpent", 104, 83, ELEM_WATER,
    180,  0, 32, 16,  0, 14, 12,
    85, 24, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_SNOWLANDS, 20, 26,
    {{ITEM_ICE_ESSENCE,3,45},{ITEM_SNAKE_VENOM,2,25},{ITEM_FROZEN_TEAR,1,15}},
    FALSE, "A serpent whose scales are frozen solid. Its venom flash-freezes."
},
[MON_POLAR_BEAR] = {
    "Polar Bear", 105, 84, ELEM_WATER,
    200,  0, 34, 18,  0, 12,  7,
    80, 22, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_SNOWLANDS, 19, 25,
    {{ITEM_MONSTER_HIDE,4,55},{ITEM_BONE_FRAGMENT,2,25},{ITEM_ICE_ESSENCE,1,15}},
    FALSE, "Enormous and territorial. Can smell prey through a blizzard."
},
[MON_CRYSTAL_IMP] = {
    "Crystal Imp", 106, 85, ELEM_WATER,
    80,  40, 16, 12, 24, 18, 15,
    55, 16, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_SNOWLANDS, 20, 26,
    {{ITEM_ICE_ESSENCE,2,45},{ITEM_CRYSTAL_SHARD,2,35},{ITEM_ARCANE_DUST,1,20}},
    FALSE, "An imp made of ice crystals that shatters and reforms."
},
[MON_GLACIER_CRAB] = {
    "Glacier Crab", 107, 86, ELEM_WATER,
    220,  0, 24, 30,  0, 16,  2,
    90, 25, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_SNOWLANDS, 21, 27,
    {{ITEM_ICE_ESSENCE,3,50},{ITEM_IRON_ORE,2,25},{ITEM_FROZEN_TEAR,1,15}},
    FALSE, "A crab encased in glacial ice thick enough to stop a siege weapon."
},
[MON_FROST_DRAGON] = {
    "Frost Dragon", 108, 87, ELEM_WATER,
    500,  0, 46, 28,  0, 24, 11,
    200, 55, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_SNOWLANDS, 23, 29,
    {{ITEM_DRAGON_SCALE,5,55},{ITEM_FROZEN_TEAR,3,30},{ITEM_DRAGON_FANG,2,20}},
    FALSE, "Rules the Frostpeak from an icy throne. Breath weapon freezes solid."
},
[MON_SNOW_WITCH] = {
    "Snow Witch", 109, 88, ELEM_WATER,
    160,  110, 22, 16, 44, 28, 12,
    160, 44, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 4, AI_REACT,
    REGION_SNOWLANDS, 22, 28,
    {{ITEM_ICE_ESSENCE,5,55},{ITEM_ARCANE_DUST,4,40},{ITEM_FROZEN_TEAR,2,20}},
    FALSE, "An immortal sorceress who chose cold eternity over a warm death."
},

/* ---- Sunken Isles / Ocean (110-121) ---------------------------- */
[MON_SEA_SLIME] = {
    "Sea Slime", 110, 89, ELEM_WATER,
    90,  0, 12,  8,  0,  8,  5,
    {N,N,A,N,N,N,N},
    35, 10, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_OCEAN, 20, 26,
    {{ITEM_PURE_WATER,2,50},{ITEM_MONSTER_HIDE,1,20},{ITEM_NONE,0,0}},
    FALSE, "A translucent slime that floats on ocean currents."
},
[MON_SHARK] = {
    "Shark", 111, 90, ELEM_WATER,
    180,  0, 36, 16,  0, 12, 14,
    80, 22, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_OCEAN, 20, 26,
    {{ITEM_MONSTER_HIDE,3,50},{ITEM_BONE_FRAGMENT,2,30},{ITEM_NONE,0,0}},
    FALSE, "A perfect predator. Drawn by blood and motion."
},
[MON_JELLYFISH] = {
    "Jellyfish", 112, 91, ELEM_WATER,
    70,  0, 14,  6,  0, 10,  7,
    30, 9, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_OCEAN, 19, 25,
    {{ITEM_PURE_WATER,2,45},{ITEM_SNAKE_VENOM,1,25},{ITEM_NONE,0,0}},
    FALSE, "Beautiful and deadly. Its sting causes paralysis."
},
[MON_WATER_ELEMENTAL] = {
    "Water Elemental", 113, 92, ELEM_WATER,
    160,  60, 26, 14, 30, 20, 11,
    90, 25, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_OCEAN, 21, 27,
    {{ITEM_PURE_WATER,3,55},{ITEM_MANA_SHARD,2,30},{ITEM_ICE_ESSENCE,1,15}},
    FALSE, "A living wave. Nearly immune to physical attack."
},
[MON_SEA_SERPENT] = {
    "Sea Serpent", 114, 93, ELEM_WATER,
    280,  0, 40, 20,  0, 18, 12,
    120, 33, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_OCEAN, 21, 27,
    {{ITEM_DRAGON_SCALE,3,40},{ITEM_SNAKE_VENOM,3,35},{ITEM_MONSTER_HIDE,3,30}},
    FALSE, "A legendary sea monster capable of sinking entire ships."
},
[MON_MERROW] = {
    "Merrow", 115, 94, ELEM_WATER,
    140,  60, 28, 14, 24, 18, 12,
    75, 21, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 4, AI_REACT,
    REGION_OCEAN, 20, 26,
    {{ITEM_PURE_WATER,2,40},{ITEM_MANA_SHARD,2,25},{ITEM_ARCANE_DUST,1,15}},
    FALSE, "A predatory fish-person that lures sailors with haunting calls."
},
[MON_DEEP_ANGLER] = {
    "Deep Angler", 116, 95, ELEM_DARK,
    200,  20, 32, 18, 14, 14,  8,
    100, 28, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_AGGRESSIVE,
    REGION_OCEAN, 21, 27,
    {{ITEM_DARK_ESSENCE,2,35},{ITEM_MONSTER_HIDE,3,45},{ITEM_NONE,0,0}},
    FALSE, "A horrifying deep-sea predator with a bioluminescent lure."
},
[MON_TIDE_CRAB] = {
    "Tide Crab", 117, 96, ELEM_WATER,
    190,  0, 26, 28,  0, 16,  3,
    75, 21, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_OCEAN, 20, 26,
    {{ITEM_IRON_ORE,3,40},{ITEM_MONSTER_HIDE,2,30},{ITEM_PURE_WATER,2,35}},
    FALSE, "A crab the size of a house that emerges at high tide to hunt."
},
[MON_CORAL_GOLEM] = {
    "Coral Golem", 118, 97, ELEM_WATER,
    320,  0, 32, 30,  0, 18,  2,
    120, 33, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_OCEAN, 22, 28,
    {{ITEM_CRYSTAL_SHARD,4,50},{ITEM_EARTH_ESSENCE,2,25},{ITEM_PURE_WATER,3,40}},
    FALSE, "A living reef animated by ancient sea magic."
},
[MON_SIREN] = {
    "Siren", 119, 98, ELEM_WATER,
    170,  90, 24, 14, 40, 28, 14,
    140, 38, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 4, AI_REACT,
    REGION_OCEAN, 22, 28,
    {{ITEM_MANA_SHARD,3,40},{ITEM_ARCANE_DUST,3,35},{ITEM_MOONSTONE,1,10}},
    FALSE, "Its song charms party members, turning them against each other."
},
[MON_LEVIATHAN_PAWN] = {
    "Leviathan Pawn", 120, 99, ELEM_WATER,
    350,  0, 40, 24,  0, 20, 10,
    150, 40, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_OCEAN, 23, 29,
    {{ITEM_DRAGON_SCALE,4,45},{ITEM_PURE_WATER,5,55},{ITEM_MONSTER_HIDE,4,40}},
    FALSE, "A fragment of the Leviathan given independent life."
},
[MON_OCEAN_DRAGON] = {
    "Ocean Dragon", 121, 100, ELEM_WATER,
    500,  0, 48, 28,  0, 26, 11,
    200, 55, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_OCEAN, 24, 30,
    {{ITEM_DRAGON_SCALE,6,55},{ITEM_DRAGON_FANG,3,25},{ITEM_CHAOS_ORB,1,5}},
    FALSE, "Master of the sunken isles. Can summon tidal waves."
},

/* ---- Shadow Realm (122-134) ------------------------------------ */
[MON_SHADOW_SLIME] = {
    "Shadow Slime", 122, 101, ELEM_DARK,
    120,  0, 16, 10,  0, 10,  5,
    {N,W,N,N,N,W,A},
    55, 16, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_SHADOWREALM, 26, 32,
    {{ITEM_DARK_ESSENCE,2,50},{ITEM_SHADOW_ESSENCE,1,20},{ITEM_NONE,0,0}},
    FALSE, "A slime formed from condensed shadow energy."
},
[MON_NIGHTMARE] = {
    "Nightmare", 123, 102, ELEM_DARK,
    210,  30, 38, 18, 14, 18, 16,
    130, 35, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 4, AI_REACT,
    REGION_SHADOWREALM, 27, 33,
    {{ITEM_DARK_ESSENCE,4,50},{ITEM_SHADOW_ESSENCE,2,30},{ITEM_VOID_CRYSTAL,1,10}},
    FALSE, "A demonic horse from the void. Its hooves never touch the ground."
},
[MON_VOID_SPRITE] = {
    "Void Sprite", 124, 103, ELEM_DARK,
    90,  60, 18, 10, 30, 24, 17,
    80, 22, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_SHADOWREALM, 26, 32,
    {{ITEM_VOID_CRYSTAL,1,30},{ITEM_DARK_ESSENCE,2,40},{ITEM_MANA_SHARD,2,25}},
    FALSE, "A sprite of void energy. Drains MP from its targets."
},
[MON_DARK_ELF] = {
    "Dark Elf", 125, 104, ELEM_DARK,
    170,  50, 36, 18, 24, 20, 16,
    110, 30, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_SHADOWREALM, 27, 33,
    {{ITEM_DARK_ESSENCE,3,45},{ITEM_SHADOW_CLOTH,2,30},{ITEM_ARCANE_DUST,2,25}},
    FALSE, "An elf who sought power in darkness. Lethal and arrogant."
},
[MON_SHADE] = {
    "Shade", 126, 105, ELEM_DARK,
    130,  40, 26, 12, 24, 22, 15,
    85, 24, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_SHADOWREALM, 26, 32,
    {{ITEM_DARK_ESSENCE,3,50},{ITEM_SHADOW_ESSENCE,1,20},{ITEM_NONE,0,0}},
    FALSE, "A being of pure shadow that cannot be harmed by physical blows."
},
[MON_SHADOW_BEAST] = {
    "Shadow Beast", 127, 106, ELEM_DARK,
    280,  0, 42, 20, 10, 18, 14,
    140, 38, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_SHADOWREALM, 27, 33,
    {{ITEM_SHADOW_ESSENCE,3,45},{ITEM_DARK_ESSENCE,4,50},{ITEM_MONSTER_HIDE,3,30}},
    FALSE, "A massive quadruped shaped from solidified darkness and rage."
},
[MON_DARK_KNIGHT_SR] = {
    "Dark Knight", 128, 107, ELEM_DARK,
    260,  0, 44, 28,  0, 20, 11,
    150, 40, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_SHADOWREALM, 28, 34,
    {{ITEM_SHADOW_CLOTH,3,40},{ITEM_DARK_ESSENCE,4,45},{ITEM_STEEL_INGOT,2,20}},
    FALSE, "An elite knight of the void. Its black blade drains life."
},
[MON_SOUL_EATER] = {
    "Soul Eater", 129, 108, ELEM_DARK,
    200,  60, 36, 16, 30, 24, 14,
    160, 43, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 4, AI_REACT,
    REGION_SHADOWREALM, 28, 34,
    {{ITEM_VOID_CRYSTAL,2,30},{ITEM_SHADOW_ESSENCE,3,40},{ITEM_DARK_ESSENCE,4,45}},
    FALSE, "Consumes the souls of the fallen to grow stronger."
},
[MON_VOID_KNIGHT] = {
    "Void Knight", 130, 109, ELEM_DARK,
    310,  0, 46, 30,  0, 22, 12,
    175, 47, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_SHADOWREALM, 29, 35,
    {{ITEM_VOIDSTONE,1,15},{ITEM_SHADOW_ESSENCE,4,45},{ITEM_DARK_ESSENCE,5,50}},
    FALSE, "A champion forged from void matter. Immune to darkness."
},
[MON_PHANTOM_MAGE] = {
    "Phantom Mage", 131, 110, ELEM_DARK,
    180,  120, 22, 14, 50, 30, 14,
    180, 48, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 4, AI_REACT,
    REGION_SHADOWREALM, 29, 35,
    {{ITEM_VOID_CRYSTAL,2,30},{ITEM_ARCANE_DUST,5,50},{ITEM_SHADOW_ESSENCE,3,35}},
    FALSE, "A mage who transcended death to continue its dark studies."
},
[MON_ABYSSAL_WRAITH] = {
    "Abyssal Wraith", 132, 111, ELEM_DARK,
    230,  60, 40, 16, 32, 28, 16,
    190, 51, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_SHADOWREALM, 29, 35,
    {{ITEM_VOIDSTONE,1,12},{ITEM_SHADOW_ESSENCE,4,45},{ITEM_CHAOS_ORB,1,5}},
    FALSE, "A wraith so ancient it has forgotten what it once was."
},
[MON_DARK_COLOSSUS] = {
    "Dark Colossus", 133, 112, ELEM_DARK,
    500,  0, 50, 34,  0, 26,  4,
    220, 58, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_SHADOWREALM, 30, 36,
    {{ITEM_SHADOW_ESSENCE,6,60},{ITEM_VOIDSTONE,2,20},{ITEM_CHAOS_ORB,1,8}},
    FALSE, "A titan sculpted from void matter. Its footsteps silence reality."
},
[MON_VOID_WALKER_MON] = {
    "Void Walker", 134, 113, ELEM_DARK,
    280,  80, 42, 20, 38, 30, 17,
    210, 56, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 4, AI_REACT,
    REGION_SHADOWREALM, 30, 36,
    {{ITEM_VOIDSTONE,2,25},{ITEM_VOID_CRYSTAL,3,35},{ITEM_SHADOW_ESSENCE,5,50}},
    FALSE, "Moves between dimensions. May vanish mid-battle and reappear."
},

/* ---- Aetheria Final Area (135-144) ----------------------------- */
[MON_AETHER_WISP] = {
    "Aether Wisp", 135, 114, ELEM_LIGHT,
    140,  80, 24, 14, 36, 30, 16,
    120, 33, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 0, AI_RANDOM,
    REGION_AETHERIA, 32, 38,
    {{ITEM_LIGHT_ESSENCE,3,55},{ITEM_MANA_SHARD,3,35},{ITEM_ASTRAL_DUST,1,15}},
    FALSE, "A wisp of pure aetheric energy. Blinds with concentrated light."
},
[MON_CRYSTAL_ANGEL] = {
    "Crystal Angel", 136, 115, ELEM_LIGHT,
    220,  80, 36, 22, 40, 32, 14,
    180, 48, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 2, AI_HEALER,
    REGION_AETHERIA, 33, 39,
    {{ITEM_LIGHT_ESSENCE,4,50},{ITEM_HOLY_FRAGMENT,2,25},{ITEM_ASTRAL_DUST,2,20}},
    FALSE, "A corrupted celestial guardian. Still heals its allies."
},
[MON_DIVINE_GOLEM] = {
    "Divine Golem", 137, 116, ELEM_LIGHT,
    450,  0, 44, 36,  0, 28,  3,
    210, 56, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_AETHERIA, 34, 40,
    {{ITEM_HOLY_FRAGMENT,3,40},{ITEM_LIGHT_ESSENCE,5,55},{ITEM_ASTRAL_DUST,3,25}},
    FALSE, "A sacred construct guarding the gates of Aetheria."
},
[MON_AETHER_KNIGHT] = {
    "Aether Knight", 138, 117, ELEM_LIGHT,
    320,  0, 50, 32,  0, 26, 13,
    220, 58, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 1, AI_AGGRESSIVE,
    REGION_AETHERIA, 34, 40,
    {{ITEM_LIGHT_ESSENCE,5,55},{ITEM_HOLY_FRAGMENT,2,25},{ITEM_STEEL_INGOT,4,30}},
    FALSE, "An elite warrior of heaven's army. Its lance strikes like lightning."
},
[MON_CORRUPTED_ANGEL] = {
    "Corrupted Angel", 139, 118, ELEM_DARK,
    280,  90, 44, 26, 44, 32, 14,
    230, 62, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 4, AI_REACT,
    REGION_AETHERIA, 35, 41,
    {{ITEM_DARK_ESSENCE,5,45},{ITEM_SHADOW_ESSENCE,4,35},{ITEM_VOID_CRYSTAL,2,15}},
    FALSE, "An angel torn from grace by the Void Lord's corruption."
},
[MON_VOID_TITAN] = {
    "Void Titan", 140, 119, ELEM_DARK,
    600,  0, 54, 38,  0, 30,  4,
    280, 75, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_AETHERIA, 36, 42,
    {{ITEM_VOIDSTONE,4,40},{ITEM_CHAOS_ORB,2,15},{ITEM_SHADOW_ESSENCE,6,55}},
    FALSE, "A titan-sized manifestation of the void. Shakes Aetheria itself."
},
[MON_AETHER_DRAGON] = {
    "Aether Dragon", 141, 120, ELEM_LIGHT,
    600,  0, 54, 32,  0, 32, 12,
    290, 78, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 3, AI_PATTERN,
    REGION_AETHERIA, 36, 42,
    {{ITEM_DRAGON_SCALE,8,60},{ITEM_LIGHT_ESSENCE,6,55},{ITEM_ASTRAL_CORE,1,5}},
    FALSE, "A divine dragon whose breath purifies and destroys in equal measure."
},
[MON_FINAL_SHADE] = {
    "Final Shade", 142, 121, ELEM_DARK,
    350,  100, 48, 24, 50, 36, 18,
    260, 70, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 4, AI_REACT,
    REGION_AETHERIA, 36, 42,
    {{ITEM_VOID_CRYSTAL,5,45},{ITEM_SHADOW_ESSENCE,6,55},{ITEM_CHAOS_ORB,2,10}},
    FALSE, "The last shadow. The darkness that remains when all light is gone."
},
[MON_HERALD_OF_VOID] = {
    "Herald of Void", 143, 122, ELEM_DARK,
    400,  120, 52, 28, 54, 38, 16,
    300, 80, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 5, AI_BOSS_PHASE,
    REGION_AETHERIA, 37, 43,
    {{ITEM_VOIDSTONE,5,50},{ITEM_CHAOS_ORB,2,12},{ITEM_ASTRAL_DUST,5,45}},
    FALSE, "The Void Lord's voice and hand. Announces the end of all things."
},
[MON_AETHER_COLOSSUS] = {
    "Aether Colossus", 144, 123, ELEM_LIGHT,
    700,  0, 56, 42,  0, 34,  5,
    320, 85, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 5, AI_BOSS_PHASE,
    REGION_AETHERIA, 38, 44,
    {{ITEM_ASTRAL_CORE,1,10},{ITEM_LIGHT_ESSENCE,8,65},{ITEM_HOLY_FRAGMENT,5,50}},
    FALSE, "The ultimate guardian construct. Activates only when Aetheria is threatened."
},

/* ---- Bosses (145-149) ------------------------------------------ */
[MON_BOSS_SERPENTIS] = {
    "Great Serpentis", 124, 0, ELEM_WATER,
    1200,  0, 45, 28,  0, 22,  8,
    500, 120, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 5, AI_BOSS_PHASE,
    REGION_BOSS, 10, 10,
    {{ITEM_DRAGON_SCALE,5,100},{ITEM_SNAKE_VENOM,5,100},{ITEM_CHAOS_ORB,1,50}},
    TRUE, "The serpent-god of the ancient ruins. Said to devour the moon."
},
[MON_BOSS_ELDER_TREANT] = {
    "Elder Treant Lord", 125, 1, ELEM_EARTH,
    1500,  60, 48, 36, 20, 26,  3,
    600, 140, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 5, AI_BOSS_PHASE,
    REGION_BOSS, 15, 15,
    {{ITEM_THORN_WOOD,8,100},{ITEM_EARTH_ESSENCE,8,100},{ITEM_TITAN_HEART,1,50}},
    TRUE, "The forest's will made flesh. Cannot be harmed except by fire."
},
[MON_BOSS_LICH] = {
    "Lich King Mordael", 126, 2, ELEM_DARK,
    1800,  200, 50, 32, 70, 40, 12,
    750, 180, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 5, AI_BOSS_PHASE,
    REGION_BOSS, 22, 22,
    {{ITEM_ANCIENT_RUNE,8,100},{ITEM_DARK_ESSENCE,8,100},{ITEM_ARCANE_CODEX,1,50}},
    TRUE, "An immortal necromancer-king who has cheated death for three centuries."
},
[MON_BOSS_INFERNO] = {
    "Inferno Dragon", 127, 3, ELEM_FIRE,
    2000,  0, 60, 38,  0, 30, 10,
    900, 220, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 5, AI_BOSS_PHASE,
    REGION_BOSS, 28, 28,
    {{ITEM_DRAGON_SCALE,10,100},{ITEM_ETERNAL_FLAME,3,100},{ITEM_BLADE_HEART,1,50}},
    TRUE, "A dragon so ancient its bones burn white. Lord of the volcano."
},
[MON_BOSS_VOID_LORD] = {
    "Lord of the Void", 128, 4, ELEM_DARK,
    3000,  300, 70, 44, 80, 50, 14,
    1500, 400, {0xFF,0,0, 0xFF,0,0, 0xFF,0,0}, 5, AI_BOSS_PHASE,
    REGION_BOSS, 40, 40,
    {{ITEM_VOIDSTONE,5,100},{ITEM_ASTRAL_CORE,3,100},{ITEM_ESSENCE_INFINITY,1,100}},
    TRUE, "The source of all corruption. Its defeat will restore Aetheria — or doom it."
},

}; /* end g_monster_db */

#undef N
#undef R
#undef W
#undef A
#undef I
