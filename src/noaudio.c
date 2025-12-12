#include <banjo/audio.h>

#include <check.h>

static void noaudio_dispose_audio(struct bj_audio_layer* p_audio, struct bj_error** p_error) {
    (void)p_error;
    bj_check(p_audio);
    bj_free(p_audio);
}

static struct bj_audio_device* noaudio_open_device(
    struct bj_audio_layer*            p_audio,
    const struct bj_audio_properties* p_properties,
    bj_audio_callback_fn        p_callback,
    void*                      p_callback_user_data,
    struct bj_error**                 p_error
) {
    (void)p_audio;
    (void)p_callback;
    (void)p_callback_user_data;
    (void)p_error;
    (void)p_properties;
	return 0;
}

static void noaudio_close_device(struct bj_audio_layer* p_audio, struct bj_audio_device* p_device) {
	(void)p_audio;
	(void)p_device;
}


static struct bj_audio_layer* noaudio_init_audio(struct bj_error** p_error) {
	struct bj_audio_layer* p_audio = bj_malloc(sizeof(struct bj_audio_layer));

	if (!p_audio) {
		bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "cannot allocate memory for noaudio");
		return 0;
	}

	p_audio->end          = noaudio_dispose_audio;
	p_audio->open_device  = noaudio_open_device;
	p_audio->close_device = noaudio_close_device;
    p_audio->data         = 0;

	return p_audio;
}

struct bj_audio_layer_create_info noaudio_audio_layer_info = {
    .name = "noaudio",
    .create = noaudio_init_audio,
};

