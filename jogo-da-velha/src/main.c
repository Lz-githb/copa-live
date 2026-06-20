// Multi-game GBA ROM: Menu + Jogo da Velha (c/ IA) + Snake + Pong

// ===== Registradores GBA =====
#define REG_DISPCNT  (*(volatile unsigned short*)0x04000000)
#define REG_VCOUNT   (*(volatile unsigned short*)0x04000006)
#define REG_KEYINPUT (*(volatile unsigned short*)0x04000130)
#define REG_TM0CNT_L (*(volatile unsigned short*)0x04000100)
#define REG_TM0CNT_H (*(volatile unsigned short*)0x04000102)
#define MEM_VRAM     ((volatile unsigned short*)0x06000000)

#define MODE3      0x0003
#define BG2_ENABLE 0x0400
#define SCREEN_W   240
#define SCREEN_H   160

typedef unsigned short u16;
typedef unsigned int   u32;
typedef unsigned char  u8;
typedef signed char    s8;
typedef signed short   s16;
typedef signed int     s32;

// ===== Cores =====
#define RGB15(r,g,b) ((r)|((g)<<5)|((b)<<10))
#define COL_BLACK   RGB15( 0, 0, 0)
#define COL_WHITE   RGB15(31,31,31)
#define COL_BG      RGB15( 1, 1, 6)
#define COL_PANEL   RGB15( 3, 3,10)
#define COL_GRID    RGB15(10,10,14)
#define COL_RED     RGB15(31, 5, 5)
#define COL_DRED    RGB15(18, 2, 2)
#define COL_BLUE    RGB15( 4, 8,30)
#define COL_DBLUE   RGB15( 2, 4,18)
#define COL_GREEN   RGB15( 3,28, 3)
#define COL_DGREEN  RGB15( 1,14, 1)
#define COL_LIME    RGB15(12,31, 4)
#define COL_YELLOW  RGB15(31,28, 0)
#define COL_CYAN    RGB15( 0,22,22)
#define COL_ORANGE  RGB15(31,17, 0)
#define COL_PURPLE  RGB15(20, 4,28)
#define COL_GRAY    RGB15(18,18,18)
#define COL_DGRAY   RGB15( 6, 6, 8)
#define COL_MGRAY   RGB15( 9, 9,12)
#define COL_GOLD    RGB15(31,26, 4)
#define COL_SILVER  RGB15(22,22,25)
#define COL_BORDER  RGB15(12,12,20)

// ===== Botões =====
#define KEY_A      0x0001
#define KEY_B      0x0002
#define KEY_SELECT 0x0004
#define KEY_START  0x0008
#define KEY_RIGHT  0x0010
#define KEY_LEFT   0x0020
#define KEY_UP     0x0040
#define KEY_DOWN   0x0080
#define KEY_R      0x0100
#define KEY_L      0x0200

static u16 keys_held, keys_down, prev_keys;

static void update_keys(void) {
    u16 cur = ~REG_KEYINPUT & 0x03FF;
    keys_down = cur & ~prev_keys;
    keys_held = cur;
    prev_keys = cur;
}

static void vsync(void) {
    while (REG_VCOUNT >= 160);
    while (REG_VCOUNT < 160);
}

// ===== Suporte mínimo sem stdlib =====
void* memset(void* s, int c, unsigned n) {
    unsigned char* p = s;
    while(n--) *p++ = (unsigned char)c;
    return s;
}

// ===== RNG (LCG com entropia do timer hardware) =====
static u32 rng_state = 0xDEADBEEF;
static u32 frame_count = 0;

static u32 rng(void) {
    rng_state = rng_state * 1664525u + 1013904223u;
    return rng_state;
}

// Timer de hardware rodando livre — fornece entropia real por timing de botões/frames
static void rng_stir(void) {
    rng_state ^= (u32)REG_TM0CNT_L * 2654435761u;
    rng_state ^= frame_count * 0x9E3779B9u;
    rng_state  = rng_state * 1664525u + 1013904223u;
    frame_count++;
}

// ===== Desenho =====
static void put_pixel(s32 x, s32 y, u16 c) {
    if ((u32)x < SCREEN_W && (u32)y < SCREEN_H)
        MEM_VRAM[y * SCREEN_W + x] = c;
}

static void fill_rect(s32 x, s32 y, s32 w, s32 h, u16 c) {
    for (s32 j = y; j < y+h; j++)
        for (s32 i = x; i < x+w; i++)
            put_pixel(i, j, c);
}

static void draw_line(s32 x0, s32 y0, s32 x1, s32 y1, s32 t, u16 c) {
    s32 dx = x1-x0, dy = y1-y0;
    s32 sx = dx>0?1:-1, sy = dy>0?1:-1;
    dx = dx<0?-dx:dx; dy = dy<0?-dy:dy;
    s32 err = dx-dy;
    for(;;) {
        for(s32 i=-t/2;i<=t/2;i++)
            dx>=dy ? put_pixel(x0,y0+i,c) : put_pixel(x0+i,y0,c);
        if (x0==x1 && y0==y1) break;
        s32 e2 = 2*err;
        if (e2>-dy){err-=dy;x0+=sx;}
        if (e2< dx){err+=dx;y0+=sy;}
    }
}

static void draw_circle(s32 cx, s32 cy, s32 r, s32 t, u16 c) {
    for (s32 k=0;k<t;k++) {
        s32 rad=r-k, x=0, y=rad, d=3-2*rad;
        while(x<=y){
            put_pixel(cx+x,cy+y,c);put_pixel(cx-x,cy+y,c);
            put_pixel(cx+x,cy-y,c);put_pixel(cx-x,cy-y,c);
            put_pixel(cx+y,cy+x,c);put_pixel(cx-y,cy+x,c);
            put_pixel(cx+y,cy-x,c);put_pixel(cx-y,cy-x,c);
            if(d<0)d+=4*x+6; else{d+=4*(x-y)+10;y--;}
            x++;
        }
    }
}

static void draw_x_at(s32 cx, s32 cy, s32 half, s32 t, u16 c) {
    draw_line(cx-half,cy-half,cx+half,cy+half,t,c);
    draw_line(cx+half,cy-half,cx-half,cy+half,t,c);
}

// Caixa com borda de 1px e preenchimento interno
static void draw_box(s32 x, s32 y, s32 w, s32 h, u16 border, u16 fill) {
    fill_rect(x,y,w,h,border);
    fill_rect(x+1,y+1,w-2,h-2,fill);
}

static void clear_screen(void) { fill_rect(0,0,SCREEN_W,SCREEN_H,COL_BG); }

// ===== Fonte bitmap 5x7 =====
typedef enum {
    CH_A,CH_B,CH_C,CH_D,CH_E,CH_G,CH_I,CH_J,CH_K,CH_L,
    CH_M,CH_N,CH_O,CH_P,CH_R,CH_S,CH_T,CH_U,CH_V,CH_W,
    CH_X,CH_Z,
    CH_0,CH_1,CH_2,CH_3,CH_4,CH_5,CH_6,CH_7,CH_8,CH_9,
    CH_SP,CH_DASH,CH_EXCL,CH_COLON
} Char;

static const u8 FONT[][5] = {
/*A*/ {0x7E,0x11,0x11,0x11,0x7E},
/*B*/ {0x7F,0x49,0x49,0x49,0x36},
/*C*/ {0x3E,0x41,0x41,0x41,0x22},
/*D*/ {0x7F,0x41,0x41,0x22,0x1C},
/*E*/ {0x7F,0x49,0x49,0x49,0x41},
/*G*/ {0x3E,0x41,0x49,0x49,0x3A},
/*I*/ {0x00,0x41,0x7F,0x41,0x00},
/*J*/ {0x20,0x40,0x41,0x3F,0x01},
/*K*/ {0x7F,0x08,0x14,0x22,0x41},
/*L*/ {0x7F,0x40,0x40,0x40,0x40},
/*M*/ {0x7F,0x02,0x04,0x02,0x7F},
/*N*/ {0x7F,0x04,0x08,0x10,0x7F},
/*O*/ {0x3E,0x41,0x41,0x41,0x3E},
/*P*/ {0x7F,0x09,0x09,0x09,0x06},
/*R*/ {0x7F,0x09,0x19,0x29,0x46},
/*S*/ {0x46,0x49,0x49,0x49,0x31},
/*T*/ {0x01,0x01,0x7F,0x01,0x01},
/*U*/ {0x3F,0x40,0x40,0x40,0x3F},
/*V*/ {0x1F,0x20,0x40,0x20,0x1F},
/*W*/ {0x3F,0x40,0x30,0x40,0x3F},
/*X*/ {0x63,0x14,0x08,0x14,0x63},
/*Z*/ {0x61,0x51,0x49,0x45,0x43},
/*0*/ {0x3E,0x51,0x49,0x45,0x3E},
/*1*/ {0x00,0x42,0x7F,0x40,0x00},
/*2*/ {0x42,0x61,0x51,0x49,0x46},
/*3*/ {0x21,0x41,0x45,0x4B,0x31},
/*4*/ {0x18,0x14,0x12,0x7F,0x10},
/*5*/ {0x27,0x45,0x45,0x45,0x39},
/*6*/ {0x3C,0x4A,0x49,0x49,0x30},
/*7*/ {0x01,0x71,0x09,0x05,0x03},
/*8*/ {0x36,0x49,0x49,0x49,0x36},
/*9*/ {0x06,0x49,0x49,0x29,0x1E},
/*SP*/{0x00,0x00,0x00,0x00,0x00},
/*-*/ {0x08,0x08,0x08,0x08,0x08},
/*!*/ {0x00,0x00,0x5F,0x00,0x00},
/*:*/ {0x00,0x36,0x36,0x00,0x00},
};

static void draw_char(s32 x, s32 y, Char c, u16 col) {
    for (s32 col2=0;col2<5;col2++) {
        u8 bits = FONT[c][col2];
        for (s32 row=0;row<7;row++)
            if (bits&(1<<row)) put_pixel(x+col2,y+row,col);
    }
}

static void draw_str(s32 x, s32 y, const Char* s, s32 len, u16 col) {
    for (s32 i=0;i<len;i++) draw_char(x+i*7,y,s[i],col);
}

static void draw_num(s32 x, s32 y, s32 n, u16 col) {
    char buf[6]; s32 i=5; buf[5]=0;
    if(n==0){draw_char(x,y,CH_0,col);return;}
    while(n>0&&i>0){i--;buf[i]='0'+n%10;n/=10;}
    for(s32 j=i;j<5;j++) draw_char(x+(j-i)*7,y,(Char)(CH_0+(buf[j]-'0')),col);
}

// Barra de título com gradiente simulado (borda clara + fundo escuro)
static void draw_titlebar(const Char* txt, s32 len, u16 fg, u16 bg) {
    fill_rect(0,0,SCREEN_W,13,bg);
    fill_rect(0,12,SCREEN_W,1,COL_BORDER);
    s32 tx = (SCREEN_W - len*7)/2;
    draw_str(tx,3,txt,len,fg);
}

// Campo de estrelas decorativo para menus
static void draw_stars(void) {
    static const s16 sx[]={12,35,58,82,105,128,152,175,198,222,
                            24,47,70,95,118,141,165,188,215,235,
                            8,31,55,78,102,125,148,172,195,218,
                            19,43,66,90,113,136,160,183,207,230};
    static const s16 sy[]={20,33,15,42,25,18,38,28,22,35,
                            50,65,55,72,60,48,68,45,58,62,
                            90,105,80,115,95,88,110,75,100,85,
                            130,145,125,138,150,122,142,135,128,148};
    s32 n=40;
    for(s32 i=0;i<n;i++){
        u16 bright=(i%3==0)?COL_WHITE:(i%3==1)?COL_SILVER:COL_MGRAY;
        put_pixel(sx[i],sy[i],bright);
        if(i%5==0){
            put_pixel(sx[i]-1,sy[i],COL_MGRAY);
            put_pixel(sx[i]+1,sy[i],COL_MGRAY);
            put_pixel(sx[i],sy[i]-1,COL_MGRAY);
            put_pixel(sx[i],sy[i]+1,COL_MGRAY);
        }
    }
}

// =================================================================
// JOGO DA VELHA
// =================================================================
#define TT_GX  22
#define TT_GY  16
#define TT_CW  52
#define TT_CH  44

static int tt_board[3][3];
static int tt_cur_r, tt_cur_c;
static int tt_player;
static int tt_winner;
static int tt_win_r0,tt_win_c0,tt_win_r1,tt_win_c1;
static int tt_score[3];
static int tt_vs_ai;
static int tt_ai_turn;

static s32 tt_cx(s32 c){return TT_GX+c*TT_CW+TT_CW/2;}
static s32 tt_cy(s32 r){return TT_GY+r*TT_CH+TT_CH/2;}

// X com sombra
static void tt_draw_x(s32 r, s32 c, u16 col) {
    s32 cx=tt_cx(c), cy=tt_cy(r), h=18;
    draw_x_at(cx+2,cy+2,h,3,COL_DRED);  // sombra
    draw_x_at(cx,cy,h,4,col);
    // brilho interno
    draw_x_at(cx,cy,h-5,2,RGB15(31,18,18));
}

// O com espessura e brilho
static void tt_draw_o(s32 r, s32 c, u16 col) {
    s32 cx=tt_cx(c), cy=tt_cy(r);
    draw_circle(cx+2,cy+2,17,4,COL_DBLUE);  // sombra
    draw_circle(cx,cy,17,5,col);
    // inner highlight
    draw_circle(cx-2,cy-2,12,2,RGB15(10,18,31));
}

static int tt_check(void) {
    for(int r=0;r<3;r++) if(tt_board[r][0]&&tt_board[r][0]==tt_board[r][1]&&tt_board[r][1]==tt_board[r][2])
        {tt_win_r0=r;tt_win_c0=0;tt_win_r1=r;tt_win_c1=2;return tt_board[r][0];}
    for(int c=0;c<3;c++) if(tt_board[0][c]&&tt_board[0][c]==tt_board[1][c]&&tt_board[1][c]==tt_board[2][c])
        {tt_win_r0=0;tt_win_c0=c;tt_win_r1=2;tt_win_c1=c;return tt_board[0][c];}
    if(tt_board[0][0]&&tt_board[0][0]==tt_board[1][1]&&tt_board[1][1]==tt_board[2][2])
        {tt_win_r0=0;tt_win_c0=0;tt_win_r1=2;tt_win_c1=2;return tt_board[0][0];}
    if(tt_board[0][2]&&tt_board[0][2]==tt_board[1][1]&&tt_board[1][1]==tt_board[2][0])
        {tt_win_r0=0;tt_win_c0=2;tt_win_r1=2;tt_win_c1=0;return tt_board[0][2];}
    int full=1;
    for(int r=0;r<3;r++) for(int c=0;c<3;c++) if(!tt_board[r][c])full=0;
    return full?3:0;
}

static int tt_minimax(int b[3][3], int depth, int is_max) {
    for(int r=0;r<3;r++){
        if(b[r][0]&&b[r][0]==b[r][1]&&b[r][1]==b[r][2]) return b[r][0]==2?10-depth:depth-10;
    }
    for(int c=0;c<3;c++){
        if(b[0][c]&&b[0][c]==b[1][c]&&b[1][c]==b[2][c]) return b[0][c]==2?10-depth:depth-10;
    }
    if(b[0][0]&&b[0][0]==b[1][1]&&b[1][1]==b[2][2]) return b[0][0]==2?10-depth:depth-10;
    if(b[0][2]&&b[0][2]==b[1][1]&&b[1][1]==b[2][0]) return b[0][2]==2?10-depth:depth-10;
    int full=1;
    for(int r=0;r<3;r++) for(int c=0;c<3;c++) if(!b[r][c]){full=0;break;}
    if(full) return 0;

    int best = is_max ? -100 : 100;
    for(int r=0;r<3;r++) for(int c=0;c<3;c++) {
        if(!b[r][c]){
            b[r][c] = is_max?2:1;
            int val = tt_minimax(b,depth+1,!is_max);
            b[r][c]=0;
            if(is_max && val>best) best=val;
            if(!is_max && val<best) best=val;
        }
    }
    return best;
}

static void tt_ai_move(void) {
    int best=-100, br=0, bc=0;
    int b[3][3];
    for(int r=0;r<3;r++) for(int c=0;c<3;c++) b[r][c]=tt_board[r][c];
    for(int r=0;r<3;r++) for(int c=0;c<3;c++) {
        if(!b[r][c]){
            b[r][c]=2;
            int val=tt_minimax(b,0,0);
            b[r][c]=0;
            if(val>best){best=val;br=r;bc=c;}
        }
    }
    tt_board[br][bc]=2;
}

static void tt_draw_board(void) {
    static const Char title[]={CH_J,CH_O,CH_G,CH_O,CH_SP,CH_D,CH_A,CH_SP,CH_V,CH_E,CH_L};
    draw_titlebar(title,11,COL_GOLD,COL_DGRAY);

    s32 gw=TT_CW*3, gh=TT_CH*3;

    // Fundo da grade com borda
    draw_box(TT_GX-2, TT_GY-2, gw+4, gh+4, COL_BORDER, COL_PANEL);

    // Linhas da grade — mais espessas e com cor mais viva
    for(int i=1;i<=2;i++) {
        fill_rect(TT_GX+i*TT_CW-2,TT_GY,3,gh,COL_GRID);
        fill_rect(TT_GX+i*TT_CW-2,TT_GY,3,gh,COL_BORDER);
    }
    for(int i=1;i<=2;i++) {
        fill_rect(TT_GX,TT_GY+i*TT_CH-2,gw,3,COL_GRID);
        fill_rect(TT_GX,TT_GY+i*TT_CH-2,gw,3,COL_BORDER);
    }

    // Decorações nas junções da grade
    for(int ri=1;ri<=2;ri++) for(int ci=1;ci<=2;ci++) {
        s32 jx=TT_GX+ci*TT_CW-2, jy=TT_GY+ri*TT_CH-2;
        fill_rect(jx-1,jy-1,5,5,COL_SILVER);
        fill_rect(jx,jy,3,3,COL_WHITE);
    }

    // Peças
    for(int r=0;r<3;r++) for(int c=0;c<3;c++){
        if(tt_board[r][c]==1) tt_draw_x(r,c,COL_RED);
        else if(tt_board[r][c]==2) tt_draw_o(r,c,COL_BLUE);
    }

    // Linha vencedora
    if(tt_winner && tt_winner!=3)
        draw_line(tt_cx(tt_win_c0),tt_cy(tt_win_r0),tt_cx(tt_win_c1),tt_cy(tt_win_r1),4,COL_GOLD);

    // Cursor animado (cantos da célula)
    if(!tt_winner && !(tt_vs_ai && tt_player==2)) {
        s32 cx=tt_cx(tt_cur_c), cy=tt_cy(tt_cur_r), h=20;
        u16 cc=COL_YELLOW;
        s32 arm=6;
        // cantos: topo-esq
        fill_rect(cx-h,cy-h,arm,2,cc); fill_rect(cx-h,cy-h,2,arm,cc);
        // topo-dir
        fill_rect(cx+h-arm+1,cy-h,arm,2,cc); fill_rect(cx+h-1,cy-h,2,arm,cc);
        // baixo-esq
        fill_rect(cx-h,cy+h-1,arm,2,cc); fill_rect(cx-h,cy+h-arm+1,2,arm,cc);
        // baixo-dir
        fill_rect(cx+h-arm+1,cy+h-1,arm,2,cc); fill_rect(cx+h-1,cy+h-arm+1,2,arm,cc);
    }

    // Painel direito
    s32 px = TT_GX + TT_CW*3 + 6;
    s32 pw = SCREEN_W - px - 2;
    draw_box(px,14,pw,SCREEN_H-16,COL_BORDER,COL_PANEL);

    // Placar
    static const Char sx[]={CH_X,CH_COLON};
    static const Char so[]={CH_O,CH_COLON};
    draw_str(px+4,20,sx,2,COL_RED);
    draw_num(px+18,20,tt_score[1],COL_RED);
    draw_str(px+4,32,so,2,COL_BLUE);
    draw_num(px+18,32,tt_score[2],COL_BLUE);

    // Separador
    fill_rect(px+2,43,pw-4,1,COL_MGRAY);

    // Status
    s32 sy=50;
    if(!tt_winner){
        static const Char vez[]={CH_V,CH_E,CH_Z};
        draw_str(px+4,sy,vez,3,COL_GRAY);
        if(tt_player==1) tt_draw_x(0,0,COL_RED); // dummy — vamos redesenhar
        // Mini preview do símbolo atual
        if(tt_player==1) draw_x_at(px+pw/2,sy+18,8,3,COL_RED);
        else             draw_circle(px+pw/2,sy+18,8,3,COL_BLUE);
    } else if(tt_winner==3){
        static const Char emp[]={CH_E,CH_M,CH_P};
        static const Char ate[]={CH_A,CH_T,CH_E};
        draw_str(px+4,sy,emp,3,COL_YELLOW);
        draw_str(px+4,sy+10,ate,3,COL_YELLOW);
    } else {
        u16 wc=(tt_winner==1)?COL_RED:COL_BLUE;
        static const Char ven[]={CH_V,CH_E,CH_N};
        static const Char ceu[]={CH_C,CH_E,CH_U};
        draw_str(px+4,sy,ven,3,wc);
        draw_str(px+4,sy+10,ceu,3,wc);
        if(tt_winner==1) draw_x_at(px+pw/2,sy+28,8,3,wc);
        else             draw_circle(px+pw/2,sy+28,8,3,wc);
    }

    // Separador
    fill_rect(px+2,SCREEN_H-28,pw-4,1,COL_MGRAY);

    // Dicas
    static const Char da[]={CH_A,CH_COLON,CH_J,CH_O,CH_G};
    static const Char dst[]={CH_S,CH_T,CH_COLON,CH_R,CH_E,CH_S};
    static const Char dsel[]={CH_S,CH_E,CH_L,CH_COLON,CH_M,CH_E,CH_N,CH_U};
    draw_str(px+2,SCREEN_H-26,da,5,COL_DGRAY);
    draw_str(px+2,SCREEN_H-17,dst,6,COL_DGRAY);
    draw_str(px+2,SCREEN_H-8,dsel,8,COL_DGRAY);
}

static void tt_reset(void) {
    for(int r=0;r<3;r++) for(int c=0;c<3;c++) tt_board[r][c]=0;
    tt_cur_r=1; tt_cur_c=1;
    tt_player=1; tt_winner=0; tt_ai_turn=0;
}

static int tt_update(void) {
    if(tt_winner) {
        if(keys_down & KEY_START) { tt_reset(); fill_rect(0,13,SCREEN_W,SCREEN_H-13,COL_BG); tt_draw_board(); }
        if(keys_down & KEY_SELECT) return 0;
        return 1;
    }
    if(tt_vs_ai && tt_player==2) {
        tt_ai_turn++;
        if(tt_ai_turn>=30){
            tt_ai_turn=0;
            tt_ai_move();
            tt_winner=tt_check();
            if(tt_winner && tt_winner!=3) tt_score[tt_winner]++;
            tt_player=1;
            fill_rect(0,13,SCREEN_W,SCREEN_H-13,COL_BG);
            tt_draw_board();
        }
        if(keys_down & KEY_SELECT) return 0;
        return 1;
    }
    int moved=0;
    if(keys_down&KEY_UP    && tt_cur_r>0){tt_cur_r--;moved=1;}
    if(keys_down&KEY_DOWN  && tt_cur_r<2){tt_cur_r++;moved=1;}
    if(keys_down&KEY_LEFT  && tt_cur_c>0){tt_cur_c--;moved=1;}
    if(keys_down&KEY_RIGHT && tt_cur_c<2){tt_cur_c++;moved=1;}
    if(keys_down&KEY_A && !tt_board[tt_cur_r][tt_cur_c]) {
        tt_board[tt_cur_r][tt_cur_c]=tt_player;
        tt_winner=tt_check();
        if(tt_winner && tt_winner!=3) tt_score[tt_winner]++;
        tt_player=(tt_player==1)?2:1;
        moved=1;
    }
    if(moved){fill_rect(0,13,SCREEN_W,SCREEN_H-13,COL_BG);tt_draw_board();}
    if(keys_down & KEY_SELECT) return 0;
    return 1;
}

// =================================================================
// SNAKE
// =================================================================
#define SN_COLS  29
#define SN_ROWS  18
#define SN_CELL  8
#define SN_OX    6
#define SN_OY    14
#define SN_MAX   (SN_COLS*SN_ROWS)

typedef struct { s16 x,y; } Vec2;

static Vec2  sn_body[SN_MAX];
static s32   sn_len, sn_head, sn_tail;
static Vec2  sn_dir, sn_next_dir;
static Vec2  sn_food;
static s32   sn_score;
static s32   sn_speed, sn_tick;
static s32   sn_dead;
static s32   sn_ai_mode;

static s8  sn_ham_dx[SN_ROWS][SN_COLS];
static s8  sn_ham_dy[SN_ROWS][SN_COLS];
static s16 sn_ham_px[SN_MAX];
static s16 sn_ham_py[SN_MAX];

static u8  sn_bfs_occ[SN_ROWS][SN_COLS];
static s8  sn_bfs_from[SN_ROWS][SN_COLS];
static s16 sn_bfs_qx[SN_MAX], sn_bfs_qy[SN_MAX];

static const s8 SN_DX[4]={1,0,-1,0};
static const s8 SN_DY[4]={0,1,0,-1};

static void sn_build_occ(void){
    for(s32 r=0;r<SN_ROWS;r++) for(s32 c=0;c<SN_COLS;c++) sn_bfs_occ[r][c]=0;
    s32 bi=sn_tail;
    for(s32 k=0;k<sn_len;k++){
        sn_bfs_occ[sn_body[bi].y][sn_body[bi].x]=1;
        bi=(bi+1)%SN_MAX;
    }
    sn_bfs_occ[sn_body[sn_head].y][sn_body[sn_head].x]=0;
}

static s32 sn_bfs_on_occ(s16 hx,s16 hy,s16 tx,s16 ty){
    for(s32 r=0;r<SN_ROWS;r++) for(s32 c=0;c<SN_COLS;c++) sn_bfs_from[r][c]=-1;
    s32 qh=0,qt=0;
    sn_bfs_from[hy][hx]=4;
    sn_bfs_qx[qt]=hx; sn_bfs_qy[qt]=hy; qt++;
    while(qh<qt){
        s16 cx=sn_bfs_qx[qh],cy=sn_bfs_qy[qh]; qh++;
        if(cx==tx&&cy==ty) break;
        for(s32 d=0;d<4;d++){
            s16 nx=cx+SN_DX[d],ny=cy+SN_DY[d];
            if((u32)nx>=(u32)SN_COLS||(u32)ny>=(u32)SN_ROWS) continue;
            if(sn_bfs_occ[ny][nx]||sn_bfs_from[ny][nx]>=0) continue;
            sn_bfs_from[ny][nx]=(s8)d;
            sn_bfs_qx[qt]=nx; sn_bfs_qy[qt]=ny; qt++;
        }
    }
    if(sn_bfs_from[ty][tx]<0) return -1;
    s16 cx=tx,cy=ty;
    for(;;){
        s32 d=sn_bfs_from[cy][cx];
        s16 px=cx-SN_DX[d],py=cy-SN_DY[d];
        if(px==hx&&py==hy) return d;
        cx=px; cy=py;
    }
}

static s32 sn_ai_dir(void){
    Vec2 h    = sn_body[sn_head];
    Vec2 tail = sn_body[sn_tail];
    sn_build_occ();

    s32 d = sn_bfs_on_occ(h.x,h.y,sn_food.x,sn_food.y);
    if(d>=0){
        sn_bfs_occ[h.y][h.x]       = 1;
        sn_bfs_occ[tail.y][tail.x] = 0;
        s32 safe = sn_bfs_on_occ(sn_food.x,sn_food.y,tail.x,tail.y);
        sn_bfs_occ[h.y][h.x]       = 0;
        sn_bfs_occ[tail.y][tail.x] = 1;
        if(safe>=0) return d;
    }

    s32 best_dir=-1, best_score=99999;
    for(s32 dir=0;dir<4;dir++){
        s16 nx=h.x+SN_DX[dir], ny=h.y+SN_DY[dir];
        if((u32)nx>=(u32)SN_COLS||(u32)ny>=(u32)SN_ROWS) continue;
        if(sn_bfs_occ[ny][nx]) continue;
        sn_bfs_occ[h.y][h.x]       = 1;
        sn_bfs_occ[tail.y][tail.x] = 0;
        s32 tail_ok = sn_bfs_on_occ(nx,ny,tail.x,tail.y);
        sn_bfs_occ[h.y][h.x]       = 0;
        sn_bfs_occ[tail.y][tail.x] = 1;
        if(tail_ok<0) continue;
        s32 mdist = (nx>sn_food.x?nx-sn_food.x:sn_food.x-nx)
                  + (ny>sn_food.y?ny-sn_food.y:sn_food.y-ny);
        if(mdist<best_score){ best_score=mdist; best_dir=dir; }
    }
    if(best_dir>=0) return best_dir;

    s8 hdx=sn_ham_dx[h.y][h.x], hdy=sn_ham_dy[h.y][h.x];
    for(s32 i=0;i<4;i++) if(SN_DX[i]==hdx&&SN_DY[i]==hdy) return i;
    return 0;
}

static void sn_build_hamilton(void) {
    s32 n=0;
    s32 top=0, bot=SN_ROWS-1, lft=0, rgt=SN_COLS-1;
    while(top<=bot && lft<=rgt){
        for(s32 c=lft;c<=rgt;c++){sn_ham_px[n]=c;sn_ham_py[n]=top;n++;}  top++;
        for(s32 r=top;r<=bot;r++){sn_ham_px[n]=rgt;sn_ham_py[n]=r;n++;}  rgt--;
        if(top<=bot){for(s32 c=rgt;c>=lft;c--){sn_ham_px[n]=c;sn_ham_py[n]=bot;n++;} bot--;}
        if(lft<=rgt){for(s32 r=bot;r>=top;r--){sn_ham_px[n]=lft;sn_ham_py[n]=r;n++;} lft++;}
    }
    for(s32 i=0;i<n;i++){
        s32 ni=(i+1)%n;
        sn_ham_dx[sn_ham_py[i]][sn_ham_px[i]] = (s8)(sn_ham_px[ni]-sn_ham_px[i]);
        sn_ham_dy[sn_ham_py[i]][sn_ham_px[i]] = (s8)(sn_ham_py[ni]-sn_ham_py[i]);
    }
}

static void sn_place_food(void) {
    for(s32 att=0; att<400; att++){
        s32 fx = (s32)(rng()&0xFFFF)*SN_COLS>>16;
        s32 fy = (s32)(rng()&0xFFFF)*SN_ROWS>>16;
        s32 hit=0, i=sn_tail;
        for(s32 k=0;k<sn_len;k++){
            if(sn_body[i].x==fx&&sn_body[i].y==fy){hit=1;break;}
            i=(i+1)%SN_MAX;
        }
        if(!hit){sn_food.x=fx;sn_food.y=fy;return;}
    }
}

// Segmento de corpo: verde escuro com linha de brilho no topo
static void sn_draw_body(s32 x, s32 y) {
    s32 px=SN_OX+x*SN_CELL, py=SN_OY+y*SN_CELL;
    fill_rect(px+1,py+1,SN_CELL-2,SN_CELL-2,COL_DGREEN);
    fill_rect(px+2,py+2,SN_CELL-4,1,COL_GREEN);
    fill_rect(px+2,py+2,1,SN_CELL-4,COL_GREEN);
}

// Cabeça: lima brilhante com olhos baseados na direção
static void sn_draw_head(s32 x, s32 y, s16 dx, s16 dy) {
    s32 px=SN_OX+x*SN_CELL, py=SN_OY+y*SN_CELL;
    fill_rect(px+1,py+1,SN_CELL-2,SN_CELL-2,COL_LIME);
    fill_rect(px+2,py+2,SN_CELL-4,1,RGB15(18,31,10));
    // olhos (2px pretos conforme direção)
    s32 ex1,ey1,ex2,ey2;
    if(dx==1)       {ex1=px+5;ey1=py+2;ex2=px+5;ey2=py+5;}
    else if(dx==-1) {ex1=px+2;ey1=py+2;ex2=px+2;ey2=py+5;}
    else if(dy==-1) {ex1=px+2;ey1=py+2;ex2=px+5;ey2=py+2;}
    else            {ex1=px+2;ey1=py+5;ex2=px+5;ey2=py+5;}
    put_pixel(ex1,ey1,COL_BLACK);
    put_pixel(ex2,ey2,COL_BLACK);
}

// Maçã: corpo vermelho + cabo verde + brilho branco
static void sn_draw_food(void) {
    s32 px=SN_OX+sn_food.x*SN_CELL, py=SN_OY+sn_food.y*SN_CELL;
    // Corpo da maçã
    fill_rect(px+2,py+3,4,4,COL_RED);
    fill_rect(px+1,py+4,6,2,COL_RED);
    // Cabo
    put_pixel(px+4,py+2,COL_DGREEN);
    // Folhinha
    put_pixel(px+5,py+2,COL_GREEN);
    // Brilho
    put_pixel(px+2,py+4,RGB15(31,18,18));
    put_pixel(px+3,py+3,RGB15(31,22,22));
}

static void sn_draw_board(void) {
    static const Char title[]={CH_S,CH_N,CH_A,CH_K,CH_E};
    draw_titlebar(title,5,COL_LIME,COL_DGRAY);

    // Borda dupla do campo
    fill_rect(SN_OX-3,SN_OY-3,SN_COLS*SN_CELL+6,3,COL_BORDER);
    fill_rect(SN_OX-3,SN_OY+SN_ROWS*SN_CELL,SN_COLS*SN_CELL+6,3,COL_BORDER);
    fill_rect(SN_OX-3,SN_OY-3,3,SN_ROWS*SN_CELL+6,COL_BORDER);
    fill_rect(SN_OX+SN_COLS*SN_CELL,SN_OY-3,3,SN_ROWS*SN_CELL+6,COL_BORDER);
    // Inner border highlight
    fill_rect(SN_OX-1,SN_OY-1,SN_COLS*SN_CELL+2,1,COL_MGRAY);
    fill_rect(SN_OX-1,SN_OY+SN_ROWS*SN_CELL,SN_COLS*SN_CELL+2,1,COL_MGRAY);
    fill_rect(SN_OX-1,SN_OY-1,1,SN_ROWS*SN_CELL+2,COL_MGRAY);
    fill_rect(SN_OX+SN_COLS*SN_CELL,SN_OY-1,1,SN_ROWS*SN_CELL+2,COL_MGRAY);

    s32 sx=SN_OX+SN_COLS*SN_CELL+6;
    draw_box(sx,14,SCREEN_W-sx-2,SCREEN_H-16,COL_BORDER,COL_PANEL);

    static const Char sc[]={CH_S,CH_C,CH_O,CH_R,CH_E};
    draw_str(sx+4,20,sc,5,COL_SILVER);
    draw_num(sx+4,32,sn_score,COL_GOLD);

    if(sn_ai_mode){
        static const Char ai[]={CH_I,CH_A};
        draw_str(sx+4,46,ai,2,COL_LIME);
    }

    fill_rect(sx+2,SCREEN_H-20,SCREEN_W-sx-4,1,COL_MGRAY);
    static const Char ctrl[]={CH_S,CH_E,CH_L};
    draw_str(sx+4,SCREEN_H-16,ctrl,3,COL_DGRAY);
}

static void sn_init(void) {
    sn_len=4; sn_head=3; sn_tail=0;
    if(sn_ai_mode){
        sn_build_hamilton();
        for(s32 i=0;i<4;i++){sn_body[i].x=i;sn_body[i].y=1;}
        sn_dir.x=1; sn_dir.y=0;
    } else {
        for(s32 i=0;i<4;i++){sn_body[i].x=SN_COLS/2-2+i;sn_body[i].y=SN_ROWS/2;}
        sn_dir.x=1;sn_dir.y=0;
    }
    sn_next_dir=sn_dir;
    sn_score=0; sn_speed=sn_ai_mode?1:8; sn_tick=0; sn_dead=0;
    sn_place_food();
    fill_rect(SN_OX,SN_OY,SN_COLS*SN_CELL,SN_ROWS*SN_CELL,COL_BG);
    sn_draw_board();
    // Desenha corpo
    s32 i=sn_tail;
    for(s32 k=0;k<sn_len-1;k++){
        sn_draw_body(sn_body[i].x,sn_body[i].y);
        i=(i+1)%SN_MAX;
    }
    sn_draw_head(sn_body[sn_head].x,sn_body[sn_head].y,sn_dir.x,sn_dir.y);
    sn_draw_food();
}

static int sn_update(void) {
    if(keys_down&KEY_SELECT) return 0;

    if(sn_dead){
        if(sn_ai_mode){
            sn_tick++;
            if(sn_tick>120){
                fill_rect(SN_OX,SN_OY,SN_COLS*SN_CELL,SN_ROWS*SN_CELL,COL_BG);
                sn_init();
            }
        } else {
            if(keys_down&KEY_A||keys_down&KEY_START){
                fill_rect(SN_OX,SN_OY,SN_COLS*SN_CELL,SN_ROWS*SN_CELL,COL_BG);
                sn_init();
            }
        }
        return 1;
    }

    sn_tick++;
    if(sn_tick < sn_speed) return 1;
    sn_tick=0;

    if(sn_ai_mode){
        s32 d=sn_ai_dir();
        sn_dir.x=SN_DX[d];
        sn_dir.y=SN_DY[d];
    } else {
        if((keys_down|keys_held)&KEY_UP    && sn_dir.y==0){sn_next_dir.x=0;sn_next_dir.y=-1;}
        if((keys_down|keys_held)&KEY_DOWN  && sn_dir.y==0){sn_next_dir.x=0;sn_next_dir.y= 1;}
        if((keys_down|keys_held)&KEY_LEFT  && sn_dir.x==0){sn_next_dir.x=-1;sn_next_dir.y=0;}
        if((keys_down|keys_held)&KEY_RIGHT && sn_dir.x==0){sn_next_dir.x= 1;sn_next_dir.y=0;}
        sn_dir=sn_next_dir;
    }

    Vec2 head=sn_body[sn_head];
    Vec2 nxt={head.x+sn_dir.x, head.y+sn_dir.y};

    if(nxt.x<0||nxt.x>=SN_COLS||nxt.y<0||nxt.y>=SN_ROWS){sn_dead=1;goto dead;}
    {s32 i=sn_tail;
    for(s32 k=0;k<sn_len;k++){
        if(sn_body[i].x==nxt.x&&sn_body[i].y==nxt.y){sn_dead=1;goto dead;}
        i=(i+1)%SN_MAX;
    }}

    sn_head=(sn_head+1)%SN_MAX;
    sn_body[sn_head]=nxt;

    // A cabeça anterior vira corpo
    sn_draw_body(head.x,head.y);

    if(nxt.x==sn_food.x && nxt.y==sn_food.y){
        sn_len++;sn_score+=10;
        if(sn_len==SN_MAX){sn_dead=1;goto zerou;}
        if(!sn_ai_mode && sn_speed>3) sn_speed--;
        sn_place_food();
        sn_draw_board();
        sn_draw_food();
    } else {
        // Apaga a cauda
        s32 px=SN_OX+sn_body[sn_tail].x*SN_CELL, py=SN_OY+sn_body[sn_tail].y*SN_CELL;
        fill_rect(px+1,py+1,SN_CELL-2,SN_CELL-2,COL_BG);
        sn_tail=(sn_tail+1)%SN_MAX;
    }
    // Nova cabeça
    sn_draw_head(nxt.x,nxt.y,sn_dir.x,sn_dir.y);
    return 1;

zerou:;
    {s32 sx2=SN_OX+SN_COLS*SN_CELL+4;
    static const Char zr[]={CH_Z,CH_E,CH_R,CH_O,CH_U};
    static const Char ex[]={CH_EXCL};
    draw_str(sx2-4,SCREEN_H/2-10,zr,5,COL_GOLD);
    draw_str(sx2+10,SCREEN_H/2-10,ex,1,COL_GOLD);}
    sn_tick=0;
    return 1;

dead:;
    {s32 i=sn_tail;
    for(s32 k=0;k<sn_len;k++){
        s32 px=SN_OX+sn_body[i].x*SN_CELL, py=SN_OY+sn_body[i].y*SN_CELL;
        fill_rect(px+1,py+1,SN_CELL-2,SN_CELL-2,COL_RED);
        i=(i+1)%SN_MAX;
    }
    s32 sx2=SN_OX+SN_COLS*SN_CELL+4;
    static const Char go[]={CH_G,CH_A,CH_M,CH_E,CH_SP,CH_O,CH_V,CH_E,CH_R};
    draw_str(sx2-2,SCREEN_H/2-10,go,9,COL_RED);
    if(!sn_ai_mode){static const Char ra[]={CH_A,CH_COLON,CH_J,CH_O,CH_G};draw_str(sx2,SCREEN_H/2+4,ra,5,COL_GRAY);}
    sn_tick=0;}
    return 1;
}

// =================================================================
// PONG
// =================================================================
#define PG_W    SCREEN_W
#define PG_H    SCREEN_H
#define PD_W    5
#define PD_H    28
#define PD_X1   8
#define PD_X2   (PG_W-8-PD_W)
#define PB_R    3   // raio da bola

static s32 pg_p1y, pg_p2y;
static s32 pg_bx, pg_by;
static s32 pg_bdx, pg_bdy;
static s32 pg_sc1, pg_sc2;
static s32 pg_mode;
static s32 pg_wait;

static void pg_reset_ball(s32 dir) {
    pg_bx=PG_W/2; pg_by=PG_H/2;
    pg_bdx=dir*2;
    // Usa timer hardware para ângulo aleatório da bola
    rng_stir();
    s32 r=(s32)(rng()%5)-2;
    pg_bdy = (r==0) ? 1 : r;
}

static void pg_init(void) {
    pg_p1y=(PG_H-PD_H)/2; pg_p2y=(PG_H-PD_H)/2;
    pg_sc1=0; pg_sc2=0; pg_wait=60;
    pg_reset_ball(1);
}

// Paddle com highlight lateral
static void pg_draw_paddle(s32 x, s32 y, u16 col, u16 hi) {
    fill_rect(x,y,PD_W,PD_H,col);
    fill_rect(x,y,1,PD_H,hi);
    fill_rect(x,y,PD_W,1,hi);
}

static void pg_draw_field(void) {
    static const Char title[]={CH_P,CH_O,CH_N,CH_G};
    draw_titlebar(title,4,COL_CYAN,COL_DGRAY);
    fill_rect(0,13,PG_W,PG_H-13,COL_BG);
    // Linha central pontilhada — mais vistosa
    for(s32 y=14;y<PG_H;y+=8){
        fill_rect(PG_W/2-1,y,2,5,COL_MGRAY);
    }
    // Placar em caixas
    draw_box(PG_W/2-26,14,18,12,COL_BORDER,COL_PANEL);
    draw_box(PG_W/2+8, 14,18,12,COL_BORDER,COL_PANEL);
    draw_num(PG_W/2-22,16,pg_sc1,COL_RED);
    draw_num(PG_W/2+12,16,pg_sc2,COL_BLUE);
}

static void pg_erase(void){
    fill_rect(PD_X1,pg_p1y,PD_W,PD_H,COL_BG);
    fill_rect(PD_X2,pg_p2y,PD_W,PD_H,COL_BG);
    // Apaga bola (círculo + margem)
    fill_rect(pg_bx-PB_R-1,pg_by-PB_R-1,PB_R*2+3,PB_R*2+3,COL_BG);
}

static void pg_draw(void){
    pg_draw_paddle(PD_X1,pg_p1y,COL_RED,RGB15(31,18,18));
    pg_draw_paddle(PD_X2,pg_p2y,COL_BLUE,RGB15(10,18,31));
    // Bola circular com brilho
    draw_circle(pg_bx,pg_by,PB_R,1,COL_WHITE);
    fill_rect(pg_bx-1,pg_by-1,2,2,COL_WHITE);
    put_pixel(pg_bx-1,pg_by-1,COL_SILVER);
}

static int pg_update(void){
    if(keys_down&KEY_SELECT) return 0;

    if(pg_wait>0){pg_wait--;return 1;}

    pg_erase();

    if(keys_held&KEY_UP   && pg_p1y>14)      pg_p1y-=2;
    if(keys_held&KEY_DOWN && pg_p1y+PD_H<PG_H) pg_p1y+=2;

    if(pg_mode==0){
        if(keys_held&KEY_L && pg_p2y>14)        pg_p2y-=2;
        if(keys_held&KEY_R && pg_p2y+PD_H<PG_H) pg_p2y+=2;
    } else {
        s32 ai_center=pg_p2y+PD_H/2;
        if(ai_center < pg_by+PB_R-2 && pg_p2y+PD_H<PG_H) pg_p2y+=2;
        if(ai_center > pg_by+PB_R+2 && pg_p2y>14)         pg_p2y-=2;
    }

    pg_bx+=pg_bdx; pg_by+=pg_bdy;

    if(pg_by<=14){pg_by=14;pg_bdy=-pg_bdy;}
    if(pg_by+PB_R>=PG_H){pg_by=PG_H-PB_R-1;pg_bdy=-pg_bdy;}

    if(pg_bdx<0 && pg_bx-PB_R<=PD_X1+PD_W && pg_bx>=PD_X1-2 &&
       pg_by+PB_R>=pg_p1y && pg_by-PB_R<=pg_p1y+PD_H){
        pg_bdx=-pg_bdx;
        s32 rel=(pg_by)-(pg_p1y+PD_H/2);
        pg_bdy=rel/5;
        pg_bx=PD_X1+PD_W+PB_R+1;
    }
    if(pg_bdx>0 && pg_bx+PB_R>=PD_X2 && pg_bx<=PD_X2+PD_W+2 &&
       pg_by+PB_R>=pg_p2y && pg_by-PB_R<=pg_p2y+PD_H){
        pg_bdx=-pg_bdx;
        s32 rel=(pg_by)-(pg_p2y+PD_H/2);
        pg_bdy=rel/5;
        pg_bx=PD_X2-PB_R-1;
    }

    if(pg_bx<0){
        pg_sc2++;
        fill_rect(PG_W/2-26,14,44,12,COL_BG);
        draw_box(PG_W/2-26,14,18,12,COL_BORDER,COL_PANEL);
        draw_box(PG_W/2+8, 14,18,12,COL_BORDER,COL_PANEL);
        draw_num(PG_W/2-22,16,pg_sc1,COL_RED);
        draw_num(PG_W/2+12,16,pg_sc2,COL_BLUE);
        pg_reset_ball(1);pg_wait=60;
    }
    if(pg_bx>PG_W){
        pg_sc1++;
        fill_rect(PG_W/2-26,14,44,12,COL_BG);
        draw_box(PG_W/2-26,14,18,12,COL_BORDER,COL_PANEL);
        draw_box(PG_W/2+8, 14,18,12,COL_BORDER,COL_PANEL);
        draw_num(PG_W/2-22,16,pg_sc1,COL_RED);
        draw_num(PG_W/2+12,16,pg_sc2,COL_BLUE);
        pg_reset_ball(-1);pg_wait=60;
    }

    pg_draw();
    return 1;
}

// =================================================================
// MENU PRINCIPAL
// =================================================================
typedef enum { GAME_MENU, GAME_TT_MODE, GAME_TT, GAME_SN_MODE, GAME_SN, GAME_PG_MODE, GAME_PG } GameState;
static GameState g_state;
static s32 menu_sel;

static void draw_main_menu(void) {
    clear_screen();
    draw_stars();

    // Barra de título estilizada
    fill_rect(0,0,SCREEN_W,14,COL_DGRAY);
    fill_rect(0,13,SCREEN_W,1,COL_BORDER);
    static const Char logo[]={CH_G,CH_A,CH_M,CH_E,CH_SP,CH_S,CH_E,CH_L,CH_E,CH_C,CH_T};
    s32 lw=11*7;
    draw_str((SCREEN_W-lw)/2,3,logo,11,COL_GOLD);

    // Decorações cantos (X e O retro)
    draw_x_at(16,80,10,3,COL_RED);
    draw_circle(224,80,10,3,COL_BLUE);

    struct { const Char* t; s32 len; u16 col; u16 hi; } items[3]={
        {(const Char[]){CH_J,CH_O,CH_G,CH_O,CH_SP,CH_D,CH_A,CH_SP,CH_V,CH_E,CH_L}, 11, COL_RED,   COL_DRED},
        {(const Char[]){CH_S,CH_N,CH_A,CH_K,CH_E},                                   5, COL_LIME,  COL_DGREEN},
        {(const Char[]){CH_P,CH_O,CH_N,CH_G},                                         4, COL_CYAN,  COL_DBLUE},
    };

    for(s32 i=0;i<3;i++){
        s32 y=35+i*36;
        s32 bx=28, bw=SCREEN_W-56;
        if(menu_sel==i){
            draw_box(bx-1,y-5,bw+2,20,items[i].col,COL_PANEL);
            // Marcador lateral esquerdo
            fill_rect(bx+1,y-3,3,16,items[i].col);
        } else {
            draw_box(bx-1,y-5,bw+2,20,COL_BORDER,COL_PANEL);
        }
        s32 tx=(SCREEN_W-items[i].len*7)/2;
        u16 fc = (menu_sel==i) ? COL_WHITE : items[i].col;
        draw_str(tx,y+2,items[i].t,items[i].len,fc);
    }

    // Ícones à direita de cada item
    draw_x_at(SCREEN_W-35,35+7,6,2,COL_RED);
    // cobra simplificada: linha horizontal de 3 quadradinhos
    for(s32 k=0;k<3;k++) fill_rect(SCREEN_W-40+k*5,71,4,4,COL_LIME);
    // bola de pong
    draw_circle(SCREEN_W-35,107+7,4,1,COL_CYAN);

    static const Char hint[]={CH_A,CH_COLON,CH_S,CH_E,CH_L,CH_E,CH_C,CH_I,CH_O,CH_N,CH_A,CH_R};
    draw_str((SCREEN_W-12*7)/2,SCREEN_H-10,hint,12,COL_DGRAY);
}

static void draw_mode_menu(s32 type) {
    clear_screen();
    draw_stars();

    const Char* t; s32 tlen; u16 tc;
    if(type==2){
        static const Char tp[]={CH_P,CH_O,CH_N,CH_G};
        t=tp;tlen=4;tc=COL_CYAN;
    } else if(type==1){
        static const Char ts[]={CH_S,CH_N,CH_A,CH_K,CH_E};
        t=ts;tlen=5;tc=COL_LIME;
    } else {
        static const Char tj[]={CH_J,CH_O,CH_G,CH_O,CH_SP,CH_D,CH_A,CH_SP,CH_V,CH_E,CH_L};
        t=tj;tlen=11;tc=COL_RED;
    }
    draw_titlebar(t,tlen,tc,COL_DGRAY);

    static const Char m1p[]={CH_J,CH_O,CH_G,CH_A,CH_D,CH_O,CH_R};
    static const Char m2p[]={CH_2,CH_SP,CH_J,CH_O,CH_G,CH_A,CH_D,CH_O,CH_R,CH_E,CH_S};
    static const Char mAI[]={CH_C,CH_O,CH_M,CH_P,CH_U,CH_T,CH_A,CH_D,CH_O,CH_R};

    s32 sel = (type==2)?pg_mode:(type==1)?sn_ai_mode:tt_vs_ai;

    for(s32 i=0;i<2;i++){
        s32 y=52+i*38;
        const Char* mt; s32 ml;
        if(i==0){ mt=(type==0)?m2p:m1p; ml=(type==0)?11:7; }
        else     { mt=mAI; ml=10; }
        s32 bx=28, bw=SCREEN_W-56;
        if(sel==i){
            draw_box(bx-1,y-5,bw+2,20,tc,COL_PANEL);
            fill_rect(bx+1,y-3,3,16,tc);
        } else {
            draw_box(bx-1,y-5,bw+2,20,COL_BORDER,COL_PANEL);
        }
        u16 fc = (sel==i) ? COL_WHITE : tc;
        draw_str((SCREEN_W-ml*7)/2,y+2,mt,ml,fc);
    }

    static const Char hint[]={CH_A,CH_COLON,CH_O,CH_K,CH_SP,CH_SP,CH_S,CH_E,CH_L,CH_COLON,CH_V,CH_O,CH_L,CH_T,CH_A,CH_R};
    draw_str((SCREEN_W-16*7)/2,SCREEN_H-10,hint,16,COL_DGRAY);
}

// =================================================================
// MAIN
// =================================================================
int main(void) {
    REG_DISPCNT = MODE3 | BG2_ENABLE;

    // Inicia timer de hardware 0 em modo livre (sem prescaler) para entropia
    REG_TM0CNT_L = 0;
    REG_TM0CNT_H = 0x0080;  // enable, F_cpu/1

    prev_keys=0; keys_held=0; keys_down=0;
    g_state=GAME_MENU; menu_sel=0;
    tt_score[1]=0; tt_score[2]=0;
    tt_vs_ai=0; pg_mode=1; sn_ai_mode=0;
    draw_main_menu();

    while(1){
        rng_stir();
        vsync();
        update_keys();

        switch(g_state){

        case GAME_MENU:
            if(keys_down&KEY_UP)   {menu_sel=(menu_sel+2)%3;draw_main_menu();}
            if(keys_down&KEY_DOWN) {menu_sel=(menu_sel+1)%3;draw_main_menu();}
            if(keys_down&KEY_A||keys_down&KEY_START){
                if(menu_sel==0){g_state=GAME_TT_MODE;tt_vs_ai=0;draw_mode_menu(0);}
                else if(menu_sel==1){g_state=GAME_SN_MODE;sn_ai_mode=0;draw_mode_menu(1);}
                else {g_state=GAME_PG_MODE;pg_mode=1;draw_mode_menu(2);}
            }
            break;

        case GAME_TT_MODE:
            if(keys_down&KEY_UP||keys_down&KEY_DOWN){tt_vs_ai=!tt_vs_ai;draw_mode_menu(0);}
            if(keys_down&KEY_A||keys_down&KEY_START){
                g_state=GAME_TT;
                tt_reset(); fill_rect(0,0,SCREEN_W,SCREEN_H,COL_BG); tt_draw_board();
            }
            if(keys_down&KEY_SELECT||keys_down&KEY_B){g_state=GAME_MENU;draw_main_menu();}
            break;

        case GAME_TT:
            if(!tt_update()){g_state=GAME_MENU;draw_main_menu();}
            break;

        case GAME_SN_MODE:
            if(keys_down&KEY_UP||keys_down&KEY_DOWN){sn_ai_mode=!sn_ai_mode;draw_mode_menu(1);}
            if(keys_down&KEY_A||keys_down&KEY_START){g_state=GAME_SN;sn_init();}
            if(keys_down&KEY_SELECT||keys_down&KEY_B){g_state=GAME_MENU;draw_main_menu();}
            break;

        case GAME_SN:
            if(!sn_update()){g_state=GAME_MENU;draw_main_menu();}
            break;

        case GAME_PG_MODE:
            if(keys_down&KEY_UP||keys_down&KEY_DOWN){pg_mode=!pg_mode;draw_mode_menu(2);}
            if(keys_down&KEY_A||keys_down&KEY_START){
                g_state=GAME_PG; pg_init(); pg_draw_field(); pg_draw();
            }
            if(keys_down&KEY_SELECT||keys_down&KEY_B){g_state=GAME_MENU;draw_main_menu();}
            break;

        case GAME_PG:
            if(!pg_update()){g_state=GAME_MENU;draw_main_menu();}
            break;
        }
    }
    return 0;
}
