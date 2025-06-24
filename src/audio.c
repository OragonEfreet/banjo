#include <banjo/log.h>

#include "audio_t.h"
#include "config.h"
#include "check.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>

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


void bj_audio_play_note(int16_t* buffer, unsigned frames, const bj_audio_properties* audio, void* user_data) {
    bj_audio_play_note_data* data = (bj_audio_play_note_data*)user_data;
    double phase           = data->phase;
    double freq            = data->frequency;
    double amplitude       = (double)audio->amplitude;
    double sample_rate     = (double)audio->sample_rate;
    double phase_increment = 2.0 * M_PI * freq / sample_rate;

    for (unsigned i = 0; i < frames; i++) {
        double output = 0.0;

        switch(data->function) {
            case BJ_AUDIO_PLAY_SINE:
                output = sin(phase);
                buffer[i] = (int16_t)(amplitude * output);
                break;

            case BJ_AUDIO_PLAY_SQUARE:
                output = sin(phase);
                buffer[i] = output > 0.0 ? (int16_t)(amplitude / 6.0) : (int16_t)(-amplitude / 6.0);
                break;

            case BJ_AUDIO_PLAY_TRIANGLE:
                // Triangle wave ranges -1..1, phase 0..2pi
                // Formula: (2 / pi) * asin(sin(phase))
                output = asin(sin(phase)) * (2.0 / M_PI);
                buffer[i] = (int16_t)(amplitude * output);
                break;

            case BJ_AUDIO_PLAY_SAWTOOTH:
                // Sawtooth: map phase (0..2pi) to -1..1 linearly
                output = (2.0 * (phase / (2.0 * M_PI))) - 1.0;
                buffer[i] = (int16_t)(amplitude * output);
                break;

            case BJ_AUDIO_PLAY_NOISE:
                // White noise between -1 and 1
                output = ((double)rand() / (double)RAND_MAX) * 2.0 - 1.0;
                buffer[i] = (int16_t)(amplitude * output);
                break;

            default:
                output = sin(phase);
                buffer[i] = (int16_t)(amplitude * output);
                break;
        }

        phase += phase_increment;
        if (phase >= 2.0 * M_PI) {
            phase -= 2.0 * M_PI;
        }
    }

    data->phase = phase;
}

