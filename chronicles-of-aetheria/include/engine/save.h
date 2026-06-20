#ifndef ENGINE_SAVE_H
#define ENGINE_SAVE_H

#include "../gba/gba_types.h"

/* =========================================================
 * Save / Load System — SRAM
 *
 * GBA cartridge SRAM: 32KB at 0x0E000000.
 * SRAM is battery-backed and survives power-off.
 *
 * We store up to 3 save slots. Each slot contains the full
 * game state snapshot. A CRC16 guards data integrity.
 *
 * SRAM write speed: ~2MB/s. For safety, write byte-by-byte
 * (SRAM does not support DMA or 16/32-bit writes).
 *
 * Layout:
 *   0x0E000000  [slot 0]  SaveSlot (512 bytes)
 *   0x0E000200  [slot 1]  SaveSlot
 *   0x0E000400  [slot 2]  SaveSlot
 *   0x0E000600  [magic + meta]
 * ========================================================= */

#define SAVE_SLOT_COUNT    3
#define SAVE_SLOT_SIZE     512    /* bytes per slot */
#define SAVE_MAGIC         0xAE7B /* "Aetheria" marker */

/* Player flags packed into bitfield */
typedef struct {
    u32 quests_started;    /* bitmask: quest 0-31 started */
    u32 quests_complete;   /* bitmask: quest 0-31 complete */
    u32 items_obtained;    /* bitmask: key items 0-31 */
    u32 switches;          /* world switches / flags 0-31 */
} WorldFlags;

/* Player stats snapshot */
typedef struct {
    u16 hp;
    u16 hp_max;
    u16 mp;
    u16 mp_max;
    u16 exp;
    u16 gold;
    u8  level;
    u8  party_size;
    u8  equip[4];          /* equipped item IDs */
} PlayerStats;

/* Location record */
typedef struct {
    u8  map_id;
    u8  spawn_id;
    s16 x, y;              /* pixel position */
} Location;

/* Full save slot */
typedef struct {
    u16        magic;
    u16        crc;        /* CRC16 of remaining bytes */
    u32        play_time;  /* seconds */
    u8         slot_index;
    u8         pad[3];
    Location   location;
    PlayerStats stats;
    WorldFlags flags;
    u8         extra[400]; /* reserved for expansion */
} SaveSlot;

/* Validation */
_Static_assert(sizeof(SaveSlot) <= SAVE_SLOT_SIZE,
               "SaveSlot exceeds SRAM slot budget");

/* ---- Save API ------------------------------------------- */

void save_init(void);

/* Write slot to SRAM; returns TRUE on success */
BOOL save_write(u8 slot, const SaveSlot* data);

/* Read slot from SRAM; returns TRUE if valid CRC */
BOOL save_read(u8 slot, SaveSlot* out);

/* Check if a slot has valid data */
BOOL save_slot_valid(u8 slot);

/* Erase a slot (fill with 0xFF) */
void save_erase(u8 slot);

/* CRC16 utility (also used to verify) */
u16 save_crc16(const void* data, u32 len);

/* Populate a SaveSlot from current game state */
void save_capture(u8 slot, SaveSlot* out);

/* Apply a SaveSlot to current game state */
void save_apply(const SaveSlot* slot);

#endif /* ENGINE_SAVE_H */
