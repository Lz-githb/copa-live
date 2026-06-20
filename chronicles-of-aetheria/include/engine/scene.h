#ifndef ENGINE_SCENE_H
#define ENGINE_SCENE_H

#include "../gba/gba_types.h"

/* =========================================================
 * Scene System
 *
 * A Scene is a self-contained gameplay area (map room,
 * menu screen, cutscene). Scenes live on a stack so you
 * can overlay them (e.g., dialogue on top of overworld).
 *
 * Scene vs State:
 *   State  = broad game MODE (overworld, battle, menu)
 *   Scene  = specific CONTENT within a state (village map,
 *             dungeon floor, item menu screen)
 * ========================================================= */

#define SCENE_STACK_MAX  8
#define SCENE_NAME_LEN   16

typedef struct Scene Scene;

struct Scene {
    char  name[SCENE_NAME_LEN];
    void  (*on_load)(Scene* self);      /* load assets, reset state */
    void  (*on_unload)(Scene* self);    /* free scene-local resources */
    void  (*on_pause)(Scene* self);     /* pushed under another scene */
    void  (*on_resume)(Scene* self);    /* top of stack again */
    void  (*update)(Scene* self);
    void  (*render)(Scene* self);
    void* data;                          /* scene-private data pointer */
    BOOL  active;
};

/* ---- Scene stack API ------------------------------------- */

void  scene_init(void);

/* Push a scene (calls on_load, then becomes active) */
void  scene_push(Scene* scene);

/* Pop top scene (calls on_unload, resumes one below) */
void  scene_pop(void);

/* Replace top scene without growing the stack */
void  scene_replace(Scene* scene);

/* Tick the active (top) scene */
void  scene_update(void);
void  scene_render(void);

/* Access the top scene */
Scene* scene_top(void);

/* How deep is the stack */
u32   scene_depth(void);

#endif /* ENGINE_SCENE_H */
