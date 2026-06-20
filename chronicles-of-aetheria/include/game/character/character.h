#ifndef CHARACTER_H
#define CHARACTER_H

#include "char_types.h"
#include "char_stats.h"
#include "char_class.h"
#include "char_equip.h"
#include "char_skill.h"
#include "char_talent.h"
#include "char_xp.h"

/* =========================================================
 * Master Character Struct
 *
 * Holds all runtime state for one party member.
 * Designed to be save/load compatible (fixed-size struct).
 *
 * Memory: ~256 bytes per character, 4 characters = 1KB
 * Lives in EWRAM persistent pool.
 * ========================================================= */

#define CHAR_NAME_MAX   12

typedef struct Character {
    /* Identity */
    char          name[CHAR_NAME_MAX];
    CharClass     cls;
    u8            party_slot;         /* 0-3 */

    /* Level & XP */
    u8            level;              /* 1-99 */
    u32           xp_total;          /* total XP accumulated */

    /* Base stats (raw attributes) */
    BaseStats     base;

    /* Derived stats (recomputed when stats/equip changes) */
    DerivedStats  derived;

    /* Current HP/MP (can be below max) */
    s16           hp_cur;
    s16           mp_cur;

    /* Equipment */
    u8            equip[EQUIP_SLOT_COUNT]; /* item_id per slot, EQUIP_NONE=0 */
    InventorySlot inventory[INVENTORY_SIZE];
    u8            inventory_count;

    /* Skills */
    u8            known_skills[CHAR_KNOWN_MAX]; /* char_skill_ids */
    u8            known_count;
    u8            hotbar[CHAR_SKILL_SLOTS];     /* active skill slots */

    /* Talent tree */
    TalentState   talents;

    /* Persistent battle status (out-of-combat) */
    u8            persist_status;  /* CharPersistStatus bitmask */

    /* Class-unique mechanic state */
    InfusionType  infusion;        /* Arcsword: current element */
    u8            infusion_turns;  /* Arcsword: turns remaining */
    BOOL          spellcharge;     /* Arcanist: charge active */
    u8            taunt_active;    /* Guardian: taunt turns left */

    /* Battle integration (populated on battle_begin) */
    /* These mirror into Battler struct at battle start */
    u8            battle_sprite_id;
    u8            battle_palette_id;

    /* Step counter for poison damage */
    u16           step_count;

    /* Padding to align to 4 bytes */
    u8            _pad[2];

} Character;

/* ---- Global party --------------------------------------- */
extern Character g_party[PARTY_SIZE];
extern u8        g_party_size;

/* ---- System API ----------------------------------------- */

/* Initialize entire character system */
void character_system_init(void);

/* Create a new character (fills struct, level 1) */
void character_create(Character* ch, const char* name, CharClass cls,
                      u8 party_slot);

/* Recompute derived stats (call after any stat change) */
void character_recompute(Character* ch);

/* Full restore (HP/MP to max, cure all persist status) */
void character_full_restore(Character* ch);

/* Partially restore HP/MP (e.g., at campfire) */
void character_rest(Character* ch, u8 hp_pct, u8 mp_pct);

/* Apply out-of-battle damage (e.g., poison step) */
void character_apply_field_damage(Character* ch, s16 dmg);

/* Check if character is KO'd */
BOOL character_is_ko(const Character* ch);

/* Called each map step (for poison, step counters) */
void character_on_step(Character* ch);

/* Serialize character to save buffer (512 bytes max) */
u16 character_serialize(const Character* ch, u8* buf, u16 buf_size);

/* Deserialize character from save buffer */
BOOL character_deserialize(Character* ch, const u8* buf, u16 buf_size);

/* ---- Battle integration --------------------------------- */

/* Populate a Battler struct from this character */
void character_to_battler(const Character* ch, struct Battler* out);

/* Apply battle results back to character (HP/MP/XP changes) */
void character_from_battle_result(Character* ch,
                                   s16 hp_delta,
                                   s16 mp_delta,
                                   u32 xp_gained);

/* ---- Utility -------------------------------------------- */

const char*  character_class_name(const Character* ch);
s16          character_hp_pct(const Character* ch);   /* 0-100 */
s16          character_mp_pct(const Character* ch);
BOOL         character_can_act(const Character* ch);  /* not KO/stone */

#endif /* CHARACTER_H */
