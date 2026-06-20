#include "battle_ui.h"
#include "battle_status.h"
#include "battle_data.h"
#include "battle_combo.h"
#include "battler.h"
#include "gba_types.h"
#include "gba_reg.h"
#include "time_weather.h"
#include "engine.h"

BattleUI g_battle_ui;

#define HUD_SB   ((vu16*)(MEM_VRAM + 0xF800))
#define HUD_PAL_NORMAL  0
#define HUD_PAL_YELLOW  1
#define HUD_PAL_RED     2
#define HUD_PAL_GREEN   3
#define HUD_PAL_BLUE    4
#define HUD_PAL_GREY    5

/* ---- Low-level tile writers ----------------------------- */

void ui_write_char(u8 col, u8 row, char c, u8 pal)
{
    u16 idx = (u16)(row * 32 + col);
    HUD_SB[idx] = SE_TID((u16)((u8)c - 32u)) | SE_PAL(pal);
}

void ui_write_str(u8 col, u8 row, const char* str, u8 pal)
{
    while (*str && col < 30) ui_write_char(col++, row, *str++, pal);
}

void ui_write_num(u8 col, u8 row, s16 num, u8 pal)
{
    char buf[6];
    s8   i = 0;
    u16  val;

    if (num < 0) { ui_write_char(col++, row, '-', pal); val = (u16)(-num); }
    else          val = (u16)num;

    if (val == 0) { ui_write_char(col, row, '0', pal); return; }
    while (val && i < 5) { buf[i++] = (char)('0' + val % 10); val /= 10; }
    while (i-- > 0) ui_write_char(col++, row, buf[i], pal);
}

void ui_fill_rect(u8 col, u8 row, u8 w, u8 h, u16 tile)
{
    u8 r, c;
    for (r = row; r < row + h; r++)
        for (c = col; c < col + w; c++)
            HUD_SB[(u16)(r * 32 + c)] = tile;
}

void ui_draw_window(u8 col, u8 row, u8 w, u8 h)
{
    u8 i;
    ui_fill_rect((u8)(col+1), (u8)(row+1), (u8)(w-2), (u8)(h-2),
                 SE_TID(0)|SE_PAL(HUD_PAL_NORMAL));
    for (i = 1; i < w-1; i++) {
        HUD_SB[(u16)(row        * 32 + col + i)] = SE_TID(5)|SE_PAL(HUD_PAL_NORMAL);
        HUD_SB[(u16)((row+h-1)  * 32 + col + i)] = SE_TID(6)|SE_PAL(HUD_PAL_NORMAL);
    }
    for (i = 1; i < h-1; i++) {
        HUD_SB[(u16)((row+i) * 32 + col      )] = SE_TID(7)|SE_PAL(HUD_PAL_NORMAL);
        HUD_SB[(u16)((row+i) * 32 + col + w-1)] = SE_TID(8)|SE_PAL(HUD_PAL_NORMAL);
    }
    HUD_SB[(u16)(row       * 32 + col      )] = SE_TID(1)|SE_PAL(HUD_PAL_NORMAL);
    HUD_SB[(u16)(row       * 32 + col + w-1)] = SE_TID(2)|SE_PAL(HUD_PAL_NORMAL);
    HUD_SB[(u16)((row+h-1) * 32 + col      )] = SE_TID(3)|SE_PAL(HUD_PAL_NORMAL);
    HUD_SB[(u16)((row+h-1) * 32 + col + w-1)] = SE_TID(4)|SE_PAL(HUD_PAL_NORMAL);
}

void ui_draw_bar(u8 col, u8 row, u8 w,
                  s16 current, s16 max, u8 color_pal)
{
    u8 filled, i;
    if (max <= 0) max = 1;
    if (current < 0) current = 0;
    filled = (u8)((u32)current * w / (u32)max);
    for (i = 0; i < w; i++) {
        u16 t = (i < filled) ? (SE_TID(9)|SE_PAL(color_pal)) : (SE_TID(10)|SE_PAL(HUD_PAL_GREY));
        HUD_SB[(u16)(row * 32 + col + i)] = t;
    }
}

/* ---- UI API --------------------------------------------- */

void battle_ui_init(void)
{
    mem_zero(&g_battle_ui, sizeof(g_battle_ui));
    g_battle_ui.state = UI_STATE_HIDDEN;
}

void battle_ui_set_active_char(u8 char_idx, const Battler* b)
{
    u8 i;
    g_battle_ui.active_char   = char_idx;
    g_battle_ui.cmd_cursor    = 0;
    g_battle_ui.skill_cursor  = 0;
    g_battle_ui.item_cursor   = 0;
    g_battle_ui.combo_cursor  = 0;
    g_battle_ui.target_cursor = 0;
    g_battle_ui.targeting_enemy = TRUE;

    g_battle_ui.skill_count = b->skill_count < BATTLE_SKILL_MAX ? b->skill_count : BATTLE_SKILL_MAX;
    for (i = 0; i < g_battle_ui.skill_count; i++)
        g_battle_ui.skill_ids[i] = b->skill_ids[i];
}

static void _nav(u8* cursor, u8 count, BOOL up)
{
    if (!count) return;
    if (up) { if (*cursor == 0) *cursor = (u8)(count-1); else (*cursor)--; }
    else    { (*cursor)++; if (*cursor >= count) *cursor = 0; }
}

BOOL battle_ui_update(Battler battlers[BATTLE_BATTLER_MAX],
                       BattleAction* out_action)
{
    BOOL up   = (BOOL)(g_input.pressed & KEY_UP);
    BOOL down = (BOOL)(g_input.pressed & KEY_DOWN);
    BOOL ok   = (BOOL)(g_input.pressed & KEY_A);
    BOOL back = (BOOL)(g_input.pressed & KEY_B);

    mem_zero(out_action, sizeof(*out_action));
    out_action->actor = g_battle_ui.active_char;

    if (g_battle_ui.state == UI_STATE_MESSAGE) {
        if (g_battle_ui.msg_timer > 0) g_battle_ui.msg_timer--;
        if (!g_battle_ui.msg_timer || ok)
            g_battle_ui.state = UI_STATE_CMD_SELECT;
        return FALSE;
    }

    switch (g_battle_ui.state) {
    case UI_STATE_CMD_SELECT:
        if (up)   _nav(&g_battle_ui.cmd_cursor, CMD_COUNT, TRUE);
        if (down) _nav(&g_battle_ui.cmd_cursor, CMD_COUNT, FALSE);
        if (ok) {
            switch (g_battle_ui.cmd_cursor) {
            case CMD_ATTACK:
                g_battle_ui.state           = UI_STATE_TARGET_SELECT;
                g_battle_ui.targeting_enemy = TRUE;
                {
                    u8 i;
                    for (i = 0; i < BATTLE_ENEMY_MAX; i++)
                        if (battlers[ENEMY_IDX(i)].alive) { g_battle_ui.target_cursor = i; break; }
                }
                break;
            case CMD_MAGIC:
                if (g_battle_ui.skill_count) g_battle_ui.state = UI_STATE_SKILL_SELECT;
                break;
            case CMD_ITEM:
                g_battle_ui.state = UI_STATE_ITEM_SELECT;
                break;
            case CMD_DEFEND:
                out_action->type        = ACTION_DEFEND;
                out_action->target_mask = (u16)TARGET_SELF(g_battle_ui.active_char);
                out_action->valid       = TRUE;
                return TRUE;
            case CMD_COMBO:
                g_battle_ui.combo_count = combo_get_available(
                    battlers, g_battle_ui.active_char,
                    g_battle_ui.combo_ids, BATTLE_COMBO_MAX);
                if (g_battle_ui.combo_count) g_battle_ui.state = UI_STATE_COMBO_SELECT;
                break;
            case CMD_FLEE:
                out_action->type  = ACTION_FLEE;
                out_action->valid = TRUE;
                return TRUE;
            }
        }
        break;

    case UI_STATE_SKILL_SELECT:
        if (back) { g_battle_ui.state = UI_STATE_CMD_SELECT; break; }
        if (up)   _nav(&g_battle_ui.skill_cursor, g_battle_ui.skill_count, TRUE);
        if (down) _nav(&g_battle_ui.skill_cursor, g_battle_ui.skill_count, FALSE);
        if (ok) {
            u8 sk_id = g_battle_ui.skill_ids[g_battle_ui.skill_cursor];
            const SkillDef* sk = skill_get(sk_id);
            if (sk->target == TGT_SINGLE_ENEMY || sk->target == TGT_RANDOM_ENEMY) {
                g_battle_ui.state           = UI_STATE_TARGET_SELECT;
                g_battle_ui.targeting_enemy = TRUE;
                g_battle_ui.target_cursor   = 0;
                g_battle_ui.skill_ids[BATTLE_SKILL_MAX-1] = sk_id;
            } else if (sk->target == TGT_SINGLE_ALLY) {
                g_battle_ui.state           = UI_STATE_TARGET_SELECT;
                g_battle_ui.targeting_enemy = FALSE;
                g_battle_ui.target_cursor   = g_battle_ui.active_char;
                g_battle_ui.skill_ids[BATTLE_SKILL_MAX-1] = sk_id;
            } else {
                u16 mask = (sk->target == TGT_ALL_ENEMIES) ? TARGET_ALL_ENEMIES :
                           (sk->target == TGT_ALL_ALLIES)  ? TARGET_ALL_ALLIES :
                           (sk->target == TGT_ALL)         ? TARGET_ALL :
                           (u16)TARGET_SELF(g_battle_ui.active_char);
                out_action->type        = ACTION_MAGIC;
                out_action->skill_id    = sk_id;
                out_action->target_mask = mask;
                out_action->valid       = TRUE;
                return TRUE;
            }
        }
        break;

    case UI_STATE_COMBO_SELECT:
        if (back) { g_battle_ui.state = UI_STATE_CMD_SELECT; break; }
        if (up)   _nav(&g_battle_ui.combo_cursor, g_battle_ui.combo_count, TRUE);
        if (down) _nav(&g_battle_ui.combo_cursor, g_battle_ui.combo_count, FALSE);
        if (ok) {
            u8 cid = g_battle_ui.combo_ids[g_battle_ui.combo_cursor];
            const ComboDef* cd = combo_get(cid);
            if (cd->target == TGT_SINGLE_ENEMY) {
                g_battle_ui.state           = UI_STATE_TARGET_SELECT;
                g_battle_ui.targeting_enemy = TRUE;
                g_battle_ui.target_cursor   = 0;
                g_battle_ui.combo_ids[BATTLE_COMBO_MAX-1] = cid;
            } else {
                out_action->type        = ACTION_COMBO;
                out_action->combo_id    = cid;
                out_action->target_mask = TARGET_ALL_ENEMIES;
                out_action->valid       = TRUE;
                return TRUE;
            }
        }
        break;

    case UI_STATE_TARGET_SELECT: {
        u8 max_tgt = g_battle_ui.targeting_enemy ? BATTLE_ENEMY_MAX : BATTLE_ALLY_MAX;
        if (back) { g_battle_ui.state = UI_STATE_CMD_SELECT; break; }
        if (up || down) {
            u8 dir = down ? 1u : (u8)(max_tgt - 1u);
            u8 i;
            for (i = 0; i < max_tgt; i++) {
                g_battle_ui.target_cursor = (u8)((g_battle_ui.target_cursor + dir) % max_tgt);
                u8 bidx = g_battle_ui.targeting_enemy ?
                    ENEMY_IDX(g_battle_ui.target_cursor) : g_battle_ui.target_cursor;
                if (battlers[bidx].alive) break;
            }
        }
        if (ok) {
            u8 bidx = g_battle_ui.targeting_enemy ?
                ENEMY_IDX(g_battle_ui.target_cursor) : g_battle_ui.target_cursor;

            if (g_battle_ui.cmd_cursor == CMD_ATTACK) {
                out_action->type        = ACTION_ATTACK;
                out_action->skill_id    = SKILL_ATTACK;
                out_action->target_mask = (u16)(1u << bidx);
                out_action->valid       = TRUE;
                return TRUE;
            } else if (g_battle_ui.cmd_cursor == CMD_MAGIC) {
                out_action->type        = ACTION_MAGIC;
                out_action->skill_id    = g_battle_ui.skill_ids[BATTLE_SKILL_MAX-1];
                out_action->target_mask = (u16)(1u << bidx);
                out_action->valid       = TRUE;
                return TRUE;
            } else if (g_battle_ui.cmd_cursor == CMD_COMBO) {
                out_action->type        = ACTION_COMBO;
                out_action->combo_id    = g_battle_ui.combo_ids[BATTLE_COMBO_MAX-1];
                out_action->target_mask = (u16)(1u << bidx);
                out_action->valid       = TRUE;
                return TRUE;
            }
        }
        break;
    }

    default: break;
    }

    return FALSE;
}

void battle_ui_render(const Battler battlers[BATTLE_BATTLER_MAX])
{
    if (g_battle_ui.state == UI_STATE_HIDDEN) return;

    battle_ui_draw_hp_bars(battlers);
    battle_ui_draw_enemies(battlers);

    switch (g_battle_ui.state) {
    case UI_STATE_CMD_SELECT: {
        u8 i;
        ui_draw_window(20, 12, 10, 8);
        for (i = 0; i < CMD_COUNT; i++) {
            u8 pal = (i == g_battle_ui.cmd_cursor) ? HUD_PAL_YELLOW : HUD_PAL_NORMAL;
            ui_write_str(21, (u8)(13+i), CMD_LABELS[i], pal);
        }
        break;
    }
    case UI_STATE_SKILL_SELECT: {
        u8 i;
        ui_draw_window(0, 12, 15, 8);
        for (i = 0; i < g_battle_ui.skill_count; i++) {
            u8 pal = (i == g_battle_ui.skill_cursor) ? HUD_PAL_YELLOW : HUD_PAL_NORMAL;
            ui_write_str(1, (u8)(13+i), skill_get(g_battle_ui.skill_ids[i])->name, pal);
            ui_write_num(10, (u8)(13+i), skill_get(g_battle_ui.skill_ids[i])->mp_cost, HUD_PAL_BLUE);
        }
        break;
    }
    case UI_STATE_COMBO_SELECT: {
        u8 i;
        ui_draw_window(0, 12, 15, 8);
        for (i = 0; i < g_battle_ui.combo_count; i++) {
            u8 pal = (i == g_battle_ui.combo_cursor) ? HUD_PAL_YELLOW : HUD_PAL_NORMAL;
            ui_write_str(1, (u8)(13+i), combo_get(g_battle_ui.combo_ids[i])->name, pal);
        }
        break;
    }
    case UI_STATE_MESSAGE:
        ui_draw_window(0, 14, 30, 4);
        ui_write_str(1, 15, g_battle_ui.msg_buf, HUD_PAL_NORMAL);
        break;
    default: break;
    }
}

void battle_ui_show_cmd(void)  { g_battle_ui.state = UI_STATE_CMD_SELECT; }
void battle_ui_hide(void)
{
    g_battle_ui.state = UI_STATE_HIDDEN;
    ui_fill_rect(0, 0, 30, 20, SE_TID(0)|SE_PAL(0));
}

void battle_ui_message(const char* msg)
{
    u8 i = 0;
    while (msg[i] && i < 63) { g_battle_ui.msg_buf[i] = msg[i]; i++; }
    g_battle_ui.msg_buf[i] = '\0';
    g_battle_ui.msg_timer  = 90;
    g_battle_ui.state      = UI_STATE_MESSAGE;
}

void battle_ui_message_clear(void)
{
    g_battle_ui.msg_timer = 0;
    g_battle_ui.state     = UI_STATE_CMD_SELECT;
}

void battle_ui_set_turn_order(const TurnSlot* order, u8 count)
{
    u8 i;
    g_battle_ui.turn_count = count < BATTLE_BATTLER_MAX ? count : BATTLE_BATTLER_MAX;
    for (i = 0; i < g_battle_ui.turn_count; i++)
        g_battle_ui.turn_order[i] = order[i].battler_idx;
}

void battle_ui_draw_hp_bars(const Battler battlers[BATTLE_BATTLER_MAX])
{
    u8 i;
    for (i = 0; i < BATTLE_ALLY_MAX; i++) {
        const Battler* b = &battlers[i];
        u8 row = (u8)(17 + i);
        u8 hp_pal = (b->hp * 4 / (b->hp_max ? b->hp_max : 1)) >= 2 ? HUD_PAL_GREEN :
                    (b->hp * 4 / (b->hp_max ? b->hp_max : 1)) >= 1 ? HUD_PAL_YELLOW : HUD_PAL_RED;
        ui_write_str(0, row, b->name, HUD_PAL_NORMAL);
        ui_draw_bar(7,  row, 8, b->hp, b->hp_max, hp_pal);
        ui_write_num(16, row, b->hp, HUD_PAL_NORMAL);
        ui_draw_bar(7,  (u8)(row), 8, b->mp, b->mp_max, HUD_PAL_BLUE);
    }
}

void battle_ui_draw_enemies(const Battler battlers[BATTLE_BATTLER_MAX])
{
    u8 i;
    ui_fill_rect(0, 0, 15, 6, SE_TID(0)|SE_PAL(0));
    for (i = 0; i < BATTLE_ENEMY_MAX; i++) {
        const Battler* b = &battlers[ENEMY_IDX(i)];
        if (!b->alive) continue;
        {
            u8 row = (u8)(i < 3 ? i : i - 3);
            u8 col = (u8)(i < 3 ? 0 : 8);
            ui_write_str(col, row, b->name, HUD_PAL_NORMAL);
            ui_draw_bar((u8)(col+7), row, 6, b->hp, b->hp_max, HUD_PAL_RED);
        }
    }
}

void battle_ui_draw_status(const Battler* b) { (void)b; }

void battle_ui_highlight_target(u8 battler_idx, BOOL on)
{
    (void)battler_idx; (void)on;
}

void battle_ui_mark_active_turn(u8 battler_idx)
{
    g_battle_ui.current_turn = battler_idx;
}
