/*
 * tileset_overworld.c
 * Chronicles of Aetheria
 *
 * 4bpp GBA tileset for overworld maps (grass, trees, water, paths, etc.).
 *
 * Encoding:
 *   Each tile is 8×8 pixels stored as 8 u32 values (one u32 per row).
 *   Within each u32 the lower nibble of byte 0 is the leftmost pixel
 *   (pixel 0) and the upper nibble of byte 3 is the rightmost pixel
 *   (pixel 7):
 *
 *     u32 row = p0 | (p1<<4) | (p2<<8) | (p3<<12)
 *             | (p4<<16) | (p5<<20) | (p6<<24) | (p7<<28);
 *
 * Palette:
 *   0  = transparent
 *   1  = dark green   (deep forest)
 *   2  = mid green    (grass)
 *   3  = light green  (light path / highlights)
 *   4  = blue         (water surface)
 *   5  = brown        (dirt / tree trunk)
 *   6  = dark brown   (rock)
 *   7  = yellow       (flower / sand highlight)
 *   8  = grey         (fence / stone)
 *   9  = white        (snow highlight / light)
 *   10 = dark blue    (deep water)
 *   11 = sand         (beach)
 *   12 = snow white   (snow field)
 *   13 = red          (red flower)
 *   14 = orange       (roof / autumn)
 *   15 = bright green (tall grass / sparkle)
 */

#include "../../include/gba/gba_types.h"

#define TILESET_OVERWORLD_COUNT 32

const u32 g_tileset_overworld[TILESET_OVERWORLD_COUNT][8] = {

    /* ------------------------------------------------------------------ */
    /* Tile 0: transparent — used as the empty/void tile                  */
    /* ------------------------------------------------------------------ */
    {
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000
    },

    /* ------------------------------------------------------------------ */
    /* Tile 1: grass — mid green base with subtle variation                */
    /* ------------------------------------------------------------------ */
    {
        0x22222222, /* . . . . . . . .  all mid-green */
        0x22322222, /* small light-green fleck col 2  */
        0x22222232, /* fleck col 6                    */
        0x23222222, /* fleck col 1                    */
        0x22222222,
        0x22232222, /* fleck col 3                    */
        0x22222322, /* fleck col 5                    */
        0x22222222
    },

    /* ------------------------------------------------------------------ */
    /* Tile 2: tree top — dark green mass with lighter centre             */
    /* ------------------------------------------------------------------ */
    {
        0x01111100, /* . D D D D D . .  dark-green crown */
        0x11131110, /* D D L D D D . .  L=light centre   */
        0x11311110, /* D D D L D D D .                   */
        0x11113110, /* D D D D L D D .                   */
        0x11131110, /* D D D L D D D .                   */
        0x11111110, /* D D D D D D D .                   */
        0x01111100, /* . D D D D D . .                   */
        0x00000000  /* (trunk handled by tile 17)         */
    },

    /* ------------------------------------------------------------------ */
    /* Tile 3: dirt path — brown base with lighter tread marks            */
    /* ------------------------------------------------------------------ */
    {
        0x55555555, /* brown solid                        */
        0x55655555, /* darker pebble                     */
        0x55555565, /* darker pebble                     */
        0x55555555,
        0x55565555, /* lighter wear mark                 */
        0x55555555,
        0x55655565, /* two pebbles                       */
        0x55555555
    },

    /* ------------------------------------------------------------------ */
    /* Tile 4: water — blue ripple pattern                                 */
    /* ------------------------------------------------------------------ */
    {
        0x44444444, /* flat blue                         */
        0x44A4444A, /* dark-blue ripple accents          */
        0x44444444,
        0xA444A444, /* ripple shifted                    */
        0x44444444,
        0x444A444A, /* ripple                            */
        0x44444444,
        0xA4444A44  /* ripple                            */
    },

    /* ------------------------------------------------------------------ */
    /* Tile 5: house wall — brown solid with mortar lines                 */
    /* ------------------------------------------------------------------ */
    {
        0x55555555,
        0x55555555,
        0x66666666, /* mortar line (dark brown)          */
        0x55555555,
        0x55555555,
        0x55555555,
        0x66666666,
        0x55555555
    },

    /* ------------------------------------------------------------------ */
    /* Tile 6: house door — dark-brown door with lighter frame            */
    /* ------------------------------------------------------------------ */
    {
        0x58888850, /* . frame frame frame frame frame . */
        0x56666650, /* . door  door  door  door  frame . */
        0x56666650,
        0x56666650,
        0x56679650, /* . door  door  knob  door  frame . */
        0x56666650,
        0x56666650,
        0x58888850  /* bottom frame                      */
    },

    /* ------------------------------------------------------------------ */
    /* Tile 7: fence horizontal — grey rails on mid-green                 */
    /* ------------------------------------------------------------------ */
    {
        0x22222222, /* grass                             */
        0x88888888, /* top rail                          */
        0x22222222,
        0x22222222,
        0x88888888, /* bottom rail                       */
        0x22222222,
        0x22222222,
        0x22222222
    },

    /* ------------------------------------------------------------------ */
    /* Tile 8: sign post — brown post, grey sign board                    */
    /* ------------------------------------------------------------------ */
    {
        0x28888820, /* . sign sign sign sign sign .      */
        0x28989820, /* . sign line sign line sign .      */
        0x28888820,
        0x20055020, /* . . trunk . trunk . . .           */
        0x20055020,
        0x20055020,
        0x22055022,
        0x22222222  /* grass base                        */
    },

    /* ------------------------------------------------------------------ */
    /* Tile 9: flower — green background, yellow centre petals            */
    /* ------------------------------------------------------------------ */
    {
        0x22222222,
        0x22277222, /* . . . Y Y . . .                   */
        0x22737722, /* . . Y * Y Y . .  *=centre         */
        0x22773722, /* . . Y Y * Y . .                   */
        0x22277222,
        0x22222322, /* small grass fleck                 */
        0x22222222,
        0x22222222
    },

    /* ------------------------------------------------------------------ */
    /* Tile 10: tall grass — bright green blades over mid green           */
    /* ------------------------------------------------------------------ */
    {
        0x22F22F22, /* blades tip                        */
        0x2FF22FF2,
        0xFF222FF2,
        0xF2222F22,
        0x22222222, /* ground level                      */
        0x22222222,
        0x22222222,
        0x22222222
    },

    /* ------------------------------------------------------------------ */
    /* Tile 11: cave entrance — dark void with rocky frame                */
    /* ------------------------------------------------------------------ */
    {
        0x66666666, /* rock ledge                        */
        0x61111116, /* rock . dark dark dark dark rock   */
        0x61111116,
        0x61111116, /* dark interior                     */
        0x61111116,
        0x66116616, /* rock supports at base             */
        0x22222222,
        0x22222222
    },

    /* ------------------------------------------------------------------ */
    /* Tile 12: stone floor — mid/light grey checkerboard                 */
    /* ------------------------------------------------------------------ */
    {
        0x88988988,
        0x98988898,
        0x88988988,
        0x98988898,
        0x88988988,
        0x98988898,
        0x88988988,
        0x98988898
    },

    /* ------------------------------------------------------------------ */
    /* Tile 13: sand / beach — warm yellow-beige flat                     */
    /* ------------------------------------------------------------------ */
    {
        0xBBBBBBBB,
        0xBBB7BBBB, /* slight highlight fleck            */
        0xBBBBBBBB,
        0xBBBBB7BB,
        0xBBBBBBBB,
        0xBB7BBBBB,
        0xBBBBBBBB,
        0xBBBBBBBB
    },

    /* ------------------------------------------------------------------ */
    /* Tile 14: water edge top — water below, grass above                 */
    /* ------------------------------------------------------------------ */
    {
        0x22222222, /* grass                             */
        0x22222222,
        0x33333333, /* light-green transition strip      */
        0x44444444, /* water starts                      */
        0x44A4444A,
        0x44444444,
        0xA444A444,
        0x44444444
    },

    /* ------------------------------------------------------------------ */
    /* Tile 15: water edge left — water right, grass left                 */
    /* ------------------------------------------------------------------ */
    {
        0x24444444,
        0x23444444, /* grass col 0, transition col 1     */
        0x24444444,
        0x23444444,
        0x24A44444,
        0x23444444,
        0x24444444,
        0x23444444
    },

    /* ------------------------------------------------------------------ */
    /* Tile 16: house roof — orange/red pitched shingles                  */
    /* ------------------------------------------------------------------ */
    {
        0x00000E00, /* . . . . . E . .  peak             */
        0x000EEE00, /* . . . E E E . .                   */
        0x00EEEEE0, /* . . E E E E E .                   */
        0x0EEEEEEE, /* . E E E E E E E                   */
        0xEEEEEEEE, /* full row                          */
        0xDEDEDEDE, /* shingle texture (alt orange)      */
        0xEDEDEDED,
        0xEEEEEEEE
    },

    /* ------------------------------------------------------------------ */
    /* Tile 17: tree trunk — brown pillar on grass                        */
    /* ------------------------------------------------------------------ */
    {
        0x22555522,
        0x22556522, /* lighter inner highlight           */
        0x22555522,
        0x22555522,
        0x22555522,
        0x22555522,
        0x25555552, /* roots spreading                   */
        0x22222222
    },

    /* ------------------------------------------------------------------ */
    /* Tile 18: fence post vertical — grey post on grass                  */
    /* ------------------------------------------------------------------ */
    {
        0x22282222,
        0x22282222,
        0x22282222,
        0x22282222,
        0x22282222,
        0x22282222,
        0x22282222,
        0x22282222
    },

    /* ------------------------------------------------------------------ */
    /* Tile 19: bridge horizontal — brown planks over water               */
    /* ------------------------------------------------------------------ */
    {
        0x55555555, /* plank top                         */
        0x56555655, /* grain lines                       */
        0x55555555,
        0x55655565,
        0x55555555,
        0x56555655,
        0x55555555,
        0x44444444  /* water visible below               */
    },

    /* ------------------------------------------------------------------ */
    /* Tile 20: bridge vertical — planks running north-south              */
    /* ------------------------------------------------------------------ */
    {
        0x54444445,
        0x55444455,
        0x54444445,
        0x55444455,
        0x54444445,
        0x55444455,
        0x54444445,
        0x55444455
    },

    /* ------------------------------------------------------------------ */
    /* Tile 21: snow — white field with subtle blue shadows               */
    /* ------------------------------------------------------------------ */
    {
        0xCCCCCCCC,
        0xCCC9CCCC, /* white sparkle                     */
        0xCCCCCCCC,
        0xCC4CCCCC, /* blue shadow dip                   */
        0xCCCCCCCC,
        0xCCCC9CCC,
        0xCCCCCCCC,
        0xC4CCCCCC
    },

    /* ------------------------------------------------------------------ */
    /* Tile 22: ice — pale blue, glassy sheen                             */
    /* ------------------------------------------------------------------ */
    {
        0xAAAAAAAA, /* deep-water reused as ice blue     */
        0xA9AAAAAA, /* white glint                       */
        0xAAAA9AAA,
        0xAAAAAAAA,
        0x9AAAAAAA,
        0xAAAAAA9A,
        0xAAAAAAAA,
        0xAA9AAAAA
    },

    /* ------------------------------------------------------------------ */
    /* Tile 23: lava — red/orange bubbling pattern                        */
    /* ------------------------------------------------------------------ */
    {
        0xDDEDDDDD, /* orange bubble highlight           */
        0xDDDDDDDD,
        0xEDDDEDDD,
        0xDDDDDDDD,
        0xDDEDDDED,
        0xDDDDDDDD,
        0xEDDDDDDD,
        0xDDDDEDDD
    },

    /* ------------------------------------------------------------------ */
    /* Tile 24: swamp — murky dark green / brown mix                      */
    /* ------------------------------------------------------------------ */
    {
        0x15151515,
        0x51151551,
        0x15515115,
        0x51155151,
        0x15151515,
        0x51511551,
        0x15151515,
        0x51555155
    },

    /* ------------------------------------------------------------------ */
    /* Tile 25: mountain rock — grey with darker cracks                   */
    /* ------------------------------------------------------------------ */
    {
        0x88888888,
        0x88688888, /* crack                             */
        0x86688688,
        0x88886688,
        0x88888688,
        0x68888886,
        0x88688888,
        0x66666666  /* base shadow                       */
    },

    /* ------------------------------------------------------------------ */
    /* Tile 26: chest closed — brown chest with gold latch                */
    /* ------------------------------------------------------------------ */
    {
        0x05555500, /* . brown brown brown brown . .     */
        0x05757500, /* . brown gold brown gold . .  lid  */
        0x05555500,
        0x07777700, /* gold band                         */
        0x05575500, /* . brown brown gold brown . .  latch */
        0x05555500,
        0x05555500,
        0x00000000
    },

    /* ------------------------------------------------------------------ */
    /* Tile 27: chest open — brown chest, dark interior, gold trim        */
    /* ------------------------------------------------------------------ */
    {
        0x05555500,
        0x06116600, /* open lid revealing dark inside    */
        0x06116600,
        0x07777700, /* gold band                         */
        0x05555500,
        0x05555500,
        0x05555500,
        0x00000000
    },

    /* ------------------------------------------------------------------ */
    /* Tile 28: crystal — blue/purple faceted gem                         */
    /* ------------------------------------------------------------------ */
    {
        0x00040000,
        0x00444900, /* blue with white glint             */
        0x04449440,
        0x04444440,
        0x44444444,
        0x04444440,
        0x00444400,
        0x00004000
    },

    /* ------------------------------------------------------------------ */
    /* Tile 29: warp tile — sparkle / teleport pad                        */
    /* ------------------------------------------------------------------ */
    {
        0x00F0F000, /* bright-green sparkle cross        */
        0x0F00F0F0,
        0xF0F0F0F0,
        0x0F00F0F0, /* rotation suggests animation       */
        0xF0F00F0F,
        0x0FF0FF00,
        0xF0F0F0F0,
        0x00F0F000
    },

    /* ------------------------------------------------------------------ */
    /* Tile 30: bookshelf — brown wood, coloured book spines              */
    /* ------------------------------------------------------------------ */
    {
        0x55555555, /* wood top                          */
        0x5DEDEDED, /* book spines: D=orange E=rooftile  */
        0x5DEDEDED,
        0x5DEDEDED,
        0x55555555, /* shelf divider                     */
        0x5D4D7D4D, /* second row: orange blue yellow    */
        0x5D4D7D4D,
        0x55555555  /* wood base                         */
    },

    /* ------------------------------------------------------------------ */
    /* Tile 31: notice board — grey board, brown post, text lines         */
    /* ------------------------------------------------------------------ */
    {
        0x08888800, /* . grey grey grey grey grey . .    */
        0x08989800, /* . line  . line  . line  . .       */
        0x08888800,
        0x08989800,
        0x08888800, /* bottom of board                   */
        0x00255200, /* . . post post post . . .          */
        0x00255200,
        0x22222222  /* grass                             */
    },
};

u16 tileset_overworld_count(void) { return TILESET_OVERWORLD_COUNT; }
