#pragma once

#include <banjo/audio.h>

#define BJ_AUDIO_AMPLITUDE 16000
#define BJ_AUDIO_SAMPLE_RATE 44100
#define BJ_AUDIO_CHANNELS 1

typedef struct bj_audio_device_data bj_audio_device_data;

struct bj_audio_device {
    struct bj_audio_properties   properties;
    uint32_t              silence;
    bj_bool               playing;
    volatile bj_bool      should_reset;
    bj_bool               should_close;
    bj_audio_callback_fn   callback;
    void*                 callback_user_data;
    bj_audio_device_data* data;
};


