#pragma once

#include <banjo/error.h>
#include <banjo/api.h>

typedef uint16_t bj_audio_format;

typedef struct bj_audio_device_t bj_audio_device;

typedef struct bj_audio_properties {
	int frequency;


} bj_audio_properties;



BANJO_EXPORT bj_audio_device* bj_open_audio_device(
	bj_error** p_error
);

BANJO_EXPORT void bj_close_audio_device(
	bj_audio_device* p_device
);

struct bj_audio_layer_data_t;

typedef struct bj_audio_layer_t {
	void (*dispose)(struct bj_audio_layer_t*, bj_error** p_error);
	void(*close_device)(struct bj_audio_layer_t*, bj_audio_device*);
	bj_audio_device*(*open_device)(struct bj_audio_layer_t*, bj_error** p_error);

	struct bj_audio_layer_data_t* data;
} bj_audio_layer;

typedef struct {
	const char* name;
	bj_audio_layer* (*create)(bj_error** p_error);
} bj_audio_layer_create_info;
