/* =========================================================
 * Chronicles of Aetheria — Entry Point & Engine Core
 *
 * Execution flow:
 *   main()
 *     engine_init()        — hardware + subsystems
 *     _game_systems_init() — all game subsystems in order
 *     state_change(TITLE)  — queue initial state
 *     loop:
 *       VBlankIntrWait()   — sync to 60 Hz
 *       engine_tick()      — state update + render + events
 * ========================================================= */

#include "../include/engine/engine.h"
#include "../include/game/game_states.h"
#include "../include/game/character/character.h"
#include "../include/game/item/inventory.h"
#include "../include/game/item/crafting.h"
#include "../include/game/item/shop.h"
#include "../include/game/quest/quest_state.h"
#include "../include/game/quest/quest_chain.h"
#include "../include/game/ui/menu_types.h"
#include "../include/game/character/char_equip.h"
#include "../include/game/character/char_types.h"
#include "../include/game/item/item_db.h"

/* Forward declarations from data modules */
void map_data_init(void);
void map_data_ext_init(void);
void monster_db_init(void);
void boss_db_init(void);
void dialogue_db_init(void);
void battle_data_init(void);
void gfx_ui_load(void);
void gfx_world_load(void);
void gfx_sprites_load_all(void);
void polish_init(void);

/* ---- Global engine variables ----------------------------- */
InputState g_input  = {0, 0, 0};
u32        g_frame  = 0;

/* ---- Play-time tracking (Timer 0 cascade) ---------------- */
static u32 s_play_seconds = 0;
static u16 s_tm0_prev     = 0;

u32 game_play_seconds(void) { return s_play_seconds; }

static void _tick_play_time(void)
{
    /* Timer 0 configured as 1-second overflow counter below */
    u16 now = REG_TM0CNT_L;
    if (now < s_tm0_prev) s_play_seconds++;
    s_tm0_prev = now;
}

/* ---- VBlank ISR (IWRAM for speed) ----------------------- */
void IWRAM_CODE engine_vblank(void)
{
    sprite_flush();           /* OAM shadow → hardware */
    audio_vblank_handler();   /* refill audio DMA buffer */
    input_update();           /* snapshot key state */
}

/* IRQ dispatch (all sources routed here by BIOS) */
void engine_isr(void)
{
    u16 fired = REG_IF & REG_IE;
    if (fired & IRQ_VBLANK) {
        engine_vblank();
        REG_IF = IRQ_VBLANK;
    }
    REG_IF = fired;
}

/* ---- Engine Init ---------------------------------------- */
void engine_init(void)
{
    REG_DISPCNT = DCNT_BLANK;   /* blank during init */
    REG_IME     = 0;

    mem_init();
    event_init();
    tile_init();
    sprite_init();
    audio_init();
    save_init();
    scene_init();

    /* BG layout:
     *   BG0 prio 0 — UI overlay (CB0 SB31)
     *   BG1 prio 1 — foreground / tree tops (CB0 SB30)
     *   BG2 prio 2 — main world layer (CB1 SB29)
     *   BG3 prio 3 — background sky (CB1 SB28)      */
    REG_BG0CNT = BG_CNT(0, 0, 0, 0, 31, 0, BG_SIZE_256x256);
    REG_BG1CNT = BG_CNT(1, 0, 0, 0, 30, 0, BG_SIZE_256x256);
    REG_BG2CNT = BG_CNT(2, 1, 0, 0, 29, 1, BG_SIZE_256x256);
    REG_BG3CNT = BG_CNT(3, 1, 0, 0, 28, 0, BG_SIZE_256x256);

    /* Play-time: Timer 0 at 1024 prescaler counts up ~16384/s;
       overflow occurs every ~4 seconds — we track seconds via
       the raw counter instead for simplicity.              */
    REG_TM0CNT_L = 0;
    REG_TM0CNT_H = TM_FREQ_1024 | TM_ENABLE;

    IRQ_HANDLER  = engine_isr;
    REG_DISPSTAT = DSTAT_VBL_IRQ;
    REG_IE       = IRQ_VBLANK;
    REG_IME      = 1;

    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2
                | DCNT_BG3  | DCNT_OBJ  | DCNT_OBJ_MAP_1D;
}

/* ---- Engine Tick ---------------------------------------- */
void engine_tick(void)
{
    state_tick();           /* update + render active state */
    event_dispatch_all();   /* flush queued game events */
    mem_frame_reset();      /* reclaim frame-scope allocations */
    g_frame++;
}

/* ---- New Game ------------------------------------------- */
static void _new_game(void)
{
    /* Party */
    character_system_init();
    character_create(&g_party[0], "Kael",   CLASS_ARCSWORD, 0);
    character_create(&g_party[1], "Mira",   CLASS_ARCANIST, 1);
    character_create(&g_party[2], "Theron", CLASS_GUARDIAN, 2);
    character_create(&g_party[3], "Syla",   CLASS_HUNTER,   3);
    g_party_size = 4;

    /* Starting bag */
    inventory_system_init();
    bag_add(IITEM_IRON_SWORD,    1);
    bag_add(IITEM_WOODEN_STAFF,  1);
    bag_add(IITEM_IRON_SHIELD,   1);
    bag_add(IITEM_BONE_DAGGER,   1);
    bag_add(ITEM_POTION,         5);
    bag_add(ITEM_ETHER,          3);
    gold_add(500);

    /* Starting equipment */
    g_party[0].equip[EQUIP_SLOT_WEAPON]  = IITEM_IRON_SWORD;
    g_party[1].equip[EQUIP_SLOT_WEAPON]  = IITEM_WOODEN_STAFF;
    g_party[2].equip[EQUIP_SLOT_WEAPON]  = IITEM_IRON_SWORD;
    g_party[2].equip[EQUIP_SLOT_OFFHAND] = IITEM_IRON_SHIELD;
    g_party[3].equip[EQUIP_SLOT_WEAPON]  = IITEM_BONE_DAGGER;

    {
        u8 i;
        for (i = 0; i < g_party_size; i++)
            character_recompute(&g_party[i]);
    }

    /* Quest / chain system */
    quest_system_init();
    chain_init();

    /* UI + world graphics */
    gfx_ui_load();
    gfx_world_load();
    gfx_sprites_load_all();

    /* Polish effects system */
    polish_init();

    menu_init();
}

/* ---- All game systems init (called once from main) ------- */
static void _game_systems_init(void)
{
    /* ROM data tables */
    battle_data_init();
    monster_db_init();
    boss_db_init();
    equip_system_init();
    crafting_init();
    dialogue_db_init();
    map_data_init();
    map_data_ext_init();
    npc_system_init();

    /* Game states */
    game_register_states();

    /* Try to load save slot 0 */
    {
        SaveSlot slot;
        if (save_read(0, &slot)) {
            save_apply(&slot);
            /* Re-register event listeners (not persisted) */
            chain_init();
            menu_init();
        } else {
            _new_game();
        }
    }
}

/* ---- Entry Point ---------------------------------------- */
int main(void)
{
    engine_init();
    _game_systems_init();
    state_change(STATE_TITLE);

    for (;;) {
        VBlankIntrWait();
        _tick_play_time();
        polish_update();
        engine_tick();
    }

    return 0;
}
