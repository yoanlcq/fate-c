#include <fate/fate.h>
#include <SDL2/SDL.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "silence.c.array"
static size_t pos = 0;
static const uint8_t *mus = silence_raw;
#define mus_len silence_raw_len
static SDL_AudioDeviceID dev = 0;
static const char *TAG = "bgmus";

static void callback(void *udata, uint8_t *stream, int32_t len) {
    memset(stream, 0, len);
    if(len > mus_len-pos)
        len = mus_len-pos;
    SDL_MixAudioFormat(stream, mus+pos, AUDIO_S16SYS, len, SDL_MIX_MAXVOLUME);
    pos += len;
    if(pos >= mus_len)
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
}
void bgmus_play(void) {
    SDL_PauseAudioDevice(dev, 0);
}
void bgmus_pause(void) {
    SDL_PauseAudioDevice(dev, 1);
}
void bgmus_cleanup(void) {
    SDL_CloseAudioDevice(dev);
}

