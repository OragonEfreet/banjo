#pragma once

#include <banjo/audio.h>

typedef struct bj_audio_device_data_t bj_audio_device_data;

struct bj_audio_device_t {
    bj_audio_device_data* data;
};
