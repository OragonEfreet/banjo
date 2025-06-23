#pragma once

#include <banjo/error.h>
#include <banjo/api.h>

#define BJ_AUDIO_AMPLITUDE 16000

typedef uint16_t bj_audio_format;

typedef struct bj_audio_device_t bj_audio_device;

typedef void (*bj_audio_callback_t)(int16_t*, unsigned int, void*);

BANJO_EXPORT bj_audio_device* bj_open_audio_device(
	bj_error** p_error,
    bj_audio_callback_t p_callback,
    void * p_callback_user_data
);

BANJO_EXPORT void bj_close_audio_device(
	bj_audio_device* p_device
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
