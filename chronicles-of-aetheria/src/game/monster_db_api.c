/* Chronicles of Aetheria — Monster DB API + Encounter Tables */

#include "../../include/game/monster_db.h"
#include "../../include/game/item/inventory.h"
#include "../../include/game/battle/battler.h"
#include "../../include/game/battle/battle_data.h"
#include "../../include/engine/memory.h"
#include "../../include/gba/gba_bios.h"

/* =========================================================
 * Encounter groups — 60 total
 * music_id 0=overworld, 1=dungeon, 2=cave, 3=boss
 * background_id matches map tileset indices
 * ========================================================= */
const MonsterGroup g_monster_groups[MONSTER_GROUP_COUNT] = {
/* Aelwyn (0-7) */
/* 0 */ {{ MON_SLIME, MON_SLIME, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_AELWYN, 0, 0, 20 },
/* 1 */ {{ MON_FOREST_RAT, MON_FOREST_RAT, MON_FOREST_RAT, 0xFF,0xFF,0xFF }, 3, REGION_AELWYN, 0, 0, 18 },
/* 2 */ {{ MON_GOBLIN_SCOUT, MON_GOBLIN_SCOUT, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_AELWYN, 0, 0, 20 },
/* 3 */ {{ MON_GOBLIN_WARRIOR, MON_GOBLIN_SCOUT, MON_GOBLIN_SCOUT, 0xFF,0xFF,0xFF }, 3, REGION_AELWYN, 0, 0, 22 },
/* 4 */ {{ MON_WILD_BOAR, 0xFF,0xFF,0xFF,0xFF,0xFF }, 1, REGION_AELWYN, 0, 0, 25 },
/* 5 */ {{ MON_BROWN_WOLF, MON_BROWN_WOLF, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_AELWYN, 0, 0, 20 },
/* 6 */ {{ MON_BANDIT, MON_BANDIT_ARCHER, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_AELWYN, 0, 0, 24 },
/* 7 */ {{ MON_BANDIT, MON_BANDIT_ARCHER, MON_BANDIT_MAGE, 0xFF,0xFF,0xFF }, 3, REGION_AELWYN, 0, 0, 28 },

/* Verdant Forest (8-14) */
/* 8 */ {{ MON_FOREST_SPIDER, MON_FOREST_SPIDER, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_VERDANT, 1, 0, 18 },
/* 9 */ {{ MON_TREANT, MON_VINE_SNAKE, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_VERDANT, 1, 0, 22 },
/*10 */ {{ MON_PIXIE, MON_PIXIE, MON_DRYAD, 0xFF,0xFF,0xFF }, 3, REGION_VERDANT, 1, 0, 20 },
/*11 */ {{ MON_ELF_ROGUE, MON_ELF_ARCHER, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_VERDANT, 1, 0, 24 },
/*12 */ {{ MON_GIANT_SPIDER, MON_FOREST_SPIDER, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_VERDANT, 1, 0, 26 },
/*13 */ {{ MON_THORN_WOLF, MON_THORN_WOLF, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_VERDANT, 1, 0, 22 },
/*14 */ {{ MON_CORRUPTED_BEAR, 0xFF,0xFF,0xFF,0xFF,0xFF }, 1, REGION_VERDANT, 1, 0, 30 },

/* Crystal Caves (15-20) */
/*15 */ {{ MON_CRYSTAL_BAT, MON_CRYSTAL_BAT, MON_CRYSTAL_BAT, 0xFF,0xFF,0xFF }, 3, REGION_CAVES, 2, 2, 16 },
/*16 */ {{ MON_KOBOLD, MON_KOBOLD, MON_KOBOLD_SHAMAN, 0xFF,0xFF,0xFF }, 3, REGION_CAVES, 2, 2, 18 },
/*17 */ {{ MON_CAVE_TROLL, MON_CAVE_SLIME, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_CAVES, 2, 2, 24 },
/*18 */ {{ MON_ROCK_GOLEM, 0xFF,0xFF,0xFF,0xFF,0xFF }, 1, REGION_CAVES, 2, 2, 28 },
/*19 */ {{ MON_CRYSTAL_SERPENT, MON_CRYSTAL_BAT, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_CAVES, 2, 2, 22 },
/*20 */ {{ MON_EARTH_WISP, MON_EARTH_WISP, MON_KOBOLD_SHAMAN, 0xFF,0xFF,0xFF }, 3, REGION_CAVES, 2, 2, 20 },

/* Murkhaven Swamp (21-26) */
/*21 */ {{ MON_BOG_SLIME, MON_PLAGUE_FROG, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_SWAMP, 3, 0, 18 },
/*22 */ {{ MON_SWAMP_CROC, MON_GIANT_LEECH, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_SWAMP, 3, 0, 22 },
/*23 */ {{ MON_WILL_O_WISP, MON_SWAMP_WRAITH, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_SWAMP, 3, 0, 24 },
/*24 */ {{ MON_TOXIC_SNAKE, MON_TOXIC_SNAKE, MON_BOG_SLIME, 0xFF,0xFF,0xFF }, 3, REGION_SWAMP, 3, 0, 20 },
/*25 */ {{ MON_DEATH_FLOWER, MON_SPORE_CLOUD, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_SWAMP, 3, 0, 26 },
/*26 */ {{ MON_SWAMP_HAG, MON_SWAMP_WRAITH, MON_WILL_O_WISP, 0xFF,0xFF,0xFF }, 3, REGION_SWAMP, 3, 0, 30 },

/* Ironpeak Mountains (27-32) */
/*27 */ {{ MON_MOUNTAIN_WOLF, MON_MOUNTAIN_WOLF, MON_FROST_WOLF, 0xFF,0xFF,0xFF }, 3, REGION_MOUNTAINS, 4, 0, 20 },
/*28 */ {{ MON_HARPY, MON_HARPY, MON_STORM_EAGLE, 0xFF,0xFF,0xFF }, 3, REGION_MOUNTAINS, 4, 0, 22 },
/*29 */ {{ MON_WYVERN, MON_CLIFF_SPIDER, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_MOUNTAINS, 4, 0, 26 },
/*30 */ {{ MON_GRANITE_TROLL, MON_MOUNTAIN_ORC, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_MOUNTAINS, 4, 0, 28 },
/*31 */ {{ MON_YETI, MON_FROST_WOLF, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_MOUNTAINS, 4, 0, 30 },
/*32 */ {{ MON_ORC_CHAMPION, MON_MOUNTAIN_ORC, MON_MOUNTAIN_ORC, 0xFF,0xFF,0xFF }, 3, REGION_MOUNTAINS, 4, 0, 32 },

/* Ancient Ruins (33-38) */
/*33 */ {{ MON_SKELETON, MON_SKELETON, MON_ZOMBIE, 0xFF,0xFF,0xFF }, 3, REGION_RUINS, 5, 1, 16 },
/*34 */ {{ MON_GHOST, MON_WRAITH, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_RUINS, 5, 1, 20 },
/*35 */ {{ MON_MUMMY, MON_CURSE_WISP, MON_CURSE_WISP, 0xFF,0xFF,0xFF }, 3, REGION_RUINS, 5, 1, 22 },
/*36 */ {{ MON_SHADOW_KNIGHT, MON_SKELETON, MON_ZOMBIE, 0xFF,0xFF,0xFF }, 3, REGION_RUINS, 5, 1, 26 },
/*37 */ {{ MON_LICH_APPRENTICE, MON_GHOST, MON_WRAITH, 0xFF,0xFF,0xFF }, 3, REGION_RUINS, 5, 1, 28 },
/*38 */ {{ MON_BONE_DRAGON, MON_VOID_SHADE, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_RUINS, 5, 1, 32 },

/* Ignarath Volcano (39-43) */
/*39 */ {{ MON_FIRE_LIZARD, MON_LAVA_SLIME, MON_FLAME_BAT, 0xFF,0xFF,0xFF }, 3, REGION_VOLCANO, 6, 1, 16 },
/*40 */ {{ MON_FIRE_IMP, MON_FIRE_IMP, MON_FIRE_ELEMENTAL, 0xFF,0xFF,0xFF }, 3, REGION_VOLCANO, 6, 1, 20 },
/*41 */ {{ MON_ASH_GOLEM, MON_MAGMA_CRAB, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_VOLCANO, 6, 1, 24 },
/*42 */ {{ MON_VOLCANO_TROLL, MON_FIRE_LIZARD, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_VOLCANO, 6, 1, 28 },
/*43 */ {{ MON_PYROMANCER, MON_FIRE_IMP, MON_FIRE_ELEMENTAL, 0xFF,0xFF,0xFF }, 3, REGION_VOLCANO, 6, 1, 30 },

/* Frostpeak Snowlands (44-48) */
/*44 */ {{ MON_SNOW_WOLF, MON_SNOW_WOLF, MON_FROST_BAT, 0xFF,0xFF,0xFF }, 3, REGION_SNOWLANDS, 7, 0, 16 },
/*45 */ {{ MON_ICE_TROLL, MON_BLIZZARD_WISP, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_SNOWLANDS, 7, 0, 22 },
/*46 */ {{ MON_WENDIGO, MON_SNOW_WOLF, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_SNOWLANDS, 7, 0, 26 },
/*47 */ {{ MON_SNOW_GIANT, MON_POLAR_BEAR, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_SNOWLANDS, 7, 0, 30 },
/*48 */ {{ MON_SNOW_WITCH, MON_CRYSTAL_IMP, MON_BLIZZARD_WISP, 0xFF,0xFF,0xFF }, 3, REGION_SNOWLANDS, 7, 0, 32 },

/* Ocean (49-52) */
/*49 */ {{ MON_SEA_SLIME, MON_JELLYFISH, MON_SHARK, 0xFF,0xFF,0xFF }, 3, REGION_OCEAN, 8, 0, 16 },
/*50 */ {{ MON_MERROW, MON_WATER_ELEMENTAL, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_OCEAN, 8, 0, 22 },
/*51 */ {{ MON_DEEP_ANGLER, MON_SEA_SERPENT, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_OCEAN, 8, 0, 28 },
/*52 */ {{ MON_SIREN, MON_TIDE_CRAB, MON_WATER_ELEMENTAL, 0xFF,0xFF,0xFF }, 3, REGION_OCEAN, 8, 0, 30 },

/* Shadow Realm (53-56) */
/*53 */ {{ MON_SHADOW_SLIME, MON_SHADE, MON_VOID_SPRITE, 0xFF,0xFF,0xFF }, 3, REGION_SHADOWREALM, 9, 1, 14 },
/*54 */ {{ MON_DARK_ELF, MON_NIGHTMARE, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_SHADOWREALM, 9, 1, 18 },
/*55 */ {{ MON_VOID_KNIGHT, MON_DARK_KNIGHT_SR, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_SHADOWREALM, 9, 1, 22 },
/*56 */ {{ MON_SOUL_EATER, MON_ABYSSAL_WRAITH, MON_PHANTOM_MAGE, 0xFF,0xFF,0xFF }, 3, REGION_SHADOWREALM, 9, 1, 26 },

/* Aetheria (57-59) */
/*57 */ {{ MON_AETHER_WISP, MON_AETHER_WISP, MON_CRYSTAL_ANGEL, 0xFF,0xFF,0xFF }, 3, REGION_AETHERIA, 10, 1, 12 },
/*58 */ {{ MON_AETHER_KNIGHT, MON_CORRUPTED_ANGEL, 0xFF,0xFF,0xFF,0xFF }, 2, REGION_AETHERIA, 10, 1, 16 },
/*59 */ {{ MON_VOID_TITAN, MON_FINAL_SHADE, MON_HERALD_OF_VOID, 0xFF,0xFF,0xFF }, 3, REGION_AETHERIA, 10, 1, 20 },
};

/* ---- API ------------------------------------------------ */

void monster_db_init(void)
{
    /* ROM data — nothing to initialize */
}

const MonsterDef* monster_get(u8 id)
{
    if (id >= MONSTER_COUNT) return &g_monster_db[0];
    return &g_monster_db[id];
}

const MonsterGroup* monster_group_get(u8 id)
{
    if (id >= MONSTER_GROUP_COUNT) return &g_monster_groups[0];
    return &g_monster_groups[id];
}

/* Simple LCG for loot/encounter RNG (no stdlib) */
static u32 s_rng = 12345;
static u32 _rand(void)
{
    s_rng = s_rng * 1664525u + 1013904223u;
    return s_rng;
}

u8 monster_random_encounter(u8 region, u8 player_level)
{
    u8 i, matches[MONSTER_GROUP_COUNT], count = 0;
    for (i = 0; i < MONSTER_GROUP_COUNT; i++) {
        const MonsterGroup* g = &g_monster_groups[i];
        if (g->region != region) continue;
        /* Check at least one monster is level-appropriate */
        {
            u8 j;
            for (j = 0; j < g->count; j++) {
                const MonsterDef* m = monster_get(g->monster_ids[j]);
                if (player_level >= m->level_min &&
                    player_level <= m->level_max + 3) {
                    matches[count++] = i;
                    break;
                }
            }
        }
        if (count >= MONSTER_GROUP_COUNT) break;
    }
    if (count == 0) return 0;
    return matches[_rand() % count];
}

void monster_to_enemy_def(const MonsterDef* mon, EnemyDef* out)
{
    u8 i;
    if (!mon || !out) return;
    out->name        = mon->name;
    out->sprite_id   = mon->sprite_id;
    out->palette_id  = mon->palette_id;
    out->element     = mon->element;
    out->hp          = mon->hp;
    out->mp          = mon->mp;
    out->atk         = mon->atk;
    out->def         = mon->def;
    out->mag         = mon->mag;
    out->mdef        = mon->mdef;
    out->spd         = mon->spd;
    for (i = 0; i < ELEM_COUNT; i++)
        out->resist[i] = mon->resist[i];
    out->exp_reward  = mon->exp_reward;
    out->gold_reward = mon->gold_reward;
    for (i = 0; i < 4; i++)
        out->skill_ids[i] = mon->skill_ids[i];
    out->skill_count = mon->skill_count;
    out->ai_type     = mon->ai_type;
}

void monster_roll_loot(const MonsterDef* mon)
{
    u8 i;
    if (!mon) return;
    for (i = 0; i < 3; i++) {
        const LootEntry* e = &mon->loot[i];
        if (e->item_id == ITEM_NONE) continue;
        if ((_rand() % 100) < e->chance)
            bag_add(e->item_id, e->quantity);
    }
}
