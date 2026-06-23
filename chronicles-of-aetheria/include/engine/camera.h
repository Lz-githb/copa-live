#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H

#include "../gba/gba_types.h"

/* =========================================================
 * Camera System
 *
 * The camera defines what region of the world map is
 * visible on the 240x160 screen. It scrolls BG layers
 * and adjusts sprite positions accordingly.
 *
 * Camera uses pixel coordinates. The camera origin is the
 * top-left corner of the visible screen in world space.
 *
 * Parallax: background layers can scroll at a fraction of
 * the camera speed to fake depth.
 * ========================================================= */

typedef struct {
    s32 x;          /* top-left world x (pixels) */
    s32 y;          /* top-left world y (pixels) */
    s32 target_x;   /* smooth-scroll target */
    s32 target_y;
    s32 world_w;    /* world size in pixels (for clamping) */
    s32 world_h;
    u8  speed;      /* lerp speed 1-16 (16=instant) */
    BOOL locked;    /* TRUE = no movement */
} Camera;

/* ---- Camera API ----------------------------------------- */

void camera_init(s32 world_w, s32 world_h);

/* Set camera position immediately (no lerp) */
void camera_set_pos(s32 x, s32 y);

/* Set lerp target (smooth follow) */
void camera_follow(s32 target_x, s32 target_y);

/* Center camera on a point */
void camera_center_on(s32 world_x, s32 world_y);

/* Update: lerp toward target, clamp to world bounds */
void camera_update(void);

/* Apply scroll to BG hardware registers */
void camera_apply_bg_scroll(void);

/* Convert world coordinates to screen coordinates */
INLINE s32 camera_world_to_screen_x(s32 world_x);
INLINE s32 camera_world_to_screen_y(s32 world_y);

/* Check if a world rect is on-screen (for culling) */
BOOL camera_is_visible(s32 wx, s32 wy, s32 w, s32 h);

/* Get current camera world position */
s32 camera_x(void);
s32 camera_y(void);

/* Lock/unlock camera movement */
void camera_lock(BOOL locked);

/* Shake effect */
void camera_shake(u8 intensity, u8 frames);

#endif /* ENGINE_CAMERA_H */
