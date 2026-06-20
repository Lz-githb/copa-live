#include "gba.h"

// ============================================================
// PALETTE INDICES
// ============================================================
#define C_BLACK    0
#define C_WHITE    1
#define C_GRAY     2
#define C_DGRAY    3
#define C_RED      4
#define C_DRED     5
#define C_GREEN    6
#define C_LGREEN   7
#define C_BLUE     8
#define C_LBLUE    9
#define C_YELLOW  10
#define C_ORANGE  11
#define C_CYAN    12
#define C_PURPLE  13
#define C_PINK    14
#define C_BROWN   15
#define C_NAVY    16
#define C_DBLUE   17
#define C_TEAL    18
#define C_SKIN    19
#define C_DKGRN   20   // snake body dark
#define C_DKGRN2  21   // snake grid checker
#define C_SNKBG   22   // snake background
#define C_DARKBG  23   // very dark bg (breakout, space inv.)
#define C_TGRID   24   // tetris grid lines
#define C_TSIDE   25   // tetris sidebar bg
#define C_CLOUD   26   // cloud / light gray
#define C_SKYHI   27   // flappy sky top
#define C_SKYLO   28   // flappy sky bottom
#define C_MNBG    29   // minesweeper bg
#define C_MNREV   30   // minesweeper revealed tile
#define C_MNUNR   31   // minesweeper unrevealed
#define C_MNSEL   32   // minesweeper cursor
#define C_MMSEL   33   // memory selected
#define C_MMUNSEL 34   // memory unselected
#define C_MENUSEL 35   // menu selection highlight
#define C_TTTBG   36   // tic-tac-toe bg
#define C_TTTHL   37   // tic-tac-toe cell highlight
#define C_MIDBLUE 38   // pac-man wall border
#define C_STARBG  39   // menu/invaders star bg

static void setup_palette(void) {
    PAL_BG[C_BLACK]  = RGB15( 0, 0, 0);
    PAL_BG[C_WHITE]  = RGB15(31,31,31);
    PAL_BG[C_GRAY]   = RGB15(15,15,15);
    PAL_BG[C_DGRAY]  = RGB15( 8, 8, 8);
    PAL_BG[C_RED]    = RGB15(31, 4, 4);
    PAL_BG[C_DRED]   = RGB15(16, 0, 0);
    PAL_BG[C_GREEN]  = RGB15( 4,24, 4);
    PAL_BG[C_LGREEN] = RGB15(10,31,10);
    PAL_BG[C_BLUE]   = RGB15( 4, 8,28);
    PAL_BG[C_LBLUE]  = RGB15(10,18,31);
    PAL_BG[C_YELLOW] = RGB15(31,28, 4);
    PAL_BG[C_ORANGE] = RGB15(31,16, 0);
    PAL_BG[C_CYAN]   = RGB15( 0,28,28);
    PAL_BG[C_PURPLE] = RGB15(20, 0,28);
    PAL_BG[C_PINK]   = RGB15(31, 8,20);
    PAL_BG[C_BROWN]  = RGB15(18, 9, 0);
    PAL_BG[C_NAVY]   = RGB15( 0, 0,16);
    PAL_BG[C_DBLUE]  = RGB15( 2, 4,18);
    PAL_BG[C_TEAL]   = RGB15( 0,16,16);
    PAL_BG[C_SKIN]   = RGB15(31,22,16);
    PAL_BG[C_DKGRN]  = RGB15( 2,14, 2);
    PAL_BG[C_DKGRN2] = RGB15( 1, 8, 1);
    PAL_BG[C_SNKBG]  = RGB15( 0, 3, 0);
    PAL_BG[C_DARKBG] = RGB15( 0, 0, 2);
    PAL_BG[C_TGRID]  = RGB15( 2, 2, 2);
    PAL_BG[C_TSIDE]  = RGB15( 2, 2, 6);
    PAL_BG[C_CLOUD]  = RGB15(26,26,28);
    PAL_BG[C_SKYHI]  = RGB15( 6,18,28);
    PAL_BG[C_SKYLO]  = RGB15( 4,12,22);
    PAL_BG[C_MNBG]   = RGB15( 8, 8,10);
    PAL_BG[C_MNREV]  = RGB15(13,13,15);
    PAL_BG[C_MNUNR]  = RGB15(15,15,18);
    PAL_BG[C_MNSEL]  = RGB15(20,20,24);
    PAL_BG[C_MMSEL]  = RGB15(16,16,24);
    PAL_BG[C_MMUNSEL]= RGB15( 8, 8,16);
    PAL_BG[C_MENUSEL]= RGB15( 4, 4,14);
    PAL_BG[C_TTTBG]  = RGB15( 1, 1, 5);
    PAL_BG[C_TTTHL]  = RGB15( 4, 4,12);
    PAL_BG[C_MIDBLUE]= RGB15( 8,12,31);
    PAL_BG[C_STARBG] = RGB15( 0, 0, 1);
}

// ============================================================
// SHARED UI
// ============================================================
#define NUM_GAMES 10
static int current_game = -1;
static int hi_scores[NUM_GAMES];
static const char *GAME_NAMES[NUM_GAMES] = {
    "1.SNAKE","2.TETRIS","3.PONG","4.JOGO DA VELHA",
    "5.BREAKOUT","6.FLAPPY BIRD","7.SPACE INVADERS",
    "8.MINESWEEPER","9.MEMORY MATCH","10.PAC-MAN"
};

static void draw_panel(int x, int y, int w, int h, uint8_t bg, uint8_t border) {
    m4_rect(x,y,w,h,bg);
    m4_border(x,y,w,h,border);
    m4_border(x+1,y+1,w-2,h-2,border);
}

static void draw_hud(const char *name, int score, int hi) {
    m4_rect(0,0,SCREEN_W,12,C_DGRAY);
    m4_hline(0,12,SCREEN_W,C_GRAY);
    draw_str(2,2,name,C_CYAN);
    draw_str(90,2,"SC:",C_WHITE);  draw_int(108,2,score,C_YELLOW);
    draw_str(148,2,"HI:",C_WHITE); draw_int(166,2,hi,C_YELLOW);
    draw_str(200,2,"B-MENU",C_GRAY);
}

static void draw_gameover(int score) {
    draw_panel(50,55,140,52,C_DBLUE,C_YELLOW);
    draw_str_centered(62,"GAME OVER",C_RED);
    draw_str(76,74,"SCORE:",C_WHITE); draw_int(112,74,score,C_YELLOW);
    draw_str_centered(86,"A-REINICIAR",C_LGREEN);
    draw_str_centered(96,"B-MENU",C_GRAY);
}

static void draw_win(const char *msg) {
    draw_panel(50,55,140,52,C_NAVY,C_LGREEN);
    draw_str_centered(62,"VOCE GANHOU!",C_LGREEN);
    draw_str_centered(74,msg,C_YELLOW);
    draw_str_centered(86,"A-REINICIAR",C_CYAN);
    draw_str_centered(96,"B-MENU",C_GRAY);
}

// ============================================================
// GAME 1: SNAKE
// ============================================================
#define SN_COLS 29
#define SN_ROWS 17
#define SN_CS    8
#define SN_OX    4
#define SN_OY   14

static struct {
    int bx[SN_COLS*SN_ROWS], by[SN_COLS*SN_ROWS];
    int len, dx, dy, ndx, ndy;
    int fx, fy, ftimer;
    int score, hi, timer, speed, state;
} sn;

static void sn_spawn_food(void) {
    int ok=0;
    while(!ok) {
        sn.fx=(int)(rng()%SN_COLS); sn.fy=(int)(rng()%SN_ROWS);
        ok=1;
        for(int i=0;i<sn.len;i++) if(sn.bx[i]==sn.fx&&sn.by[i]==sn.fy){ok=0;break;}
    }
}

static void sn_init(void) {
    sn.len=3; sn.dx=1; sn.dy=0; sn.ndx=1; sn.ndy=0;
    sn.bx[0]=14; sn.by[0]=8; sn.bx[1]=13; sn.by[1]=8; sn.bx[2]=12; sn.by[2]=8;
    sn.score=0; sn.timer=0; sn.speed=8; sn.state=0; sn.ftimer=0;
    sn_spawn_food();
}

static int sn_update(void) {
    uint16_t p=key_pressed();
    if(p&KEY_B) return -1;
    if(sn.state==1) { if(p&KEY_A) sn_init(); return 0; }
    if((p&KEY_UP)   &&sn.dy==0){sn.ndx=0;sn.ndy=-1;}
    if((p&KEY_DOWN) &&sn.dy==0){sn.ndx=0;sn.ndy= 1;}
    if((p&KEY_LEFT) &&sn.dx==0){sn.ndx=-1;sn.ndy=0;}
    if((p&KEY_RIGHT)&&sn.dx==0){sn.ndx= 1;sn.ndy=0;}
    sn.ftimer++;
    if(++sn.timer<sn.speed) return 0;
    sn.timer=0;
    sn.dx=sn.ndx; sn.dy=sn.ndy;
    int nx=sn.bx[0]+sn.dx, ny=sn.by[0]+sn.dy;
    if(nx<0||nx>=SN_COLS||ny<0||ny>=SN_ROWS){sn.state=1;return 0;}
    // check self-collision (exclude tail which will move)
    for(int i=0;i<sn.len-1;i++) if(sn.bx[i]==nx&&sn.by[i]==ny){sn.state=1;return 0;}
    for(int i=sn.len;i>0;i--){sn.bx[i]=sn.bx[i-1];sn.by[i]=sn.by[i-1];}
    sn.bx[0]=nx; sn.by[0]=ny;
    if(nx==sn.fx&&ny==sn.fy){
        sn.len++; sn.score++;
        if(sn.score>sn.hi) sn.hi=sn.score;
        sn.speed=imax(3,8-sn.score/5);
        sn_spawn_food();
    }
    return 0;
}

static void sn_draw(void) {
    m4_fill(C_SNKBG);
    // checkerboard grid
    for(int x=0;x<SN_COLS;x++) for(int y=0;y<SN_ROWS;y++) {
        uint8_t c=((x+y)&1)?C_DKGRN2:C_SNKBG;
        m4_rect(SN_OX+x*SN_CS,SN_OY+y*SN_CS,SN_CS-1,SN_CS-1,c);
    }
    // food: pulsing
    int fx=SN_OX+sn.fx*SN_CS, fy=SN_OY+sn.fy*SN_CS;
    uint8_t fc=(sn.ftimer/8)&1?C_RED:C_ORANGE;
    m4_rect(fx+1,fy+1,SN_CS-2,SN_CS-2,fc);
    m4_pixel(fx+2,fy+2,C_WHITE);
    // snake
    for(int i=sn.len-1;i>=0;i--) {
        int px=SN_OX+sn.bx[i]*SN_CS, py=SN_OY+sn.by[i]*SN_CS;
        uint8_t c=(i==0)?C_LGREEN:(i%2==0)?C_GREEN:C_DKGRN;
        m4_rect(px+1,py+1,SN_CS-2,SN_CS-2,c);
        if(i==0) { m4_pixel(px+1,py+1,C_WHITE); m4_pixel(px+5,py+1,C_WHITE); }
    }
    m4_border(SN_OX-1,SN_OY-1,SN_COLS*SN_CS+2,SN_ROWS*SN_CS+2,C_GREEN);
    draw_hud("SNAKE",sn.score,sn.hi);
    if(sn.state==1) draw_gameover(sn.score);
}

// ============================================================
// GAME 2: TETRIS
// ============================================================
#define TT_COLS 10
#define TT_ROWS 18
#define TT_CS    8
#define TT_OX   44
#define TT_OY   14

// Standard SRS piece offsets [piece][rotation][cell] = {col, row}
static const int8_t TT_PIECES[7][4][4][2] = {
    // I
    {{{0,1},{1,1},{2,1},{3,1}}, {{2,0},{2,1},{2,2},{2,3}}, {{0,2},{1,2},{2,2},{3,2}}, {{1,0},{1,1},{1,2},{1,3}}},
    // J
    {{{0,0},{0,1},{1,1},{2,1}}, {{1,0},{2,0},{1,1},{1,2}}, {{0,1},{1,1},{2,1},{2,2}}, {{1,0},{1,1},{0,2},{1,2}}},
    // L
    {{{2,0},{0,1},{1,1},{2,1}}, {{1,0},{1,1},{1,2},{2,2}}, {{0,1},{1,1},{2,1},{0,2}}, {{0,0},{1,0},{1,1},{1,2}}},
    // O
    {{{1,0},{2,0},{1,1},{2,1}}, {{1,0},{2,0},{1,1},{2,1}}, {{1,0},{2,0},{1,1},{2,1}}, {{1,0},{2,0},{1,1},{2,1}}},
    // S
    {{{1,0},{2,0},{0,1},{1,1}}, {{1,0},{1,1},{2,1},{2,2}}, {{1,0},{2,0},{0,1},{1,1}}, {{1,0},{1,1},{2,1},{2,2}}},
    // T
    {{{1,0},{0,1},{1,1},{2,1}}, {{1,0},{1,1},{2,1},{1,2}}, {{0,1},{1,1},{2,1},{1,2}}, {{1,0},{0,1},{1,1},{1,2}}},
    // Z
    {{{0,0},{1,0},{1,1},{2,1}}, {{2,0},{1,1},{2,1},{1,2}}, {{0,0},{1,0},{1,1},{2,1}}, {{2,0},{1,1},{2,1},{1,2}}},
};
static const uint8_t TT_COLORS[7]={C_CYAN,C_BLUE,C_ORANGE,C_YELLOW,C_LGREEN,C_PURPLE,C_RED};

static struct {
    int board[TT_ROWS][TT_COLS];
    uint8_t bcolor[TT_ROWS][TT_COLS];
    int piece,rot,px,py,next_piece;
    int score,hi,lines,level;
    int timer,drop_speed,state;
} tt;

static int tt_collide(int piece,int rot,int px,int py) {
    for(int i=0;i<4;i++){
        int bx=px+TT_PIECES[piece][rot][i][0];
        int by=py+TT_PIECES[piece][rot][i][1];
        if(bx<0||bx>=TT_COLS||by>=TT_ROWS) return 1;
        if(by>=0&&by<TT_ROWS&&tt.board[by][bx]) return 1;
    }
    return 0;
}

static void tt_place(void) {
    for(int i=0;i<4;i++){
        int bx=tt.px+TT_PIECES[tt.piece][tt.rot][i][0];
        int by=tt.py+TT_PIECES[tt.piece][tt.rot][i][1];
        if(by>=0&&by<TT_ROWS){tt.board[by][bx]=1;tt.bcolor[by][bx]=TT_COLORS[tt.piece];}
    }
    int cleared=0;
    for(int y=TT_ROWS-1;y>=0;y--){
        int full=1;
        for(int x=0;x<TT_COLS;x++) if(!tt.board[y][x]){full=0;break;}
        if(full){
            cleared++;
            for(int r=y;r>0;r--) for(int c=0;c<TT_COLS;c++){tt.board[r][c]=tt.board[r-1][c];tt.bcolor[r][c]=tt.bcolor[r-1][c];}
            for(int c=0;c<TT_COLS;c++) tt.board[0][c]=0;
            y++; // recheck same row
        }
    }
    static const int pts[5]={0,100,300,500,800};
    if(cleared>0){
        tt.lines+=cleared; tt.level=tt.lines/10+1;
        tt.score+=pts[cleared]*tt.level;
        if(tt.score>tt.hi) tt.hi=tt.score;
        tt.drop_speed=imax(2,48-tt.level*4);
    }
}

static void tt_new_piece(void) {
    tt.piece=tt.next_piece; tt.next_piece=(int)(rng()%7);
    tt.rot=0; tt.px=3; tt.py=0;
}

static void tt_init(void) {
    for(int y=0;y<TT_ROWS;y++) for(int x=0;x<TT_COLS;x++){tt.board[y][x]=0;}
    tt.score=0;tt.lines=0;tt.level=1;tt.timer=0;tt.drop_speed=48;tt.state=0;
    tt.next_piece=(int)(rng()%7);
    tt_new_piece();
}

static int tt_update(void) {
    uint16_t p=key_pressed();
    if(p&KEY_B) return -1;
    if(tt.state==1){if(p&KEY_A)tt_init();return 0;}
    if((p&KEY_LEFT) &&!tt_collide(tt.piece,tt.rot,tt.px-1,tt.py)) tt.px--;
    if((p&KEY_RIGHT)&&!tt_collide(tt.piece,tt.rot,tt.px+1,tt.py)) tt.px++;
    if(p&KEY_UP){
        int nr=(tt.rot+1)%4;
        if(!tt_collide(tt.piece,nr,tt.px,tt.py)) tt.rot=nr;
        else if(!tt_collide(tt.piece,nr,tt.px-1,tt.py)){tt.px--;tt.rot=nr;}
        else if(!tt_collide(tt.piece,nr,tt.px+1,tt.py)){tt.px++;tt.rot=nr;}
    }
    int spd=(key_down()&KEY_DOWN)?2:tt.drop_speed;
    if(++tt.timer>=spd){
        tt.timer=0;
        if(!tt_collide(tt.piece,tt.rot,tt.px,tt.py+1)) tt.py++;
        else{tt_place();tt_new_piece();if(tt_collide(tt.piece,tt.rot,tt.px,tt.py))tt.state=1;}
    }
    return 0;
}

static void tt_draw_block(int bx,int by,uint8_t col) {
    int px=TT_OX+bx*TT_CS, py=TT_OY+by*TT_CS;
    m4_rect(px,py,TT_CS-1,TT_CS-1,col);
    m4_hline(px,py,TT_CS-2,C_WHITE);
    m4_vline(px,py,TT_CS-2,C_WHITE);
    m4_hline(px,py+TT_CS-2,TT_CS-2,C_BLACK);
    m4_vline(px+TT_CS-2,py,TT_CS-2,C_BLACK);
}

static void tt_draw(void) {
    m4_fill(C_DGRAY);
    m4_rect(TT_OX,TT_OY,TT_COLS*TT_CS,TT_ROWS*TT_CS,C_BLACK);
    for(int x=0;x<=TT_COLS;x++) m4_vline(TT_OX+x*TT_CS,TT_OY,TT_ROWS*TT_CS,C_TGRID);
    for(int y=0;y<=TT_ROWS;y++) m4_hline(TT_OX,TT_OY+y*TT_CS,TT_COLS*TT_CS,C_TGRID);
    for(int y=0;y<TT_ROWS;y++) for(int x=0;x<TT_COLS;x++)
        if(tt.board[y][x]) tt_draw_block(x,y,tt.bcolor[y][x]);
    // current piece
    for(int i=0;i<4;i++){
        int bx=tt.px+TT_PIECES[tt.piece][tt.rot][i][0];
        int by=tt.py+TT_PIECES[tt.piece][tt.rot][i][1];
        if(by>=0) tt_draw_block(bx,by,TT_COLORS[tt.piece]);
    }
    // ghost
    int gy=tt.py;
    while(!tt_collide(tt.piece,tt.rot,tt.px,gy+1)) gy++;
    if(gy!=tt.py) for(int i=0;i<4;i++){
        int bx=tt.px+TT_PIECES[tt.piece][tt.rot][i][0];
        int by=gy+TT_PIECES[tt.piece][tt.rot][i][1];
        if(by>=0) m4_border(TT_OX+bx*TT_CS,TT_OY+by*TT_CS,TT_CS-1,TT_CS-1,C_GRAY);
    }
    m4_border(TT_OX-1,TT_OY-1,TT_COLS*TT_CS+2,TT_ROWS*TT_CS+2,C_WHITE);
    // sidebar
    draw_panel(130,14,110,145,C_TSIDE,C_BLUE);
    draw_str(136,20,"NEXT:",C_CYAN);
    for(int i=0;i<4;i++){
        int bx=TT_PIECES[tt.next_piece][0][i][0];
        int by=TT_PIECES[tt.next_piece][0][i][1];
        m4_rect(148+bx*7,32+by*7,6,6,TT_COLORS[tt.next_piece]);
    }
    draw_str(136,68,"SCORE:",C_WHITE); draw_int(136,78,tt.score,C_YELLOW);
    draw_str(136,94,"LINES:",C_WHITE); draw_int(136,104,tt.lines,C_CYAN);
    draw_str(136,118,"LEVEL:",C_WHITE); draw_int(136,128,tt.level,C_LGREEN);
    draw_str(136,142,"HI:",C_WHITE);   draw_int(136,152,tt.hi,C_YELLOW);
    draw_hud("TETRIS",tt.score,tt.hi);
    if(tt.state==1) draw_gameover(tt.score);
}

// ============================================================
// GAME 3: PONG
// ============================================================
#define PN_PH  30
#define PN_PW   4
#define PN_BR   4
#define PN_P1X  8
#define PN_P2X (SCREEN_W-8-PN_PW)

static struct {
    int bx,by,bdx,bdy; // * 256
    int p1y,p2y,p1sc,p2sc,hi;
    int flash,state;
} pong;

static void pong_serve(int dir) {
    pong.bx=(SCREEN_W/2)<<8; pong.by=(SCREEN_H/2)<<8;
    pong.bdx=dir*(2<<8); pong.bdy=((int)(rng()%3)-1)*(2<<8);
    if(pong.bdy==0) pong.bdy=1<<8;
}

static void pong_init(void) {
    pong.p1y=SCREEN_H/2-PN_PH/2; pong.p2y=pong.p1y;
    pong.p1sc=0; pong.p2sc=0; pong.flash=0; pong.state=0;
    pong_serve(1);
}

static int pong_update(void) {
    uint16_t k=key_down(), p=key_pressed();
    if(p&KEY_B) return -1;
    if((k&KEY_UP)  &&pong.p1y>14)         pong.p1y-=3;
    if((k&KEY_DOWN)&&pong.p1y<SCREEN_H-PN_PH-1) pong.p1y+=3;
    // AI
    int bcy=pong.by>>8, p2cy=pong.p2y+PN_PH/2, diff=bcy-p2cy;
    if(diff>1)  pong.p2y+=imin(3,diff);
    if(diff<-1) pong.p2y-=imin(3,-diff);
    pong.p2y=iclamp(pong.p2y,14,SCREEN_H-PN_PH-1);
    pong.bx+=pong.bdx; pong.by+=pong.bdy;
    int bx=pong.bx>>8, by=pong.by>>8;
    if(by<=14)         { pong.bdy= iabs(pong.bdy); }
    if(by>=SCREEN_H-PN_BR) { pong.bdy=-iabs(pong.bdy); }
    // paddle 1
    if(bx<=PN_P1X+PN_PW+PN_BR&&bx>=PN_P1X+PN_PW-2&&by>=pong.p1y-PN_BR&&by<=pong.p1y+PN_PH+PN_BR){
        pong.bdx=iabs(pong.bdx)+(1<<6);
        pong.bdy=(by-pong.p1y-PN_PH/2)*(1<<6);
    }
    // paddle 2
    if(bx>=PN_P2X-PN_BR&&bx<=PN_P2X+2&&by>=pong.p2y-PN_BR&&by<=pong.p2y+PN_PH+PN_BR){
        pong.bdx=-(iabs(pong.bdx)+(1<<6));
        pong.bdy=(by-pong.p2y-PN_PH/2)*(1<<6);
    }
    // speed cap
    if(iabs(pong.bdx)>(5<<8)) pong.bdx=(pong.bdx>0)?(5<<8):-(5<<8);
    if(iabs(pong.bdy)>(4<<8)) pong.bdy=(pong.bdy>0)?(4<<8):-(4<<8);
    if(bx<2)         {pong.p2sc++;pong.flash=30;pong_serve(1);}
    if(bx>SCREEN_W-2){pong.p1sc++;if(pong.p1sc>pong.hi)pong.hi=pong.p1sc;pong.flash=30;pong_serve(-1);}
    if(pong.flash>0) pong.flash--;
    return 0;
}

static void pong_draw(void) {
    m4_fill(C_BLACK);
    for(int y=14;y<SCREEN_H;y+=6) m4_rect(SCREEN_W/2-1,y,2,3,C_DGRAY);
    m4_rect(0,0,SCREEN_W,13,C_DGRAY); m4_hline(0,13,SCREEN_W,C_GRAY);
    draw_str(2,3,"PONG",C_CYAN);
    draw_str(80,3,"P1:",C_WHITE); draw_int(96,3,pong.p1sc,C_YELLOW);
    draw_str(120,3,"CPU:",C_WHITE); draw_int(144,3,pong.p2sc,C_RED);
    draw_str(175,3,"HI:",C_WHITE); draw_int(193,3,pong.hi,C_YELLOW);
    m4_hline(0,13,SCREEN_W,C_GRAY);
    // paddles
    uint8_t p1c=(pong.flash>0)?C_YELLOW:C_LBLUE;
    m4_rect(PN_P1X,pong.p1y,PN_PW,PN_PH,p1c);
    m4_rect(PN_P1X+1,pong.p1y,1,PN_PH,C_WHITE);
    m4_rect(PN_P2X,pong.p2y,PN_PW,PN_PH,C_RED);
    m4_rect(PN_P2X+2,pong.p2y,1,PN_PH,C_WHITE);
    // ball
    int bx=pong.bx>>8, by=pong.by>>8;
    m4_rect(bx-PN_BR,by-PN_BR,PN_BR*2,PN_BR*2,C_WHITE);
    m4_pixel(bx-PN_BR,by-PN_BR,C_GRAY); m4_pixel(bx+PN_BR-1,by-PN_BR,C_GRAY);
    draw_str(70,SCREEN_H-10,"CIMA/BAIXO-MOVER",C_DGRAY);
}

// ============================================================
// GAME 4: JOGO DA VELHA
// ============================================================
static const int TTT_WINS[8][3]={{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};

static struct {
    int board[9];
    int cur,turn,state,ai_delay;
    int winner_line[3];
    int hi_x,hi_o;
} ttt;

static int ttt_has_win(int player) {
    for(int i=0;i<8;i++)
        if(ttt.board[TTT_WINS[i][0]]==player&&ttt.board[TTT_WINS[i][1]]==player&&ttt.board[TTT_WINS[i][2]]==player) return 1;
    return 0;
}

static int ttt_check_win(int player) {
    for(int i=0;i<8;i++)
        if(ttt.board[TTT_WINS[i][0]]==player&&ttt.board[TTT_WINS[i][1]]==player&&ttt.board[TTT_WINS[i][2]]==player){
            ttt.winner_line[0]=TTT_WINS[i][0]; ttt.winner_line[1]=TTT_WINS[i][1]; ttt.winner_line[2]=TTT_WINS[i][2];
            return 1;
        }
    return 0;
}

static int ttt_minimax(int depth, int is_max) {
    if(ttt_has_win(1)) return -10+depth;
    if(ttt_has_win(2)) return  10-depth;
    int empty=0; for(int i=0;i<9;i++) if(!ttt.board[i]) empty++;
    if(!empty) return 0;
    int best=is_max?-100:100;
    for(int i=0;i<9;i++){
        if(!ttt.board[i]){
            ttt.board[i]=is_max?2:1;
            int v=ttt_minimax(depth+1,!is_max);
            ttt.board[i]=0;
            if(is_max){if(v>best)best=v;} else{if(v<best)best=v;}
        }
    }
    return best;
}

static int ttt_ai_move(void) {
    int best=-100,pos=-1;
    for(int i=0;i<9;i++){
        if(!ttt.board[i]){
            ttt.board[i]=2;
            int v=ttt_minimax(0,0);
            ttt.board[i]=0;
            if(v>best){best=v;pos=i;}
        }
    }
    return pos;
}

static void ttt_init(void) {
    for(int i=0;i<9;i++) ttt.board[i]=0;
    ttt.cur=4; ttt.turn=1; ttt.state=0; ttt.ai_delay=0;
}

static int ttt_update(void) {
    uint16_t p=key_pressed();
    if(p&KEY_B){if(ttt.state){ttt_init();return 0;}return -1;}
    if(p&KEY_A&&ttt.state){ttt_init();return 0;}
    if(ttt.state) return 0;
    if(ttt.turn==1){
        if((p&KEY_LEFT) &&(ttt.cur%3)>0) ttt.cur--;
        if((p&KEY_RIGHT)&&(ttt.cur%3)<2) ttt.cur++;
        if((p&KEY_UP)   &&ttt.cur>=3)    ttt.cur-=3;
        if((p&KEY_DOWN) &&ttt.cur<6)     ttt.cur+=3;
        if((p&KEY_A)&&!ttt.board[ttt.cur]){
            ttt.board[ttt.cur]=1;
            if(ttt_check_win(1)){ttt.state=1;ttt.hi_x++;}
            else{int e=0;for(int i=0;i<9;i++)if(!ttt.board[i])e++;if(!e)ttt.state=3;else ttt.turn=2;}
        }
    } else {
        if(++ttt.ai_delay<30) return 0;
        ttt.ai_delay=0;
        int mv=ttt_ai_move();
        if(mv>=0) ttt.board[mv]=2;
        if(ttt_check_win(2)){ttt.state=2;ttt.hi_o++;}
        else{int e=0;for(int i=0;i<9;i++)if(!ttt.board[i])e++;if(!e)ttt.state=3;else ttt.turn=1;}
    }
    return 0;
}

static void ttt_draw(void) {
    m4_fill(C_TTTBG);
    m4_rect(0,0,SCREEN_W,13,C_DGRAY); m4_hline(0,13,SCREEN_W,C_GRAY);
    draw_str(2,3,"JOGO DA VELHA",C_CYAN);
    draw_str(120,3,"X:",C_YELLOW); draw_int(132,3,ttt.hi_x,C_YELLOW);
    draw_str(155,3,"O:",C_RED);    draw_int(167,3,ttt.hi_o,C_RED);

    int bx=48,by=20,cw=44,ch=44;
    m4_rect(bx,by,cw*3,ch*3,C_DGRAY);
    m4_vline(bx+cw,by,ch*3,C_WHITE); m4_vline(bx+cw*2,by,ch*3,C_WHITE);
    m4_hline(bx,by+ch,cw*3,C_WHITE); m4_hline(bx,by+ch*2,cw*3,C_WHITE);

    for(int i=0;i<9;i++){
        int cx=bx+(i%3)*cw+cw/2, cy=by+(i/3)*ch+ch/2;
        if(ttt.turn==1&&ttt.state==0&&i==ttt.cur)
            m4_rect(bx+(i%3)*cw+1,by+(i/3)*ch+1,cw-2,ch-2,C_TTTHL);
        if(ttt.board[i]==1){
            // X: two diagonals
            int x0=bx+(i%3)*cw+6, y0=by+(i/3)*ch+6;
            int x1=x0+cw-12-1, y1=y0+ch-12-1;
            for(int d=0;d<=x1-x0;d++){
                m4_pixel(x0+d,y0+d,C_YELLOW);
                m4_pixel(x0+d+1,y0+d,C_YELLOW);
                m4_pixel(x0+d,y1-d,C_YELLOW);
                m4_pixel(x0+d+1,y1-d,C_YELLOW);
            }
        } else if(ttt.board[i]==2){
            // O: filled ring
            int r=16, r2=(r-4)*(r-4), r1=r*r;
            for(int dy=-r;dy<=r;dy++) for(int dx=-r;dx<=r;dx++){
                int d=dx*dx+dy*dy;
                if(d<=r1&&d>=r2) m4_pixel(cx+dx,cy+dy,C_RED);
            }
        }
    }
    // winner line
    if(ttt.state==1||ttt.state==2){
        uint8_t lc=(ttt.state==1)?C_YELLOW:C_RED;
        int a=ttt.winner_line[0], b=ttt.winner_line[2];
        int ax=bx+(a%3)*cw+cw/2, ay=by+(a/3)*ch+ch/2;
        int bbx=bx+(b%3)*cw+cw/2,bby=by+(b/3)*ch+ch/2;
        int ddx=bbx-ax, ddy=bby-ay;
        int steps=iabs(ddx)>iabs(ddy)?iabs(ddx):iabs(ddy);
        for(int s=0;s<=steps;s++){
            int px2=ax+ddx*s/steps, py2=ay+ddy*s/steps;
            for(int d=-2;d<=2;d++){m4_pixel(px2+d,py2,lc);m4_pixel(px2,py2+d,lc);}
        }
    }
    if(ttt.state==0)
        draw_str(4,by+ch*3+6,ttt.turn==1?"SUA VEZ (X)":"CPU PENSANDO....",C_WHITE);
    else if(ttt.state==1) draw_win("VOCE GANHOU!");
    else if(ttt.state==2){draw_gameover(0);draw_str_centered(75,"CPU GANHOU",C_RED);}
    else{draw_panel(60,60,120,40,C_NAVY,C_CYAN);draw_str_centered(68,"EMPATE!",C_YELLOW);draw_str_centered(80,"A-REINICIAR",C_WHITE);}
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

static const uint8_t BO_COLORS[BO_BRICK_ROWS]={C_RED,C_ORANGE,C_YELLOW,C_LGREEN,C_CYAN,C_BLUE};

static struct {
    int bricks[BO_BRICK_ROWS][BO_BRICK_COLS];
    int px, bx,by,bdx,bdy; // fixed *256
    int score,hi,lives,state;
} bo;

static void bo_reset_ball(void) {
    bo.bx=(SCREEN_W/2)<<8; bo.by=(130)<<8;
    bo.bdx=(2<<8)+(1<<7); bo.bdy=-(2<<8);
}

static void bo_init(void) {
    for(int r=0;r<BO_BRICK_ROWS;r++) for(int c=0;c<BO_BRICK_COLS;c++) bo.bricks[r][c]=1;
    bo.px=SCREEN_W/2; bo.lives=3; bo.score=0; bo.state=0;
    bo_reset_ball();
}

static int bo_update(void) {
    uint16_t k=key_down(), p=key_pressed();
    if(p&KEY_B) return -1;
    if(bo.state){if(p&KEY_A)bo_init();return 0;}
    if((k&KEY_LEFT) &&bo.px>BO_PW/2+4) bo.px-=4;
    if((k&KEY_RIGHT)&&bo.px<SCREEN_W-BO_PW/2-4) bo.px+=4;
    bo.bx+=bo.bdx; bo.by+=bo.bdy;
    int bx=bo.bx>>8, by=bo.by>>8;
    if(bx<=4)          bo.bdx= iabs(bo.bdx);
    if(bx>=SCREEN_W-4) bo.bdx=-iabs(bo.bdx);
    if(by<=14)         bo.bdy= iabs(bo.bdy);
    int py=SCREEN_H-16;
    if(by>=py-4&&by<=py+BO_PH+2&&bx>=bo.px-BO_PW/2&&bx<=bo.px+BO_PW/2){
        bo.bdy=-iabs(bo.bdy);
        int rel=bx-bo.px;
        bo.bdx=rel*(1<<5);
        if(iabs(bo.bdx)<(1<<7)) bo.bdx=(bo.bdx>=0)?(1<<7):-(1<<7);
    }
    if(by>SCREEN_H){
        bo.lives--;
        if(bo.lives<=0) bo.state=1;
        else bo_reset_ball();
    }
    int col=(bx-BO_BX)/BO_BW, row=(by-BO_BY)/BO_BH;
    if(row>=0&&row<BO_BRICK_ROWS&&col>=0&&col<BO_BRICK_COLS&&bo.bricks[row][col]){
        bo.bricks[row][col]=0;
        bo.score+=10*(BO_BRICK_ROWS-row);
        if(bo.score>bo.hi) bo.hi=bo.score;
        int left=0; for(int r=0;r<BO_BRICK_ROWS;r++) for(int c=0;c<BO_BRICK_COLS;c++) if(bo.bricks[r][c])left++;
        if(!left) bo.state=2;
        int bkcx=BO_BX+col*BO_BW+BO_BW/2, bkcy=BO_BY+row*BO_BH+BO_BH/2;
        if(iabs(bx-bkcx)>iabs(by-bkcy)) bo.bdx=-bo.bdx; else bo.bdy=-bo.bdy;
    }
    if(iabs(bo.bdx)>(4<<8)) bo.bdx=(bo.bdx>0)?(4<<8):-(4<<8);
    return 0;
}

static void bo_draw(void) {
    m4_fill(C_DARKBG);
    for(int r=0;r<BO_BRICK_ROWS;r++) for(int c=0;c<BO_BRICK_COLS;c++){
        if(!bo.bricks[r][c]) continue;
        int x=BO_BX+c*BO_BW, y=BO_BY+r*BO_BH;
        m4_rect(x+1,y+1,BO_BW-2,BO_BH-2,BO_COLORS[r]);
        m4_hline(x+1,y+1,BO_BW-2,C_WHITE);
        m4_vline(x+1,y+1,BO_BH-2,C_WHITE);
    }
    int py=SCREEN_H-16;
    m4_rect(bo.px-BO_PW/2,py,BO_PW,BO_PH,C_LBLUE);
    m4_hline(bo.px-BO_PW/2,py,BO_PW,C_WHITE);
    int bx=bo.bx>>8, by2=bo.by>>8;
    m4_rect(bx-3,by2-3,6,6,C_WHITE);
    m4_pixel(bx-2,by2-2,C_LBLUE);
    for(int i=0;i<bo.lives;i++) m4_rect(SCREEN_W-8-i*8,py,5,4,C_LBLUE);
    draw_hud("BREAKOUT",bo.score,bo.hi);
    if(bo.state==1) draw_gameover(bo.score);
    if(bo.state==2) draw_win("NIVEL COMPLETO!");
}

// ============================================================
// GAME 6: FLAPPY BIRD
// ============================================================
#define FL_GRAVITY  18
#define FL_JUMP   -520
#define FL_PIPE_W  20
#define FL_GAP     52
#define FL_PIPE_SPD 2

static struct {
    int by, bvy;  // bird y, vy (fixed *256)
    int pipes[4][2]; // x, gap_y
    int score,hi,state,timer,scroll;
} fl;

static void fl_init(void) {
    fl.by=(SCREEN_H/2)<<8; fl.bvy=0;
    for(int i=0;i<4;i++){fl.pipes[i][0]=SCREEN_W+40+i*80;fl.pipes[i][1]=30+(int)(rng()%65);}
    fl.score=0; fl.state=0; fl.timer=0; fl.scroll=0;
}

static int fl_update(void) {
    uint16_t p=key_pressed();
    if(p&KEY_B) return -1;
    if(fl.state==2){if(p&KEY_A)fl_init();return 0;}
    if(fl.state==0){if(p&KEY_A){fl.state=1;fl.bvy=FL_JUMP;}return 0;}
    if(p&KEY_A) fl.bvy=FL_JUMP;
    fl.bvy+=FL_GRAVITY; fl.by+=fl.bvy;
    fl.scroll=(fl.scroll+1)%SCREEN_W;
    int by=fl.by>>8;
    if(by<=14||by>=SCREEN_H-8){fl.state=2;return 0;}
    for(int i=0;i<4;i++){
        fl.pipes[i][0]-=FL_PIPE_SPD;
        if(fl.pipes[i][0]<-FL_PIPE_W){
            fl.pipes[i][0]=SCREEN_W+FL_PIPE_W;
            fl.pipes[i][1]=25+(int)(rng()%70);
            fl.score++; if(fl.score>fl.hi)fl.hi=fl.score;
        }
        int px=fl.pipes[i][0], gap=fl.pipes[i][1]+14;
        int bl=17,br=25, pl=px-FL_PIPE_W/2+2, pr=px+FL_PIPE_W/2-2;
        if(br>=pl&&bl<=pr&&(by-3<gap||by+3>gap+FL_GAP)){fl.state=2;return 0;}
    }
    return 0;
}

static void fl_draw(void) {
    // sky
    for(int y=14;y<SCREEN_H-8;y++) m4_hline(0,y,SCREEN_W,(y<90)?C_SKYHI:C_SKYLO);
    // clouds
    for(int i=0;i<3;i++){
        int cx=(200-fl.scroll/3+i*80)%SCREEN_W, cy=22+i*18;
        m4_rect(cx,cy,24,8,C_CLOUD); m4_rect(cx+4,cy-4,16,6,C_CLOUD);
    }
    // pipes
    for(int i=0;i<4;i++){
        int px=fl.pipes[i][0], gap=fl.pipes[i][1]+14;
        m4_rect(px-FL_PIPE_W/2,14,FL_PIPE_W,gap-14-FL_PIPE_W/2,C_GREEN);
        m4_rect(px-FL_PIPE_W/2-2,gap-FL_PIPE_W/2,FL_PIPE_W+4,FL_PIPE_W/2,C_LGREEN);
        int bot=gap+FL_GAP+FL_PIPE_W/2;
        m4_rect(px-FL_PIPE_W/2-2,gap+FL_GAP,FL_PIPE_W+4,FL_PIPE_W/2,C_LGREEN);
        m4_rect(px-FL_PIPE_W/2,bot,FL_PIPE_W,SCREEN_H-8-bot,C_GREEN);
    }
    // ground
    m4_rect(0,SCREEN_H-8,SCREEN_W,8,C_BROWN); m4_hline(0,SCREEN_H-8,SCREEN_W,C_YELLOW);
    // bird
    int by=fl.by>>8, tilt=iclamp(fl.bvy>>9,-2,3);
    m4_rect(15,by-4,12,8,C_YELLOW);
    m4_pixel(23,by-2,C_ORANGE); m4_rect(25,by-1+tilt,3,2,C_ORANGE);
    m4_pixel(26,by-3+tilt,C_BLACK);
    // hud
    m4_rect(0,0,SCREEN_W,13,C_DGRAY); m4_hline(0,13,SCREEN_W,C_GRAY);
    draw_str(2,3,"FLAPPY",C_CYAN);
    draw_str(60,3,"SC:",C_WHITE); draw_int(78,3,fl.score,C_YELLOW);
    draw_str(110,3,"HI:",C_WHITE); draw_int(128,3,fl.hi,C_YELLOW);
    draw_str(172,3,"B-MENU",C_GRAY);
    if(fl.state==0){draw_panel(60,60,120,40,C_NAVY,C_CYAN);draw_str_centered(68,"FLAPPY BIRD",C_YELLOW);draw_str_centered(80,"A-COMECAR",C_WHITE);}
    if(fl.state==2) draw_gameover(fl.score);
}

// ============================================================
// GAME 7: SPACE INVADERS
// ============================================================
#define SI_COLS 10
#define SI_ROWS  4
#define SI_CW   20
#define SI_CH   14
#define SI_OX   10
#define SI_OY   22
#define SI_MAX_BULLETS 3
#define SI_MAX_EBULLETS 4

static struct {
    int alive[SI_ROWS][SI_COLS];
    int ox,oy,dir,speed_timer,speed;
    int px;
    int bx[SI_MAX_BULLETS],by[SI_MAX_BULLETS];
    int ebx[SI_MAX_EBULLETS],eby[SI_MAX_EBULLETS];
    int score,hi,lives,state,anim,shoot_timer,wave;
} si;

static int si_alive_count(void) {
    int n=0; for(int r=0;r<SI_ROWS;r++) for(int c=0;c<SI_COLS;c++) if(si.alive[r][c])n++;
    return n;
}

static void si_init(void) {
    for(int r=0;r<SI_ROWS;r++) for(int c=0;c<SI_COLS;c++) si.alive[r][c]=1;
    si.ox=0;si.oy=0;si.dir=1;si.speed_timer=0;
    si.speed=imax(5,25-si.wave*3);
    si.px=SCREEN_W/2;
    for(int i=0;i<SI_MAX_BULLETS;i++) si.bx[i]=-1;
    for(int i=0;i<SI_MAX_EBULLETS;i++) si.ebx[i]=-1;
    si.state=0;si.anim=0;si.shoot_timer=0;
}

static int si_update(void) {
    uint16_t k=key_down(), p=key_pressed();
    if(p&KEY_B) return -1;
    if(si.state){
        if(p&KEY_A){
            int was_win=(si.state==2);
            si.score=0;
            int saved_wave=si.wave;
            si_init();
            if(was_win){si.wave=saved_wave+1;}
        }
        return 0;
    }
    if((k&KEY_LEFT) &&si.px>8) si.px-=3;
    if((k&KEY_RIGHT)&&si.px<SCREEN_W-8) si.px+=3;
    if(p&KEY_A){
        for(int i=0;i<SI_MAX_BULLETS;i++){
            if(si.bx[i]<0){si.bx[i]=si.px;si.by[i]=SCREEN_H-24;break;}
        }
    }
    // move player bullets
    for(int i=0;i<SI_MAX_BULLETS;i++){
        if(si.bx[i]<0) continue;
        si.by[i]-=5;
        if(si.by[i]<14){si.bx[i]=-1;continue;}
        for(int r=0;r<SI_ROWS;r++) for(int c=0;c<SI_COLS;c++){
            if(!si.alive[r][c]) continue;
            int ax=SI_OX+si.ox+c*SI_CW, ay=SI_OY+si.oy+r*SI_CH;
            if(si.bx[i]>=ax&&si.bx[i]<=ax+SI_CW-2&&si.by[i]>=ay&&si.by[i]<=ay+SI_CH-2){
                si.alive[r][c]=0; si.bx[i]=-1;
                si.score+=10*(SI_ROWS-r);
                if(si.score>si.hi) si.hi=si.score;
                if(!si_alive_count()) si.state=2;
                goto next_bul;
            }
        }
        next_bul:;
    }
    si.anim++;
    if(++si.speed_timer>=si.speed){
        si.speed_timer=0;
        si.ox+=si.dir*4;
        int lx=SI_OX+si.ox, rx=SI_OX+si.ox+(SI_COLS-1)*SI_CW+SI_CW;
        if(rx>=SCREEN_W-4||lx<=4){si.dir=-si.dir;si.oy+=SI_CH/2;}
        for(int r=0;r<SI_ROWS;r++) for(int c=0;c<SI_COLS;c++)
            if(si.alive[r][c]&&SI_OY+si.oy+r*SI_CH+SI_CH>SCREEN_H-24){si.state=1;return 0;}
    }
    if(++si.shoot_timer>40){
        si.shoot_timer=0;
        for(int i=0;i<SI_MAX_EBULLETS;i++){
            if(si.ebx[i]<0){
                int col2=(int)(rng()%SI_COLS);
                for(int r=SI_ROWS-1;r>=0;r--){
                    if(si.alive[r][col2]){
                        si.ebx[i]=SI_OX+si.ox+col2*SI_CW+SI_CW/2;
                        si.eby[i]=SI_OY+si.oy+r*SI_CH+SI_CH;
                        break;
                    }
                }
                break;
            }
        }
    }
    for(int i=0;i<SI_MAX_EBULLETS;i++){
        if(si.ebx[i]<0) continue;
        si.eby[i]+=3;
        if(si.eby[i]>SCREEN_H){si.ebx[i]=-1;continue;}
        if(iabs(si.ebx[i]-si.px)<8&&iabs(si.eby[i]-(SCREEN_H-18))<6){
            si.ebx[i]=-1; si.lives--;
            if(si.lives<=0) si.state=1;
        }
    }
    return 0;
}

static void si_draw_alien(int x, int y, int row, int frame) {
    static const uint8_t COLS[4]={C_RED,C_ORANGE,C_CYAN,C_LGREEN};
    uint8_t c=COLS[row];
    if(row==0){
        m4_rect(x+4,y,8,3,c); m4_rect(x+2,y+3,12,4,c);
        m4_pixel(x+(frame?1:3),y+7,c); m4_pixel(x+(frame?12:10),y+7,c);
        m4_pixel(x+3,y+1,C_BLACK); m4_pixel(x+9,y+1,C_BLACK);
    } else if(row==1){
        m4_rect(x+2,y+1,12,3,c); m4_rect(x,y+4,16,3,c);
        m4_pixel(x+(frame?0:2),y+7,c); m4_pixel(x+(frame?14:12),y+7,c);
        m4_pixel(x+5,y,c); m4_pixel(x+10,y,c);
    } else {
        m4_rect(x+2,y+2,12,5,c); m4_rect(x,y+4,4,3,c); m4_rect(x+12,y+4,4,3,c);
        m4_pixel(x+(frame?1:3),y+7,c); m4_pixel(x+(frame?13:11),y+7,c);
    }
}

static void si_draw(void) {
    m4_fill(C_STARBG);
    // stars
    for(int i=0;i<30;i++) m4_pixel((i*73+11)%SCREEN_W,14+(i*47)%(SCREEN_H-14),(i%3==0)?C_WHITE:C_GRAY);
    int frame=(si.anim/8)&1;
    for(int r=0;r<SI_ROWS;r++) for(int c=0;c<SI_COLS;c++){
        if(!si.alive[r][c]) continue;
        si_draw_alien(SI_OX+si.ox+c*SI_CW, SI_OY+si.oy+r*SI_CH, r, frame);
    }
    int py=SCREEN_H-20;
    m4_rect(si.px-6,py+4,12,5,C_LBLUE); m4_rect(si.px-2,py,4,5,C_LBLUE); m4_rect(si.px-8,py+7,16,3,C_BLUE);
    for(int i=0;i<SI_MAX_BULLETS;i++)  if(si.bx[i]>=0)  m4_rect(si.bx[i]-1,si.by[i],2,6,C_YELLOW);
    for(int i=0;i<SI_MAX_EBULLETS;i++) if(si.ebx[i]>=0) m4_rect(si.ebx[i]-1,si.eby[i],2,5,C_RED);
    for(int i=0;i<si.lives;i++) m4_rect(4+i*12,SCREEN_H-10,8,6,C_LBLUE);
    m4_hline(0,SCREEN_H-12,SCREEN_W,C_LGREEN);
    draw_hud("INVADERS",si.score,si.hi);
    if(si.state==1) draw_gameover(si.score);
    if(si.state==2) draw_win("ONDA DESTRUIDA!");
}

// ============================================================
// GAME 8: MINESWEEPER
// ============================================================
#define MS_COLS 14
#define MS_ROWS 10
#define MS_CS   15
#define MS_OX   ((SCREEN_W-MS_COLS*MS_CS)/2)
#define MS_OY   18
#define MS_MINES 18

static struct {
    int board[MS_ROWS][MS_COLS];    // -1=mine, 0-8=adj count
    int revealed[MS_ROWS][MS_COLS];
    int flagged[MS_ROWS][MS_COLS];
    int cx,cy,mines_left,score,hi,state,first;
} ms;

static void ms_count(void) {
    for(int r=0;r<MS_ROWS;r++) for(int c=0;c<MS_COLS;c++){
        if(ms.board[r][c]<0) continue;
        int n=0;
        for(int dr=-1;dr<=1;dr++) for(int dc=-1;dc<=1;dc++){
            int nr=r+dr,nc=c+dc;
            if(nr>=0&&nr<MS_ROWS&&nc>=0&&nc<MS_COLS&&ms.board[nr][nc]<0) n++;
        }
        ms.board[r][c]=n;
    }
}

static void ms_place_mines(int ar, int ac) {
    int placed=0;
    while(placed<MS_MINES){
        int r=(int)(rng()%MS_ROWS),c=(int)(rng()%MS_COLS);
        if(ms.board[r][c]<0) continue;
        if(iabs(r-ar)<=1&&iabs(c-ac)<=1) continue;
        ms.board[r][c]=-1; placed++;
    }
    ms_count();
}

// Iterative flood fill (avoids stack overflow on GBA)
static void ms_reveal(int sr, int sc) {
    static int qr[MS_ROWS*MS_COLS], qc[MS_ROWS*MS_COLS];
    int head=0,tail=0;
    if(ms.revealed[sr][sc]||ms.flagged[sr][sc]) return;
    ms.revealed[sr][sc]=1;
    if(ms.board[sr][sc]!=0) return;
    qr[tail]=sr; qc[tail]=sc; tail++;
    while(head<tail){
        int r=qr[head],c=qc[head]; head++;
        for(int dr=-1;dr<=1;dr++) for(int dc=-1;dc<=1;dc++){
            int nr=r+dr,nc=c+dc;
            if(nr<0||nr>=MS_ROWS||nc<0||nc>=MS_COLS) continue;
            if(ms.revealed[nr][nc]||ms.flagged[nr][nc]) continue;
            ms.revealed[nr][nc]=1;
            if(ms.board[nr][nc]==0){qr[tail]=nr;qc[tail]=nc;tail++;}
        }
    }
}

static void ms_init(void) {
    for(int r=0;r<MS_ROWS;r++) for(int c=0;c<MS_COLS;c++){ms.board[r][c]=0;ms.revealed[r][c]=0;ms.flagged[r][c]=0;}
    ms.cx=MS_COLS/2;ms.cy=MS_ROWS/2;ms.mines_left=MS_MINES;ms.score=0;ms.state=0;ms.first=1;
}

static int ms_update(void) {
    uint16_t p=key_pressed();
    if(p&KEY_B){if(ms.state){ms_init();return 0;}return -1;}
    if(p&KEY_A&&ms.state){ms_init();return 0;}
    if(ms.state) return 0;
    if((p&KEY_LEFT) &&ms.cx>0) ms.cx--;
    if((p&KEY_RIGHT)&&ms.cx<MS_COLS-1) ms.cx++;
    if((p&KEY_UP)   &&ms.cy>0) ms.cy--;
    if((p&KEY_DOWN) &&ms.cy<MS_ROWS-1) ms.cy++;
    if(p&KEY_A){
        if(ms.first){ms_place_mines(ms.cy,ms.cx);ms.first=0;}
        if(ms.flagged[ms.cy][ms.cx]) return 0;
        if(ms.board[ms.cy][ms.cx]<0){
            for(int r=0;r<MS_ROWS;r++) for(int c=0;c<MS_COLS;c++) if(ms.board[r][c]<0) ms.revealed[r][c]=1;
            ms.state=1;
        } else {
            ms_reveal(ms.cy,ms.cx);
            int hidden=0;
            for(int r=0;r<MS_ROWS;r++) for(int c=0;c<MS_COLS;c++) if(!ms.revealed[r][c]&&ms.board[r][c]>=0) hidden++;
            if(!hidden){ms.state=2;ms.score=MS_MINES*50;if(ms.score>ms.hi)ms.hi=ms.score;}
        }
    }
    if((p&KEY_L)||(p&KEY_R)){
        if(!ms.revealed[ms.cy][ms.cx]){
            ms.flagged[ms.cy][ms.cx]=!ms.flagged[ms.cy][ms.cx];
            ms.mines_left+=ms.flagged[ms.cy][ms.cx]?-1:1;
        }
    }
    return 0;
}

static const uint8_t MS_NUM_COL[9]={C_BLACK,C_BLUE,C_GREEN,C_RED,C_NAVY,C_DRED,C_TEAL,C_BLACK,C_GRAY};

static void ms_draw(void) {
    m4_fill(C_MNBG);
    m4_rect(0,0,SCREEN_W,13,C_DGRAY); m4_hline(0,13,SCREEN_W,C_GRAY);
    draw_str(2,3,"CAMPO MINADO",C_CYAN);
    draw_str(108,3,"MINAS:",C_WHITE); draw_int(144,3,ms.mines_left,C_RED);
    draw_str(168,3,"HI:",C_WHITE); draw_int(186,3,ms.hi,C_YELLOW);
    draw_str(2,SCREEN_H-10,"A-REVEAL  L/R-FLAG  B-MENU",C_DGRAY);
    for(int r=0;r<MS_ROWS;r++) for(int c=0;c<MS_COLS;c++){
        int x=MS_OX+c*MS_CS, y=MS_OY+r*MS_CS;
        if(ms.revealed[r][c]){
            m4_rect(x,y,MS_CS-1,MS_CS-1,C_MNREV);
            m4_border(x,y,MS_CS-1,MS_CS-1,C_GRAY);
            if(ms.board[r][c]<0){m4_rect(x+3,y+3,MS_CS-7,MS_CS-7,C_RED);}
            else if(ms.board[r][c]>0){char s[2]={'0'+ms.board[r][c],0};draw_str(x+4,y+4,s,MS_NUM_COL[ms.board[r][c]]);}
        } else {
            uint8_t bg=(r==ms.cy&&c==ms.cx)?C_MNSEL:C_MNUNR;
            m4_rect(x,y,MS_CS-1,MS_CS-1,bg);
            m4_hline(x,y,MS_CS-1,C_WHITE); m4_vline(x,y,MS_CS-1,C_WHITE);
            m4_hline(x,y+MS_CS-2,MS_CS-1,C_DGRAY); m4_vline(x+MS_CS-2,y,MS_CS-1,C_DGRAY);
            if(ms.flagged[r][c]){m4_rect(x+5,y+3,2,6,C_RED);m4_rect(x+7,y+3,4,3,C_RED);}
        }
    }
    m4_border(MS_OX-1,MS_OY-1,MS_COLS*MS_CS+1,MS_ROWS*MS_CS+1,C_WHITE);
    if(ms.state==1) draw_gameover(0);
    if(ms.state==2) draw_win("CAMPO LIMPO!");
}

// ============================================================
// GAME 9: MEMORY MATCH
// ============================================================
#define MM_COLS 4
#define MM_ROWS 4
#define MM_CS   36
#define MM_OX   ((SCREEN_W-MM_COLS*MM_CS)/2)
#define MM_OY   18

static const uint8_t MM_COLORS[8]={C_RED,C_ORANGE,C_YELLOW,C_LGREEN,C_CYAN,C_BLUE,C_PURPLE,C_PINK};
static const char    MM_SYMS[8]={'A','B','C','D','E','F','G','H'};

static struct {
    int cards[MM_ROWS*MM_COLS];
    int flipped[MM_ROWS*MM_COLS],matched[MM_ROWS*MM_COLS];
    int cx,cy,sel1,sel2,flip_timer;
    int pairs,moves,score,hi,state;
} mm;

static void mm_init(void) {
    for(int i=0;i<MM_ROWS*MM_COLS;i++) mm.cards[i]=i/2;
    for(int i=MM_ROWS*MM_COLS-1;i>0;i--){int j=(int)(rng()%(i+1));int t=mm.cards[i];mm.cards[i]=mm.cards[j];mm.cards[j]=t;}
    for(int i=0;i<MM_ROWS*MM_COLS;i++){mm.flipped[i]=0;mm.matched[i]=0;}
    mm.cx=0;mm.cy=0;mm.sel1=-1;mm.sel2=-1;mm.flip_timer=0;
    mm.pairs=0;mm.moves=0;mm.score=0;mm.state=0;
}

static int mm_update(void) {
    uint16_t p=key_pressed();
    if(p&KEY_B){if(mm.state){mm_init();return 0;}return -1;}
    if(p&KEY_A&&mm.state){mm_init();return 0;}
    if(mm.state) return 0;
    if(mm.sel2>=0){
        if(++mm.flip_timer>50){
            if(mm.cards[mm.sel1]==mm.cards[mm.sel2]){
                mm.matched[mm.sel1]=mm.matched[mm.sel2]=1;
                mm.pairs++;
                int bonus=imax(0,50-mm.moves);
                mm.score+=100+bonus;
                if(mm.score>mm.hi) mm.hi=mm.score;
                if(mm.pairs==MM_ROWS*MM_COLS/2) mm.state=2;
            } else {
                mm.flipped[mm.sel1]=mm.flipped[mm.sel2]=0;
            }
            mm.sel1=mm.sel2=-1;mm.flip_timer=0;
        }
        return 0;
    }
    if((p&KEY_LEFT) &&mm.cx>0) mm.cx--;
    if((p&KEY_RIGHT)&&mm.cx<MM_COLS-1) mm.cx++;
    if((p&KEY_UP)   &&mm.cy>0) mm.cy--;
    if((p&KEY_DOWN) &&mm.cy<MM_ROWS-1) mm.cy++;
    if(p&KEY_A){
        int idx=mm.cy*MM_COLS+mm.cx;
        if(mm.matched[idx]||mm.flipped[idx]) return 0;
        mm.flipped[idx]=1; mm.moves++;
        if(mm.sel1<0) mm.sel1=idx;
        else if(mm.sel1!=idx){mm.sel2=idx;mm.flip_timer=0;}
    }
    return 0;
}

static void mm_draw(void) {
    m4_fill(C_NAVY);
    m4_rect(0,0,SCREEN_W,13,C_DGRAY); m4_hline(0,13,SCREEN_W,C_GRAY);
    draw_str(2,3,"MEMORY MATCH",C_CYAN);
    draw_str(100,3,"MOV:",C_WHITE); draw_int(124,3,mm.moves,C_YELLOW);
    draw_str(158,3,"HI:",C_WHITE); draw_int(176,3,mm.hi,C_YELLOW);
    for(int r=0;r<MM_ROWS;r++) for(int c=0;c<MM_COLS;c++){
        int idx=r*MM_COLS+c;
        int x=MM_OX+c*MM_CS, y=MM_OY+r*MM_CS;
        int sel=(mm.cx==c&&mm.cy==r);
        if(mm.matched[idx]){
            m4_rect(x+1,y+1,MM_CS-3,MM_CS-3,MM_COLORS[mm.cards[idx]]);
            m4_border(x+1,y+1,MM_CS-3,MM_CS-3,C_WHITE);
            draw_char(x+MM_CS/2-2,y+MM_CS/2-3,MM_SYMS[mm.cards[idx]],C_WHITE);
        } else if(mm.flipped[idx]||mm.sel1==idx||mm.sel2==idx){
            m4_rect(x+1,y+1,MM_CS-3,MM_CS-3,MM_COLORS[mm.cards[idx]]);
            m4_border(x+1,y+1,MM_CS-3,MM_CS-3,C_WHITE);
            draw_char(x+MM_CS/2-2,y+MM_CS/2-3,MM_SYMS[mm.cards[idx]],C_WHITE);
        } else {
            m4_rect(x+1,y+1,MM_CS-3,MM_CS-3,sel?C_MMSEL:C_MMUNSEL);
            m4_border(x+1,y+1,MM_CS-3,MM_CS-3,sel?C_YELLOW:C_GRAY);
            m4_pixel(x+MM_CS/2,y+MM_CS/2,C_GRAY);
        }
    }
    draw_str(4,SCREEN_H-10,"A-VIRAR  SETAS-MOVER  B-MENU",C_DGRAY);
    if(mm.state==2) draw_win("TODAS PARES!");
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
    int board[PM_ROWS][PM_COLS];
    int px,py,pdx,pdy,pnx,pny,panim,scared,timer;
    int gx[4],gy[4],gdx[4],gdy[4],gtimer[4];
    int score,hi,lives,state,dots;
} pm;

static int pm_walk(int tx, int ty) {
    if(tx<0||tx>=PM_COLS||ty<0||ty>=PM_ROWS) return 0;
    return pm.board[ty][tx]!=1;
}

static void pm_ghost_move(int g) {
    if(--pm.gtimer[g]>0) return;
    pm.gtimer[g]=4;
    int tx=pm.gx[g]/PM_TILE, ty=pm.gy[g]/PM_TILE;
    static const int DX[4]={1,-1,0,0}, DY[4]={0,0,1,-1};
    int best=-1, best_d=9999;
    for(int d=0;d<4;d++){
        int nx=tx+DX[d], ny=ty+DY[d];
        if(!pm_walk(nx,ny)) continue;
        if(DX[d]==-pm.gdx[g]&&DY[d]==-pm.gdy[g]) continue;
        int dist;
        if(pm.scared) dist=(int)(rng()%200);
        else{ int ddx=pm.px/PM_TILE-nx, ddy=pm.py/PM_TILE-ny; dist=iabs(ddx)+iabs(ddy); }
        if(dist<best_d){best_d=dist;best=d;}
    }
    if(best>=0){pm.gdx[g]=DX[best];pm.gdy[g]=DY[best];}
    pm.gx[g]+=pm.gdx[g]*PM_TILE; pm.gy[g]+=pm.gdy[g]*PM_TILE;
    // wrap tunnel
    if(pm.gx[g]<0) pm.gx[g]=(PM_COLS-1)*PM_TILE;
    if(pm.gx[g]>=PM_COLS*PM_TILE) pm.gx[g]=0;
}

static void pm_init(void) {
    for(int r=0;r<PM_ROWS;r++) for(int c=0;c<PM_COLS;c++) pm.board[r][c]=PM_MAP[r][c];
    pm.px=4*PM_TILE; pm.py=12*PM_TILE;
    pm.pdx=0;pm.pdy=0;pm.pnx=0;pm.pny=0;pm.panim=0;pm.scared=0;pm.timer=0;
    static const int GX[4]={9,10,9,10}, GY[4]={8,8,9,9};
    for(int g=0;g<4;g++){
        pm.gx[g]=GX[g]*PM_TILE; pm.gy[g]=GY[g]*PM_TILE;
        pm.gdx[g]=(g<2)?1:-1; pm.gdy[g]=0;
        pm.gtimer[g]=10+g*5;
    }
    pm.score=0; pm.lives=3; pm.state=0;
    pm.dots=0;
    for(int r=0;r<PM_ROWS;r++) for(int c=0;c<PM_COLS;c++) if(pm.board[r][c]==2||pm.board[r][c]==3) pm.dots++;
}

static int pm_update(void) {
    uint16_t k=key_down(), p=key_pressed();
    if(p&KEY_B){if(pm.state){pm_init();return 0;}return -1;}
    if(p&KEY_A&&pm.state){pm_init();return 0;}
    if(pm.state) return 0;
    if(k&KEY_LEFT) {pm.pnx=-1;pm.pny=0;}
    if(k&KEY_RIGHT){pm.pnx= 1;pm.pny=0;}
    if(k&KEY_UP)   {pm.pnx=0;pm.pny=-1;}
    if(k&KEY_DOWN) {pm.pnx=0;pm.pny= 1;}
    if(++pm.timer<3) return 0;
    pm.timer=0; pm.panim++;
    // try buffered dir
    if(pm_walk(pm.px/PM_TILE+pm.pnx, pm.py/PM_TILE+pm.pny)){pm.pdx=pm.pnx;pm.pdy=pm.pny;}
    // move
    int nx=pm.px/PM_TILE+pm.pdx, ny=pm.py/PM_TILE+pm.pdy;
    if(pm_walk(nx,ny)){
        pm.px+=pm.pdx*PM_TILE; pm.py+=pm.pdy*PM_TILE;
        if(pm.px<0) pm.px=(PM_COLS-1)*PM_TILE;
        if(pm.px>=PM_COLS*PM_TILE) pm.px=0;
    }
    int ptx=pm.px/PM_TILE, pty=pm.py/PM_TILE;
    if(pm.board[pty][ptx]==2){pm.board[pty][ptx]=0;pm.score+=10;pm.dots--;if(pm.score>pm.hi)pm.hi=pm.score;}
    else if(pm.board[pty][ptx]==3){pm.board[pty][ptx]=0;pm.score+=50;pm.dots--;pm.scared=180;if(pm.score>pm.hi)pm.hi=pm.score;}
    if(!pm.dots){pm.state=2;return 0;}
    if(pm.scared>0) pm.scared--;
    for(int g=0;g<4;g++) pm_ghost_move(g);
    for(int g=0;g<4;g++){
        if(iabs(pm.gx[g]-pm.px)<PM_TILE&&iabs(pm.gy[g]-pm.py)<PM_TILE){
            if(pm.scared){pm.score+=200;pm.gx[g]=9*PM_TILE;pm.gy[g]=8*PM_TILE;}
            else{pm.lives--;if(pm.lives<=0)pm.state=1;else{pm.px=4*PM_TILE;pm.py=12*PM_TILE;pm.pdx=pm.pdy=0;}}
        }
    }
    return 0;
}

static const uint8_t GCOL[4]={C_RED,C_PINK,C_CYAN,C_ORANGE};

static void pm_draw(void) {
    m4_fill(C_BLACK);
    for(int r=0;r<PM_ROWS;r++) for(int c=0;c<PM_COLS;c++){
        int x=PM_OX+c*PM_TILE, y=PM_OY+r*PM_TILE;
        int t=pm.board[r][c];
        if(t==1){m4_rect(x,y,PM_TILE,PM_TILE,C_BLUE);m4_border(x,y,PM_TILE,PM_TILE,C_MIDBLUE);}
        else if(t==2){m4_pixel(x+PM_TILE/2,y+PM_TILE/2,C_YELLOW);}
        else if(t==3){
            if((pm.timer/4)&1) m4_rect(x+PM_TILE/2-2,y+PM_TILE/2-2,4,4,C_YELLOW);
        }
    }
    // ghosts
    for(int g=0;g<4;g++){
        int gx=PM_OX+pm.gx[g], gy=PM_OY+pm.gy[g];
        uint8_t gc=pm.scared?((pm.scared<40&&(pm.scared/4)&1)?C_WHITE:C_BLUE):GCOL[g];
        m4_rect(gx,gy,PM_TILE,PM_TILE-2,gc);
        // wavy feet
        int fr=(pm.panim/4)&1;
        m4_pixel(gx+fr,gy+PM_TILE-2,gc); m4_pixel(gx+2+fr,gy+PM_TILE-2,gc);
        m4_pixel(gx+4+(fr^1),gy+PM_TILE-2,gc); m4_pixel(gx+6+(fr^1),gy+PM_TILE-2,gc);
        if(!pm.scared){
            m4_pixel(gx+1,gy+2,C_WHITE); m4_pixel(gx+5,gy+2,C_WHITE);
            m4_pixel(gx+1+pm.gdx[g],gy+2+pm.gdy[g],C_NAVY);
            m4_pixel(gx+5+pm.gdx[g],gy+2+pm.gdy[g],C_NAVY);
        }
    }
    // pac-man
    int px2=PM_OX+pm.px, py2=PM_OY+pm.py;
    m4_rect(px2,py2,PM_TILE,PM_TILE,C_YELLOW);
    int mouth=(pm.panim/2)&3;
    if(mouth<3){
        if(pm.pdx>0)       m4_rect(px2+PM_TILE/2,py2+mouth,PM_TILE/2,PM_TILE-mouth*2,C_BLACK);
        else if(pm.pdx<0)  m4_rect(px2,           py2+mouth,PM_TILE/2,PM_TILE-mouth*2,C_BLACK);
        else if(pm.pdy<0)  m4_rect(px2+mouth,     py2,      PM_TILE-mouth*2,PM_TILE/2,C_BLACK);
        else               m4_rect(px2+mouth,     py2+PM_TILE/2,PM_TILE-mouth*2,PM_TILE/2,C_BLACK);
    }
    for(int i=0;i<pm.lives;i++) m4_rect(PM_OX+i*10,SCREEN_H-8,7,7,C_YELLOW);
    draw_hud("PAC-MAN",pm.score,pm.hi);
    if(pm.state==1) draw_gameover(pm.score);
    if(pm.state==2) draw_win("FASE COMPLETA!");
}

// ============================================================
// MENU
// ============================================================
static int menu_sel=0, menu_scroll=0, menu_anim=0;

static void menu_draw(void) {
    menu_anim++;
    m4_fill(C_STARBG);
    for(int i=0;i<48;i++){
        int sx=(i*53+7)%SCREEN_W, sy=(i*37+11)%SCREEN_H;
        uint8_t sc=(i%3==0)?C_WHITE:(i%3==1)?C_GRAY:C_DGRAY;
        m4_pixel(sx,sy,sc);
    }
    int tw=(menu_anim/30)%16;
    m4_pixel((tw*67+3)%SCREEN_W,(tw*43+5)%SCREEN_H,C_YELLOW);
    draw_panel(10,8,220,22,C_NAVY,C_YELLOW);
    draw_str_centered(12,"* GBA MINI GAMES *",C_YELLOW);
    draw_str_centered(20,"10 JOGOS CLASSICOS",C_CYAN);
    int vis=7;
    for(int i=0;i<NUM_GAMES;i++){
        if(i<menu_scroll||i>=menu_scroll+vis) continue;
        int slot=i-menu_scroll, y=35+slot*17;
        if(i==menu_sel){
            m4_rect(10,y-1,220,15,C_MENUSEL);
            m4_border(10,y-1,220,15,C_YELLOW);
            draw_str(18,y+2,GAME_NAMES[i],C_YELLOW);
            draw_str(188,y+2,"HI:",C_WHITE); draw_int(206,y+2,hi_scores[i],C_CYAN);
        } else {
            draw_str(18,y+2,GAME_NAMES[i],C_GRAY);
        }
    }
    m4_vline(234,35,vis*17,C_DGRAY);
    int sb_h=vis*17/NUM_GAMES+1;
    m4_rect(232,35+menu_scroll*17/NUM_GAMES,4,sb_h,C_GRAY);
    draw_str_centered(148,"SETAS-NAVEGAR  A-JOGAR",C_GRAY);
}

static void menu_update(void) {
    uint16_t p=key_pressed();
    if((p&KEY_UP)&&menu_sel>0){menu_sel--;if(menu_sel<menu_scroll)menu_scroll=menu_sel;}
    if((p&KEY_DOWN)&&menu_sel<NUM_GAMES-1){menu_sel++;if(menu_sel>=menu_scroll+7)menu_scroll=menu_sel-6;}
    if((p&KEY_A)||(p&KEY_START)){
        current_game=menu_sel;
        switch(current_game){
            case 0:sn_init();break; case 1:tt_init();break; case 2:pong_init();break;
            case 3:ttt_init();break; case 4:bo_init();break; case 5:fl_init();break;
            case 6:si_init();break; case 7:ms_init();break; case 8:mm_init();break;
            case 9:pm_init();break;
        }
    }
}

// ============================================================
// MAIN
// ============================================================
int main(void) {
    // Start displaying page 0, draw to page 1
    REG_DISPCNT = DCNT_MODE4 | DCNT_BG2;
    draw_buf = PAGE1;
    setup_palette();
    current_game=-1;
    // zero scores
    for(int i=0;i<NUM_GAMES;i++) hi_scores[i]=0;

    while(1){
        // Draw to back buffer, then flip at vsync
        if(current_game<0){
            menu_update();
            menu_draw();
        } else {
            int ret=0;
            switch(current_game){
                case 0:ret=sn_update();sn_draw();break;
                case 1:ret=tt_update();tt_draw();break;
                case 2:ret=pong_update();pong_draw();break;
                case 3:ret=ttt_update();ttt_draw();break;
                case 4:ret=bo_update();bo_draw();break;
                case 5:ret=fl_update();fl_draw();break;
                case 6:ret=si_update();si_draw();break;
                case 7:ret=ms_update();ms_draw();break;
                case 8:ret=mm_update();mm_draw();break;
                case 9:ret=pm_update();pm_draw();break;
            }
            if(ret<0){ current_game=-1; }
        }
        flip(); // wait vblank then swap pages — zero tearing
    }
    return 0;
}
