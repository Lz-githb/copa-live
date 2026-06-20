#include "../../include/engine/audio.h"
#include "../../include/gba/gba_reg.h"
#include "../../include/gba/gba_dma.h"
#include "../../include/gba/gba_bios.h"

/* =========================================================
 * Audio System Implementation
 *
 * We use a double-buffer approach for BGM:
 *   Two buffers of BUFFER_SIZE samples alternate.
 *   Timer 0 triggers DMA1 to feed FIFO A at sample rate.
 *   VBlank ISR calls audio_vblank_handler() to decode/copy
 *   the next block into the inactive buffer.
 * ========================================================= */

#define BUFFER_SIZE  176   /* samples per buffer (~13ms at 13312Hz) */

/* Double-buffer in IWRAM for DMA speed */
static s8 IWRAM_DATA s_buf_a[BUFFER_SIZE] ALIGN4;
static s8 IWRAM_DATA s_buf_b[BUFFER_SIZE] ALIGN4;

typedef struct {
    const AudioClip* clip;
    u32              pos;         /* current sample offset */
    BOOL             playing;
    BOOL             paused;
    u8               volume;      /* 0-100 */
    u8               active_buf;  /* 0 = buf_a is playing, 1 = buf_b */
} BgmState;

static BgmState    s_bgm;
static u8          s_master_vol = 100;

/* ---- Buffer fill ---------------------------------------- */

/* Fill one buffer with BGM samples (or silence) */
static void _fill_buffer(s8* buf) {
    u32 i;
    if (!s_bgm.playing || s_bgm.paused || !s_bgm.clip) {
        mem_zero(buf, BUFFER_SIZE);
        return;
    }

    const s8* src = s_bgm.clip->data;
    u32 len       = s_bgm.clip->length;
    u32 pos       = s_bgm.pos;

    for (i = 0; i < BUFFER_SIZE; i++) {
        if (pos >= len) {
            if (s_bgm.clip->loop) {
                pos = 0;
            } else {
                buf[i] = 0;
                continue;
            }
        }
        /* Scale by volume */
        s32 sample = (s32)src[pos++];
        sample = (sample * s_bgm.volume) / 100;
        buf[i] = (s8)sample;
    }

    s_bgm.pos = pos;
    if (pos >= len && !s_bgm.clip->loop) {
        s_bgm.playing = FALSE;
    }
}

/* ---- Hardware setup ------------------------------------- */

static void _start_dma_fifo(void) {
    /* DMA1 feeds FIFO A from active buffer */
    s8* src = (s_bgm.active_buf == 0) ? s_buf_a : s_buf_b;
    dma1_audio_fifo_a(src);

    /* Timer 0: drives DMA1 at sample rate */
    REG_TM0CNT_H = 0;
    REG_TM0CNT_L = AUDIO_TIMER_RELOAD;
    REG_TM0CNT_H = TM_ENABLE | TM_FREQ_1;
}

/* ---- Public API ----------------------------------------- */

void audio_init(void) {
    /* Enable sound master */
    REG_SOUNDCNT_X = SNDCNT_X_MASTER_ENABLE;

    /* Route FIFO A left+right, full volume, timer 0 */
    REG_SOUNDCNT_H = SNDCNT_H_DMG_VOL50 |
                     SNDCNT_H_DSSA_VOL100 |
                     SNDCNT_H_DSSA_L |
                     SNDCNT_H_DSSA_R |
                     SNDCNT_H_DSSA_TM0 |
                     SNDCNT_H_DSSA_RESET;

    REG_SOUNDBIAS = 0x0200;   /* recommended bias */

    mem_zero(&s_bgm, sizeof(s_bgm));
    mem_zero(s_buf_a, BUFFER_SIZE);
    mem_zero(s_buf_b, BUFFER_SIZE);
    s_bgm.volume = 100;
}

void audio_shutdown(void) {
    dma1_stop();
    REG_TM0CNT_H   = 0;
    REG_SOUNDCNT_X = 0;
}

void audio_bgm_play(const AudioClip* clip) {
    if (!clip) return;
    dma1_stop();
    REG_TM0CNT_H = 0;

    s_bgm.clip       = clip;
    s_bgm.pos        = 0;
    s_bgm.playing    = TRUE;
    s_bgm.paused     = FALSE;
    s_bgm.active_buf = 0;

    /* Pre-fill both buffers */
    _fill_buffer(s_buf_a);
    _fill_buffer(s_buf_b);

    /* Reset FIFO */
    REG_SOUNDCNT_H |= SNDCNT_H_DSSA_RESET;

    _start_dma_fifo();
}

void audio_bgm_stop(void) {
    dma1_stop();
    REG_TM0CNT_H = 0;
    s_bgm.playing = FALSE;
    s_bgm.clip    = NULL;
    mem_zero(s_buf_a, BUFFER_SIZE);
    mem_zero(s_buf_b, BUFFER_SIZE);
}

void audio_bgm_pause(void) {
    s_bgm.paused = TRUE;
}

void audio_bgm_resume(void) {
    s_bgm.paused = FALSE;
}

void audio_bgm_set_volume(u8 vol) {
    s_bgm.volume = CLAMP(vol, 0, 100);
}

BOOL audio_bgm_playing(void) {
    return s_bgm.playing && !s_bgm.paused;
}

void audio_sfx_play(const AudioClip* clip) {
    /* Simplified: play one-shot SFX on FIFO B (DMA2) */
    if (!clip || !clip->data) return;
    dma2_stop();
    REG_SOUNDCNT_H |= SNDCNT_H_DSSB_RESET |
                      SNDCNT_H_DSSB_VOL100 |
                      SNDCNT_H_DSSB_L |
                      SNDCNT_H_DSSB_R |
                      SNDCNT_H_DSSB_TM0;
    dma2_audio_fifo_b(clip->data);
}

void audio_sfx_stop_all(void) {
    dma2_stop();
}

void audio_set_master_volume(u8 vol) {
    s_master_vol = CLAMP(vol, 0, 100);
}

/* Called from VBlank ISR — must be fast (in IWRAM) */
void IWRAM_CODE audio_vblank_handler(void) {
    if (!s_bgm.playing) return;

    /* Swap buffers: fill the one NOT currently being DMA'd */
    s_bgm.active_buf ^= 1;
    s8* fill_buf = (s_bgm.active_buf == 0) ? s_buf_b : s_buf_a;
    _fill_buffer(fill_buf);

    /* Redirect DMA1 to the now-active buffer */
    dma1_stop();
    s8* dma_buf = (s_bgm.active_buf == 0) ? s_buf_a : s_buf_b;
    dma1_audio_fifo_a(dma_buf);
}

void audio_update(void) {
    /* Future: crossfade, fade-in/fade-out logic per frame */
}
