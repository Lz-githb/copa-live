#ifndef GBA_TYPES_H
#define GBA_TYPES_H

/* =========================================================
 * GBA Primitive Types
 * All GBA code should use these typedefs for clarity
 * ========================================================= */

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef signed char        s8;
typedef signed short       s16;
typedef signed int         s32;
typedef volatile u8        vu8;
typedef volatile u16       vu16;
typedef volatile u32       vu32;
typedef volatile s8        vs8;
typedef volatile s16       vs16;
typedef volatile s32       vs32;

typedef u16 COLOR;   /* 15-bit BGR555 */
typedef u16 TILE4;   /* 4bpp tile row (8 pixels packed) */
typedef u32 TILE8;   /* 8bpp tile row (8 pixels packed) */

/* Fixed-point 8.8 */
typedef s32 FIXED;
#define INT_TO_FIXED(n)   ((n) << 8)
#define FIXED_TO_INT(n)   ((n) >> 8)
#define FIXED_MUL(a, b)   (((a) * (b)) >> 8)

/* Boolean */
typedef u32 BOOL;
#define TRUE  1
#define FALSE 0
#define NULL  ((void*)0)

/* Memory section attributes */
#define IWRAM_CODE __attribute__((section(".iwram"), long_call))
#define IWRAM_DATA __attribute__((section(".iwram")))
#define EWRAM_DATA __attribute__((section(".ewram")))
#define EWRAM_BSS  __attribute__((section(".sbss")))

/* Alignment */
#define ALIGN4  __attribute__((aligned(4)))
#define ALIGN32 __attribute__((aligned(32)))

/* Inline */
#define INLINE static inline

/* Array size helper */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* Clamp */
#define CLAMP(val, lo, hi) ((val) < (lo) ? (lo) : (val) > (hi) ? (hi) : (val))
#define MIN(a, b)          ((a) < (b) ? (a) : (b))
#define MAX(a, b)          ((a) > (b) ? (a) : (b))

/* GBA screen dimensions */
#define SCREEN_W  240
#define SCREEN_H  160

/* BGR555 color constructor */
#define RGB15(r, g, b) ((COLOR)((r) | ((g) << 5) | ((b) << 10)))

#endif /* GBA_TYPES_H */
