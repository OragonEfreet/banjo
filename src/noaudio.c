#include <banjo/audio.h>
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/video.h>

#include "check.h"
#include "window_t.h"

static void noaudio_dispose_audio(bj_audio_layer* p_audio, bj_error** p_error) {
    (void)p_error;
    bj_check(p_audio);
    bj_free(p_audio);
}

static bj_audio_device* noaudio_open_device(
    bj_audio_layer*     p_audio,
    bj_error**          p_error,
    bj_audio_callback_t p_callback,
    void*               p_callback_user_data
) {
    (void)p_audio;
    (void)p_error;
    (void)p_callback;
    (void)p_callback_user_data;
	return 0;
}

static void noaudio_close_device(bj_audio_layer* p_audio, bj_audio_device* p_device) {
	(void)p_audio;
	(void)p_device;
}


static bj_audio_layer* noaudio_init_audio(bj_error** p_error) {
	bj_audio_layer* p_audio = bj_malloc(sizeof(bj_audio_layer));

	if (!p_audio) {
		bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "cannot allocate memory for noaudio");
		return 0;
	}

	p_audio->dispose = noaudio_dispose_audio;
	p_audio->open_device = noaudio_open_device;
	p_audio->close_device = noaudio_close_device;
    p_audio->data = 0;

	return p_audio;
}

bj_audio_layer_create_info noaudio_layer_info = {
    .name = "noaudio",
    .create = noaudio_init_audio,
};

