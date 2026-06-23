#include "../../include/game/game_states.h"

/* =========================================================
 * Title Screen State
 * Shows "CHRONICLES OF AETHERIA" + blinking "PRESS START"
 * using an embedded 8x8 1bpp font rendered into VRAM.
 * ========================================================= */

/* 8x8 1bpp font: A-Z (indices 0-25), space (index 26) */
static const u8 s_font[27][8] = {
    {0x18,0x3C,0x66,0x7E,0x66,0x66,0x66,0x00}, /* A */
    {0x7C,0x66,0x66,0x7C,0x66,0x66,0x7C,0x00}, /* B */
    {0x3C,0x66,0x60,0x60,0x60,0x66,0x3C,0x00}, /* C */
    {0x78,0x6C,0x66,0x66,0x66,0x6C,0x78,0x00}, /* D */
    {0x7E,0x60,0x60,0x7C,0x60,0x60,0x7E,0x00}, /* E */
    {0x7E,0x60,0x60,0x7C,0x60,0x60,0x60,0x00}, /* F */
    {0x3C,0x66,0x60,0x6E,0x66,0x66,0x3C,0x00}, /* G */
    {0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00}, /* H */
    {0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00}, /* I */
    {0x1E,0x0C,0x0C,0x0C,0x6C,0x6C,0x38,0x00}, /* J */
    {0x66,0x6C,0x78,0x70,0x78,0x6C,0x66,0x00}, /* K */
    {0x60,0x60,0x60,0x60,0x60,0x60,0x7E,0x00}, /* L */
    {0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x00}, /* M */
    {0x66,0x76,0x7E,0x6E,0x66,0x66,0x66,0x00}, /* N */
    {0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00}, /* O */
    {0x7C,0x66,0x66,0x7C,0x60,0x60,0x60,0x00}, /* P */
    {0x3C,0x66,0x66,0x66,0x6E,0x3C,0x06,0x00}, /* Q */
    {0x7C,0x66,0x66,0x7C,0x78,0x6C,0x66,0x00}, /* R */
    {0x3C,0x66,0x60,0x3C,0x06,0x66,0x3C,0x00}, /* S */
    {0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00}, /* T */
    {0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x00}, /* U */
    {0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00}, /* V */
    {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00}, /* W */
    {0x66,0x66,0x3C,0x18,0x3C,0x66,0x66,0x00}, /* X */
    {0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00}, /* Y */
    {0x7E,0x06,0x0C,0x18,0x30,0x60,0x7E,0x00}, /* Z */
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, /* space */
};

static u32 s_blink_timer;
static BOOL s_press_start_visible;

/* Write 1bpp font glyph as a 4bpp tile into charblock 0.
 * Lit pixels get palette index fg_col; dark pixels get 0 (transparent). */
static void _load_font_tile(u8 tile_idx, u8 font_idx, u8 fg_col)
{
    u8 row;
    for (row = 0; row < 8; row++) {
        u8  bits = s_font[font_idx][row];
        u32 word = 0;
        u8  px;
        for (px = 0; px < 8; px++) {
            u32 c = ((bits >> (7 - px)) & 1) ? fg_col : 0u;
            word |= c << (px * 4);
        }
        TILE_MEM[0][tile_idx][row] = word;
    }
}

/* Write text string to BG0 map (screenblock 30), centered on the given row.
 * pal_bank selects which 16-colour BG palette bank (bits 12-15 of map entry). */
static void _draw_text(const char* str, u8 map_row, u8 pal_bank)
{
    u8 len = 0;
    const char* p = str;
    u8 col;
    while (*p++) len++;
    col = (30 - len) / 2;
    while (*str) {
        u16 tile = (*str == ' ') ? 0u : (u16)(*str - 'A' + 1);
        MAP_MEM[30][map_row][col] = tile | ((u16)pal_bank << 12);
        col++;
        str++;
    }
}

static void _draw_press_start(BOOL visible)
{
    static const char k_text[] = "PRESS START";
    const char* p = k_text;
    u8 col = (30 - 11) / 2;   /* 11 = strlen("PRESS START") */
    u8 row = 14;
    while (*p) {
        u16 tile = (*p == ' ') ? 0u : (u16)(*p - 'A' + 1);
        /* pal_bank 1 = white; blank tile (0) when hidden */
        MAP_MEM[30][row][col] = visible ? (tile | (1u << 12)) : 0u;
        col++;
        p++;
    }
}

void state_title_enter(void)
{
    u8 i;
    u8 r, c;

    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_MAP_1D;

    tile_bg_setup(BG_LAYER_UI, 0, 30, 0, 0, BG_SIZE_256x256);
    tile_bg_enable(BG_LAYER_UI, TRUE);

    /* Palette bank 0: dark-blue bg, gold text (title) */
    PAL_BG_MEM[0]  = RGB15(2,  2,  8);   /* dark blue — backdrop & transparent pixels */
    PAL_BG_MEM[1]  = RGB15(31, 24,  0);  /* gold */

    /* Palette bank 1: same bg, white text ("PRESS START") */
    PAL_BG_MEM[16] = RGB15(2,  2,  8);   /* dark blue */
    PAL_BG_MEM[17] = RGB15(31, 31, 31);  /* white */

    /* Tile 0 = blank space (all zeros = transparent) */
    for (i = 0; i < 8; i++) TILE_MEM[0][0][i] = 0;

    /* Tiles 1-26 = A-Z, colour index 1 (mapped by pal bank at draw time) */
    for (i = 0; i < 26; i++)
        _load_font_tile((u8)(i + 1), i, 1);

    /* Clear visible portion of screenblock 30 */
    for (r = 0; r < 20; r++)
        for (c = 0; c < 30; c++)
            MAP_MEM[30][r][c] = 0;

    /* Draw title at row 6 (gold, pal bank 0) */
    _draw_text("CHRONICLES OF AETHERIA", 6, 0);

    /* Draw initial "PRESS START" */
    s_blink_timer         = 0;
    s_press_start_visible = TRUE;
    _draw_press_start(TRUE);
}

void state_title_exit(void)
{
    tile_bg_enable(BG_LAYER_UI, FALSE);
    sprite_clear_all();
}

void state_title_update(void)
{
    s_blink_timer++;
    if (s_blink_timer >= 30) {
        s_blink_timer         = 0;
        s_press_start_visible = !s_press_start_visible;
        _draw_press_start(s_press_start_visible);
    }

    if (key_pressed(KEY_START))
        state_change(STATE_OVERWORLD);

    if (key_pressed(KEY_SELECT))
        state_push(STATE_MENU);
}

void state_title_render(void)
{
    /* BG0 tile map is updated in update(); nothing to do per-frame */
}

const GameState g_state_title = {
    .id       = STATE_TITLE,
    .on_enter = state_title_enter,
    .on_exit  = state_title_exit,
    .update   = state_title_update,
    .render   = state_title_render,
};
