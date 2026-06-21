#include "inventory.h"
#include "menu_types.h"
#include "crafting.h"
#include "item_db.h"

/* =========================================================
 * Crafting Menu
 *
 * Layout (30x20):
 *  ┌──────────────────────────────┐  row 0
 *  │  CRAFTING           [G:xxx] │  row 1
 *  ├──────────────────────────────┤  row 2
 *  │ > Recipe Name   [BASIC]     │  rows 3-14  (list, 12 visible)
 *  │   Recipe Name   [FORGE]     │
 *  ├──────────────────────────────┤  row 15
 *  │  Ingredient x2  Ingredient  │  rows 15-17 (ingredient panel)
 *  ├──────────────────────────────┤  row 18
 *  │  [A] Craft  [B] Back        │  row 19
 *  └──────────────────────────────┘
 * ========================================================= */

#define CRAFT_VISIBLE_ROWS  12
#define CRAFT_LIST_TOP       3

typedef struct {
    u8  list[RECIPE_COUNT]; /* known recipe indices */
    u8  list_count;
    u8  cursor;
    u8  scroll;
} CraftMenuState;

static CraftMenuState s_craft;

static void _rebuild_list(void)
{
    u8 i;
    s_craft.list_count = 0;
    for (i = 0; i < RECIPE_COUNT; i++) {
        if (craft_is_known(i))
            s_craft.list[s_craft.list_count++] = i;
    }
}

static const char* _diff_label(CraftDifficulty d)
{
    switch (d) {
    case CRAFT_BASIC:    return "BASIC   ";
    case CRAFT_FORGE:    return "FORGE   ";
    case CRAFT_ALCHEMY:  return "ALCHEMY ";
    case CRAFT_LEGENDARY:return "LEGEND  ";
    default:             return "        ";
    }
}

static void _render_ingredient(u8 col, u8 row, const Ingredient* ing)
{
    if (ing->item_id == ITEM_NONE) {
        ui_write_str(col, row, "            ", PAL_DIM);
        return;
    }
    {
        const ItemDef* d = item_get(ing->item_id);
        if (!d) return;
        ui_write_str(col, row, d->name, PAL_TEXT);
        ui_write_char((u8)(col + 10), row, 'x', PAL_DIM);
        ui_write_num((u8)(col + 11), row, ing->quantity, PAL_TEXT);
    }
}

static void _render_recipe_panel(u8 recipe_id)
{
    const CraftRecipe* r = &g_recipe_table[recipe_id];
    u8 i;

    /* Ingredient rows */
    for (i = 0; i < RECIPE_INGREDIENT_MAX; i++) {
        u8 col = (i < 2) ? 1 : 16;
        u8 row = (u8)(15 + (i % 2));
        _render_ingredient(col, row, &r->ingredients[i]);
    }

    /* Result */
    {
        const ItemDef* res = item_get(r->result_id);
        if (res) {
            ui_write_str(1, 17, ">> ", PAL_HI);
            ui_write_str(4, 17, res->name, PAL_HI);
            ui_write_char(16, 17, 'x', PAL_DIM);
            ui_write_num(17, 17, r->result_qty, PAL_HI);
        }
    }

    /* Can craft? */
    {
        u8 pal = craft_can_make(recipe_id) ? PAL_HI : PAL_DIM;
        ui_write_str(1, 18, "[A] CRAFT", pal);
        ui_write_str(12, 18, "[B] BACK", PAL_TEXT);
    }
}

/* ---- Public API ---------------------------------------- */

void menu_crafting_open(void)
{
    _rebuild_list();
    s_craft.cursor = 0;
    s_craft.scroll = 0;
    g_menu.dirty   = TRUE;
}

void menu_crafting_update(void)
{
    u16 pressed = g_menu.dirty ? 0 : 0; /* just use g_input */
    pressed = g_input.pressed;

    if (pressed & KEY_UP) {
        if (s_craft.cursor > 0) s_craft.cursor--;
        else if (s_craft.scroll > 0) s_craft.scroll--;
        g_menu.dirty = TRUE;
    }
    if (pressed & KEY_DOWN) {
        u8 total = s_craft.list_count;
        if (s_craft.cursor < CRAFT_VISIBLE_ROWS - 1 &&
            s_craft.scroll + s_craft.cursor < total - 1)
            s_craft.cursor++;
        else if (s_craft.scroll + CRAFT_VISIBLE_ROWS < total)
            s_craft.scroll++;
        g_menu.dirty = TRUE;
    }
    if (pressed & KEY_A) {
        u8 idx = (u8)(s_craft.scroll + s_craft.cursor);
        if (idx < s_craft.list_count) {
            u8 rid = s_craft.list[idx];
            if (craft_can_make(rid)) {
                craft_execute(rid);
                _rebuild_list();
                g_menu.dirty = TRUE;
            }
        }
    }
}

void menu_crafting_render(void)
{
    u8 i;
    ui_draw_window_titled(0, 0, 30, 20, "CRAFTING");

    /* Gold */
    ui_write_str(20, 1, "G:", PAL_TEXT);
    ui_write_num(22, 1, (s32)gold_get(), PAL_HI);

    /* Recipe list */
    for (i = 0; i < CRAFT_VISIBLE_ROWS; i++) {
        u8 idx = (u8)(s_craft.scroll + i);
        u8 row = (u8)(CRAFT_LIST_TOP + i);
        if (idx >= s_craft.list_count) break;
        {
            const CraftRecipe* r = &g_recipe_table[s_craft.list[idx]];
            u8 pal = (i == s_craft.cursor) ? PAL_HI : PAL_TEXT;
            if (i == s_craft.cursor)
                ui_draw_cursor(1, row);
            ui_write_str(3, row, r->name, pal);
            ui_write_str(20, row, _diff_label(r->difficulty),
                         craft_can_make(s_craft.list[idx]) ? PAL_TEXT : PAL_DIM);
        }
    }

    /* Recipe detail if list non-empty */
    if (s_craft.list_count > 0) {
        u8 idx = (u8)(s_craft.scroll + s_craft.cursor);
        if (idx < s_craft.list_count)
            _render_recipe_panel(s_craft.list[idx]);
    }
}
