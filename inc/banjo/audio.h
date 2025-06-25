#pragma once

#include <banjo/error.h>
#include <banjo/api.h>

typedef struct bj_audio_device_t bj_audio_device;

typedef struct bj_audio_properties_t {
    int16_t      amplitude;
    unsigned int channels;
    unsigned int sample_rate;
    uint16_t     silence;
} bj_audio_properties;

typedef void (*bj_audio_callback_t)(
    int16_t* buffer,
    unsigned frames,
    const bj_audio_properties* audio,
    void* user_data,
    uint64_t base_sample_index
);


BANJO_EXPORT bj_audio_device* bj_open_audio_device(
	bj_error** p_error,
    bj_audio_callback_t p_callback,
    void * p_callback_user_data
);

BANJO_EXPORT void bj_close_audio_device(
	bj_audio_device* p_device
);

BANJO_EXPORT void bj_audio_device_play(
    bj_audio_device* p_device
);

BANJO_EXPORT void bj_audio_device_pause(
    bj_audio_device* p_device
);

BANJO_EXPORT void bj_audio_device_reset(
    bj_audio_device* p_device
);

BANJO_EXPORT void bj_audio_device_stop(
    bj_audio_device* p_device
);

BANJO_EXPORT bj_bool bj_audio_device_is_playing(
    const bj_audio_device* p_device
);

typedef struct bj_audio_play_note_data_t {
    enum {
        BJ_AUDIO_PLAY_SINE,
        BJ_AUDIO_PLAY_SQUARE,
        BJ_AUDIO_PLAY_TRIANGLE,
        BJ_AUDIO_PLAY_SAWTOOTH,
    } function;
    double frequency;
    double phase;
} bj_audio_play_note_data;

BANJO_EXPORT void bj_audio_play_note(
    int16_t* buffer,
    unsigned frames,
    const bj_audio_properties* audio,
    void* user_data,
    uint64_t base_sample_index
);

struct bj_audio_layer_data_t;

typedef struct bj_audio_layer_t {
	bj_audio_device*(*detect_devices)(struct bj_audio_layer_t*);
	void (*dispose)(struct bj_audio_layer_t*, bj_error** p_error);
	void(*close_device)(struct bj_audio_layer_t*, bj_audio_device*);
	bj_audio_device*(*open_device)(struct bj_audio_layer_t*, bj_error** p_error, bj_audio_callback_t, void*);
	struct bj_audio_layer_data_t* data;
} bj_audio_layer;

typedef struct {
	const char* name;
	bj_audio_layer* (*create)(bj_error** p_error);
} bj_audio_layer_create_info;
