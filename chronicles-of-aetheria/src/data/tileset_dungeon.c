/*
 * tileset_dungeon.c
 * Chronicles of Aetheria
 *
 * 4bpp GBA tileset for dungeon / cave environments.
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
 *   1  = black          (void / shadow)
 *   2  = dark grey      (stone wall)
 *   3  = mid grey       (stone surface)
 *   4  = light grey     (floor)
 *   5  = brown          (dirt / wood)
 *   6  = dark green     (moss)
 *   7  = orange         (torch flame)
 *   8  = yellow         (torch glow / candle)
 *   9  = blue           (water pool / magic)
 *   10 = red            (lava)
 *   11 = dark red       (lava shadow / blood)
 *   12 = purple         (magic circle / crystal)
 *   13 = white          (bone / skull / highlight)
 *   14 = dark brown     (aged wood / sarcophagus)
 *   15 = bright gold    (treasure / altar trim)
 */

#include "../../include/gba/gba_types.h"

#define TILESET_DUNGEON_COUNT 24

const u32 g_tileset_dungeon[TILESET_DUNGEON_COUNT][8] = {

    /* ------------------------------------------------------------------ */
    /* Tile 0: transparent                                                 */
    /* ------------------------------------------------------------------ */
    {
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000
    },

    /* ------------------------------------------------------------------ */
    /* Tile 1: stone wall solid — dark grey mass with rough texture       */
    /* ------------------------------------------------------------------ */
    {
        0x22222222,
        0x22322222, /* mid-grey pebble                   */
        0x22222322,
        0x23222232,
        0x22232222,
        0x22222222,
        0x32222223,
        0x22222222
    },

    /* ------------------------------------------------------------------ */
    /* Tile 2: stone floor — light grey with subtle grout lines           */
    /* ------------------------------------------------------------------ */
    {
        0x33333333, /* mid-grey top grout line           */
        0x34444443,
        0x34444443,
        0x34444443,
        0x33333333, /* horizontal grout                  */
        0x34444443,
        0x34444443,
        0x33333333
    },

    /* ------------------------------------------------------------------ */
    /* Tile 3: dungeon door closed — heavy iron door in stone frame       */
    /* ------------------------------------------------------------------ */
    {
        0x22222222, /* stone header                      */
        0x21111112, /* stone . door door door door stone */
        0x21221112, /* . iron iron iron iron iron .      */
        0x21221112,
        0x21F21112, /* keyhole highlight (yellow)        */
        0x21221112,
        0x21111112,
        0x22222222  /* stone threshold                   */
    },

    /* ------------------------------------------------------------------ */
    /* Tile 4: dungeon door open — open archway into black void           */
    /* ------------------------------------------------------------------ */
    {
        0x22222222,
        0x21111112,
        0x21111112, /* black void interior               */
        0x21111112,
        0x21111112,
        0x21111112,
        0x22222222,
        0x22222222
    },

    /* ------------------------------------------------------------------ */
    /* Tile 5: torch frame 1 — flame larger, brighter                     */
    /* ------------------------------------------------------------------ */
    {
        0x00080000, /* . . . Y . . . .  top flicker      */
        0x00787000, /* . . Y O Y . . .  O=orange Y=yellow */
        0x07878700, /* . Y O Y O Y . .                   */
        0x07777700, /* . Y Y Y Y Y . .                   */
        0x07878700, /* . Y O Y O Y . .                   */
        0x00555000, /* . . bkr bkr bkr . .  bracket      */
        0x00505000,
        0x00000000
    },

    /* ------------------------------------------------------------------ */
    /* Tile 6: torch frame 2 — flame shifted, animation pair with tile 5  */
    /* ------------------------------------------------------------------ */
    {
        0x00700000, /* . . Y . . . . .  tip shifted left  */
        0x00877000, /* . . Y O Y . . .                   */
        0x07787700, /* . Y Y O Y Y . .                   */
        0x07777700,
        0x00787000, /* . . Y O Y . . .  base contracts    */
        0x00555000,
        0x00505000,
        0x00000000
    },

    /* ------------------------------------------------------------------ */
    /* Tile 7: chest — brown chest with gold hardware                     */
    /* ------------------------------------------------------------------ */
    {
        0x05555500,
        0x05F5F500, /* gold studs on lid                 */
        0x05555500,
        0x0FFFFFFF, /* full gold band                    */
        0x05F55500, /* gold latch                        */
        0x05555500,
        0x05555500,
        0x00000000
    },

    /* ------------------------------------------------------------------ */
    /* Tile 8: sarcophagus — stone coffin with carved face                */
    /* ------------------------------------------------------------------ */
    {
        0x03333300,
        0x03D3D300, /* bone/white eyes carved in stone   */
        0x03333300,
        0x03D3D300, /* nose/mouth lines                  */
        0x03D3D300,
        0x03333300,
        0x03333300,
        0x00000000
    },

    /* ------------------------------------------------------------------ */
    /* Tile 9: stairs down — grey steps descending into black             */
    /* ------------------------------------------------------------------ */
    {
        0x44444444, /* top landing                       */
        0x33333333, /* riser                             */
        0x34444443,
        0x23333332, /* riser deeper                      */
        0x24444442,
        0x12222221,
        0x11111111, /* bottom — black void               */
        0x11111111
    },

    /* ------------------------------------------------------------------ */
    /* Tile 10: stairs up — steps rising from dungeon floor               */
    /* ------------------------------------------------------------------ */
    {
        0x33333333, /* top stair (bright)                */
        0x34444443,
        0x33333333,
        0x44444444,
        0x33333333,
        0x34444443,
        0x33333333,
        0x44444444  /* floor landing                     */
    },

    /* ------------------------------------------------------------------ */
    /* Tile 11: trap floor — stone floor with a hidden pressure plate     */
    /* ------------------------------------------------------------------ */
    {
        0x44444444,
        0x43333334,
        0x43434343, /* plate seam lines                  */
        0x43434343,
        0x43333334,
        0x43434343,
        0x43434343,
        0x44444444
    },

    /* ------------------------------------------------------------------ */
    /* Tile 12: water pool — dark blue with lighter ripples               */
    /* ------------------------------------------------------------------ */
    {
        0x99999999,
        0x99199919, /* blue ripple highlight             */
        0x99999999,
        0x91999199,
        0x99999999,
        0x99919991,
        0x99999999,
        0x91999199
    },

    /* ------------------------------------------------------------------ */
    /* Tile 13: lava pool — red/orange bubbling surface                   */
    /* ------------------------------------------------------------------ */
    {
        0xAAAAAAAA, /* red base                          */
        0xAA7AAAAA, /* orange bubble                     */
        0xAAAAAAAA,
        0xA7AAA7AA,
        0xAAAAAAAA,
        0xAAA7AAAA,
        0x7AAAAAAA,
        0xAAAA7AAA
    },

    /* ------------------------------------------------------------------ */
    /* Tile 14: pillar top — cap of a stone column                        */
    /* ------------------------------------------------------------------ */
    {
        0x33333333, /* wide cap top                      */
        0x32222223, /* inset body                        */
        0x32222223,
        0x32222223,
        0x32222223,
        0x33333333, /* base flange                       */
        0x03333330, /* narrowing body begins             */
        0x03322330
    },

    /* ------------------------------------------------------------------ */
    /* Tile 15: pillar middle — shaft of stone column                     */
    /* ------------------------------------------------------------------ */
    {
        0x03322330,
        0x03322330,
        0x03322330,
        0x03322330,
        0x03322330,
        0x03322330,
        0x03322330,
        0x03322330
    },

    /* ------------------------------------------------------------------ */
    /* Tile 16: wall torch — bracket mounted on stone wall                */
    /* ------------------------------------------------------------------ */
    {
        0x22022022, /* stone wall with bracket gap       */
        0x22870722, /* . . Y O Y . . .  flame           */
        0x22787722, /* . . Y O Y . . .                  */
        0x22777722, /* . . Y Y Y . . .                  */
        0x22050022, /* . . bracket . . .                 */
        0x22222222, /* wall                              */
        0x22222222,
        0x22222222
    },

    /* ------------------------------------------------------------------ */
    /* Tile 17: mossy wall — dark-grey stone covered in patches of green  */
    /* ------------------------------------------------------------------ */
    {
        0x22222222,
        0x26622226, /* moss patches                      */
        0x26662226,
        0x22222222,
        0x22622222,
        0x26666622,
        0x22666222,
        0x22222222
    },

    /* ------------------------------------------------------------------ */
    /* Tile 18: cracked wall — stone wall with visible fracture lines     */
    /* ------------------------------------------------------------------ */
    {
        0x22222222,
        0x22122222, /* crack starts top                  */
        0x21122222,
        0x22112222, /* crack widens                      */
        0x22211222,
        0x22221222,
        0x22222122, /* crack exits bottom-right          */
        0x22222222
    },

    /* ------------------------------------------------------------------ */
    /* Tile 19: secret door — looks like a wall but has subtle seam       */
    /* ------------------------------------------------------------------ */
    {
        0x22222222,
        0x22322222,
        0x22232222,
        0x33333333, /* horizontal seam — barely visible  */
        0x22232222,
        0x22322222,
        0x22232222,
        0x22222222
    },

    /* ------------------------------------------------------------------ */
    /* Tile 20: magic circle — glowing purple rune circle on stone floor  */
    /* ------------------------------------------------------------------ */
    {
        0x44C4C444, /* . . P . P . . .  P=purple         */
        0x4C444C44, /* . P . . . P . .                   */
        0xC4444444, /* P . . . . . . .                   */
        0x4C4CC4C4, /* rune spokes                       */
        0x4C4CC4C4,
        0xC4444444,
        0x4C444C44,
        0x44C4C444
    },

    /* ------------------------------------------------------------------ */
    /* Tile 21: crystal pillar — glowing blue/purple gem column           */
    /* ------------------------------------------------------------------ */
    {
        0x09C9C900, /* . B P B P B . .  B=blue P=purple  */
        0x09C99C90, /* . B P . . P B .                   */
        0x09999990, /* . B B B B B B .                   */
        0x09C9C990, /* facets                            */
        0x09C9C990,
        0x09999990,
        0x09C99C90,
        0x09C9C900
    },

    /* ------------------------------------------------------------------ */
    /* Tile 22: bone pile — scattered bones and skull on dungeon floor    */
    /* ------------------------------------------------------------------ */
    {
        0x44444444, /* floor                             */
        0x44D44D44, /* . . bone . bone . . .  D=white    */
        0x4DD44DD4, /* bone pile                         */
        0xDDD4DDDD, /* dense pile centre                 */
        0xDDDDDDDD, /* skull row                         */
        0x4DDDDDD4,
        0x44D4D444,
        0x44444444
    },

    /* ------------------------------------------------------------------ */
    /* Tile 23: altar — stone platform with gold trim and red cloth       */
    /* ------------------------------------------------------------------ */
    {
        0x3FFFFFFF, /* gold trim along top               */
        0x3AAAAAAF, /* red/dark-red cloth surface        */
        0x3AAAAAAF,
        0x3FFFFFFF, /* gold middle band                  */
        0x33333333, /* stone base begins                 */
        0x32222223,
        0x32222223,
        0x33333333
    },
};

u16 tileset_dungeon_count(void) { return TILESET_DUNGEON_COUNT; }
