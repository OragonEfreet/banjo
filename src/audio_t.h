#pragma once

#include <banjo/audio.h>

#define BJ_AUDIO_AMPLITUDE 16000
#define BJ_AUDIO_SAMPLE_RATE 44100
#define BJ_AUDIO_CHANNELS 1

typedef struct bj_audio_device_data_t bj_audio_device_data;

struct bj_audio_device_t {
    bj_audio_properties   properties;
    uint32_t              silence;
    bj_bool               playing;
    volatile bj_bool      should_reset;
    bj_bool               should_close;
    bj_audio_callback_t   p_callback;
    void*                 p_callback_user_data;
    bj_audio_device_data* data;
};


