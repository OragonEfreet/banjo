#pragma once

#include <banjo/audio.h>

struct bj_audio_stream_t {
    int a;
};

typedef struct bj_audio_device_data_t bj_audio_device_data;

struct bj_audio_device_t {
    unsigned int channels;
    unsigned int sample_rate;
    struct bj_audio_stream_t stream;
    bj_audio_device_data* data;
};


