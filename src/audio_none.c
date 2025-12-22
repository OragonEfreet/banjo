#include <banjo/audio.h>

#include "audio_layer.h"
#include "check.h"

static void noaudio_dispose_audio(struct bj_error** p_error) {
    (void)p_error;
}

static struct bj_audio_device* noaudio_open_device(
    const struct bj_audio_properties* p_properties,
    bj_audio_callback_fn        p_callback,
    void*                      p_callback_user_data,
    struct bj_error**                 p_error
) {
    (void)p_callback;
    (void)p_callback_user_data;
    (void)p_error;
    (void)p_properties;
	return 0;
}

static void noaudio_close_device(struct bj_audio_device* p_device) {
	(void)p_device;
}


static bj_bool noaudio_init_audio(
    struct bj_audio_layer* layer,
    struct bj_error**      p_error
) {
    (void)p_error;
	layer->end          = noaudio_dispose_audio;
	layer->open_device  = noaudio_open_device;
	layer->close_device = noaudio_close_device;

	return BJ_TRUE;
}

struct bj_audio_layer_create_info noaudio_audio_layer_info = {
    .name   = "noaudio",
    .create = noaudio_init_audio,
};

