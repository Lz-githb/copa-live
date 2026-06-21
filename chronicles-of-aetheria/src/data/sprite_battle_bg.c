/*
 * sprite_battle_bg.c — Battle background tile maps
 * Chronicles of Aetheria GBA RPG
 *
 * Each battle background is a 30-wide × 10-tall arrangement of tile IDs
 * that fills the lower half of the battle screen (BG3).  Tile IDs reference
 * the shared world tileset:
 *
 *   0-31   overworld tiles (0=blank/sky, 1=grass, 2=tree, 3=water,
 *                            4=wall, 5=door, 6=path, 7=flower,
 *                            8=fence, 9=bush, 10=deep water, 11=sand,
 *                            12=stone floor, 13=rubble, 14=bridge,
 *                            15=chest, 16=sign, 17=rock, 18=hill,
 *                            19=cliff, 20=ice, 21=snow, 22=lava,
 *                            23=lava rock, 24=volcano rock, 25=ash,
 *                            26=crystal, 27=dark ground, 28=void,
 *                            29=dark rock, 30=shadow ground, 31=abyss)
 *   100+   dungeon tiles  (100=dungeon floor, 101=dungeon wall,
 *                          102=dungeon pillar, 103=dungeon door,
 *                          104=dungeon rubble, 105=dungeon chest)
 */

#include "../../include/gba/gba_types.h"

/* -----------------------------------------------------------------------
 * Constants
 * ----------------------------------------------------------------------- */
#define BATTLE_BG_COUNT  6
#define BATTLE_BG_W     30
#define BATTLE_BG_H     10

/* Battle background IDs */
#define BBG_FOREST   0
#define BBG_CAVE     1
#define BBG_CASTLE   2
#define BBG_OCEAN    3
#define BBG_VOLCANO  4
#define BBG_SHADOW   5

/* Tile ID shorthand (overworld tileset) */
#define T_BLANK      0   /* sky / empty                */
#define T_GRASS      1   /* basic grass                */
#define T_TREE       2   /* tree canopy                */
#define T_WATER      3   /* shallow / rippling water   */
#define T_WALL       4   /* generic wall               */
#define T_PATH       6   /* dirt path                  */
#define T_BUSH       9   /* dense bush / undergrowth   */
#define T_DWATER    10   /* deep water                 */
#define T_SAND      11   /* sand / beach               */
#define T_STONE     12   /* stone floor / cobbles      */
#define T_RUBBLE    13   /* rubble / debris            */
#define T_ROCK      17   /* standalone rock            */
#define T_HILL      18   /* hill / raised earth        */
#define T_CLIFF     19   /* cliff face                 */
#define T_LAVA      22   /* lava pool                  */
#define T_LVRCK     23   /* lava-crusted rock          */
#define T_VRCK      24   /* volcano rock               */
#define T_ASH       25   /* ash / soot ground          */
#define T_DKGND     27   /* dark ground                */
#define T_VOID      28   /* void / nothingness         */
#define T_DKRCK     29   /* dark rock                  */
#define T_SHDW      30   /* shadow ground              */
#define T_ABYSS     31   /* abyss / deep shadow        */

/* Dungeon tile shorthand */
#define T_DFLOOR   100   /* dungeon floor              */
#define T_DWALL    101   /* dungeon wall               */
#define T_DPILLAR  102   /* dungeon pillar             */
#define T_DRUB     104   /* dungeon rubble             */

/* -----------------------------------------------------------------------
 * Background tile maps  (BATTLE_BG_H rows × BATTLE_BG_W cols = 300 u16s)
 *
 * Layout (rows 0-9, top to bottom of the battle background strip):
 *   row 0-1   sky / ceiling / upper atmosphere
 *   row 2-4   mid-ground / tree-line / walls
 *   row 5-6   lower mid / transition
 *   row 7-9   ground / floor
 * ----------------------------------------------------------------------- */

/* Helper macro — 30 tile values in one row */
#define ROW30(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,A,B,C,D) \
    (a),(b),(c),(d),(e),(f),(g),(h),(i),(j),(k),(l),(m),(n),(o),               \
    (p),(q),(r),(s),(t),(u),(v),(w),(x),(y),(z),(A),(B),(C),(D)

const u16 g_battle_bg[BATTLE_BG_COUNT][BATTLE_BG_H * BATTLE_BG_W] = {

    /* ------------------------------------------------------------------ */
    /* BBG_FOREST (0) — Verdant woodland                                   */
    /* Sky (blank) at top, tree canopy in middle, grass floor at bottom    */
    /* ------------------------------------------------------------------ */
    {
        /* row 0 — open sky */
        ROW30(T_BLANK,T_BLANK,T_BLANK,T_BLANK,T_BLANK,T_BLANK,T_BLANK,T_BLANK,
              T_BLANK,T_BLANK,T_BLANK,T_BLANK,T_BLANK,T_BLANK,T_BLANK,T_BLANK,
              T_BLANK,T_BLANK,T_BLANK,T_BLANK,T_BLANK,T_BLANK,T_BLANK,T_BLANK,
              T_BLANK,T_BLANK,T_BLANK,T_BLANK,T_BLANK,T_BLANK),
        /* row 1 — sky with tree tops beginning */
        ROW30(T_BLANK,T_BLANK,T_TREE, T_BLANK,T_BLANK,T_BLANK,T_TREE, T_BLANK,
              T_BLANK,T_TREE, T_BLANK,T_BLANK,T_BLANK,T_BLANK,T_TREE, T_BLANK,
              T_BLANK,T_BLANK,T_TREE, T_BLANK,T_BLANK,T_TREE, T_BLANK,T_BLANK,
              T_BLANK,T_BLANK,T_TREE, T_BLANK,T_BLANK,T_BLANK),
        /* row 2 — dense tree canopy */
        ROW30(T_TREE, T_TREE, T_TREE, T_TREE, T_BLANK,T_TREE, T_TREE, T_TREE,
              T_TREE, T_TREE, T_TREE, T_BLANK,T_TREE, T_TREE, T_TREE, T_TREE,
              T_TREE, T_BLANK,T_TREE, T_TREE, T_TREE, T_TREE, T_TREE, T_BLANK,
              T_TREE, T_TREE, T_TREE, T_TREE, T_BLANK,T_TREE ),
        /* row 3 — canopy continues */
        ROW30(T_TREE, T_TREE, T_TREE, T_TREE, T_TREE, T_TREE, T_TREE, T_TREE,
              T_TREE, T_TREE, T_TREE, T_TREE, T_TREE, T_TREE, T_TREE, T_TREE,
              T_TREE, T_TREE, T_TREE, T_TREE, T_TREE, T_TREE, T_TREE, T_TREE,
              T_TREE, T_TREE, T_TREE, T_TREE, T_TREE, T_TREE ),
        /* row 4 — lower canopy, bush beginning */
        ROW30(T_TREE, T_BUSH,T_TREE, T_TREE, T_BUSH, T_TREE, T_TREE, T_BUSH,
              T_TREE, T_TREE, T_BUSH, T_TREE, T_TREE, T_BUSH, T_TREE, T_TREE,
              T_BUSH, T_TREE, T_TREE, T_BUSH, T_TREE, T_TREE, T_BUSH, T_TREE,
              T_TREE, T_BUSH, T_TREE, T_TREE, T_BUSH, T_TREE ),
        /* row 5 — bush / undergrowth */
        ROW30(T_BUSH, T_BUSH, T_GRASS,T_BUSH, T_BUSH, T_GRASS,T_BUSH, T_BUSH,
              T_GRASS,T_BUSH, T_BUSH, T_GRASS,T_BUSH, T_BUSH, T_GRASS,T_BUSH,
              T_BUSH, T_GRASS,T_BUSH, T_BUSH, T_GRASS,T_BUSH, T_BUSH, T_GRASS,
              T_BUSH, T_BUSH, T_GRASS,T_BUSH, T_BUSH, T_GRASS),
        /* row 6 — grass with scattered bushes */
        ROW30(T_GRASS,T_GRASS,T_BUSH, T_GRASS,T_GRASS,T_GRASS,T_BUSH, T_GRASS,
              T_GRASS,T_GRASS,T_BUSH, T_GRASS,T_GRASS,T_GRASS,T_BUSH, T_GRASS,
              T_GRASS,T_GRASS,T_BUSH, T_GRASS,T_GRASS,T_GRASS,T_BUSH, T_GRASS,
              T_GRASS,T_GRASS,T_BUSH, T_GRASS,T_GRASS,T_GRASS),
        /* row 7 — grass floor */
        ROW30(T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,
              T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,
              T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,
              T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS),
        /* row 8 — grass with path */
        ROW30(T_GRASS,T_GRASS,T_GRASS,T_PATH, T_PATH, T_PATH, T_PATH, T_PATH,
              T_PATH, T_PATH, T_PATH, T_PATH, T_PATH, T_PATH, T_PATH, T_PATH,
              T_PATH, T_PATH, T_PATH, T_PATH, T_PATH, T_PATH, T_PATH, T_PATH,
              T_PATH, T_PATH, T_PATH, T_GRASS,T_GRASS,T_GRASS),
        /* row 9 — solid grass base */
        ROW30(T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,
              T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,
              T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,
              T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS,T_GRASS),
    },

    /* ------------------------------------------------------------------ */
    /* BBG_CAVE (1) — Underground cavern                                   */
    /* Black void at top, dungeon walls mid, stone floor at bottom         */
    /* ------------------------------------------------------------------ */
    {
        /* row 0 — pitch black ceiling */
        ROW30(T_DWALL,T_DWALL,T_DWALL,T_DWALL,T_DWALL,T_DWALL,T_DWALL,T_DWALL,
              T_DWALL,T_DWALL,T_DWALL,T_DWALL,T_DWALL,T_DWALL,T_DWALL,T_DWALL,
              T_DWALL,T_DWALL,T_DWALL,T_DWALL,T_DWALL,T_DWALL,T_DWALL,T_DWALL,
              T_DWALL,T_DWALL,T_DWALL,T_DWALL,T_DWALL,T_DWALL),
        /* row 1 — ceiling with stalactite hints */
        ROW30(T_DWALL,T_DWALL,T_DPILLAR,T_DWALL,T_DWALL,T_DWALL,T_DPILLAR,T_DWALL,
              T_DWALL,T_DWALL,T_DPILLAR,T_DWALL,T_DWALL,T_DWALL,T_DPILLAR,T_DWALL,
              T_DWALL,T_DWALL,T_DPILLAR,T_DWALL,T_DWALL,T_DWALL,T_DPILLAR,T_DWALL,
              T_DWALL,T_DWALL,T_DPILLAR,T_DWALL,T_DWALL,T_DWALL),
        /* row 2 — upper cave void */
        ROW30(T_DWALL,T_VOID, T_VOID, T_VOID, T_DWALL,T_VOID, T_VOID, T_VOID,
              T_DWALL,T_VOID, T_VOID, T_VOID, T_DWALL,T_VOID, T_VOID, T_VOID,
              T_DWALL,T_VOID, T_VOID, T_VOID, T_DWALL,T_VOID, T_VOID, T_VOID,
              T_DWALL,T_VOID, T_VOID, T_VOID, T_DWALL,T_DWALL),
        /* row 3 — mid cave with rock pillars */
        ROW30(T_DWALL,T_VOID, T_VOID, T_VOID, T_DPILLAR,T_VOID,T_VOID, T_VOID,
              T_DPILLAR,T_VOID,T_VOID, T_VOID, T_DPILLAR,T_VOID,T_VOID, T_VOID,
              T_DPILLAR,T_VOID,T_VOID, T_VOID, T_DPILLAR,T_VOID,T_VOID, T_VOID,
              T_DPILLAR,T_VOID,T_VOID, T_VOID, T_DWALL,T_DWALL),
        /* row 4 — cave wall layer */
        ROW30(T_DWALL,T_DWALL,T_DWALL,T_STONE,T_DWALL,T_DWALL,T_STONE,T_DWALL,
              T_DWALL,T_STONE,T_DWALL,T_DWALL,T_STONE,T_DWALL,T_DWALL,T_STONE,
              T_DWALL,T_DWALL,T_STONE,T_DWALL,T_DWALL,T_STONE,T_DWALL,T_DWALL,
              T_STONE,T_DWALL,T_DWALL,T_STONE,T_DWALL,T_DWALL),
        /* row 5 — rough cave floor emerging */
        ROW30(T_STONE,T_DRUB, T_STONE,T_STONE,T_DRUB, T_STONE,T_STONE,T_DRUB,
              T_STONE,T_STONE,T_DRUB, T_STONE,T_STONE,T_DRUB, T_STONE,T_STONE,
              T_DRUB, T_STONE,T_STONE,T_DRUB, T_STONE,T_STONE,T_DRUB, T_STONE,
              T_STONE,T_DRUB, T_STONE,T_STONE,T_DRUB, T_STONE),
        /* row 6 — cave floor with rubble */
        ROW30(T_DFLOOR,T_DFLOOR,T_DRUB,T_DFLOOR,T_DFLOOR,T_DRUB,T_DFLOOR,T_DFLOOR,
              T_DRUB,T_DFLOOR,T_DFLOOR,T_DRUB, T_DFLOOR,T_DFLOOR,T_DRUB,T_DFLOOR,
              T_DFLOOR,T_DRUB,T_DFLOOR,T_DFLOOR,T_DRUB, T_DFLOOR,T_DFLOOR,T_DRUB,
              T_DFLOOR,T_DFLOOR,T_DRUB, T_DFLOOR,T_DFLOOR,T_DRUB),
        /* row 7 — dungeon floor */
        ROW30(T_DFLOOR,T_DFLOOR,T_DFLOOR,T_DFLOOR,T_DFLOOR,T_DFLOOR,T_DFLOOR,T_DFLOOR,
              T_DFLOOR,T_DFLOOR,T_DFLOOR,T_DFLOOR,T_DFLOOR,T_DFLOOR,T_DFLOOR,T_DFLOOR,
              T_DFLOOR,T_DFLOOR,T_DFLOOR,T_DFLOOR,T_DFLOOR,T_DFLOOR,T_DFLOOR,T_DFLOOR,
              T_DFLOOR,T_DFLOOR,T_DFLOOR,T_DFLOOR,T_DFLOOR,T_DFLOOR),
        /* row 8 — stone floor with occasional rubble */
        ROW30(T_STONE,T_DFLOOR,T_DFLOOR,T_STONE,T_DFLOOR,T_DFLOOR,T_STONE,T_DFLOOR,
              T_DFLOOR,T_STONE,T_DFLOOR,T_DFLOOR,T_STONE,T_DFLOOR,T_DFLOOR,T_STONE,
              T_DFLOOR,T_DFLOOR,T_STONE,T_DFLOOR,T_DFLOOR,T_STONE,T_DFLOOR,T_DFLOOR,
              T_STONE,T_DFLOOR,T_DFLOOR,T_STONE,T_DFLOOR,T_DFLOOR),
        /* row 9 — solid stone base */
        ROW30(T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,
              T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,
              T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,
              T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE),
    },

    /* ------------------------------------------------------------------ */
    /* BBG_CASTLE (2) — Castle interior / courtyard                        */
    /* Stone ceiling, castle walls, cobblestone floor                      */
    /* ------------------------------------------------------------------ */
    {
        /* row 0 — stone ceiling */
        ROW30(T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL,
              T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL,
              T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL,
              T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL ),
        /* row 1 — battlements / parapet silhouette */
        ROW30(T_WALL, T_BLANK,T_WALL, T_BLANK,T_WALL, T_BLANK,T_WALL, T_BLANK,
              T_WALL, T_BLANK,T_WALL, T_BLANK,T_WALL, T_BLANK,T_WALL, T_BLANK,
              T_WALL, T_BLANK,T_WALL, T_BLANK,T_WALL, T_BLANK,T_WALL, T_BLANK,
              T_WALL, T_BLANK,T_WALL, T_BLANK,T_WALL, T_BLANK),
        /* row 2 — castle wall upper */
        ROW30(T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL,
              T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL,
              T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL,
              T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL ),
        /* row 3 — wall with window openings */
        ROW30(T_WALL, T_WALL, T_BLANK,T_BLANK,T_WALL, T_WALL, T_BLANK,T_BLANK,
              T_WALL, T_WALL, T_BLANK,T_BLANK,T_WALL, T_WALL, T_BLANK,T_BLANK,
              T_WALL, T_WALL, T_BLANK,T_BLANK,T_WALL, T_WALL, T_BLANK,T_BLANK,
              T_WALL, T_WALL, T_BLANK,T_BLANK,T_WALL, T_WALL ),
        /* row 4 — wall continues */
        ROW30(T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL,
              T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL,
              T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL,
              T_WALL, T_WALL, T_WALL, T_WALL, T_WALL, T_WALL ),
        /* row 5 — lower wall / wall base */
        ROW30(T_WALL, T_STONE,T_STONE,T_STONE,T_WALL, T_STONE,T_STONE,T_STONE,
              T_WALL, T_STONE,T_STONE,T_STONE,T_WALL, T_STONE,T_STONE,T_STONE,
              T_WALL, T_STONE,T_STONE,T_STONE,T_WALL, T_STONE,T_STONE,T_STONE,
              T_WALL, T_STONE,T_STONE,T_STONE,T_WALL, T_STONE),
        /* row 6 — stone floor transition */
        ROW30(T_STONE,T_STONE,T_RUBBLE,T_STONE,T_STONE,T_STONE,T_RUBBLE,T_STONE,
              T_STONE,T_STONE,T_RUBBLE,T_STONE,T_STONE,T_STONE,T_RUBBLE,T_STONE,
              T_STONE,T_STONE,T_RUBBLE,T_STONE,T_STONE,T_STONE,T_RUBBLE,T_STONE,
              T_STONE,T_STONE,T_RUBBLE,T_STONE,T_STONE,T_STONE),
        /* row 7 — cobblestone floor */
        ROW30(T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,
              T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,
              T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,
              T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE),
        /* row 8 — floor with path down centre */
        ROW30(T_STONE,T_STONE,T_STONE,T_PATH, T_PATH, T_PATH, T_PATH, T_PATH,
              T_PATH, T_PATH, T_PATH, T_PATH, T_PATH, T_PATH, T_PATH, T_PATH,
              T_PATH, T_PATH, T_PATH, T_PATH, T_PATH, T_PATH, T_PATH, T_PATH,
              T_PATH, T_PATH, T_PATH, T_STONE,T_STONE,T_STONE),
        /* row 9 — solid stone base */
        ROW30(T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,
              T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,
              T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,
              T_STONE,T_STONE,T_STONE,T_STONE,T_STONE,T_STONE),
    },

    /* ------------------------------------------------------------------ */
    /* BBG_OCEAN (3) — Open sea                                            */
    /* All water with wave ripple patterns                                 */
    /* ------------------------------------------------------------------ */
    {
        /* row 0 — deep sea horizon */
        ROW30(T_DWATER,T_DWATER,T_DWATER,T_DWATER,T_DWATER,T_DWATER,T_DWATER,T_DWATER,
              T_DWATER,T_DWATER,T_DWATER,T_DWATER,T_DWATER,T_DWATER,T_DWATER,T_DWATER,
              T_DWATER,T_DWATER,T_DWATER,T_DWATER,T_DWATER,T_DWATER,T_DWATER,T_DWATER,
              T_DWATER,T_DWATER,T_DWATER,T_DWATER,T_DWATER,T_DWATER),
        /* row 1 — deep water */
        ROW30(T_DWATER,T_DWATER,T_DWATER,T_WATER,T_DWATER,T_DWATER,T_WATER, T_DWATER,
              T_DWATER,T_WATER, T_DWATER,T_DWATER,T_WATER, T_DWATER,T_DWATER,T_WATER,
              T_DWATER,T_DWATER,T_WATER, T_DWATER,T_DWATER,T_WATER, T_DWATER,T_DWATER,
              T_WATER, T_DWATER,T_DWATER,T_WATER, T_DWATER,T_DWATER),
        /* row 2 — mid ocean */
        ROW30(T_DWATER,T_WATER, T_DWATER,T_DWATER,T_DWATER,T_WATER, T_DWATER,T_DWATER,
              T_DWATER,T_WATER, T_DWATER,T_DWATER,T_DWATER,T_WATER, T_DWATER,T_DWATER,
              T_DWATER,T_WATER, T_DWATER,T_DWATER,T_DWATER,T_WATER, T_DWATER,T_DWATER,
              T_DWATER,T_WATER, T_DWATER,T_DWATER,T_DWATER,T_WATER ),
        /* row 3 — ripple band */
        ROW30(T_WATER, T_WATER, T_DWATER,T_WATER, T_WATER, T_DWATER,T_WATER, T_WATER,
              T_DWATER,T_WATER, T_WATER, T_DWATER,T_WATER, T_WATER, T_DWATER,T_WATER,
              T_WATER, T_DWATER,T_WATER, T_WATER, T_DWATER,T_WATER, T_WATER, T_DWATER,
              T_WATER, T_WATER, T_DWATER,T_WATER, T_WATER, T_DWATER),
        /* row 4 — ocean mid */
        ROW30(T_DWATER,T_WATER, T_WATER, T_DWATER,T_DWATER,T_WATER, T_WATER, T_DWATER,
              T_DWATER,T_WATER, T_WATER, T_DWATER,T_DWATER,T_WATER, T_WATER, T_DWATER,
              T_DWATER,T_WATER, T_WATER, T_DWATER,T_DWATER,T_WATER, T_WATER, T_DWATER,
              T_DWATER,T_WATER, T_WATER, T_DWATER,T_DWATER,T_WATER ),
        /* row 5 — shallow ripples */
        ROW30(T_WATER, T_DWATER,T_WATER, T_WATER, T_WATER, T_DWATER,T_WATER, T_WATER,
              T_WATER, T_DWATER,T_WATER, T_WATER, T_WATER, T_DWATER,T_WATER, T_WATER,
              T_WATER, T_DWATER,T_WATER, T_WATER, T_WATER, T_DWATER,T_WATER, T_WATER,
              T_WATER, T_DWATER,T_WATER, T_WATER, T_WATER, T_DWATER),
        /* row 6 — more ripples */
        ROW30(T_WATER, T_WATER, T_WATER, T_DWATER,T_WATER, T_WATER, T_WATER, T_DWATER,
              T_WATER, T_WATER, T_WATER, T_DWATER,T_WATER, T_WATER, T_WATER, T_DWATER,
              T_WATER, T_WATER, T_WATER, T_DWATER,T_WATER, T_WATER, T_WATER, T_DWATER,
              T_WATER, T_WATER, T_WATER, T_DWATER,T_WATER, T_WATER ),
        /* row 7 — surface level */
        ROW30(T_WATER, T_WATER, T_DWATER,T_WATER, T_WATER, T_WATER, T_DWATER,T_WATER,
              T_WATER, T_WATER, T_DWATER,T_WATER, T_WATER, T_WATER, T_DWATER,T_WATER,
              T_WATER, T_WATER, T_DWATER,T_WATER, T_WATER, T_WATER, T_DWATER,T_WATER,
              T_WATER, T_WATER, T_DWATER,T_WATER, T_WATER, T_WATER ),
        /* row 8 — shallow water near shore */
        ROW30(T_SAND,  T_WATER, T_WATER, T_WATER, T_SAND,  T_WATER, T_WATER, T_WATER,
              T_SAND,  T_WATER, T_WATER, T_WATER, T_SAND,  T_WATER, T_WATER, T_WATER,
              T_SAND,  T_WATER, T_WATER, T_WATER, T_SAND,  T_WATER, T_WATER, T_WATER,
              T_SAND,  T_WATER, T_WATER, T_WATER, T_SAND,  T_WATER ),
        /* row 9 — sandy sea floor / base */
        ROW30(T_SAND,  T_SAND,  T_SAND,  T_SAND,  T_SAND,  T_SAND,  T_SAND,  T_SAND,
              T_SAND,  T_SAND,  T_SAND,  T_SAND,  T_SAND,  T_SAND,  T_SAND,  T_SAND,
              T_SAND,  T_SAND,  T_SAND,  T_SAND,  T_SAND,  T_SAND,  T_SAND,  T_SAND,
              T_SAND,  T_SAND,  T_SAND,  T_SAND,  T_SAND,  T_SAND  ),
    },

    /* ------------------------------------------------------------------ */
    /* BBG_VOLCANO (4) — Active volcanic region                            */
    /* Red sky at top, volcano rock mid, lava at base                     */
    /* ------------------------------------------------------------------ */
    {
        /* row 0 — red sky / smoke */
        ROW30(T_ASH,  T_ASH,  T_ASH,  T_ASH,  T_ASH,  T_ASH,  T_ASH,  T_ASH,
              T_ASH,  T_ASH,  T_ASH,  T_ASH,  T_ASH,  T_ASH,  T_ASH,  T_ASH,
              T_ASH,  T_ASH,  T_ASH,  T_ASH,  T_ASH,  T_ASH,  T_ASH,  T_ASH,
              T_ASH,  T_ASH,  T_ASH,  T_ASH,  T_ASH,  T_ASH  ),
        /* row 1 — smoke / haze */
        ROW30(T_ASH,  T_VRCK, T_ASH,  T_ASH,  T_ASH,  T_VRCK, T_ASH,  T_ASH,
              T_ASH,  T_VRCK, T_ASH,  T_ASH,  T_ASH,  T_VRCK, T_ASH,  T_ASH,
              T_ASH,  T_VRCK, T_ASH,  T_ASH,  T_ASH,  T_VRCK, T_ASH,  T_ASH,
              T_ASH,  T_VRCK, T_ASH,  T_ASH,  T_ASH,  T_VRCK ),
        /* row 2 — volcano silhouette peak */
        ROW30(T_VRCK, T_VRCK, T_VRCK, T_ASH,  T_VRCK, T_VRCK, T_VRCK, T_ASH,
              T_VRCK, T_VRCK, T_VRCK, T_ASH,  T_VRCK, T_VRCK, T_VRCK, T_ASH,
              T_VRCK, T_VRCK, T_VRCK, T_ASH,  T_VRCK, T_VRCK, T_VRCK, T_ASH,
              T_VRCK, T_VRCK, T_VRCK, T_ASH,  T_VRCK, T_VRCK ),
        /* row 3 — upper volcano rock */
        ROW30(T_VRCK, T_VRCK, T_LVRCK,T_VRCK, T_VRCK, T_LVRCK,T_VRCK, T_VRCK,
              T_LVRCK,T_VRCK, T_VRCK, T_LVRCK,T_VRCK, T_VRCK, T_LVRCK,T_VRCK,
              T_VRCK, T_LVRCK,T_VRCK, T_VRCK, T_LVRCK,T_VRCK, T_VRCK, T_LVRCK,
              T_VRCK, T_VRCK, T_LVRCK,T_VRCK, T_VRCK, T_LVRCK),
        /* row 4 — rock with lava seams */
        ROW30(T_VRCK, T_LAVA, T_VRCK, T_VRCK, T_LAVA, T_VRCK, T_VRCK, T_LAVA,
              T_VRCK, T_VRCK, T_LAVA, T_VRCK, T_VRCK, T_LAVA, T_VRCK, T_VRCK,
              T_LAVA, T_VRCK, T_VRCK, T_LAVA, T_VRCK, T_VRCK, T_LAVA, T_VRCK,
              T_VRCK, T_LAVA, T_VRCK, T_VRCK, T_LAVA, T_VRCK ),
        /* row 5 — lava rock layer */
        ROW30(T_LVRCK,T_LVRCK,T_LAVA, T_LVRCK,T_LVRCK,T_LAVA, T_LVRCK,T_LVRCK,
              T_LAVA, T_LVRCK,T_LVRCK,T_LAVA, T_LVRCK,T_LVRCK,T_LAVA, T_LVRCK,
              T_LVRCK,T_LAVA, T_LVRCK,T_LVRCK,T_LAVA, T_LVRCK,T_LVRCK,T_LAVA,
              T_LVRCK,T_LVRCK,T_LAVA, T_LVRCK,T_LVRCK,T_LAVA ),
        /* row 6 — flowing lava */
        ROW30(T_LAVA, T_LAVA, T_LVRCK,T_LAVA, T_LAVA, T_LVRCK,T_LAVA, T_LAVA,
              T_LVRCK,T_LAVA, T_LAVA, T_LVRCK,T_LAVA, T_LAVA, T_LVRCK,T_LAVA,
              T_LAVA, T_LVRCK,T_LAVA, T_LAVA, T_LVRCK,T_LAVA, T_LAVA, T_LVRCK,
              T_LAVA, T_LAVA, T_LVRCK,T_LAVA, T_LAVA, T_LVRCK),
        /* row 7 — lava pool */
        ROW30(T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA,
              T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA,
              T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA,
              T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA ),
        /* row 8 — lava with rock islands */
        ROW30(T_LAVA, T_VRCK, T_LAVA, T_LAVA, T_LAVA, T_VRCK, T_LAVA, T_LAVA,
              T_LAVA, T_VRCK, T_LAVA, T_LAVA, T_LAVA, T_VRCK, T_LAVA, T_LAVA,
              T_LAVA, T_VRCK, T_LAVA, T_LAVA, T_LAVA, T_VRCK, T_LAVA, T_LAVA,
              T_LAVA, T_VRCK, T_LAVA, T_LAVA, T_LAVA, T_VRCK ),
        /* row 9 — lava base */
        ROW30(T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA,
              T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA,
              T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA,
              T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA, T_LAVA ),
    },

    /* ------------------------------------------------------------------ */
    /* BBG_SHADOW (5) — Shadow realm / dark dimension                      */
    /* All dark / void tiles with scattered shadow rocks                   */
    /* ------------------------------------------------------------------ */
    {
        /* row 0 — pure void */
        ROW30(T_VOID, T_VOID, T_VOID, T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
              T_VOID, T_VOID, T_VOID, T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
              T_VOID, T_VOID, T_VOID, T_VOID, T_VOID, T_VOID, T_VOID, T_VOID,
              T_VOID, T_VOID, T_VOID, T_VOID, T_VOID, T_VOID ),
        /* row 1 — void with abyss clusters */
        ROW30(T_VOID, T_ABYSS,T_VOID, T_VOID, T_ABYSS,T_VOID, T_VOID, T_ABYSS,
              T_VOID, T_VOID, T_ABYSS,T_VOID, T_VOID, T_ABYSS,T_VOID, T_VOID,
              T_ABYSS,T_VOID, T_VOID, T_ABYSS,T_VOID, T_VOID, T_ABYSS,T_VOID,
              T_VOID, T_ABYSS,T_VOID, T_VOID, T_ABYSS,T_VOID ),
        /* row 2 — dark sky */
        ROW30(T_ABYSS,T_VOID, T_ABYSS,T_ABYSS,T_VOID, T_ABYSS,T_ABYSS,T_VOID,
              T_ABYSS,T_ABYSS,T_VOID, T_ABYSS,T_ABYSS,T_VOID, T_ABYSS,T_ABYSS,
              T_VOID, T_ABYSS,T_ABYSS,T_VOID, T_ABYSS,T_ABYSS,T_VOID, T_ABYSS,
              T_ABYSS,T_VOID, T_ABYSS,T_ABYSS,T_VOID, T_ABYSS),
        /* row 3 — shadow with dark rocks */
        ROW30(T_DKGND,T_VOID, T_DKRCK,T_DKGND,T_VOID, T_DKRCK,T_DKGND,T_VOID,
              T_DKRCK,T_DKGND,T_VOID, T_DKRCK,T_DKGND,T_VOID, T_DKRCK,T_DKGND,
              T_VOID, T_DKRCK,T_DKGND,T_VOID, T_DKRCK,T_DKGND,T_VOID, T_DKRCK,
              T_DKGND,T_VOID, T_DKRCK,T_DKGND,T_VOID, T_DKRCK),
        /* row 4 — dark landscape */
        ROW30(T_DKRCK,T_DKGND,T_DKGND,T_DKRCK,T_DKGND,T_DKGND,T_DKRCK,T_DKGND,
              T_DKGND,T_DKRCK,T_DKGND,T_DKGND,T_DKRCK,T_DKGND,T_DKGND,T_DKRCK,
              T_DKGND,T_DKGND,T_DKRCK,T_DKGND,T_DKGND,T_DKRCK,T_DKGND,T_DKGND,
              T_DKRCK,T_DKGND,T_DKGND,T_DKRCK,T_DKGND,T_DKGND),
        /* row 5 — shadow ground mix */
        ROW30(T_SHDW, T_DKGND,T_SHDW, T_SHDW, T_DKGND,T_SHDW, T_SHDW, T_DKGND,
              T_SHDW, T_SHDW, T_DKGND,T_SHDW, T_SHDW, T_DKGND,T_SHDW, T_SHDW,
              T_DKGND,T_SHDW, T_SHDW, T_DKGND,T_SHDW, T_SHDW, T_DKGND,T_SHDW,
              T_SHDW, T_DKGND,T_SHDW, T_SHDW, T_DKGND,T_SHDW ),
        /* row 6 — shadow floor */
        ROW30(T_SHDW, T_SHDW, T_DKRCK,T_SHDW, T_SHDW, T_DKRCK,T_SHDW, T_SHDW,
              T_DKRCK,T_SHDW, T_SHDW, T_DKRCK,T_SHDW, T_SHDW, T_DKRCK,T_SHDW,
              T_SHDW, T_DKRCK,T_SHDW, T_SHDW, T_DKRCK,T_SHDW, T_SHDW, T_DKRCK,
              T_SHDW, T_SHDW, T_DKRCK,T_SHDW, T_SHDW, T_DKRCK),
        /* row 7 — dark ground */
        ROW30(T_DKGND,T_DKGND,T_DKGND,T_DKGND,T_DKGND,T_DKGND,T_DKGND,T_DKGND,
              T_DKGND,T_DKGND,T_DKGND,T_DKGND,T_DKGND,T_DKGND,T_DKGND,T_DKGND,
              T_DKGND,T_DKGND,T_DKGND,T_DKGND,T_DKGND,T_DKGND,T_DKGND,T_DKGND,
              T_DKGND,T_DKGND,T_DKGND,T_DKGND,T_DKGND,T_DKGND),
        /* row 8 — dark ground with shadow rock */
        ROW30(T_DKGND,T_SHDW, T_DKGND,T_DKRCK,T_DKGND,T_SHDW, T_DKGND,T_DKRCK,
              T_DKGND,T_SHDW, T_DKGND,T_DKRCK,T_DKGND,T_SHDW, T_DKGND,T_DKRCK,
              T_DKGND,T_SHDW, T_DKGND,T_DKRCK,T_DKGND,T_SHDW, T_DKGND,T_DKRCK,
              T_DKGND,T_SHDW, T_DKGND,T_DKRCK,T_DKGND,T_SHDW ),
        /* row 9 — void base */
        ROW30(T_ABYSS,T_ABYSS,T_ABYSS,T_ABYSS,T_ABYSS,T_ABYSS,T_ABYSS,T_ABYSS,
              T_ABYSS,T_ABYSS,T_ABYSS,T_ABYSS,T_ABYSS,T_ABYSS,T_ABYSS,T_ABYSS,
              T_ABYSS,T_ABYSS,T_ABYSS,T_ABYSS,T_ABYSS,T_ABYSS,T_ABYSS,T_ABYSS,
              T_ABYSS,T_ABYSS,T_ABYSS,T_ABYSS,T_ABYSS,T_ABYSS),
    },
};

/* =======================================================================
 * battle_bg_render
 *
 * Writes the tile map for the given battle background into the specified
 * GBA screenblock, starting at tile row 10 (the lower half of the screen).
 *
 * Parameters:
 *   bg_id       — one of BBG_FOREST … BBG_SHADOW
 *   screenblock — GBA screenblock index (0-31); each block is 0x800 bytes
 * ======================================================================= */
void battle_bg_render(u8 bg_id, u8 screenblock)
{
    volatile u16* sb;
    const u16*    tiles;
    u16 i, j;

    if (bg_id >= BATTLE_BG_COUNT)
        return;

    /* Screenblock base in VRAM: 0x06000000 + screenblock * 0x800 */
    sb    = (volatile u16*)(0x06000000 + ((u32)screenblock * 0x800u));
    tiles = g_battle_bg[bg_id];

    /*
     * GBA tilemap is 32 tiles wide.  We write BATTLE_BG_W (30) tiles per
     * row into columns 0-29, starting at tile row 10 of the screenblock.
     */
    for (i = 0; i < BATTLE_BG_H; i++)
    {
        for (j = 0; j < BATTLE_BG_W; j++)
        {
            sb[((u16)10 + i) * 32u + j] = tiles[(u16)(i * BATTLE_BG_W) + j];
        }
    }
}
