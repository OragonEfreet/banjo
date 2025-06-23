#include <banjo/log.h>

#include "audio_t.h"
#include "config.h"
#include "check.h"

#include <assert.h>

extern bj_audio_layer_create_info alsa_layer_info;
extern bj_audio_layer_create_info mme_layer_info;
extern bj_audio_layer_create_info noaudio_layer_info;

static const bj_audio_layer_create_info* layer_infos[] = {
#if BJ_HAS_FEATURE(MME)
    &mme_layer_info,
#endif
#if BJ_HAS_FEATURE(ALSA)
    &alsa_layer_info,
#endif
    &noaudio_layer_info,
};

extern bj_audio_layer* s_audio;

bj_audio_layer* bj_init_audio(bj_error** p_error) {
    assert(s_audio == 0);

    const size_t n_layers = sizeof(layer_infos) / sizeof(bj_audio_layer_create_info*);

    for (size_t b = 0; b < n_layers; ++b) {
        bj_error* sub_err = 0;

        const bj_audio_layer_create_info* p_create_info = layer_infos[b];
        s_audio = p_create_info->create(&sub_err);

        if (sub_err) {
            bj_message(s_audio == 0 ? 0 : 1, 0, 0,
                "while trying %s audio layer: %s (code 0x%08X)",
                p_create_info->name, sub_err->message, sub_err->code
            );
            bj_clear_error(&sub_err);
        }

        if(s_audio != 0) {
            bj_info("audio: %s", p_create_info->name);
            break;
        }

    }

    if(s_audio == 0) {
        bj_set_error(p_error, BJ_ERROR_INITIALIZE, "no suitable audio");
        return 0;
    }

    return s_audio;
}

void bj_dispose_audio(bj_audio_layer* p_audio, bj_error** p_error) {
    p_audio->dispose(p_audio, p_error);
}

bj_audio_device* bj_open_audio_device(
	bj_error** p_error,
    bj_audio_callback_t p_callback,
    void * p_callback_user_data
) {
    bj_check_or_0(s_audio);
    return s_audio->open_device(s_audio, p_error, p_callback, p_callback_user_data);
}

void bj_close_audio_device(
    bj_audio_device* p_device
) {
    bj_check(s_audio);
    s_audio->close_device(s_audio, p_device);
}

