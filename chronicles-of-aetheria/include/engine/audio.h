#ifndef ENGINE_AUDIO_H
#define ENGINE_AUDIO_H

#include "../gba/gba_types.h"

/* =========================================================
 * Audio System — DirectSound (DMA-based)
 *
 * GBA has two DMA audio FIFOs (A and B). We use:
 *   Channel A — BGM (streamed from ROM, looping)
 *   Channel B — SFX (one-shot, short samples)
 *
 * Sample rate: 13312 Hz (timer reload: 65536 - 1024 = 64512)
 * This gives 13312 Hz at 16.78 MHz / 1024 cycles.
 * Higher rates (18157, 22050) are possible with faster timers
 * but consume more ROM bandwidth.
 *
 * Audio data must be 8-bit signed PCM, 4-byte aligned.
 * ========================================================= */

#define AUDIO_SAMPLE_RATE    13312
#define AUDIO_BGM_CHANNELS   1    /* mono BGM */
#define SFX_SLOTS            4    /* concurrent SFX limit */

/* Timer reload value for 13312 Hz:
 *   reload = 65536 - (16777216 / 13312) = 65536 - 1260 = 64276 */
#define AUDIO_TIMER_RELOAD   64276

typedef struct {
    const s8* data;    /* pointer to PCM data in ROM */
    u32       length;  /* in samples */
    BOOL      loop;    /* TRUE = loop forever */
} AudioClip;

typedef struct {
    const AudioClip* clip;
    u32  pos;          /* current playback position */
    u8   volume;       /* 0-255 */
    BOOL active;
} SfxChannel;

/* ---- Audio API ------------------------------------------ */

void audio_init(void);
void audio_shutdown(void);

/* BGM: streamed background music */
void audio_bgm_play(const AudioClip* clip);
void audio_bgm_stop(void);
void audio_bgm_pause(void);
void audio_bgm_resume(void);
void audio_bgm_set_volume(u8 vol);   /* 0-100 */
BOOL audio_bgm_playing(void);

/* SFX: fire-and-forget sound effects */
void audio_sfx_play(const AudioClip* clip);
void audio_sfx_stop_all(void);

/* Master volume 0-100 */
void audio_set_master_volume(u8 vol);

/* Called from VBlank ISR to refill audio FIFO */
void audio_vblank_handler(void) IWRAM_CODE;

/* Called every frame to advance BGM stream position */
void audio_update(void);

#endif /* ENGINE_AUDIO_H */
