#include "../../include/engine/camera.h"
#include "../../include/engine/tile.h"
#include "../../include/gba/gba_reg.h"

/* =========================================================
 * Camera System Implementation
 * ========================================================= */

static Camera s_cam;

/* Shake state */
static u8  s_shake_intensity;
static u8  s_shake_frames;
static u32 s_shake_seed;   /* cheap LCG for shake offsets */

/* Simple LCG for camera shake noise */
static s16 _shake_offset(void) {
    s_shake_seed = s_shake_seed * 1664525u + 1013904223u;
    s32 n = (s32)(s_shake_seed >> 24);   /* -128..127 */
    return (s16)((n * s_shake_intensity) >> 7);
}

void camera_init(s32 world_w, s32 world_h) {
    s_cam.x        = 0;
    s_cam.y        = 0;
    s_cam.target_x = 0;
    s_cam.target_y = 0;
    s_cam.world_w  = world_w;
    s_cam.world_h  = world_h;
    s_cam.speed    = 8;
    s_cam.locked   = FALSE;
    s_shake_intensity = 0;
    s_shake_frames    = 0;
    s_shake_seed      = 0xDEADBEEF;
}

void camera_set_pos(s32 x, s32 y) {
    s_cam.x = CLAMP(x, 0, s_cam.world_w - SCREEN_W);
    s_cam.y = CLAMP(y, 0, s_cam.world_h - SCREEN_H);
    s_cam.target_x = s_cam.x;
    s_cam.target_y = s_cam.y;
}

void camera_follow(s32 target_x, s32 target_y) {
    s_cam.target_x = target_x;
    s_cam.target_y = target_y;
}

void camera_center_on(s32 world_x, s32 world_y) {
    s32 cx = world_x - SCREEN_W / 2;
    s32 cy = world_y - SCREEN_H / 2;
    camera_follow(cx, cy);
}

void camera_update(void) {
    if (s_cam.locked) return;

    /* Linear interpolation toward target */
    s32 dx = s_cam.target_x - s_cam.x;
    s32 dy = s_cam.target_y - s_cam.y;

    /* Lerp: advance by speed/16 of distance per frame */
    s_cam.x += (dx * s_cam.speed) >> 4;
    s_cam.y += (dy * s_cam.speed) >> 4;

    /* Clamp to world bounds */
    s_cam.x = CLAMP(s_cam.x, 0, s_cam.world_w - SCREEN_W);
    s_cam.y = CLAMP(s_cam.y, 0, s_cam.world_h - SCREEN_H);
}

void camera_apply_bg_scroll(void) {
    s32 ox = s_cam.x;
    s32 oy = s_cam.y;

    /* Apply shake offset */
    if (s_shake_frames > 0) {
        ox += _shake_offset();
        oy += _shake_offset();
        s_shake_frames--;
    }

    /* Main world layer (BG2) scrolls 1:1 */
    REG_BG2HOFS = (u16)(ox & 0x1FF);
    REG_BG2VOFS = (u16)(oy & 0x1FF);

    /* Foreground layer (BG1) scrolls same as world */
    REG_BG1HOFS = (u16)(ox & 0x1FF);
    REG_BG1VOFS = (u16)(oy & 0x1FF);

    /* Background/sky layer (BG3) scrolls at half speed for parallax */
    REG_BG3HOFS = (u16)((ox >> 1) & 0x1FF);
    REG_BG3VOFS = (u16)((oy >> 1) & 0x1FF);

    /* HUD (BG0) never scrolls */
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
}

INLINE s32 camera_world_to_screen_x(s32 world_x) {
    return world_x - s_cam.x;
}

INLINE s32 camera_world_to_screen_y(s32 world_y) {
    return world_y - s_cam.y;
}

BOOL camera_is_visible(s32 wx, s32 wy, s32 w, s32 h) {
    return (wx + w > s_cam.x) && (wx < s_cam.x + SCREEN_W) &&
           (wy + h > s_cam.y) && (wy < s_cam.y + SCREEN_H);
}

s32 camera_x(void) { return s_cam.x; }
s32 camera_y(void) { return s_cam.y; }

void camera_lock(BOOL locked) { s_cam.locked = locked; }

void camera_shake(u8 intensity, u8 frames) {
    s_shake_intensity = intensity;
    s_shake_frames    = frames;
}
