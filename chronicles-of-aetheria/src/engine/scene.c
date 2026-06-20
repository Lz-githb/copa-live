#include "../../include/engine/scene.h"
#include "../../include/gba/gba_bios.h"

/* =========================================================
 * Scene System Implementation
 * ========================================================= */

static Scene* s_stack[SCENE_STACK_MAX];
static u32    s_depth = 0;

void scene_init(void) {
    s_depth = 0;
    u32 i;
    for (i = 0; i < SCENE_STACK_MAX; i++) s_stack[i] = NULL;
}

void scene_push(Scene* scene) {
    if (!scene || s_depth >= SCENE_STACK_MAX) return;

    /* Pause the current top scene */
    if (s_depth > 0 && s_stack[s_depth - 1]->on_pause) {
        s_stack[s_depth - 1]->on_pause(s_stack[s_depth - 1]);
    }

    s_stack[s_depth++] = scene;
    scene->active = TRUE;

    if (scene->on_load) {
        scene->on_load(scene);
    }
}

void scene_pop(void) {
    if (s_depth == 0) return;

    Scene* top = s_stack[--s_depth];
    top->active = FALSE;

    if (top->on_unload) {
        top->on_unload(top);
    }

    /* Resume the scene below */
    if (s_depth > 0 && s_stack[s_depth - 1]->on_resume) {
        s_stack[s_depth - 1]->on_resume(s_stack[s_depth - 1]);
    }
}

void scene_replace(Scene* scene) {
    if (!scene) return;

    /* Unload current top without restoring the one below */
    if (s_depth > 0) {
        Scene* top = s_stack[s_depth - 1];
        top->active = FALSE;
        if (top->on_unload) top->on_unload(top);
        s_depth--;
    }

    s_stack[s_depth++] = scene;
    scene->active = TRUE;

    if (scene->on_load) {
        scene->on_load(scene);
    }
}

void scene_update(void) {
    if (s_depth == 0) return;
    Scene* top = s_stack[s_depth - 1];
    if (top && top->update) {
        top->update(top);
    }
}

void scene_render(void) {
    if (s_depth == 0) return;
    Scene* top = s_stack[s_depth - 1];
    if (top && top->render) {
        top->render(top);
    }
}

Scene* scene_top(void) {
    if (s_depth == 0) return NULL;
    return s_stack[s_depth - 1];
}

u32 scene_depth(void) {
    return s_depth;
}
