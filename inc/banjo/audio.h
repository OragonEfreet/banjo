#pragma once

#include <banjo/error.h>

typedef void (*bj_audio_dispose_fn)(struct bj_audio_layer_t*, bj_error** p_error);

struct bj_audio_layer_data_t;

typedef struct bj_audio_layer_t {
	bj_audio_dispose_fn dispose;

	struct bj_audio_layer_data_t* data;
} bj_audio_layer;

typedef struct {
	const char* name;
	bj_audio_layer* (*create)(bj_error** p_error);
} bj_audio_layer_create_info;
