#ifndef GBA_REG_H
#define GBA_REG_H

#include "gba_types.h"

/* =========================================================
 * GBA Hardware Registers
 * All addresses and bit-fields for direct hardware access
 * ========================================================= */

/* ---- Memory Base Addresses -------------------------------- */
#define MEM_EWRAM  0x02000000
#define MEM_IWRAM  0x03000000
#define MEM_IO     0x04000000
#define MEM_PAL    0x05000000
#define MEM_VRAM   0x06000000
#define MEM_OAM    0x07000000
#define MEM_ROM    0x08000000
#define MEM_SRAM   0x0E000000

/* ---- Memory as arrays ------------------------------------- */
#define PAL_BG_MEM     ((COLOR*)(MEM_PAL))
#define PAL_OBJ_MEM    ((COLOR*)(MEM_PAL + 0x200))
#define VRAM_BG_MEM    ((u16*)(MEM_VRAM))
#define OAM_MEM        ((u32*)(MEM_OAM))
#define SRAM_MEM       ((u8*)(MEM_SRAM))

/* Tile and map bases in VRAM */
#define TILE_MEM       ((u32(*)[512][8])(MEM_VRAM))         /* [charblock][tile][row] */
#define MAP_MEM        ((u16(*)[32][32])(MEM_VRAM))          /* [screenblock][row][col] */
#define TILE_MEM_OBJ   ((u32(*)[512][8])(MEM_VRAM + 0x10000))

/* ---- Display Registers ------------------------------------ */
#define REG_DISPCNT    (*(vu32*)(MEM_IO + 0x000))
#define REG_DISPSTAT   (*(vu16*)(MEM_IO + 0x004))
#define REG_VCOUNT     (*(vu16*)(MEM_IO + 0x006))

/* DISPCNT bits */
#define DCNT_MODE0     0x0000
#define DCNT_MODE1     0x0001
#define DCNT_MODE2     0x0002
#define DCNT_OBJ_MAP_1D 0x0040
#define DCNT_BLANK     0x0080
#define DCNT_BG0       0x0100
#define DCNT_BG1       0x0200
#define DCNT_BG2       0x0400
#define DCNT_BG3       0x0800
#define DCNT_OBJ       0x1000
#define DCNT_WIN0      0x2000
#define DCNT_WIN1      0x4000
#define DCNT_WINOBJ    0x8000

/* DISPSTAT bits */
#define DSTAT_IN_VBL   0x0001
#define DSTAT_IN_HBL   0x0002
#define DSTAT_IN_VCT   0x0004
#define DSTAT_VBL_IRQ  0x0008
#define DSTAT_HBL_IRQ  0x0010
#define DSTAT_VCT_IRQ  0x0020

/* ---- Background Registers --------------------------------- */
#define REG_BG0CNT     (*(vu16*)(MEM_IO + 0x008))
#define REG_BG1CNT     (*(vu16*)(MEM_IO + 0x00A))
#define REG_BG2CNT     (*(vu16*)(MEM_IO + 0x00C))
#define REG_BG3CNT     (*(vu16*)(MEM_IO + 0x00E))
#define REG_BG0HOFS    (*(vu16*)(MEM_IO + 0x010))
#define REG_BG0VOFS    (*(vu16*)(MEM_IO + 0x012))
#define REG_BG1HOFS    (*(vu16*)(MEM_IO + 0x014))
#define REG_BG1VOFS    (*(vu16*)(MEM_IO + 0x016))
#define REG_BG2HOFS    (*(vu16*)(MEM_IO + 0x018))
#define REG_BG2VOFS    (*(vu16*)(MEM_IO + 0x01A))
#define REG_BG3HOFS    (*(vu16*)(MEM_IO + 0x01C))
#define REG_BG3VOFS    (*(vu16*)(MEM_IO + 0x01E))

/* BGCNT builder macro */
#define BG_CNT(prio, cbb, mosaic, clr256, sbb, wrap, size) \
    ((prio) | ((cbb) << 2) | ((mosaic) << 6) | ((clr256) << 7) | \
     ((sbb) << 8) | ((wrap) << 13) | ((size) << 14))

/* BG sizes (regular) */
#define BG_SIZE_256x256  0
#define BG_SIZE_512x256  1
#define BG_SIZE_256x512  2
#define BG_SIZE_512x512  3

/* ---- Interrupt Registers ---------------------------------- */
#define REG_IE         (*(vu16*)(MEM_IO + 0x200))
#define REG_IF         (*(vu16*)(MEM_IO + 0x202))
#define REG_IME        (*(vu16*)(MEM_IO + 0x208))

/* IRQ bits */
#define IRQ_VBLANK     0x0001
#define IRQ_HBLANK     0x0002
#define IRQ_VCOUNT     0x0004
#define IRQ_TIMER0     0x0008
#define IRQ_TIMER1     0x0010
#define IRQ_TIMER2     0x0020
#define IRQ_TIMER3     0x0040
#define IRQ_DMA0       0x0100
#define IRQ_DMA1       0x0200
#define IRQ_DMA2       0x0400
#define IRQ_DMA3       0x0800

/* IRQ handler address (BIOS-managed) */
#define IRQ_HANDLER    (*(void(**)(void))(MEM_IWRAM + 0x7FFC))

/* ---- Input Registers -------------------------------------- */
#define REG_KEYINPUT   (*(vu16*)(MEM_IO + 0x130))
#define REG_KEYCNT     (*(vu16*)(MEM_IO + 0x132))

/* Key bits (0 = pressed) */
#define KEY_A          0x0001
#define KEY_B          0x0002
#define KEY_SELECT     0x0004
#define KEY_START      0x0008
#define KEY_RIGHT      0x0010
#define KEY_LEFT       0x0020
#define KEY_UP         0x0040
#define KEY_DOWN       0x0080
#define KEY_R          0x0100
#define KEY_L          0x0200
#define KEY_MASK       0x03FF

/* ---- Timer Registers -------------------------------------- */
#define REG_TM0CNT_L   (*(vu16*)(MEM_IO + 0x100))
#define REG_TM0CNT_H   (*(vu16*)(MEM_IO + 0x102))
#define REG_TM1CNT_L   (*(vu16*)(MEM_IO + 0x104))
#define REG_TM1CNT_H   (*(vu16*)(MEM_IO + 0x106))
#define REG_TM2CNT_L   (*(vu16*)(MEM_IO + 0x108))
#define REG_TM2CNT_H   (*(vu16*)(MEM_IO + 0x10A))
#define REG_TM3CNT_L   (*(vu16*)(MEM_IO + 0x10C))
#define REG_TM3CNT_H   (*(vu16*)(MEM_IO + 0x10E))

/* Timer control bits */
#define TM_FREQ_1      0x0000
#define TM_FREQ_64     0x0001
#define TM_FREQ_256    0x0002
#define TM_FREQ_1024   0x0003
#define TM_CASCADE     0x0004
#define TM_IRQ         0x0040
#define TM_ENABLE      0x0080

/* ---- DMA Registers ---------------------------------------- */
#define REG_DMA0SAD    (*(vu32*)(MEM_IO + 0x0B0))
#define REG_DMA0DAD    (*(vu32*)(MEM_IO + 0x0B4))
#define REG_DMA0CNT_L  (*(vu16*)(MEM_IO + 0x0B8))
#define REG_DMA0CNT_H  (*(vu16*)(MEM_IO + 0x0BA))
#define REG_DMA1SAD    (*(vu32*)(MEM_IO + 0x0BC))
#define REG_DMA1DAD    (*(vu32*)(MEM_IO + 0x0C0))
#define REG_DMA1CNT_L  (*(vu16*)(MEM_IO + 0x0C4))
#define REG_DMA1CNT_H  (*(vu16*)(MEM_IO + 0x0C6))
#define REG_DMA2SAD    (*(vu32*)(MEM_IO + 0x0C8))
#define REG_DMA2DAD    (*(vu32*)(MEM_IO + 0x0CC))
#define REG_DMA2CNT_L  (*(vu16*)(MEM_IO + 0x0D0))
#define REG_DMA2CNT_H  (*(vu16*)(MEM_IO + 0x0D2))
#define REG_DMA3SAD    (*(vu32*)(MEM_IO + 0x0D4))
#define REG_DMA3DAD    (*(vu32*)(MEM_IO + 0x0D8))
#define REG_DMA3CNT_L  (*(vu16*)(MEM_IO + 0x0DC))
#define REG_DMA3CNT_H  (*(vu16*)(MEM_IO + 0x0DE))

/* DMA control bits */
#define DMA_DST_INC    0x0000
#define DMA_DST_DEC    0x0020
#define DMA_DST_FIXED  0x0040
#define DMA_DST_RELOAD 0x0060
#define DMA_SRC_INC    0x0000
#define DMA_SRC_DEC    0x0080
#define DMA_SRC_FIXED  0x0100
#define DMA_REPEAT     0x0200
#define DMA_16BIT      0x0000
#define DMA_32BIT      0x0400
#define DMA_AT_NOW     0x0000
#define DMA_AT_VBLANK  0x1000
#define DMA_AT_HBLANK  0x2000
#define DMA_AT_SPECIAL 0x3000
#define DMA_IRQ        0x4000
#define DMA_ENABLE     0x8000

/* ---- Audio Registers -------------------------------------- */
#define REG_SOUND1CNT_L  (*(vu16*)(MEM_IO + 0x060))
#define REG_SOUND1CNT_H  (*(vu16*)(MEM_IO + 0x062))
#define REG_SOUND1CNT_X  (*(vu16*)(MEM_IO + 0x064))
#define REG_SOUND2CNT_L  (*(vu16*)(MEM_IO + 0x068))
#define REG_SOUND2CNT_H  (*(vu16*)(MEM_IO + 0x06C))
#define REG_SOUND3CNT_L  (*(vu16*)(MEM_IO + 0x070))
#define REG_SOUND3CNT_H  (*(vu16*)(MEM_IO + 0x072))
#define REG_SOUND3CNT_X  (*(vu16*)(MEM_IO + 0x074))
#define REG_SOUND4CNT_L  (*(vu16*)(MEM_IO + 0x078))
#define REG_SOUND4CNT_H  (*(vu16*)(MEM_IO + 0x07C))
#define REG_SOUNDCNT_L   (*(vu16*)(MEM_IO + 0x080))
#define REG_SOUNDCNT_H   (*(vu16*)(MEM_IO + 0x082))
#define REG_SOUNDCNT_X   (*(vu16*)(MEM_IO + 0x084))
#define REG_SOUNDBIAS    (*(vu16*)(MEM_IO + 0x088))
#define REG_FIFO_A       (*(vu32*)(MEM_IO + 0x0A0))
#define REG_FIFO_B       (*(vu32*)(MEM_IO + 0x0A4))

/* SOUNDCNT_H bits */
#define SNDCNT_H_DMG_VOL25   0x0000
#define SNDCNT_H_DMG_VOL50   0x0001
#define SNDCNT_H_DMG_VOL100  0x0002
#define SNDCNT_H_DSSA_VOL50  0x0000
#define SNDCNT_H_DSSA_VOL100 0x0004
#define SNDCNT_H_DSSB_VOL50  0x0000
#define SNDCNT_H_DSSB_VOL100 0x0008
#define SNDCNT_H_DSSA_R      0x0100
#define SNDCNT_H_DSSA_L      0x0200
#define SNDCNT_H_DSSA_TM0    0x0000
#define SNDCNT_H_DSSA_TM1    0x0400
#define SNDCNT_H_DSSA_RESET  0x0800
#define SNDCNT_H_DSSB_R      0x1000
#define SNDCNT_H_DSSB_L      0x2000
#define SNDCNT_H_DSSB_TM0    0x0000
#define SNDCNT_H_DSSB_TM1    0x4000
#define SNDCNT_H_DSSB_RESET  0x8000

/* SOUNDCNT_X */
#define SNDCNT_X_MASTER_ENABLE 0x0080

/* ---- OAM (Sprite Attribute) structure --------------------- */
typedef struct {
    u16 attr0;
    u16 attr1;
    u16 attr2;
    s16 fill;
} OBJ_ATTR;

typedef struct {
    u16 fill0[3]; s16 pa;
    u16 fill1[3]; s16 pb;
    u16 fill2[3]; s16 pc;
    u16 fill3[3]; s16 pd;
} OBJ_AFFINE;

#define OAM_OBJ_MEM   ((OBJ_ATTR*)(MEM_OAM))
#define OAM_AFF_MEM   ((OBJ_AFFINE*)(MEM_OAM))

/* OBJ_ATTR0 bits */
#define OBJ_Y(y)            ((y) & 0xFF)
#define OBJ_ROT_SCALE       0x0100
#define OBJ_DOUBLE          0x0200
#define OBJ_HIDE            0x0200  /* when ROT_SCALE=0 */
#define OBJ_DISABLE         0x0200
#define OBJ_BLD             0x0400
#define OBJ_WIN             0x0800
#define OBJ_MOSAIC          0x1000
#define OBJ_8BPP            0x2000
#define OBJ_4BPP            0x0000
#define OBJ_SQUARE          0x0000
#define OBJ_WIDE            0x4000
#define OBJ_TALL            0x8000

/* OBJ_ATTR1 bits */
#define OBJ_X(x)            ((x) & 0x1FF)
#define OBJ_AFF_ID(n)       ((n) << 9)
#define OBJ_HFLIP           0x1000
#define OBJ_VFLIP           0x2000
#define OBJ_SIZE_8          0x0000
#define OBJ_SIZE_16         0x4000
#define OBJ_SIZE_32         0x8000
#define OBJ_SIZE_64         0xC000

/* OBJ_ATTR2 bits */
#define OBJ_TID(n)          ((n) & 0x3FF)
#define OBJ_PRIO(n)         ((n) << 10)
#define OBJ_PAL(n)          ((n) << 12)

/* Map entry bits */
#define SE_TID(n)    ((n) & 0x3FF)
#define SE_HFLIP     0x0400
#define SE_VFLIP     0x0800
#define SE_PAL(n)    ((n) << 12)

#endif /* GBA_REG_H */
