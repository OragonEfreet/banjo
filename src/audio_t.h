#pragma once

#include <banjo/audio.h>

typedef struct bj_audio_device_data_t bj_audio_device_data;

struct bj_audio_device_t {
    unsigned int          channels;
    unsigned int          sample_rate;
    bj_audio_callback_t   p_callback;
    void*                 p_callback_user_data;
    bj_audio_device_data* data;
};


