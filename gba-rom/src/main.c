#include "gba.h"

// ============================================================
// COLOR PALETTE
// ============================================================
#define C_BLACK   RGB15( 0, 0, 0)
#define C_WHITE   RGB15(31,31,31)
#define C_GRAY    RGB15(15,15,15)
#define C_DGRAY   RGB15( 8, 8, 8)
#define C_RED     RGB15(31, 4, 4)
#define C_DRED    RGB15(16, 0, 0)
#define C_GREEN   RGB15( 4,28, 4)
#define C_LGREEN  RGB15(12,31,12)
#define C_BLUE    RGB15( 4, 8,31)
#define C_LBLUE   RGB15(12,20,31)
#define C_YELLOW  RGB15(31,28, 4)
#define C_ORANGE  RGB15(31,18, 0)
#define C_CYAN    RGB15( 0,28,28)
#define C_PURPLE  RGB15(20, 0,28)
#define C_PINK    RGB15(31, 8,20)
#define C_BROWN   RGB15(18, 9, 0)
#define C_SKIN    RGB15(31,24,18)
#define C_NAVY    RGB15( 0, 0,18)
#define C_DBLUE   RGB15( 2, 5,20)
#define C_TEAL    RGB15( 0,18,18)

// ============================================================
// FORWARD DECLARATIONS
// ============================================================
typedef struct { void (*init)(void); int (*update)(void); void (*draw)(void); const char *name; } Game;

// ============================================================
// GAME: MENU
// ============================================================
#define NUM_GAMES 10
static int menu_sel = 0;
static int menu_timer = 0;
static const char *GAME_NAMES[NUM_GAMES] = {
    "1. SNAKE", "2. TETRIS", "3. PONG", "4. JOGO DA VELHA",
    "5. BREAKOUT", "6. FLAPPY BIRD", "7. SPACE INVADERS",
    "8. MINESWEEPER", "9. MEMORY MATCH", "10. PAC-MAN"
};
static int hi_scores[NUM_GAMES] = {0};
static int current_game = -1; // -1 = menu

// ============================================================
// UTILITIES
// ============================================================
static void draw_panel(int x, int y, int w, int h, uint16_t bg, uint16_t border) {
    m3_rect(x, y, w, h, bg);
    m3_border(x, y, w, h, border);
    m3_border(x+1, y+1, w-2, h-2, border);
}

static void draw_hud(const char *name, int score, int hi) {
    m3_rect(0, 0, SCREEN_W, 12, C_DGRAY);
    m3_hline(0, 12, SCREEN_W, C_GRAY);
    draw_str(2, 2, name, C_CYAN);
    draw_str(90, 2, "SC:", C_WHITE);
    draw_int(108, 2, score, C_YELLOW);
    draw_str(148, 2, "HI:", C_WHITE);
    draw_int(166, 2, hi, C_YELLOW);
    draw_str(198, 2, "A:OK B:MENU", C_GRAY);
}

static void draw_gameover(int score) {
    draw_panel(50, 55, 140, 55, C_DBLUE, C_YELLOW);
    draw_str_centered(62, "GAME OVER", C_RED);
    draw_str(76, 74, "SCORE:", C_WHITE);
    draw_int(112, 74, score, C_YELLOW);
    draw_str_centered(86, "A-REINICIAR", C_LGREEN);
    draw_str_centered(96, "B-MENU", C_GRAY);
}

static void draw_win(const char *msg) {
    draw_panel(50, 55, 140, 55, C_NAVY, C_LGREEN);
    draw_str_centered(62, "VOCE GANHOU!", C_LGREEN);
    draw_str_centered(74, msg, C_YELLOW);
    draw_str_centered(86, "A-REINICIAR", C_CYAN);
    draw_str_centered(96, "B-MENU", C_GRAY);
}

// ============================================================
// GAME 1: SNAKE
// ============================================================
#define SN_COLS 29
#define SN_ROWS 18
#define SN_CS   8
#define SN_OX   4
#define SN_OY   14

static struct {
    int bx[SN_COLS*SN_ROWS], by[SN_COLS*SN_ROWS];
    int len, dx, dy, ndx, ndy;
    int fx, fy;
    int score, hi;
    int timer, speed;
    int state; // 0=play 1=dead
} sn;

static void sn_spawn_food(void) {
    int ok = 0;
    while (!ok) {
        sn.fx = (int)(rng() % SN_COLS);
        sn.fy = (int)(rng() % SN_ROWS);
        ok = 1;
        for (int i = 0; i < sn.len; i++)
            if (sn.bx[i]==sn.fx && sn.by[i]==sn.fy) { ok=0; break; }
    }
}

static void sn_init(void) {
    sn.len=3; sn.dx=1; sn.dy=0; sn.ndx=1; sn.ndy=0;
    sn.bx[0]=14; sn.by[0]=9;
    sn.bx[1]=13; sn.by[1]=9;
    sn.bx[2]=12; sn.by[2]=9;
    sn.score=0; sn.timer=0; sn.speed=8; sn.state=0;
    sn_spawn_food();
}

static int sn_update(void) {
    if (sn.state==1) {
        uint16_t p = key_pressed();
        if (p & KEY_A) sn_init();
        if (p & KEY_B) return -1;
        return 0;
    }
    uint16_t k = key_down();
    uint16_t p = key_pressed();
    if ((p & KEY_UP)    && sn.dy==0) { sn.ndx=0; sn.ndy=-1; }
    if ((p & KEY_DOWN)  && sn.dy==0) { sn.ndx=0; sn.ndy= 1; }
    if ((p & KEY_LEFT)  && sn.dx==0) { sn.ndx=-1; sn.ndy=0; }
    if ((p & KEY_RIGHT) && sn.dx==0) { sn.ndx= 1; sn.ndy=0; }
    (void)k;

    if (++sn.timer < sn.speed) return 0;
    sn.timer = 0;
    sn.dx = sn.ndx; sn.dy = sn.ndy;

    int nx = sn.bx[0]+sn.dx, ny = sn.by[0]+sn.dy;
    if (nx<0||nx>=SN_COLS||ny<0||ny>=SN_ROWS) { sn.state=1; return 0; }
    for (int i=0;i<sn.len;i++) if (sn.bx[i]==nx&&sn.by[i]==ny) { sn.state=1; return 0; }

    for (int i=sn.len;i>0;i--) { sn.bx[i]=sn.bx[i-1]; sn.by[i]=sn.by[i-1]; }
    sn.bx[0]=nx; sn.by[0]=ny;

    if (nx==sn.fx && ny==sn.fy) {
        sn.len++;
        sn.score++;
        if (sn.score > sn.hi) sn.hi=sn.score;
        if (sn.score%5==0 && sn.speed>3) sn.speed--;
        sn_spawn_food();
    }
    return 0;
}

static void sn_draw(void) {
    m3_fill(RGB15(2,4,2));
    // grid
    for (int x=0;x<SN_COLS;x++) for (int y=0;y<SN_ROWS;y++) {
        uint16_t c = ((x+y)&1) ? RGB15(2,6,2) : RGB15(2,5,2);
        m3_rect(SN_OX+x*SN_CS, SN_OY+y*SN_CS, SN_CS-1, SN_CS-1, c);
    }
    // food (pulsing red apple)
    int fx = SN_OX+sn.fx*SN_CS, fy = SN_OY+sn.fy*SN_CS;
    m3_rect(fx+1,fy+1,SN_CS-2,SN_CS-2, C_RED);
    m3_pixel(fx+2,fy+2, C_WHITE);
    m3_pixel(fx+4,fy+1, C_LGREEN);

    // snake body
    for (int i=sn.len-1;i>=0;i--) {
        int px=SN_OX+sn.bx[i]*SN_CS, py=SN_OY+sn.by[i]*SN_CS;
        uint16_t c = (i==0) ? C_LGREEN : C_GREEN;
        m3_rect(px+1,py+1,SN_CS-2,SN_CS-2, c);
        if (i==0) {
            // head detail
            m3_pixel(px+1,py+1, C_WHITE);
            m3_pixel(px+5,py+1, C_WHITE);
        }
    }
    m3_border(SN_OX-1, SN_OY-1, SN_COLS*SN_CS+2, SN_ROWS*SN_CS+2, C_GREEN);
    draw_hud("SNAKE", sn.score, sn.hi);
    if (sn.state==1) draw_gameover(sn.score);
}

// ============================================================
// GAME 2: TETRIS
// ============================================================
#define TT_COLS 10
#define TT_ROWS 18
#define TT_CS   8
#define TT_OX   44
#define TT_OY   14

static const int TT_PIECES[7][4][4][2] = {
    {{{0,0},{1,0},{2,0},{3,0}}, {{1,0},{1,1},{1,2},{1,3}}, {{0,0},{1,0},{2,0},{3,0}}, {{1,0},{1,1},{1,2},{1,3}}}, // I
    {{{0,0},{0,1},{1,1},{2,1}}, {{0,0},{1,0},{0,1},{0,2}}, {{0,0},{1,0},{2,0},{2,1}}, {{1,0},{1,1},{0,2},{1,2}}}, // J
    {{{2,0},{0,1},{1,1},{2,1}}, {{0,0},{0,1},{0,2},{1,2}}, {{0,0},{1,0},{2,0},{0,1}}, {{0,0},{1,0},{1,1},{1,2}}}, // L
    {{{0,0},{1,0},{0,1},{1,1}}, {{0,0},{1,0},{0,1},{1,1}}, {{0,0},{1,0},{0,1},{1,1}}, {{0,0},{1,0},{0,1},{1,1}}}, // O
    {{{1,0},{2,0},{0,1},{1,1}}, {{0,0},{0,1},{1,1},{1,2}}, {{1,0},{2,0},{0,1},{1,1}}, {{0,0},{0,1},{1,1},{1,2}}}, // S
    {{{1,0},{0,1},{1,1},{2,1}}, {{0,0},{0,1},{1,1},{0,2}}, {{0,0},{1,0},{2,0},{1,1}}, {{1,0},{0,1},{1,1},{1,2}}}, // T
    {{{0,0},{1,0},{1,1},{2,1}}, {{1,0},{0,1},{1,1},{0,2}}, {{0,0},{1,0},{1,1},{2,1}}, {{1,0},{0,1},{1,1},{0,2}}}, // Z
};
static const uint16_t TT_COLORS[7] = { C_CYAN, C_BLUE, C_ORANGE, C_YELLOW, C_LGREEN, C_PURPLE, C_RED };

static struct {
    int board[TT_ROWS][TT_COLS];
    uint16_t bcolor[TT_ROWS][TT_COLS];
    int piece, rot, px, py;
    int next_piece;
    int score, hi, lines, level;
    int timer, drop_speed;
    int state; // 0=play 1=over
    int flash_timer, flash_row;
} tt;

static void tt_new_piece(void) {
    tt.piece = tt.next_piece;
    tt.next_piece = (int)(rng() % 7);
    tt.rot = 0; tt.px = 4; tt.py = 0;
}

static int tt_collide(int piece, int rot, int px, int py) {
    for (int i=0;i<4;i++) {
        int bx = px + TT_PIECES[piece][rot][i][0];
        int by = py + TT_PIECES[piece][rot][i][1];
        if (bx<0||bx>=TT_COLS||by>=TT_ROWS) return 1;
        if (by>=0 && tt.board[by][bx]) return 1;
    }
    return 0;
}

static void tt_place(void) {
    for (int i=0;i<4;i++) {
        int bx = tt.px + TT_PIECES[tt.piece][tt.rot][i][0];
        int by = tt.py + TT_PIECES[tt.piece][tt.rot][i][1];
        if (by>=0) { tt.board[by][bx]=1; tt.bcolor[by][bx]=TT_COLORS[tt.piece]; }
    }
    // clear full lines
    int cleared=0;
    for (int y=TT_ROWS-1;y>=0;y--) {
        int full=1;
        for (int x=0;x<TT_COLS;x++) if (!tt.board[y][x]) { full=0; break; }
        if (full) {
            cleared++;
            for (int r=y;r>0;r--)
                for (int c=0;c<TT_COLS;c++) { tt.board[r][c]=tt.board[r-1][c]; tt.bcolor[r][c]=tt.bcolor[r-1][c]; }
            for (int c=0;c<TT_COLS;c++) { tt.board[0][c]=0; }
            y++;
        }
    }
    int pts[5]={0,100,300,500,800};
    if (cleared>0) {
        tt.lines += cleared;
        tt.level = tt.lines/10 + 1;
        tt.score += pts[cleared] * tt.level;
        if (tt.score > tt.hi) tt.hi = tt.score;
        tt.drop_speed = imax(2, 48 - tt.level*4);
    }
}

static void tt_init(void) {
    for (int y=0;y<TT_ROWS;y++) for (int x=0;x<TT_COLS;x++) { tt.board[y][x]=0; }
    tt.score=0; tt.lines=0; tt.level=1; tt.timer=0; tt.drop_speed=48; tt.state=0;
    tt.next_piece = (int)(rng()%7);
    tt_new_piece();
}

static int tt_update(void) {
    if (tt.state==1) {
        uint16_t p = key_pressed();
        if (p & KEY_A) tt_init();
        if (p & KEY_B) return -1;
        return 0;
    }
    uint16_t p = key_pressed();
    if (p & KEY_B) return -1;

    // move left/right
    if ((p & KEY_LEFT)  && !tt_collide(tt.piece,tt.rot,tt.px-1,tt.py)) tt.px--;
    if ((p & KEY_RIGHT) && !tt_collide(tt.piece,tt.rot,tt.px+1,tt.py)) tt.px++;

    // rotate
    if (p & KEY_UP) {
        int nr = (tt.rot+1)%4;
        if (!tt_collide(tt.piece,nr,tt.px,tt.py)) tt.rot=nr;
        else if (!tt_collide(tt.piece,nr,tt.px-1,tt.py)) { tt.px--; tt.rot=nr; }
        else if (!tt_collide(tt.piece,nr,tt.px+1,tt.py)) { tt.px++; tt.rot=nr; }
    }

    // soft drop
    int speed = (key_down() & KEY_DOWN) ? 2 : tt.drop_speed;

    if (++tt.timer >= speed) {
        tt.timer=0;
        if (!tt_collide(tt.piece,tt.rot,tt.px,tt.py+1)) {
            tt.py++;
        } else {
            tt_place();
            tt_new_piece();
            if (tt_collide(tt.piece,tt.rot,tt.px,tt.py)) tt.state=1;
        }
    }
    return 0;
}

static void tt_draw_block(int bx, int by, uint16_t col) {
    int px = TT_OX + bx*TT_CS, py = TT_OY + by*TT_CS;
    m3_rect(px, py, TT_CS-1, TT_CS-1, col);
    // highlight top-left
    m3_hline(px, py, TT_CS-2, RGB15(31,31,31));
    m3_vline(px, py, TT_CS-2, RGB15(31,31,31));
    // shadow bottom-right
    m3_hline(px, py+TT_CS-2, TT_CS-2, C_BLACK);
    m3_vline(px+TT_CS-2, py, TT_CS-2, C_BLACK);
}

static void tt_draw(void) {
    m3_fill(C_DGRAY);
    // board bg
    m3_rect(TT_OX, TT_OY, TT_COLS*TT_CS, TT_ROWS*TT_CS, C_BLACK);
    // grid lines
    for (int x=0;x<=TT_COLS;x++) m3_vline(TT_OX+x*TT_CS, TT_OY, TT_ROWS*TT_CS, RGB15(3,3,3));
    for (int y=0;y<=TT_ROWS;y++) m3_hline(TT_OX, TT_OY+y*TT_CS, TT_COLS*TT_CS, RGB15(3,3,3));

    // placed blocks
    for (int y=0;y<TT_ROWS;y++) for (int x=0;x<TT_COLS;x++)
        if (tt.board[y][x]) tt_draw_block(x, y, tt.bcolor[y][x]);

    // current piece
    for (int i=0;i<4;i++) {
        int bx=tt.px+TT_PIECES[tt.piece][tt.rot][i][0];
        int by=tt.py+TT_PIECES[tt.piece][tt.rot][i][1];
        if (by>=0) tt_draw_block(bx, by, TT_COLORS[tt.piece]);
    }
    // ghost
    int ghost_y = tt.py;
    while (!tt_collide(tt.piece,tt.rot,tt.px,ghost_y+1)) ghost_y++;
    if (ghost_y != tt.py) {
        for (int i=0;i<4;i++) {
            int bx=tt.px+TT_PIECES[tt.piece][tt.rot][i][0];
            int by=ghost_y+TT_PIECES[tt.piece][tt.rot][i][1];
            if (by>=0) m3_border(TT_OX+bx*TT_CS, TT_OY+by*TT_CS, TT_CS-1, TT_CS-1, C_GRAY);
        }
    }
    m3_border(TT_OX-1, TT_OY-1, TT_COLS*TT_CS+2, TT_ROWS*TT_CS+2, C_WHITE);

    // sidebar
    draw_panel(130, 14, 110, 145, RGB15(3,3,8), C_BLUE);
    draw_str(136, 20, "NEXT:", C_CYAN);
    // next piece preview
    for (int i=0;i<4;i++) {
        int bx=TT_PIECES[tt.next_piece][0][i][0];
        int by=TT_PIECES[tt.next_piece][0][i][1];
        m3_rect(150+bx*7, 32+by*7, 6, 6, TT_COLORS[tt.next_piece]);
    }
    draw_str(136, 70, "SCORE:", C_WHITE);
    draw_int(136, 80, tt.score, C_YELLOW);
    draw_str(136, 96, "LINES:", C_WHITE);
    draw_int(136, 106, tt.lines, C_CYAN);
    draw_str(136, 120, "LEVEL:", C_WHITE);
    draw_int(136, 130, tt.level, C_LGREEN);
    draw_str(136, 144, "HI:", C_WHITE);
    draw_int(136, 154, tt.hi, C_YELLOW);

    draw_hud("TETRIS", tt.score, tt.hi);
    if (tt.state==1) draw_gameover(tt.score);
}

// ============================================================
// GAME 3: PONG
// ============================================================
static struct {
    int ball_x, ball_y, ball_dx, ball_dy; // * 256 fixed point
    int p1y, p2y, p1score, p2score;
    int ai_speed;
    int state; // 0=play
    int flash;
    int hi;
} pong;

#define PN_PH  28
#define PN_PW  4
#define PN_BR  4
#define PN_P1X 8
#define PN_P2X (SCREEN_W-8-PN_PW)

static void pong_serve(int dir) {
    pong.ball_x = (SCREEN_W/2) << 8;
    pong.ball_y = (SCREEN_H/2) << 8;
    pong.ball_dx = dir * (2<<8);
    pong.ball_dy = (rng()%3-1) * (2<<8);
    if (pong.ball_dy==0) pong.ball_dy = 1<<8;
}

static void pong_init(void) {
    pong.p1y = SCREEN_H/2 - PN_PH/2;
    pong.p2y = SCREEN_H/2 - PN_PH/2;
    pong.p1score = 0; pong.p2score = 0;
    pong.ai_speed = 2; pong.state = 0; pong.flash = 0;
    pong_serve(1);
}

static int pong_update(void) {
    uint16_t k = key_down();
    uint16_t p = key_pressed();
    if (p & KEY_B) return -1;

    // Player 1 (left)
    if ((k & KEY_UP)   && pong.p1y > 14) pong.p1y -= 3;
    if ((k & KEY_DOWN) && pong.p1y < SCREEN_H-PN_PH-1) pong.p1y += 3;

    // AI player 2
    int ball_cy = (pong.ball_y>>8);
    int p2cy = pong.p2y + PN_PH/2;
    int diff = ball_cy - p2cy;
    if (diff > 1) pong.p2y += imin(pong.ai_speed, diff);
    else if (diff < -1) pong.p2y -= imin(pong.ai_speed, -diff);
    pong.p2y = iclamp(pong.p2y, 14, SCREEN_H-PN_PH-1);

    // Move ball
    pong.ball_x += pong.ball_dx;
    pong.ball_y += pong.ball_dy;
    int bx = pong.ball_x>>8, by = pong.ball_y>>8;

    // Wall bounce
    if (by <= 14) { pong.ball_dy = iabs(pong.ball_dy); }
    if (by >= SCREEN_H-PN_BR-1) { pong.ball_dy = -iabs(pong.ball_dy); }

    // Paddle 1 collision
    if (bx <= PN_P1X+PN_PW+PN_BR && bx >= PN_P1X+PN_PW-2 && by >= pong.p1y-PN_BR && by <= pong.p1y+PN_PH+PN_BR) {
        pong.ball_dx = iabs(pong.ball_dx) + (1<<6);
        int rel = (by - pong.p1y) - PN_PH/2;
        pong.ball_dy = rel * (1<<6);
    }
    // Paddle 2 collision
    if (bx >= PN_P2X-PN_BR && bx <= PN_P2X+2 && by >= pong.p2y-PN_BR && by <= pong.p2y+PN_PH+PN_BR) {
        pong.ball_dx = -(iabs(pong.ball_dx) + (1<<6));
        int rel = (by - pong.p2y) - PN_PH/2;
        pong.ball_dy = rel * (1<<6);
    }
    // clamp ball speed
    if (iabs(pong.ball_dx) > (5<<8)) pong.ball_dx = (pong.ball_dx>0)?(5<<8):-(5<<8);
    if (iabs(pong.ball_dy) > (4<<8)) pong.ball_dy = (pong.ball_dy>0)?(4<<8):-(4<<8);

    // Score
    if (bx < 2) { pong.p2score++; pong.flash=30; pong_serve(1); }
    if (bx > SCREEN_W-2) { pong.p1score++; if(pong.p1score>pong.hi)pong.hi=pong.p1score; pong.flash=30; pong_serve(-1); }
    if (pong.flash>0) pong.flash--;
    return 0;
}

static void pong_draw(void) {
    m3_fill(C_BLACK);
    // center dashed line
    for (int y=14;y<SCREEN_H;y+=6) m3_rect(SCREEN_W/2-1, y, 2, 3, C_DGRAY);

    // score area
    m3_rect(0, 0, SCREEN_W, 13, C_DGRAY);
    draw_str(2, 3, "PONG", C_CYAN);
    draw_str(80, 3, "P1:", C_WHITE);
    draw_int(96, 3, pong.p1score, C_YELLOW);
    draw_str(120, 3, "CPU:", C_WHITE);
    draw_int(144, 3, pong.p2score, C_RED);
    draw_str(175, 3, "HI:", C_WHITE);
    draw_int(193, 3, pong.hi, C_YELLOW);
    m3_hline(0, 13, SCREEN_W, C_GRAY);

    // paddles
    uint16_t p1c = (pong.flash>0) ? C_YELLOW : C_LBLUE;
    m3_rect(PN_P1X, pong.p1y, PN_PW, PN_PH, p1c);
    m3_rect(PN_P1X+1, pong.p1y, 1, PN_PH, C_WHITE);
    m3_rect(PN_P2X, pong.p2y, PN_PW, PN_PH, C_RED);
    m3_rect(PN_P2X+2, pong.p2y, 1, PN_PH, C_WHITE);

    // ball
    int bx=pong.ball_x>>8, by=pong.ball_y>>8;
    m3_rect(bx-PN_BR, by-PN_BR, PN_BR*2, PN_BR*2, C_WHITE);
    m3_pixel(bx-PN_BR, by-PN_BR, C_GRAY);
    m3_pixel(bx+PN_BR-1, by-PN_BR, C_GRAY);

    draw_str(90, SCREEN_H-10, "CIMA/BAIXO-MOVER", C_DGRAY);
}

// ============================================================
// GAME 4: JOGO DA VELHA
// ============================================================
static struct {
    int board[9]; // 0=empty 1=X 2=O
    int cur, turn; // turn: 1=X 2=O(cpu)
    int state; // 0=play 1=win_x 2=win_o 3=draw
    int winner_line[3];
    int hi_x, hi_o;
    int ai_delay;
} ttt;

static const int TTT_WINS[8][3] = {
    {0,1,2},{3,4,5},{6,7,8}, // rows
    {0,3,6},{1,4,7},{2,5,8}, // cols
    {0,4,8},{2,4,6}          // diagonals
};

static int ttt_check_win(int player) {
    for (int i=0;i<8;i++) {
        if (ttt.board[TTT_WINS[i][0]]==player &&
            ttt.board[TTT_WINS[i][1]]==player &&
            ttt.board[TTT_WINS[i][2]]==player) {
            ttt.winner_line[0]=TTT_WINS[i][0];
            ttt.winner_line[1]=TTT_WINS[i][1];
            ttt.winner_line[2]=TTT_WINS[i][2];
            return 1;
        }
    }
    return 0;
}

static int ttt_minimax(int *board, int depth, int is_max) {
    if (ttt_check_win(1)) return -10+depth;
    if (ttt_check_win(2)) return  10-depth;
    int has_empty=0;
    for (int i=0;i<9;i++) if (!board[i]) { has_empty=1; break; }
    if (!has_empty) return 0;

    int best = is_max ? -100 : 100;
    for (int i=0;i<9;i++) {
        if (!board[i]) {
            board[i] = is_max ? 2 : 1;
            int v = ttt_minimax(board, depth+1, !is_max);
            board[i] = 0;
            if (is_max) { if (v>best) best=v; }
            else        { if (v<best) best=v; }
        }
    }
    return best;
}

static int ttt_ai_move(void) {
    int best_val=-100, best_pos=-1;
    for (int i=0;i<9;i++) {
        if (!ttt.board[i]) {
            ttt.board[i]=2;
            int v=ttt_minimax(ttt.board,0,0);
            ttt.board[i]=0;
            if (v>best_val) { best_val=v; best_pos=i; }
        }
    }
    return best_pos;
}

static void ttt_init(void) {
    for (int i=0;i<9;i++) ttt.board[i]=0;
    ttt.cur=4; ttt.turn=1; ttt.state=0; ttt.ai_delay=0;
}

static int ttt_update(void) {
    uint16_t p = key_pressed();
    if (p & KEY_B) { if (ttt.state) { ttt_init(); return 0; } return -1; }
    if (p & KEY_A && ttt.state) { ttt_init(); return 0; }

    if (ttt.state) return 0;

    if (ttt.turn==1) {
        if ((p & KEY_LEFT)  && (ttt.cur%3)>0) ttt.cur--;
        if ((p & KEY_RIGHT) && (ttt.cur%3)<2) ttt.cur++;
        if ((p & KEY_UP)    && ttt.cur>=3)    ttt.cur-=3;
        if ((p & KEY_DOWN)  && ttt.cur<6)     ttt.cur+=3;
        if ((p & KEY_A) && !ttt.board[ttt.cur]) {
            ttt.board[ttt.cur]=1;
            if (ttt_check_win(1)) { ttt.state=1; ttt.hi_x++; }
            else {
                int empty=0; for(int i=0;i<9;i++) if(!ttt.board[i]) empty++;
                if (!empty) ttt.state=3;
                else ttt.turn=2;
            }
        }
    } else {
        // AI
        if (++ttt.ai_delay < 30) return 0;
        ttt.ai_delay=0;
        int mv = ttt_ai_move();
        if (mv>=0) ttt.board[mv]=2;
        if (ttt_check_win(2)) { ttt.state=2; ttt.hi_o++; }
        else {
            int empty=0; for(int i=0;i<9;i++) if(!ttt.board[i]) empty++;
            if (!empty) ttt.state=3;
            else ttt.turn=1;
        }
    }
    return 0;
}

static void ttt_draw(void) {
    m3_fill(C_DBLUE);
    // title
    m3_rect(0,0,SCREEN_W,13,C_DGRAY);
    draw_str(2,3,"JOGO DA VELHA",C_CYAN);
    draw_str(120,3,"X:",C_YELLOW); draw_int(132,3,ttt.hi_x,C_YELLOW);
    draw_str(155,3,"O:",C_RED);    draw_int(167,3,ttt.hi_o,C_RED);
    m3_hline(0,13,SCREEN_W,C_GRAY);

    // board (centered)
    int bx=48, by=20, cw=44, ch=44;
    m3_rect(bx, by, cw*3, ch*3, RGB15(2,2,6));
    m3_vline(bx+cw,   by, ch*3, C_WHITE);
    m3_vline(bx+cw*2, by, ch*3, C_WHITE);
    m3_hline(bx, by+ch,   cw*3, C_WHITE);
    m3_hline(bx, by+ch*2, cw*3, C_WHITE);

    for (int i=0;i<9;i++) {
        int cx = bx + (i%3)*cw + cw/2;
        int cy = by + (i/3)*ch + ch/2;

        // cursor
        if (ttt.turn==1 && ttt.state==0 && i==ttt.cur) {
            m3_rect(bx+(i%3)*cw+1, by+(i/3)*ch+1, cw-2, ch-2, RGB15(4,4,12));
        }

        if (ttt.board[i]==1) {
            // X - two diagonal lines
            m3_rect(cx-14,cy-14,3,3,C_YELLOW);
            for (int d=0;d<26;d++) {
                m3_pixel(cx-13+d, cy-13+d, C_YELLOW);
                m3_pixel(cx-13+d, cy+13-d, C_YELLOW);
                m3_pixel(cx-12+d, cy-13+d, C_YELLOW);
                m3_pixel(cx-12+d, cy+13-d, C_YELLOW);
            }
        } else if (ttt.board[i]==2) {
            // O - circle
            int r=14;
            for (int angle=0; angle<64; angle++) {
                // rough circle using sin/cos approximation
                int a = angle*4; // 0..252 -> degrees/1.4
                // Use quarter symmetry
                int qx = (a<16) ? a : (a<32) ? 31-a : (a<48) ? a-32 : 63-a;
                int qy = (a<16) ? 15-a : (a<32) ? a-16 : (a<48) ? 47-a : a-48;
                // Normalize to circle
                // Instead just draw 4 mirrored pixels using simple approach
                (void)qx;(void)qy;
                // Actually let's just draw using thick border
                for (int t=0;t<4;t++) {
                    static const int dx4[4]={1,0,-1,0}, dy4[4]={0,1,0,-1};
                    int px2=cx+dx4[t]*r, py2=cy+dy4[t]*r;
                    m3_rect(px2-1,py2-1,3,3,C_RED);
                }
            }
            // Simple thick circle via rect subtraction
            m3_rect(cx-r, cy-r, r*2, r*2, C_RED);
            m3_rect(cx-r+3, cy-r+3, r*2-6, r*2-6, RGB15(2,2,6));
        }
    }

    // winner line
    if (ttt.state==1 || ttt.state==2) {
        uint16_t lc = (ttt.state==1) ? C_YELLOW : C_RED;
        int a=ttt.winner_line[0], b=ttt.winner_line[2];
        int ax=bx+(a%3)*cw+cw/2, ay=by+(a/3)*ch+ch/2;
        int bx2=bx+(b%3)*cw+cw/2, by2=by+(b/3)*ch+ch/2;
        // draw thick line
        for (int d=-2;d<=2;d++) {
            // simple approach: vertical or horizontal or diagonal
            int dx2=bx2-ax, dy2=by2-ay;
            int steps=iabs(dx2)>iabs(dy2)?iabs(dx2):iabs(dy2);
            for (int s=0;s<=steps;s++) {
                int px2=ax+dx2*s/steps, py2=ay+dy2*s/steps;
                m3_pixel(px2+d, py2, lc);
                m3_pixel(px2, py2+d, lc);
            }
        }
    }

    // status
    if (ttt.state==0) {
        draw_str(4, by+ch*3+4, ttt.turn==1 ? "SUA VEZ (X)" : "CPU PENSANDO...", C_WHITE);
        draw_str(4, by+ch*3+14, "DIRECOES-MOVER  A-JOGAR", C_GRAY);
    } else if (ttt.state==1) {
        draw_win("VOCE GANHOU!");
    } else if (ttt.state==2) {
        draw_gameover(0);
        draw_str_centered(75,"CPU GANHOU",C_RED);
    } else {
        draw_panel(60,60,120,40,C_NAVY,C_CYAN);
        draw_str_centered(68,"EMPATE!",C_YELLOW);
        draw_str_centered(78,"A-REINICIAR",C_WHITE);
    }
}

// ============================================================
// GAME 5: BREAKOUT
// ============================================================
#define BO_BRICK_COLS 12
#define BO_BRICK_ROWS  6
#define BO_BW  18
#define BO_BH   7
#define BO_BX   6
#define BO_BY  18
#define BO_PW  28
#define BO_PH   4

static struct {
    int bricks[BO_BRICK_ROWS][BO_BRICK_COLS];
    int px; // paddle x center
    int ball_x, ball_y, ball_dx, ball_dy; // *256
    int score, hi, lives;
    int state; // 0=play 1=dead 2=win
    int timer;
} bo;

static const uint16_t BO_COLORS[BO_BRICK_ROWS] = {
    C_RED, C_ORANGE, C_YELLOW, C_LGREEN, C_CYAN, C_BLUE
};

static void bo_init(void) {
    for (int r=0;r<BO_BRICK_ROWS;r++) for (int c=0;c<BO_BRICK_COLS;c++) bo.bricks[r][c]=1;
    bo.px=SCREEN_W/2;
    bo.ball_x=(SCREEN_W/2)<<8; bo.ball_y=(130)<<8;
    bo.ball_dx=(2<<8)+(1<<7); bo.ball_dy=-(2<<8);
    bo.score=0; bo.lives=3; bo.state=0; bo.timer=0;
}

static int bo_update(void) {
    uint16_t k = key_down();
    uint16_t p = key_pressed();
    if (p & KEY_B) return -1;

    if (bo.state==1||bo.state==2) {
        if (p & KEY_A) bo_init();
        return 0;
    }

    if ((k & KEY_LEFT)  && bo.px > BO_PW/2+4) bo.px -= 4;
    if ((k & KEY_RIGHT) && bo.px < SCREEN_W-BO_PW/2-4) bo.px += 4;

    bo.ball_x += bo.ball_dx;
    bo.ball_y += bo.ball_dy;
    int bx=bo.ball_x>>8, by=bo.ball_y>>8;

    // walls
    if (bx<=4) { bo.ball_dx=iabs(bo.ball_dx); }
    if (bx>=SCREEN_W-4) { bo.ball_dx=-iabs(bo.ball_dx); }
    if (by<=14) { bo.ball_dy=iabs(bo.ball_dy); }

    // paddle
    int py = SCREEN_H - 16;
    if (by >= py-4 && by <= py+BO_PH+2 && bx>=bo.px-BO_PW/2 && bx<=bo.px+BO_PW/2) {
        bo.ball_dy = -iabs(bo.ball_dy);
        int rel = bx - bo.px;
        bo.ball_dx = rel * (1<<5);
        if (iabs(bo.ball_dx)<(1<<7)) bo.ball_dx=(bo.ball_dx>=0)?(1<<7):-(1<<7);
    }

    // bottom
    if (by > SCREEN_H) {
        bo.lives--;
        if (bo.lives<=0) { bo.state=1; }
        else {
            bo.ball_x=(SCREEN_W/2)<<8; bo.ball_y=(130)<<8;
            bo.ball_dx=(2<<8); bo.ball_dy=-(2<<8);
        }
    }

    // bricks
    int col=(bx-BO_BX)/BO_BW, row=(by-BO_BY)/BO_BH;
    if (row>=0&&row<BO_BRICK_ROWS&&col>=0&&col<BO_BRICK_COLS&&bo.bricks[row][col]) {
        bo.bricks[row][col]=0;
        bo.score+=10*(BO_BRICK_ROWS-row);
        if (bo.score>bo.hi) bo.hi=bo.score;
        // check win
        int left=0; for(int r=0;r<BO_BRICK_ROWS;r++) for(int c=0;c<BO_BRICK_COLS;c++) if(bo.bricks[r][c]) left++;
        if (!left) bo.state=2;
        // bounce
        int brick_cx = BO_BX + col*BO_BW + BO_BW/2;
        int brick_cy = BO_BY + row*BO_BH + BO_BH/2;
        if (iabs(bx-brick_cx) > iabs(by-brick_cy)) bo.ball_dx=-bo.ball_dx;
        else bo.ball_dy=-bo.ball_dy;
    }
    // clamp speed
    if (iabs(bo.ball_dx)>(4<<8)) bo.ball_dx=(bo.ball_dx>0)?(4<<8):-(4<<8);
    return 0;
}

static void bo_draw(void) {
    m3_fill(RGB15(1,1,3));
    // bricks
    for (int r=0;r<BO_BRICK_ROWS;r++) for (int c=0;c<BO_BRICK_COLS;c++) {
        if (!bo.bricks[r][c]) continue;
        int x=BO_BX+c*BO_BW, y=BO_BY+r*BO_BH;
        m3_rect(x+1,y+1,BO_BW-2,BO_BH-2, BO_COLORS[r]);
        m3_hline(x+1,y+1,BO_BW-2, C_WHITE);
        m3_vline(x+1,y+1,BO_BH-2, C_WHITE);
    }
    // paddle
    int py=SCREEN_H-16;
    m3_rect(bo.px-BO_PW/2, py, BO_PW, BO_PH, C_LBLUE);
    m3_hline(bo.px-BO_PW/2, py, BO_PW, C_WHITE);
    // ball
    int bx=bo.ball_x>>8, by2=bo.ball_y>>8;
    m3_rect(bx-3,by2-3,6,6,C_WHITE);
    m3_pixel(bx-2,by2-2,C_LBLUE);
    // lives
    for (int i=0;i<bo.lives;i++) m3_rect(SCREEN_W-8-i*8,py,5,4,C_LBLUE);
    draw_hud("BREAKOUT",bo.score,bo.hi);
    if (bo.state==1) draw_gameover(bo.score);
    if (bo.state==2) draw_win("NIVEL COMPLETO!");
}

// ============================================================
// GAME 6: FLAPPY BIRD
// ============================================================
#define FL_GRAVITY  16  // * 256
#define FL_JUMP    -500
#define FL_PIPE_W   20
#define FL_GAP      50
#define FL_PIPE_SPD  2

static struct {
    int bird_y, bird_vy; // *256
    int pipes[4][2]; // x, gap_y
    int pipe_idx;
    int score, hi;
    int state; // 0=idle 1=play 2=dead
    int timer;
    int bg_scroll;
} fl;

static void fl_spawn_pipe(int x) {
    fl.pipes[fl.pipe_idx][0] = x;
    fl.pipes[fl.pipe_idx][1] = 30 + (int)(rng() % 70);
    fl.pipe_idx = (fl.pipe_idx+1)%4;
}

static void fl_init(void) {
    fl.bird_y = (SCREEN_H/2)<<8; fl.bird_vy=0;
    for (int i=0;i<4;i++) { fl.pipes[i][0]=SCREEN_W+100+i*80; fl.pipes[i][1]=50; }
    fl.pipe_idx=0; fl.score=0; fl.state=0; fl.timer=0; fl.bg_scroll=0;
    for (int i=0;i<4;i++) fl_spawn_pipe(SCREEN_W + 40 + i*80);
}

static int fl_update(void) {
    uint16_t p = key_pressed();
    if (p & KEY_B) return -1;

    if (fl.state==2) {
        if (p & KEY_A) fl_init();
        return 0;
    }
    if (fl.state==0) {
        if (p & KEY_A) { fl.state=1; fl.bird_vy=FL_JUMP; }
        return 0;
    }

    if ((p & KEY_A)) fl.bird_vy = FL_JUMP;

    fl.bird_vy += FL_GRAVITY;
    fl.bird_y  += fl.bird_vy;
    fl.bg_scroll = (fl.bg_scroll + 1) % SCREEN_W;

    int by = fl.bird_y>>8;
    if (by <= 13 || by >= SCREEN_H-8) { fl.state=2; return 0; }

    for (int i=0;i<4;i++) {
        fl.pipes[i][0] -= FL_PIPE_SPD;
        if (fl.pipes[i][0] < -FL_PIPE_W) {
            fl.pipes[i][0] = SCREEN_W + FL_PIPE_W;
            fl.pipes[i][1] = 20 + (int)(rng()%70);
            fl.score++;
            if (fl.score>fl.hi) fl.hi=fl.score;
        }
        // collision
        int px=fl.pipes[i][0], gap=fl.pipes[i][1];
        if (18>=px-FL_PIPE_W && 22<=px+FL_PIPE_W) {
            if (by<gap+14 || by>gap+14+FL_GAP) { fl.state=2; return 0; }
        }
    }
    return 0;
}

static void fl_draw(void) {
    // sky gradient
    for (int y=14;y<SCREEN_H;y++) {
        int t = (y-14)*16/(SCREEN_H-14);
        m3_hline(0, y, SCREEN_W, RGB15(t/2+6, t/2+12, 24-t/3));
    }
    m3_rect(0,0,SCREEN_W,13,C_DGRAY); m3_hline(0,13,SCREEN_W,C_GRAY);

    // clouds
    for (int i=0;i<3;i++) {
        int cx = (200 - fl.bg_scroll/3 + i*80) % SCREEN_W;
        int cy = 25 + i*20;
        m3_rect(cx, cy, 24, 8, RGB15(28,28,28));
        m3_rect(cx+4, cy-4, 16, 6, RGB15(28,28,28));
    }

    // pipes
    for (int i=0;i<4;i++) {
        int px=fl.pipes[i][0], gap=fl.pipes[i][1]+14;
        // top pipe
        m3_rect(px-FL_PIPE_W/2, 14, FL_PIPE_W, gap-FL_PIPE_W/2-14, C_GREEN);
        m3_rect(px-FL_PIPE_W/2-2, gap-FL_PIPE_W/2, FL_PIPE_W+4, FL_PIPE_W/2, C_LGREEN);
        // bottom pipe
        int bot_y = gap+FL_GAP+FL_PIPE_W/2;
        m3_rect(px-FL_PIPE_W/2-2, gap+FL_GAP, FL_PIPE_W+4, FL_PIPE_W/2, C_LGREEN);
        m3_rect(px-FL_PIPE_W/2, bot_y, FL_PIPE_W, SCREEN_H-bot_y, C_GREEN);
    }

    // ground
    m3_rect(0, SCREEN_H-8, SCREEN_W, 8, C_BROWN);
    m3_hline(0, SCREEN_H-8, SCREEN_W, C_YELLOW);

    // bird
    int by=fl.bird_y>>8;
    int tilt = iclamp(fl.bird_vy>>9, -2, 3);
    m3_rect(15, by-4, 12, 8, C_YELLOW);
    m3_pixel(23, by-2, C_ORANGE);
    m3_rect(25, by-1, 3, 2, C_ORANGE);
    m3_pixel(26, by-3+tilt, C_BLACK);

    draw_str(2,3,"FLAPPY",C_CYAN);
    draw_str(60,3,"SC:",C_WHITE); draw_int(78,3,fl.score,C_YELLOW);
    draw_str(110,3,"HI:",C_WHITE); draw_int(128,3,fl.hi,C_YELLOW);

    if (fl.state==0) {
        draw_panel(60,60,120,40,C_NAVY,C_CYAN);
        draw_str_centered(68,"FLAPPY BIRD",C_YELLOW);
        draw_str_centered(80,"A-COMECAR",C_WHITE);
    }
    if (fl.state==2) draw_gameover(fl.score);
}

// ============================================================
// GAME 7: SPACE INVADERS
// ============================================================
#define SI_COLS 10
#define SI_ROWS  4
#define SI_CW   20
#define SI_CH   14
#define SI_OX   10
#define SI_OY   20
#define SI_MAX_BULLETS 3
#define SI_MAX_ENEMY_BULLETS 4

static struct {
    int alive[SI_ROWS][SI_COLS];
    int ox, oy, dir; // alien group position offset
    int speed_timer, speed;
    int px; // player x center
    int bullets[SI_MAX_BULLETS][2]; // x,y (-1=inactive)
    int ebullets[SI_MAX_ENEMY_BULLETS][2];
    int score, hi, lives;
    int state; // 0=play 1=dead 2=win
    int anim;
    int shoot_timer;
    int wave;
} si;

static void si_init(void) {
    for (int r=0;r<SI_ROWS;r++) for (int c=0;c<SI_COLS;c++) si.alive[r][c]=1;
    si.ox=0; si.oy=0; si.dir=1;
    si.speed=25; si.speed_timer=0;
    si.px=SCREEN_W/2;
    for (int i=0;i<SI_MAX_BULLETS;i++) si.bullets[i][0]=-1;
    for (int i=0;i<SI_MAX_ENEMY_BULLETS;i++) si.ebullets[i][0]=-1;
    si.score=0; si.lives=3; si.state=0; si.anim=0; si.shoot_timer=0;
}

static int si_count_alive(void) {
    int n=0; for(int r=0;r<SI_ROWS;r++) for(int c=0;c<SI_COLS;c++) if(si.alive[r][c]) n++;
    return n;
}

static int si_update(void) {
    uint16_t k=key_down(), p=key_pressed();
    if (p & KEY_B) return -1;

    if (si.state) {
        if (p & KEY_A) { si.score=0; si_init(); if(si.state==2){si.wave++;si.speed=imax(5,25-si.wave*3);} }
        return 0;
    }

    // player move
    if ((k&KEY_LEFT)  && si.px>8)           si.px-=3;
    if ((k&KEY_RIGHT) && si.px<SCREEN_W-8)  si.px+=3;

    // player shoot
    if (p&KEY_A) {
        for (int i=0;i<SI_MAX_BULLETS;i++) {
            if (si.bullets[i][0]<0) {
                si.bullets[i][0]=si.px;
                si.bullets[i][1]=SCREEN_H-22;
                break;
            }
        }
    }

    // move bullets
    for (int i=0;i<SI_MAX_BULLETS;i++) {
        if (si.bullets[i][0]<0) continue;
        si.bullets[i][1]-=5;
        if (si.bullets[i][1]<14) { si.bullets[i][0]=-1; continue; }
        // hit alien
        for (int r=0;r<SI_ROWS;r++) for (int c=0;c<SI_COLS;c++) {
            if (!si.alive[r][c]) continue;
            int ax=SI_OX+si.ox+c*SI_CW, ay=SI_OY+si.oy+r*SI_CH;
            if (si.bullets[i][0]>=ax && si.bullets[i][0]<=ax+SI_CW-2 &&
                si.bullets[i][1]>=ay && si.bullets[i][1]<=ay+SI_CH-2) {
                si.alive[r][c]=0;
                si.bullets[i][0]=-1;
                si.score+=10*(SI_ROWS-r);
                if (si.score>si.hi) si.hi=si.score;
                if (!si_count_alive()) si.state=2;
                goto next_bullet;
            }
        }
        next_bullet:;
    }

    // enemy movement
    si.anim++;
    if (++si.speed_timer >= si.speed) {
        si.speed_timer=0;
        si.ox += si.dir*4;
        // check edges
        int left_edge=SI_OX+si.ox, right_edge=SI_OX+si.ox+(SI_COLS-1)*SI_CW+SI_CW;
        if (right_edge>=SCREEN_W-4 || left_edge<=4) {
            si.dir=-si.dir;
            si.oy+=SI_CH/2;
        }
        // aliens reach player
        for (int r=0;r<SI_ROWS;r++) for (int c=0;c<SI_COLS;c++) {
            if (si.alive[r][c] && SI_OY+si.oy+r*SI_CH+SI_CH>SCREEN_H-24) {
                si.state=1; return 0;
            }
        }
    }

    // enemy shoot
    if (++si.shoot_timer>40) {
        si.shoot_timer=0;
        // find a random alive alien in bottom row of each column
        for (int i=0;i<SI_MAX_ENEMY_BULLETS;i++) {
            if (si.ebullets[i][0]<0) {
                int c=(int)(rng()%SI_COLS);
                for (int r=SI_ROWS-1;r>=0;r--) {
                    if (si.alive[r][c]) {
                        si.ebullets[i][0]=SI_OX+si.ox+c*SI_CW+SI_CW/2;
                        si.ebullets[i][1]=SI_OY+si.oy+r*SI_CH+SI_CH;
                        break;
                    }
                }
                break;
            }
        }
    }

    // move enemy bullets
    for (int i=0;i<SI_MAX_ENEMY_BULLETS;i++) {
        if (si.ebullets[i][0]<0) continue;
        si.ebullets[i][1]+=3;
        if (si.ebullets[i][1]>SCREEN_H) { si.ebullets[i][0]=-1; continue; }
        // hit player
        if (iabs(si.ebullets[i][0]-si.px)<8 && iabs(si.ebullets[i][1]-(SCREEN_H-18))<6) {
            si.ebullets[i][0]=-1;
            si.lives--;
            if (si.lives<=0) si.state=1;
        }
    }
    return 0;
}

static void si_draw_alien(int x, int y, int row, int anim) {
    uint16_t cols[4]={C_RED,C_ORANGE,C_CYAN,C_LGREEN};
    uint16_t c=cols[row];
    // Simple alien shapes per row
    if (row==0) {
        // UFO shape
        m3_rect(x+4,y,8,3,c); m3_rect(x+2,y+3,12,4,c);
        m3_pixel(x+(anim?1:3),y+7,c); m3_pixel(x+(anim?12:10),y+7,c);
    } else if (row==1) {
        m3_rect(x+2,y+1,12,3,c); m3_rect(x,y+4,16,3,c);
        m3_pixel(x+(anim?0:2),y+7,c); m3_pixel(x+(anim?14:12),y+7,c);
        m3_pixel(x+5,y,c); m3_pixel(x+10,y,c);
    } else {
        m3_rect(x+2,y+2,12,5,c); m3_rect(x,y+4,4,3,c); m3_rect(x+12,y+4,4,3,c);
        m3_pixel(x+(anim?1:3),y+7,c); m3_pixel(x+(anim?13:11),y+7,c);
    }
}

static void si_draw(void) {
    m3_fill(C_BLACK);
    // starfield
    for (int i=0;i<30;i++) {
        int sx=(i*73+11)%SCREEN_W, sy=14+(i*47)%(SCREEN_H-14);
        m3_pixel(sx,sy, (i%3==0)?C_WHITE:C_GRAY);
    }
    // aliens
    int frame=(si.anim/8)&1;
    for (int r=0;r<SI_ROWS;r++) for (int c=0;c<SI_COLS;c++) {
        if (!si.alive[r][c]) continue;
        int ax=SI_OX+si.ox+c*SI_CW, ay=SI_OY+si.oy+r*SI_CH;
        si_draw_alien(ax, ay, r, frame);
    }
    // player ship
    int py=SCREEN_H-20;
    m3_rect(si.px-6,py+4,12,5,C_LBLUE);
    m3_rect(si.px-2,py,4,5,C_LBLUE);
    m3_rect(si.px-8,py+7,16,3,C_BLUE);
    // player bullets
    for (int i=0;i<SI_MAX_BULLETS;i++) {
        if (si.bullets[i][0]<0) continue;
        m3_rect(si.bullets[i][0]-1,si.bullets[i][1],2,6,C_YELLOW);
    }
    // enemy bullets
    for (int i=0;i<SI_MAX_ENEMY_BULLETS;i++) {
        if (si.ebullets[i][0]<0) continue;
        m3_rect(si.ebullets[i][0]-1,si.ebullets[i][1],2,5,C_RED);
    }
    // lives
    for (int i=0;i<si.lives;i++) m3_rect(4+i*12,SCREEN_H-10,8,6,C_LBLUE);
    // ground line
    m3_hline(0,SCREEN_H-12,SCREEN_W,C_LGREEN);
    draw_hud("INVADERS",si.score,si.hi);
    if (si.state==1) draw_gameover(si.score);
    if (si.state==2) draw_win("ONDA DESTRUIDA!");
}

// ============================================================
// GAME 8: MINESWEEPER
// ============================================================
#define MS_COLS 14
#define MS_ROWS 10
#define MS_CS   15
#define MS_OX   ((SCREEN_W - MS_COLS*MS_CS)/2)
#define MS_OY   18
#define MS_MINES 18

static struct {
    int board[MS_ROWS][MS_COLS]; // -1=mine, 0-8=count
    int revealed[MS_ROWS][MS_COLS];
    int flagged[MS_ROWS][MS_COLS];
    int cx, cy;
    int mines_left, score, hi;
    int state; // 0=play 1=dead 2=win
    int first_click;
} ms;

static void ms_count_neighbors(void) {
    for (int r=0;r<MS_ROWS;r++) for (int c=0;c<MS_COLS;c++) {
        if (ms.board[r][c]<0) continue;
        int cnt=0;
        for (int dr=-1;dr<=1;dr++) for (int dc=-1;dc<=1;dc++) {
            int nr=r+dr, nc=c+dc;
            if (nr>=0&&nr<MS_ROWS&&nc>=0&&nc<MS_COLS&&ms.board[nr][nc]<0) cnt++;
        }
        ms.board[r][c]=cnt;
    }
}

static void ms_place_mines(int avoid_r, int avoid_c) {
    int placed=0;
    while (placed<MS_MINES) {
        int r=(int)(rng()%MS_ROWS), c=(int)(rng()%MS_COLS);
        if (ms.board[r][c]<0) continue;
        if (iabs(r-avoid_r)<=1 && iabs(c-avoid_c)<=1) continue;
        ms.board[r][c]=-1; placed++;
    }
    ms_count_neighbors();
}

static void ms_reveal(int r, int c) {
    if (r<0||r>=MS_ROWS||c<0||c>=MS_COLS) return;
    if (ms.revealed[r][c]||ms.flagged[r][c]) return;
    ms.revealed[r][c]=1;
    if (ms.board[r][c]==0) {
        for (int dr=-1;dr<=1;dr++) for (int dc=-1;dc<=1;dc++) ms_reveal(r+dr,c+dc);
    }
}

static void ms_init(void) {
    for (int r=0;r<MS_ROWS;r++) for (int c=0;c<MS_COLS;c++) {
        ms.board[r][c]=0; ms.revealed[r][c]=0; ms.flagged[r][c]=0;
    }
    ms.cx=MS_COLS/2; ms.cy=MS_ROWS/2;
    ms.mines_left=MS_MINES; ms.score=0; ms.state=0; ms.first_click=1;
}

static int ms_update(void) {
    uint16_t p=key_pressed();
    if (p & KEY_B) { if(ms.state){ms_init();return 0;} return -1; }
    if (p & KEY_A && ms.state) { ms_init(); return 0; }
    if (ms.state) return 0;

    if ((p&KEY_LEFT)  && ms.cx>0) ms.cx--;
    if ((p&KEY_RIGHT) && ms.cx<MS_COLS-1) ms.cx++;
    if ((p&KEY_UP)    && ms.cy>0) ms.cy--;
    if ((p&KEY_DOWN)  && ms.cy<MS_ROWS-1) ms.cy++;

    if (p & KEY_A) {
        if (ms.first_click) { ms_place_mines(ms.cy,ms.cx); ms.first_click=0; }
        if (ms.flagged[ms.cy][ms.cx]) return 0;
        if (ms.board[ms.cy][ms.cx]<0) {
            // reveal all mines
            for (int r=0;r<MS_ROWS;r++) for (int c=0;c<MS_COLS;c++) if(ms.board[r][c]<0) ms.revealed[r][c]=1;
            ms.state=1;
        } else {
            ms_reveal(ms.cy,ms.cx);
            // check win
            int hidden=0;
            for (int r=0;r<MS_ROWS;r++) for (int c=0;c<MS_COLS;c++) if(!ms.revealed[r][c]&&ms.board[r][c]>=0) hidden++;
            if (!hidden) { ms.state=2; ms.score=MS_MINES*50; if(ms.score>ms.hi)ms.hi=ms.score; }
        }
    }
    if (p & KEY_L || p & KEY_R) {
        // toggle flag
        if (!ms.revealed[ms.cy][ms.cx]) {
            ms.flagged[ms.cy][ms.cx]=!ms.flagged[ms.cy][ms.cx];
            ms.mines_left += ms.flagged[ms.cy][ms.cx] ? -1 : 1;
        }
    }
    return 0;
}

static const uint16_t MS_NUM_COLORS[9]={C_BLACK,C_BLUE,C_GREEN,C_RED,C_NAVY,C_DRED,C_TEAL,C_BLACK,C_GRAY};

static void ms_draw(void) {
    m3_fill(RGB15(10,10,12));
    m3_rect(0,0,SCREEN_W,13,C_DGRAY);
    draw_str(2,3,"CAMPO MINADO",C_CYAN);
    draw_str(108,3,"MINAS:",C_WHITE); draw_int(144,3,ms.mines_left,C_RED);
    draw_str(168,3,"HI:",C_WHITE); draw_int(186,3,ms.hi,C_YELLOW);
    m3_hline(0,13,SCREEN_W,C_GRAY);
    draw_str(2,SCREEN_H-10,"A-REVELAR  L/R-BANDEIRA  B-MENU",C_DGRAY);

    for (int r=0;r<MS_ROWS;r++) for (int c=0;c<MS_COLS;c++) {
        int x=MS_OX+c*MS_CS, y=MS_OY+r*MS_CS;
        if (ms.revealed[r][c]) {
            m3_rect(x,y,MS_CS-1,MS_CS-1, RGB15(14,14,16));
            m3_border(x,y,MS_CS-1,MS_CS-1, C_GRAY);
            if (ms.board[r][c]<0) {
                // mine
                m3_rect(x+3,y+3,MS_CS-7,MS_CS-7,C_RED);
                m3_pixel(x+MS_CS/2,y+1,C_RED);
                m3_pixel(x+1,y+MS_CS/2,C_RED);
            } else if (ms.board[r][c]>0) {
                char s[2]={'0'+ms.board[r][c],0};
                draw_str(x+4,y+4,s,MS_NUM_COLORS[ms.board[r][c]]);
            }
        } else {
            // unrevealed
            uint16_t bg=(r==ms.cy&&c==ms.cx) ? RGB15(20,20,24) : RGB15(16,16,20);
            m3_rect(x,y,MS_CS-1,MS_CS-1,bg);
            m3_hline(x,y,MS_CS-1,C_WHITE);
            m3_vline(x,y,MS_CS-1,C_WHITE);
            m3_hline(x,y+MS_CS-2,MS_CS-1,C_DGRAY);
            m3_vline(x+MS_CS-2,y,MS_CS-1,C_DGRAY);
            if (ms.flagged[r][c]) {
                m3_rect(x+5,y+3,2,6,C_RED);
                m3_rect(x+7,y+3,4,3,C_RED);
            }
        }
    }
    m3_border(MS_OX-1,MS_OY-1,MS_COLS*MS_CS+1,MS_ROWS*MS_CS+1,C_WHITE);
    if (ms.state==1) draw_gameover(0);
    if (ms.state==2) draw_win("CAMPO LIMPO!");
}

// ============================================================
// GAME 9: MEMORY MATCH
// ============================================================
#define MM_COLS 4
#define MM_ROWS 4
#define MM_CS  36
#define MM_OX  ((SCREEN_W - MM_COLS*MM_CS)/2)
#define MM_OY  18

static struct {
    int cards[MM_ROWS*MM_COLS]; // 0-7 pairs
    int flipped[MM_ROWS*MM_COLS];
    int matched[MM_ROWS*MM_COLS];
    int cx, cy;
    int sel1, sel2; // -1 = none
    int flip_timer;
    int pairs_found, moves;
    int score, hi;
    int state; // 0=play 2=win
} mm;

static const uint16_t MM_COLORS[8]={C_RED,C_ORANGE,C_YELLOW,C_LGREEN,C_CYAN,C_BLUE,C_PURPLE,C_PINK};
static const char MM_SYMBOLS[8]={'A','B','C','D','E','F','G','H'};

static void mm_init(void) {
    // shuffle cards
    for (int i=0;i<MM_ROWS*MM_COLS;i++) mm.cards[i]=i/2;
    // Fisher-Yates
    for (int i=MM_ROWS*MM_COLS-1;i>0;i--) {
        int j=(int)(rng()%(i+1));
        int t=mm.cards[i]; mm.cards[i]=mm.cards[j]; mm.cards[j]=t;
    }
    for (int i=0;i<MM_ROWS*MM_COLS;i++) { mm.flipped[i]=0; mm.matched[i]=0; }
    mm.cx=0; mm.cy=0; mm.sel1=-1; mm.sel2=-1; mm.flip_timer=0;
    mm.pairs_found=0; mm.moves=0; mm.score=0; mm.state=0;
}

static int mm_update(void) {
    uint16_t p=key_pressed();
    if (p & KEY_B) { if(mm.state){mm_init();return 0;} return -1; }
    if (p & KEY_A && mm.state) { mm_init(); return 0; }
    if (mm.state) return 0;

    // wait for flip animation
    if (mm.sel2>=0) {
        if (++mm.flip_timer>50) {
            if (mm.cards[mm.sel1]==mm.cards[mm.sel2]) {
                mm.matched[mm.sel1]=mm.matched[mm.sel2]=1;
                mm.pairs_found++;
                mm.score+=100-mm.moves;
                if (mm.score>mm.hi) mm.hi=mm.score;
                if (mm.pairs_found==MM_ROWS*MM_COLS/2) mm.state=2;
            } else {
                mm.flipped[mm.sel1]=mm.flipped[mm.sel2]=0;
            }
            mm.sel1=mm.sel2=-1; mm.flip_timer=0;
        }
        return 0;
    }

    if ((p&KEY_LEFT)  && mm.cx>0) mm.cx--;
    if ((p&KEY_RIGHT) && mm.cx<MM_COLS-1) mm.cx++;
    if ((p&KEY_UP)    && mm.cy>0) mm.cy--;
    if ((p&KEY_DOWN)  && mm.cy<MM_ROWS-1) mm.cy++;

    if (p & KEY_A) {
        int idx=mm.cy*MM_COLS+mm.cx;
        if (mm.matched[idx]||mm.flipped[idx]) return 0;
        mm.flipped[idx]=1;
        mm.moves++;
        if (mm.sel1<0) { mm.sel1=idx; }
        else if (mm.sel1!=idx) { mm.sel2=idx; mm.flip_timer=0; }
    }
    return 0;
}

static void mm_draw(void) {
    m3_fill(C_NAVY);
    m3_rect(0,0,SCREEN_W,13,C_DGRAY);
    draw_str(2,3,"MEMORY MATCH",C_CYAN);
    draw_str(100,3,"MOV:",C_WHITE); draw_int(124,3,mm.moves,C_YELLOW);
    draw_str(158,3,"HI:",C_WHITE); draw_int(176,3,mm.hi,C_YELLOW);
    m3_hline(0,13,SCREEN_W,C_GRAY);

    for (int r=0;r<MM_ROWS;r++) for (int c=0;c<MM_COLS;c++) {
        int idx=r*MM_COLS+c;
        int x=MM_OX+c*MM_CS, y=MM_OY+r*MM_CS;
        int selected=(mm.cx==c && mm.cy==r);

        if (mm.matched[idx]) {
            m3_rect(x+1,y+1,MM_CS-3,MM_CS-3, MM_COLORS[mm.cards[idx]]);
            m3_border(x+1,y+1,MM_CS-3,MM_CS-3,C_WHITE);
            draw_char(x+MM_CS/2-2,y+MM_CS/2-3,MM_SYMBOLS[mm.cards[idx]],C_WHITE);
            // checkmark
            m3_pixel(x+4,y+MM_CS-6,C_WHITE); m3_pixel(x+5,y+MM_CS-5,C_WHITE);
            m3_pixel(x+6,y+MM_CS-4,C_WHITE);
        } else if (mm.flipped[idx]||mm.sel1==idx||mm.sel2==idx) {
            m3_rect(x+1,y+1,MM_CS-3,MM_CS-3, MM_COLORS[mm.cards[idx]]);
            m3_border(x+1,y+1,MM_CS-3,MM_CS-3,C_WHITE);
            draw_char(x+MM_CS/2-2,y+MM_CS/2-3,MM_SYMBOLS[mm.cards[idx]],C_WHITE);
        } else {
            uint16_t bg=selected?RGB15(18,18,26):RGB15(10,10,18);
            m3_rect(x+1,y+1,MM_CS-3,MM_CS-3,bg);
            m3_border(x+1,y+1,MM_CS-3,MM_CS-3,selected?C_YELLOW:C_GRAY);
            m3_pixel(x+MM_CS/2,y+MM_CS/2,C_GRAY);
        }
    }
    draw_str(2,SCREEN_H-10,"A-VIRAR  SETAS-MOVER",C_DGRAY);
    if (mm.state==2) draw_win("TODAS PARES!");
}

// ============================================================
// GAME 10: PAC-MAN
// ============================================================
#define PM_TILE 8
#define PM_COLS 20
#define PM_ROWS 16
#define PM_OX   20
#define PM_OY   14

static const uint8_t PM_MAP[PM_ROWS][PM_COLS] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,1},
    {1,3,1,1,2,1,1,1,2,1,1,2,1,1,1,2,1,1,3,1},
    {1,2,1,1,2,1,1,1,2,1,1,2,1,1,1,2,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,2,1,2,1,1,1,1,1,1,2,1,2,1,1,2,1},
    {1,2,2,2,2,1,2,2,2,1,1,2,2,2,1,2,2,2,2,1},
    {1,1,1,1,2,1,1,0,0,0,0,0,1,1,1,2,1,1,1,1},
    {1,1,1,1,2,1,0,0,0,0,0,0,0,1,1,2,1,1,1,1},
    {0,0,0,0,2,0,0,1,1,0,0,1,1,0,0,2,0,0,0,0},
    {1,1,1,1,2,1,0,0,0,0,0,0,0,1,1,2,1,1,1,1},
    {1,1,1,1,2,1,0,1,1,1,1,1,1,0,1,2,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,2,1,1,1,2,1,1,2,1,1,1,2,1,1,2,1},
    {1,3,2,1,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

static struct {
    int board[PM_ROWS][PM_COLS]; // copy of map (mutable dots)
    // pac
    int px, py, pdx, pdy, pnx, pny; // tile coords * 8
    int panim;
    // ghosts
    int gx[4], gy[4], gdx[4], gdy[4];
    int ghost_timer[4];
    int ghost_scared;
    int score, hi, lives;
    int state; // 0=play 1=dead 2=win
    int dots_left;
    int timer;
} pm;

static int pm_walkable(int tx, int ty) {
    if (tx<0||tx>=PM_COLS||ty<0||ty>=PM_ROWS) return 0;
    return pm.board[ty][tx]!=1;
}

static void pm_ghost_move(int g) {
    if (--pm.ghost_timer[g]>0) return;
    pm.ghost_timer[g]=4;
    int tx=pm.gx[g]/PM_TILE, ty=pm.gy[g]/PM_TILE;
    // try to continue, else turn
    int dirs[4][2]={{1,0},{-1,0},{0,1},{0,-1}};
    int best=-1, best_dist=9999;
    for (int d=0;d<4;d++) {
        int nx=tx+dirs[d][0], ny=ty+dirs[d][1];
        if (!pm_walkable(nx,ny)) continue;
        if (dirs[d][0]==-pm.gdx[g]&&dirs[d][1]==-pm.gdy[g]) continue; // no U-turn
        int dist;
        if (pm.ghost_scared) {
            dist=(int)rng()%100;
        } else {
            int tdx=pm.px/PM_TILE-nx, tdy=pm.py/PM_TILE-ny;
            dist=iabs(tdx)+iabs(tdy);
        }
        if (dist<best_dist) { best_dist=dist; best=d; }
    }
    if (best>=0) {
        pm.gdx[g]=dirs[best][0]; pm.gdy[g]=dirs[best][1];
    }
    pm.gx[g]+=pm.gdx[g]*PM_TILE;
    pm.gy[g]+=pm.gdy[g]*PM_TILE;
}

static void pm_init(void) {
    for (int r=0;r<PM_ROWS;r++) for (int c=0;c<PM_COLS;c++) pm.board[r][c]=PM_MAP[r][c];
    pm.px=PM_TILE*10; pm.py=PM_TILE*12;
    pm.pdx=0; pm.pdy=0; pm.pnx=0; pm.pny=0;
    pm.panim=0; pm.ghost_scared=0; pm.timer=0;
    // ghosts start positions
    int gsx[4]={9,10,9,10}, gsy[4]={8,8,9,9};
    for (int g=0;g<4;g++) {
        pm.gx[g]=gsx[g]*PM_TILE; pm.gy[g]=gsy[g]*PM_TILE;
        pm.gdx[g]=(g<2)?1:-1; pm.gdy[g]=0;
        pm.ghost_timer[g]=10+g*5;
    }
    pm.score=0; pm.lives=3; pm.state=0;
    pm.dots_left=0;
    for (int r=0;r<PM_ROWS;r++) for (int c=0;c<PM_COLS;c++)
        if (pm.board[r][c]==2||pm.board[r][c]==3) pm.dots_left++;
}

static int pm_update(void) {
    uint16_t k=key_down(), p=key_pressed();
    if (p&KEY_B) { if(pm.state){pm_init();return 0;} return -1; }
    if (p&KEY_A&&pm.state) { pm_init(); return 0; }
    if (pm.state) return 0;

    // buffer next direction
    if (k&KEY_LEFT)  { pm.pnx=-1; pm.pny=0; }
    if (k&KEY_RIGHT) { pm.pnx= 1; pm.pny=0; }
    if (k&KEY_UP)    { pm.pnx=0;  pm.pny=-1; }
    if (k&KEY_DOWN)  { pm.pnx=0;  pm.pny= 1; }

    if (++pm.timer<3) return 0;
    pm.timer=0;
    pm.panim++;

    // try buffered direction
    int tx=pm.px/PM_TILE + pm.pnx, ty=pm.py/PM_TILE + pm.pny;
    if (pm_walkable(tx,ty)) { pm.pdx=pm.pnx; pm.pdy=pm.pny; }

    // move pac
    int nx=pm.px/PM_TILE+pm.pdx, ny=pm.py/PM_TILE+pm.pdy;
    if (pm_walkable(nx,ny)) {
        pm.px+=pm.pdx*PM_TILE; pm.py+=pm.pdy*PM_TILE;
        // wrap tunnel
        if (pm.px<0) pm.px=(PM_COLS-1)*PM_TILE;
        if (pm.px>=(PM_COLS)*PM_TILE) pm.px=0;
    }

    // eat dot
    int ptx=pm.px/PM_TILE, pty=pm.py/PM_TILE;
    if (pm.board[pty][ptx]==2) {
        pm.board[pty][ptx]=0; pm.score+=10; pm.dots_left--;
        if(pm.score>pm.hi)pm.hi=pm.score;
    } else if (pm.board[pty][ptx]==3) {
        pm.board[pty][ptx]=0; pm.score+=50; pm.dots_left--;
        pm.ghost_scared=180;
        if(pm.score>pm.hi)pm.hi=pm.score;
    }
    if (!pm.dots_left) { pm.state=2; return 0; }
    if (pm.ghost_scared>0) pm.ghost_scared--;

    // move ghosts
    for (int g=0;g<4;g++) pm_ghost_move(g);

    // ghost collision
    for (int g=0;g<4;g++) {
        if (iabs(pm.gx[g]-pm.px)<PM_TILE && iabs(pm.gy[g]-pm.py)<PM_TILE) {
            if (pm.ghost_scared) {
                pm.score+=200; pm.gx[g]=9*PM_TILE; pm.gy[g]=8*PM_TILE;
            } else {
                pm.lives--;
                if (pm.lives<=0) pm.state=1;
                else { pm.px=10*PM_TILE; pm.py=12*PM_TILE; pm.pdx=pm.pdy=0; }
            }
        }
    }
    return 0;
}

static void pm_draw(void) {
    m3_fill(C_BLACK);
    // draw maze
    for (int r=0;r<PM_ROWS;r++) for (int c=0;c<PM_COLS;c++) {
        int x=PM_OX+c*PM_TILE, y=PM_OY+r*PM_TILE;
        int t=pm.board[r][c];
        if (t==1) {
            m3_rect(x,y,PM_TILE,PM_TILE,C_BLUE);
            m3_border(x,y,PM_TILE,PM_TILE,C_LBLUE);
        } else if (t==2) {
            m3_pixel(x+PM_TILE/2, y+PM_TILE/2, C_YELLOW);
        } else if (t==3) {
            // power pellet
            int pulse=(pm.timer/4)&1;
            if (pulse) m3_rect(x+PM_TILE/2-2,y+PM_TILE/2-2,4,4,C_YELLOW);
        }
    }
    // ghosts
    static const uint16_t GC[4]={C_RED,C_PINK,C_CYAN,C_ORANGE};
    for (int g=0;g<4;g++) {
        int gx=PM_OX+pm.gx[g], gy=PM_OY+pm.gy[g];
        uint16_t gc=pm.ghost_scared?C_BLUE:GC[g];
        m3_rect(gx,gy,PM_TILE,PM_TILE-2,gc);
        // ghost "feet"
        m3_pixel(gx,gy+PM_TILE-2,gc); m3_pixel(gx+2,gy+PM_TILE-2,gc);
        m3_pixel(gx+4,gy+PM_TILE-2,gc); m3_pixel(gx+6,gy+PM_TILE-2,gc);
        // eyes
        if (!pm.ghost_scared) {
            m3_pixel(gx+1,gy+2,C_WHITE); m3_pixel(gx+5,gy+2,C_WHITE);
            m3_pixel(gx+1+pm.gdx[g],gy+2+pm.gdy[g],C_NAVY);
            m3_pixel(gx+5+pm.gdx[g],gy+2+pm.gdy[g],C_NAVY);
        }
    }
    // pac-man
    int px2=PM_OX+pm.px, py2=PM_OY+pm.py;
    int mouth=(pm.panim/2)&3;
    m3_rect(px2,py2,PM_TILE,PM_TILE,C_YELLOW);
    if (mouth<3) {
        // mouth opening
        if (pm.pdx>0) m3_rect(px2+PM_TILE/2,py2+mouth,PM_TILE/2,PM_TILE-mouth*2,C_BLACK);
        else if (pm.pdx<0) m3_rect(px2,py2+mouth,PM_TILE/2,PM_TILE-mouth*2,C_BLACK);
        else if (pm.pdy<0) m3_rect(px2+mouth,py2,PM_TILE-mouth*2,PM_TILE/2,C_BLACK);
        else m3_rect(px2+mouth,py2+PM_TILE/2,PM_TILE-mouth*2,PM_TILE/2,C_BLACK);
    }
    // lives
    for (int i=0;i<pm.lives;i++) m3_rect(PM_OX+i*10,SCREEN_H-8,7,7,C_YELLOW);
    draw_hud("PAC-MAN",pm.score,pm.hi);
    if (pm.state==1) draw_gameover(pm.score);
    if (pm.state==2) draw_win("FASE COMPLETA!");
}

// ============================================================
// MENU
// ============================================================
static int menu_scroll = 0;
static int menu_anim = 0;

static void menu_draw(void) {
    // animated bg
    menu_anim++;
    for (int y=0;y<SCREEN_H;y++) for (int x=0;x<SCREEN_W;x+=4) {
        int v = ((x^y)^(menu_anim/8)) & 0xF;
        m3_pixel(x,y, RGB15(0,0,v/2));
    }

    // title box
    draw_panel(10, 8, 220, 22, C_NAVY, C_YELLOW);
    draw_str_centered(12, "* GBA MINI GAMES *", C_YELLOW);
    draw_str_centered(20, "10 JOGOS CLASSICOS", C_CYAN);

    // game list
    int visible = 7;
    for (int i=0;i<NUM_GAMES;i++) {
        if (i < menu_scroll || i >= menu_scroll+visible) continue;
        int slot = i - menu_scroll;
        int y = 35 + slot*17;
        if (i == menu_sel) {
            m3_rect(10, y-1, 220, 15, RGB15(6,6,18));
            m3_border(10, y-1, 220, 15, C_YELLOW);
            draw_str(18, y+2, GAME_NAMES[i], C_YELLOW);
            draw_str(188, y+2, "HI:", C_WHITE);
            draw_int(206, y+2, hi_scores[i], C_CYAN);
        } else {
            draw_str(18, y+2, GAME_NAMES[i], C_GRAY);
        }
    }

    // scrollbar
    m3_vline(234, 35, visible*17, C_DGRAY);
    int sb_h = visible*17/NUM_GAMES;
    int sb_y = 35 + menu_scroll*17/NUM_GAMES;
    m3_rect(232, sb_y, 4, sb_h, C_GRAY);

    draw_str_centered(148, "SETAS-NAVEGAR  A-JOGAR", C_GRAY);
    draw_str_centered(155, "B-VOLTAR  START-JOGAR", C_DGRAY);
}

static int menu_update(void) {
    uint16_t p = key_pressed();
    if ((p & KEY_UP) && menu_sel>0) {
        menu_sel--;
        if (menu_sel < menu_scroll) menu_scroll=menu_sel;
    }
    if ((p & KEY_DOWN) && menu_sel<NUM_GAMES-1) {
        menu_sel++;
        if (menu_sel >= menu_scroll+7) menu_scroll=menu_sel-6;
    }
    if ((p & KEY_A) || (p & KEY_START)) {
        current_game = menu_sel;
        // init selected game
        switch (current_game) {
            case 0: sn_init(); break;
            case 1: tt_init(); break;
            case 2: pong_init(); break;
            case 3: ttt_init(); break;
            case 4: bo_init(); break;
            case 5: fl_init(); break;
            case 6: si_init(); break;
            case 7: ms_init(); break;
            case 8: mm_init(); break;
            case 9: pm_init(); break;
        }
    }
    return 0;
}

// ============================================================
// MAIN LOOP
// ============================================================
int main(void) {
    REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;

    sn.hi=0; tt.hi=0; pong.hi=0; ttt.hi_x=0; ttt.hi_o=0;
    bo.hi=0; fl.hi=0; si.hi=0; ms.hi=0; mm.hi=0; pm.hi=0;
    current_game=-1;

    while (1) {
        vsync();
        if (current_game < 0) {
            menu_update();
            menu_draw();
        } else {
            int ret = 0;
            switch (current_game) {
                case 0: ret=sn_update(); sn_draw(); break;
                case 1: ret=tt_update(); tt_draw(); break;
                case 2: ret=pong_update(); pong_draw(); break;
                case 3: ret=ttt_update(); ttt_draw(); break;
                case 4: ret=bo_update(); bo_draw(); break;
                case 5: ret=fl_update(); fl_draw(); break;
                case 6: ret=si_update(); si_draw(); break;
                case 7: ret=ms_update(); ms_draw(); break;
                case 8: ret=mm_update(); mm_draw(); break;
                case 9: ret=pm_update(); pm_draw(); break;
            }
            if (ret < 0) {
                hi_scores[current_game] = 0; // update hi later
                current_game = -1;
            }
        }
    }
    return 0;
}
