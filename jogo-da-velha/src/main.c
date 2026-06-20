// Jogo da Velha para GBA - feito do zero
// Compilado com arm-none-eabi-gcc sem libgba

// ========== Registradores GBA ==========
#define REG_DISPCNT   (*(volatile unsigned short*)0x04000000)
#define REG_VCOUNT    (*(volatile unsigned short*)0x04000006)
#define REG_KEYINPUT  (*(volatile unsigned short*)0x04000130)
#define REG_BG0CNT    (*(volatile unsigned short*)0x04000008)
#define REG_BG0HOFS   (*(volatile unsigned short*)0x04000010)
#define REG_BG0VOFS   (*(volatile unsigned short*)0x04000012)

// Paleta e VRAM
#define MEM_PALRAM    ((volatile unsigned short*)0x05000000)
#define MEM_VRAM      ((volatile unsigned short*)0x06000000)
#define MEM_OAM       ((volatile unsigned int*)0x07000000)

// Botões
#define KEY_A      0x0001
#define KEY_B      0x0002
#define KEY_SELECT 0x0004
#define KEY_START  0x0008
#define KEY_RIGHT  0x0010
#define KEY_LEFT   0x0020
#define KEY_UP     0x0040
#define KEY_DOWN   0x0080

#define KEY_PRESSED(k)  (!(REG_KEYINPUT & (k)))

// Display mode 3 (bitmap 240x160 15bpp)
#define MODE3 0x0003
#define BG2_ENABLE 0x0400

#define SCREEN_W 240
#define SCREEN_H 160

typedef unsigned short u16;
typedef unsigned int   u32;
typedef unsigned char  u8;

// ========== Cor RGB15 ==========
#define RGB15(r,g,b) ((r) | ((g)<<5) | ((b)<<10))

// Cores
#define COL_BG      RGB15(3, 3, 10)    // azul escuro
#define COL_GRID    RGB15(20,20,20)    // cinza claro
#define COL_X       RGB15(31, 8, 8)   // vermelho
#define COL_O       RGB15(8, 8, 31)   // azul
#define COL_CURSOR  RGB15(28,28, 0)   // amarelo
#define COL_WHITE   RGB15(31,31,31)
#define COL_WIN     RGB15(5, 25, 5)   // verde escuro
#define COL_DRAW    RGB15(20,20, 5)   // amarelo escuro

// ========== Pixel na VRAM modo 3 ==========
static inline void put_pixel(int x, int y, u16 color) {
    MEM_VRAM[y * SCREEN_W + x] = color;
}

// Retângulo preenchido
static void fill_rect(int x, int y, int w, int h, u16 color) {
    for (int j = y; j < y + h; j++)
        for (int i = x; i < x + w; i++)
            put_pixel(i, j, color);
}

// Linha de Bresenham
static void draw_line(int x0, int y0, int x1, int y1, int thick, u16 color) {
    int dx = x1 - x0, dy = y1 - y0;
    int sx = dx > 0 ? 1 : -1, sy = dy > 0 ? 1 : -1;
    dx = dx < 0 ? -dx : dx;
    dy = dy < 0 ? -dy : dy;
    int err = dx - dy;
    while (1) {
        for (int t = -thick/2; t <= thick/2; t++) {
            if (dx >= dy) put_pixel(x0, y0 + t, color);
            else          put_pixel(x0 + t, y0, color);
        }
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 <  dx) { err += dx; y0 += sy; }
    }
}

// Círculo (Bresenham)
static void draw_circle(int cx, int cy, int r, int thick, u16 color) {
    for (int t = 0; t < thick; t++) {
        int rad = r - t;
        int x = 0, y = rad, d = 3 - 2 * rad;
        while (x <= y) {
            put_pixel(cx+x, cy+y, color); put_pixel(cx-x, cy+y, color);
            put_pixel(cx+x, cy-y, color); put_pixel(cx-x, cy-y, color);
            put_pixel(cx+y, cy+x, color); put_pixel(cx-y, cy+x, color);
            put_pixel(cx+y, cy-x, color); put_pixel(cx-y, cy-x, color);
            if (d < 0) d += 4*x + 6;
            else { d += 4*(x-y) + 10; y--; }
            x++;
        }
    }
}

// ========== VSync ==========
static void vsync(void) {
    while (REG_VCOUNT >= 160);
    while (REG_VCOUNT < 160);
}

// ========== Estado do jogo ==========
// 0=vazio, 1=X, 2=O
static int board[3][3];
static int cursor_row, cursor_col;
static int current_player; // 1=X, 2=O
static int winner;         // 0=nenhum, 1=X, 2=O, 3=empate
static int win_line[4];    // r0,c0,r1,c1 da linha vencedora
static u16 prev_keys;
static int redraw_needed;

// ========== Leitura de teclas (borda de subida) ==========
static u16 keys_down;

static void update_keys(void) {
    u16 cur = ~REG_KEYINPUT & 0x03FF;
    keys_down = cur & ~prev_keys;
    prev_keys = cur;
}

// ========== Grade e layout ==========
// Grade 3x3 centralizada
#define GRID_X   40   // x do canto esquerdo da grade
#define GRID_Y   20   // y do canto superior
#define CELL_W   50
#define CELL_H   40
#define LINE_T   3    // espessura das linhas da grade

static int cell_cx(int col) { return GRID_X + col * CELL_W + CELL_W/2; }
static int cell_cy(int row) { return GRID_Y + row * CELL_H + CELL_H/2; }

// ========== Desenho ==========

static void clear_screen(void) {
    fill_rect(0, 0, SCREEN_W, SCREEN_H, COL_BG);
}

static void draw_grid(void) {
    int gw = CELL_W * 3;
    int gh = CELL_H * 3;
    // 2 linhas verticais
    for (int i = 1; i <= 2; i++)
        fill_rect(GRID_X + i*CELL_W - LINE_T/2, GRID_Y, LINE_T, gh, COL_GRID);
    // 2 linhas horizontais
    for (int i = 1; i <= 2; i++)
        fill_rect(GRID_X, GRID_Y + i*CELL_H - LINE_T/2, gw, LINE_T, COL_GRID);
}

static void draw_x(int row, int col, u16 color) {
    int cx = cell_cx(col), cy = cell_cy(row);
    int half = 14;
    draw_line(cx-half, cy-half, cx+half, cy+half, 4, color);
    draw_line(cx+half, cy-half, cx-half, cy+half, 4, color);
}

static void draw_o(int row, int col, u16 color) {
    draw_circle(cell_cx(col), cell_cy(row), 15, 4, color);
}

static void draw_cursor(int row, int col) {
    int cx = cell_cx(col), cy = cell_cy(row);
    int half = 20;
    // Quadrado pontilhado nos cantos
    for (int i = 0; i < 8; i++) {
        put_pixel(cx - half + i, cy - half, COL_CURSOR);
        put_pixel(cx + half - i, cy - half, COL_CURSOR);
        put_pixel(cx - half + i, cy + half, COL_CURSOR);
        put_pixel(cx + half - i, cy + half, COL_CURSOR);
        put_pixel(cx - half, cy - half + i, COL_CURSOR);
        put_pixel(cx - half, cy + half - i, COL_CURSOR);
        put_pixel(cx + half, cy - half + i, COL_CURSOR);
        put_pixel(cx + half, cy + half - i, COL_CURSOR);
    }
}

// ========== Texto bitmap 5x7 simplificado ==========
// Fonte compacta para caracteres necessários, indexada manualmente
// Índice: A=0,B=1,C=2,D=3,E=4,G=5,I=6,J=7,L=8,M=9,N=10,O=11,P=12,R=13,S=14,T=15,U=16,V=17,X=18,Z=19,
//         0=20,1=21,2=22,3=23,4=24,5=25,6=26,7=27,8=28,9=29,SP=30,'-'=31

static const u8 FONT[][5] = {
/*A*/ {0x7E,0x11,0x11,0x11,0x7E},
/*B*/ {0x7F,0x49,0x49,0x49,0x36},
/*C*/ {0x3E,0x41,0x41,0x41,0x22},
/*D*/ {0x7F,0x41,0x41,0x22,0x1C},
/*E*/ {0x7F,0x49,0x49,0x49,0x41},
/*G*/ {0x3E,0x41,0x49,0x49,0x3A},
/*I*/ {0x00,0x41,0x7F,0x41,0x00},
/*J*/ {0x20,0x40,0x41,0x3F,0x01},
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
};

typedef enum {
    CH_A=0,CH_B,CH_C,CH_D,CH_E,CH_G,CH_I,CH_J,CH_L,CH_M,CH_N,CH_O,
    CH_P,CH_R,CH_S,CH_T,CH_U,CH_V,CH_X,CH_Z,
    CH_0,CH_1,CH_2,CH_3,CH_4,CH_5,CH_6,CH_7,CH_8,CH_9,CH_SP,CH_DASH
} Char;

static void draw_char(int x, int y, Char c, u16 color) {
    for (int col = 0; col < 5; col++) {
        u8 bits = FONT[c][col];
        for (int row = 0; row < 7; row++) {
            if (bits & (1 << row))
                put_pixel(x + col, y + row, color);
        }
    }
}

static void draw_str(int x, int y, const Char* str, int len, u16 color) {
    for (int i = 0; i < len; i++)
        draw_char(x + i * 7, y, str[i], color);
}

// ========== Lógica do jogo ==========

static void reset_game(void) {
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            board[r][c] = 0;
    cursor_row = 1; cursor_col = 1;
    current_player = 1;
    winner = 0;
    redraw_needed = 1;
}

static int check_winner(void) {
    // Linhas
    for (int r = 0; r < 3; r++) {
        if (board[r][0] && board[r][0]==board[r][1] && board[r][1]==board[r][2]) {
            win_line[0]=r; win_line[1]=0; win_line[2]=r; win_line[3]=2;
            return board[r][0];
        }
    }
    // Colunas
    for (int c = 0; c < 3; c++) {
        if (board[0][c] && board[0][c]==board[1][c] && board[1][c]==board[2][c]) {
            win_line[0]=0; win_line[1]=c; win_line[2]=2; win_line[3]=c;
            return board[0][c];
        }
    }
    // Diagonais
    if (board[0][0] && board[0][0]==board[1][1] && board[1][1]==board[2][2]) {
        win_line[0]=0; win_line[1]=0; win_line[2]=2; win_line[3]=2;
        return board[0][0];
    }
    if (board[0][2] && board[0][2]==board[1][1] && board[1][1]==board[2][0]) {
        win_line[0]=0; win_line[1]=2; win_line[2]=2; win_line[3]=0;
        return board[0][2];
    }
    // Empate
    int full = 1;
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            if (!board[r][c]) full = 0;
    if (full) return 3;
    return 0;
}

static void draw_win_line(void) {
    int x0 = cell_cx(win_line[1]);
    int y0 = cell_cy(win_line[0]);
    int x1 = cell_cx(win_line[3]);
    int y1 = cell_cy(win_line[2]);
    draw_line(x0, y0, x1, y1, 3, COL_WIN);
}

// ========== Área de status (direita da grade) ==========
#define STATUS_X 195
#define STATUS_Y  20

static void draw_status(void) {
    // Limpa área de status
    fill_rect(STATUS_X - 5, 0, SCREEN_W - STATUS_X + 5, SCREEN_H, COL_BG);

    if (winner == 0) {
        // "VEZ"
        static const Char vez[] = {CH_V,CH_E,CH_Z};
        draw_str(STATUS_X, STATUS_Y, vez, 3, COL_WHITE);
        // X ou O
        if (current_player == 1)
            draw_x(0, 0, COL_X); // desenha direto na área de status
        // Vou usar draw_char direto:
        if (current_player == 1)
            draw_char(STATUS_X + 4, STATUS_Y + 12, CH_X, COL_X);
        else
            draw_char(STATUS_X + 4, STATUS_Y + 12, CH_O, COL_O);
    } else if (winner == 3) {
        // "EMPATE"
        static const Char emp[] = {CH_E,CH_M,CH_SP,CH_P,CH_A,CH_T,CH_E};
        draw_str(STATUS_X - 15, STATUS_Y + 20, emp, 7, COL_DRAW);
    } else {
        // "VENCEU"
        static const Char ven[] = {CH_V,CH_E,CH_N,CH_C,CH_E,CH_U};
        u16 wc = (winner == 1) ? COL_X : COL_O;
        Char p = (winner == 1) ? CH_X : CH_O;
        draw_char(STATUS_X, STATUS_Y + 6, p, wc);
        draw_str(STATUS_X - 7, STATUS_Y + 18, ven, 6, wc);
    }

    // Instrução na base
    static const Char press_a[]  = {CH_A,CH_SP,CH_J,CH_O,CH_G,CH_A,CH_R};
    static const Char press_st[] = {CH_S,CH_T,CH_A,CH_R,CH_T};
    draw_str(4, SCREEN_H - 20, press_a, 7, COL_WHITE);
    draw_str(4, SCREEN_H - 10, press_st, 5, COL_WHITE);
}

// ========== Redesenha tudo ==========
static void draw_all(void) {
    clear_screen();
    draw_grid();

    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            if (board[r][c] == 1) draw_x(r, c, COL_X);
            else if (board[r][c] == 2) draw_o(r, c, COL_O);
        }
    }

    if (winner && winner != 3)
        draw_win_line();

    if (!winner)
        draw_cursor(cursor_row, cursor_col);

    draw_status();
    redraw_needed = 0;
}

// ========== Título ==========
static void draw_title(void) {
    clear_screen();
    // "JOGO DA VEL"
    static const Char t1[] = {CH_J,CH_O,CH_G,CH_O};
    static const Char t2[] = {CH_D,CH_A,CH_SP,CH_V,CH_E,CH_L};
    static const Char press[] = {CH_S,CH_T,CH_A,CH_R,CH_T};
    draw_str(60, 50, t1, 4, COL_X);
    draw_str(32, 65, t2, 6, COL_O);
    draw_str(72, 100, press, 5, COL_WHITE);

    // Desenha X e O decorativos
    draw_x(0, 0, COL_X); // vai aparecer na grade mas ok - vou posicionar manual
    // X decorativo
    draw_line(20, 30, 36, 46, 4, COL_X);
    draw_line(36, 30, 20, 46, 4, COL_X);
    // O decorativo
    draw_circle(204, 38, 14, 4, COL_O);
}

// ========== Main ==========
int main(void) {
    REG_DISPCNT = MODE3 | BG2_ENABLE;

    int state = 0; // 0=título, 1=jogo
    prev_keys = 0;
    reset_game();

    draw_title();

    while (1) {
        vsync();
        update_keys();

        if (state == 0) {
            if (keys_down & KEY_START) {
                state = 1;
                reset_game();
                draw_all();
            }
        } else {
            // Movimento do cursor
            if (!winner) {
                int moved = 0;
                if (keys_down & KEY_UP)    { if (cursor_row > 0) { cursor_row--; moved=1; } }
                if (keys_down & KEY_DOWN)  { if (cursor_row < 2) { cursor_row++; moved=1; } }
                if (keys_down & KEY_LEFT)  { if (cursor_col > 0) { cursor_col--; moved=1; } }
                if (keys_down & KEY_RIGHT) { if (cursor_col < 2) { cursor_col++; moved=1; } }

                if (moved) redraw_needed = 1;

                // Jogar
                if (keys_down & KEY_A) {
                    if (board[cursor_row][cursor_col] == 0) {
                        board[cursor_row][cursor_col] = current_player;
                        winner = check_winner();
                        current_player = (current_player == 1) ? 2 : 1;
                        redraw_needed = 1;
                    }
                }
            }

            // Reiniciar
            if (keys_down & KEY_START) {
                if (winner) {
                    reset_game();
                    redraw_needed = 1;
                }
            }

            // Voltar ao título
            if (keys_down & KEY_SELECT) {
                state = 0;
                draw_title();
            }

            if (redraw_needed) draw_all();
        }
    }
    return 0;
}
