#include <fate/fate.h>
#include <SDL2/SDL.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "res.h"

static size_t pos = 0;
static fe_iov mus;
static SDL_AudioDeviceID dev = 0;
static const char *TAG = "bgmus";

static void callback(void *udata, uint8_t *stream, int32_t len) {
    memset(stream, 0, len);
    if(len > mus.len-pos)
        len = mus.len-pos;
    SDL_MixAudioFormat(stream, (void*)mus.base+pos, AUDIO_S16SYS, len, SDL_MIX_MAXVOLUME);
    pos += len;
    if(pos >= mus.len)
        pos = 0;
}


#ifndef FE_TARGET_EMSCRIPTEN
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
    int16_t *frames;
    fe_iov iov = {0};
    res_load(&iov, res.musics.wind.ogg);
    int frame_count = stb_vorbis_decode_memory((void*)iov.base, iov.len, &channels, &sample_rate, &frames);
    fe_iov_deinit(&iov);
    if(!(channels == 2 && sample_rate == 44100)) {
        fe_logi(TAG, "Source ogg is being converted. channels=%d, freq=%d\n", channels, sample_rate);
        SDL_AudioCVT cvt;
        SDL_BuildAudioCVT(&cvt, AUDIO_S16, channels, sample_rate, AUDIO_S16, 2, 44100);
        fe_dbg_assert(cvt.needed);
        if(frame_count*channels*sizeof(int16_t) < cvt.len*cvt.len_mult) {
            fe_logi(TAG, "Reallocating buffer.\n");
            frames = fe_mem_heaprealloc(frames, cvt.len*cvt.len_mult, uint8_t, "");
        }
        cvt.len = frame_count * channels * sizeof(int16_t);
        cvt.buf = (void*)frames;
        SDL_ConvertAudio(&cvt);
        mus.len = cvt.len_cvt;
        mus.base = (void*)cvt.buf;
    } else {
        fe_logi(TAG, "Source ogg matched our specs!\n");
        mus.len = frame_count*2*sizeof(int16_t); // *2 because stereo
        mus.base = (void*)frames;
    }
}
void bgmus_play(void) {
    SDL_PauseAudioDevice(dev, 0);
}
void bgmus_pause(void) {
    SDL_PauseAudioDevice(dev, 1);
}
void bgmus_cleanup(void) {
    SDL_CloseAudioDevice(dev);
    fe_iov_deinit(&mus);
}
#else
void bgmus_setup(void) {}
void bgmus_pause(void) {}
void bgmus_play(void) {}
void bgmus_cleanup(void) {}
#endif
