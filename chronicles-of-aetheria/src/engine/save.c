#include "../../include/engine/save.h"
#include "../../include/gba/gba_reg.h"
#include "../../include/gba/gba_bios.h"

/* =========================================================
 * Save / Load System Implementation
 *
 * SRAM byte-write rule: NEVER use DMA or 32/16-bit writes
 * to SRAM — only 8-bit writes work reliably on real hardware.
 * ========================================================= */

/* SRAM slot offsets */
static const u32 s_slot_offsets[SAVE_SLOT_COUNT] = {
    0x0000,   /* slot 0: 0x0E000000 */
    0x0200,   /* slot 1: 0x0E000200 */
    0x0400,   /* slot 2: 0x0E000400 */
};

/* ---- CRC16 (CCITT) --------------------------------------- */

u16 save_crc16(const void* data, u32 len) {
    const u8* p = (const u8*)data;
    u16 crc = 0xFFFF;
    u32 i, j;
    for (i = 0; i < len; i++) {
        crc ^= (u16)p[i] << 8;
        for (j = 0; j < 8; j++) {
            if (crc & 0x8000) crc = (crc << 1) ^ 0x1021;
            else              crc <<= 1;
        }
    }
    return crc;
}

/* ---- SRAM byte-by-byte I/O ------------------------------ */

static void _sram_write(u32 offset, const void* src, u32 len) {
    const u8* s = (const u8*)src;
    u8* dst     = SRAM_MEM + offset;
    u32 i;
    for (i = 0; i < len; i++) {
        dst[i] = s[i];
    }
}

static void _sram_read(u32 offset, void* dst, u32 len) {
    const u8* src = SRAM_MEM + offset;
    u8* d = (u8*)dst;
    u32 i;
    for (i = 0; i < len; i++) {
        d[i] = src[i];
    }
}

/* ---- Public API ----------------------------------------- */

void save_init(void) {
    /* Nothing to init — SRAM is always live */
}

BOOL save_write(u8 slot, const SaveSlot* data) {
    if (slot >= SAVE_SLOT_COUNT || !data) return FALSE;

    SaveSlot tmp;
    mem_copy(&tmp, data, sizeof(SaveSlot));

    tmp.magic      = SAVE_MAGIC;
    tmp.slot_index = slot;
    tmp.crc        = 0;

    /* CRC covers everything after the crc field itself */
    u32 crc_offset = (u32)(&tmp.slot_index) - (u32)(&tmp);
    tmp.crc = save_crc16(&tmp.slot_index,
                         sizeof(SaveSlot) - crc_offset);

    _sram_write(s_slot_offsets[slot], &tmp, sizeof(SaveSlot));
    return TRUE;
}

BOOL save_read(u8 slot, SaveSlot* out) {
    if (slot >= SAVE_SLOT_COUNT || !out) return FALSE;

    _sram_read(s_slot_offsets[slot], out, sizeof(SaveSlot));

    if (out->magic != SAVE_MAGIC) return FALSE;

    u16 stored_crc = out->crc;
    out->crc       = 0;

    u32 crc_offset = (u32)(&out->slot_index) - (u32)(out);
    u16 calc_crc   = save_crc16(&out->slot_index,
                                 sizeof(SaveSlot) - crc_offset);
    out->crc = stored_crc;

    return (stored_crc == calc_crc);
}

BOOL save_slot_valid(u8 slot) {
    if (slot >= SAVE_SLOT_COUNT) return FALSE;
    SaveSlot tmp;
    return save_read(slot, &tmp);
}

void save_erase(u8 slot) {
    if (slot >= SAVE_SLOT_COUNT) return;
    u8* dst = SRAM_MEM + s_slot_offsets[slot];
    u32 i;
    for (i = 0; i < SAVE_SLOT_SIZE; i++) {
        dst[i] = 0xFF;
    }
}

/* Game-specific: populate from global game state */
void save_capture(u8 slot, SaveSlot* out) {
    (void)slot;
    mem_zero(out, sizeof(SaveSlot));
    /* TODO: fill from g_player, g_world, etc. when those exist */
    out->magic = SAVE_MAGIC;
}

/* Game-specific: restore global game state */
void save_apply(const SaveSlot* slot) {
    (void)slot;
    /* TODO: restore g_player, g_world, etc. */
}
