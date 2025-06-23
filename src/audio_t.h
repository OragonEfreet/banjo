#pragma once

#include <banjo/audio.h>

#define BJ_AUDIO_AMPLITUDE 16000

typedef struct bj_audio_device_data_t bj_audio_device_data;

struct bj_audio_device_t {
    bj_audio_properties   properties;
    bj_audio_callback_t   p_callback;
    void*                 p_callback_user_data;
    bj_audio_device_data* data;
};


