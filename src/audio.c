#include <banjo/audio.h>
#include <banjo/log.h>

#include "config.h"
#include "check.h"

extern bj_audio_layer_create_info mme_layer_info;
extern bj_audio_layer_create_info noaudio_layer_info;

static const bj_audio_layer_create_info* layer_infos[] = {
#if BJ_HAS_FEATURE(MME)
&mme_layer_info,
#endif
&noaudio_layer_info,
};

extern bj_audio_layer* s_audio;

bj_audio_layer* bj_init_audio(bj_error** p_error) {
    const size_t n_layers = sizeof(layer_infos) / sizeof(bj_audio_layer_create_info*);

    for (size_t b = 0; b < n_layers; ++b) {

        bj_error* sub_err = 0;

        const bj_audio_layer_create_info* p_create_info = layer_infos[b];
        bj_trace("trying %s audio layer", p_create_info->name);
        bj_audio_layer* p_layer = p_create_info->create(&sub_err);

        if (sub_err) {
            bj_message(p_layer == 0 ? 0 : 1, 0, 0,
                "while trying %s audio layer: %s (code 0x%08X)",
                p_create_info->name, sub_err->message, sub_err->code
            );
            bj_clear_error(&sub_err);
        }

        if (p_layer != 0) {
            bj_info("%s audio layer ready", p_create_info->name);
            return p_layer;
        }
    }

    bj_set_error(p_error, BJ_ERROR_INITIALIZE, "no audio layer found");
    return 0;
}

void bj_dispose_audio(bj_audio_layer* p_audio, bj_error** p_error) {
    p_audio->dispose(p_audio, p_error);
    bj_info("disposed audio layer");
}

bj_audio_device* bj_open_audio_device(
    bj_error** p_error
) {
    bj_check_or_0(s_audio);
    return s_audio->open_device(s_audio, p_error);
}

void bj_close_audio_device(
    bj_audio_device* p_device
) {
    bj_check(s_audio);
    s_audio->close_device(s_audio, p_device);
}
