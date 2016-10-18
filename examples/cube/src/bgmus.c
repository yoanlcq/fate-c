#include <fate/fate.h>
#include <SDL2/SDL.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "res.h"

static size_t pos = 0;
static const fe_iov mus;
static SDL_AudioDeviceID dev = 0;
static const char *TAG = "bgmus";

static void callback(void *udata, uint8_t *stream, int32_t len) {
    memset(stream, 0, len);
    if(len > mus.len-pos)
        len = mus.len-pos;
    SDL_MixAudioFormat(stream, mus.base+pos, AUDIO_S16SYS, len, SDL_MIX_MAXVOLUME);
    pos += len;
    if(pos >= mus.len)
        pos = 0;
}

void bgmus_setup(void) {
    SDL_AudioSpec want, have;
    want.freq = 44100;
    want.format = AUDIO_S16SYS;
    want.channels = 2;
    want.samples = 4096;
    want.callback = callback;
    dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if(!dev)
        fe_loge(TAG, "Failed to open audio: %s\n", SDL_GetError());
    // Load file...
    int channels=2, sample_rate=44100;
    int16_t *samples;
    fe_iov iov;
    res_load(&iov, res.musics.deeper);
    int sample_count = stb_vorbis_decode_memory(iov.base, iov.len, &channels, &sample_rate, &samples);
    fe_iov_deinit(&iov);
    fe_dbg_hope(channels == 2);
    fe_dbg_hope(sample_rate == 44100);
    mus.base = samples;
    mus.len = sample_count;
}
void bgmus_play(void) {
    SDL_PauseAudioDevice(dev, 0);
}
void bgmus_pause(void) {
    SDL_PauseAudioDevice(dev, 1);
}
void bgmus_cleanup(void) {
    SDL_CloseAudioDevice(dev);
    fe_iov_deinit(mus);
}

