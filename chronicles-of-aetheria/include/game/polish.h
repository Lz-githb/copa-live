#ifndef GAME_POLISH_H
#define GAME_POLISH_H

#include "../engine/engine.h"

/* -----------------------------------------------------------------------
 * polish.h — Stage 15 screen-feel polish system for Chronicles of Aetheria
 * Screen flash, camera shake, fade transitions, palette cycling
 * ----------------------------------------------------------------------- */

/* Flash colour constants */
#define FLASH_WHITE  0
#define FLASH_RED    1
#define FLASH_GREEN  2

/* Fade direction (opaque to callers) */
typedef enum { FADE_NONE = 0, FADE_OUT, FADE_IN } FadeDir;

/* ---------------------------------------------------------------------- */
/*  Init / master update                                                   */
/* ---------------------------------------------------------------------- */
void polish_init(void);
void polish_update(void);   /* call once per frame before rendering */

/* ---------------------------------------------------------------------- */
/*  Screen flash                                                           */
/* ---------------------------------------------------------------------- */
void polish_flash(u8 color, u8 duration);
void polish_flash_update(void);

/* ---------------------------------------------------------------------- */
/*  Camera shake                                                           */
/* ---------------------------------------------------------------------- */
void polish_shake(u8 intensity, u8 duration);
void polish_shake_update(void);
s8   polish_shake_x(void);
s8   polish_shake_y(void);

/* ---------------------------------------------------------------------- */
/*  Fade to / from black                                                   */
/* ---------------------------------------------------------------------- */
void polish_fade_out(u8 speed, void (*on_done)(void));
void polish_fade_in(u8 speed, void (*on_done)(void));
void polish_fade_update(void);
BOOL polish_fade_done(void);

/* ---------------------------------------------------------------------- */
/*  Footstep throttle helper                                               */
/* ---------------------------------------------------------------------- */
/* Returns TRUE every 8 frames while the player is moving; call each frame */
BOOL polish_footstep_tick(void);

/* ---------------------------------------------------------------------- */
/*  FPS stabilizer                                                         */
/* ---------------------------------------------------------------------- */
/* Returns TRUE if the current frame should be skipped (frame-skip guard) */
BOOL polish_should_skip_render(void);

#endif /* GAME_POLISH_H */
