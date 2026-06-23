#ifndef GFX_WORLD_H
#define GFX_WORLD_H

/* Load world tilesets into GBA VRAM charblocks.
   BG2 charblock 1 = overworld tiles.
   BG3 charblock 1 = dungeon/cave tiles (shared charblock, swapped on map load).
   Call gfx_world_load_overworld() or gfx_world_load_dungeon() when changing maps. */

void gfx_world_load(void);            /* load default (overworld) on game start */
void gfx_world_load_overworld(void);  /* switch to overworld tileset */
void gfx_world_load_dungeon(void);    /* switch to dungeon tileset */
void gfx_chars_load(void);
void gfx_npcs_load(void);
void gfx_monsters_load(void);
void gfx_sprites_load_all(void);      /* call once at startup: chars + npcs + monsters */

#endif /* GFX_WORLD_H */
