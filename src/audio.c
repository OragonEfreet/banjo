#include <banjo/assert.h>
#include <banjo/math.h>

#include "audio_t.h"
#include "config.h"
#include "check.h"


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

bj_audio_layer* bj_begin_audio(bj_error** p_error) {
    bj_assert(s_audio == 0);

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

void bj_end_audio(bj_audio_layer* p_audio, bj_error** p_error) {
    // TODO?
    p_audio->end(p_audio, p_error);
}

bj_audio_device* bj_open_audio_device(
    const bj_audio_properties* p_properties,
    bj_audio_callback_t        p_callback,
    void*                      p_callback_user_data,
    bj_error**                 p_error
) {
    bj_check_or_0(s_audio);
    return s_audio->open_device(s_audio, p_properties, p_callback, p_callback_user_data, p_error);
}

void bj_close_audio_device(
    bj_audio_device* p_device
) {
    bj_check(s_audio);
    p_device->should_close = BJ_TRUE;
    s_audio->close_device(s_audio, p_device);
}


void bj_audio_device_play(
    bj_audio_device* p_device
) {
    bj_check(p_device);
    p_device->playing = BJ_TRUE;
}

void bj_audio_device_pause(
    bj_audio_device* p_device
) {
    bj_check(p_device);
    p_device->playing = BJ_FALSE;
}

bj_bool bj_audio_device_is_playing(
    const bj_audio_device* p_device
) {
    return p_device ? p_device->playing : BJ_FALSE;
}

void bj_audio_device_reset(
    bj_audio_device* p_device
) {
    bj_check(p_device);
    p_device->should_reset = BJ_TRUE;
}

void bj_audio_device_stop(
    bj_audio_device* p_device
) {
    bj_check(p_device);
    bj_audio_device_pause(p_device);
    bj_audio_device_reset(p_device);
}

inline static double make_note_value(
    unsigned i,
    int      function,
    uint64_t base_sample_index, 
    double   phase_step
) {
    uint64_t sample_index = base_sample_index + i;
    double phase = bj_fmod(sample_index * phase_step, 2.0 * BJ_PI); // wrap phase

    switch (function) {
        case BJ_AUDIO_PLAY_SINE:
            return bj_sin(phase);

        case BJ_AUDIO_PLAY_SQUARE:
            return bj_sin(phase) > 0.0 ? 0.2 : -0.2;

        case BJ_AUDIO_PLAY_TRIANGLE: {
            double t = phase / (2.0 * BJ_PI);
            return 4.0 * bj_fabs(t - bj_floor(t + 0.5)) - 1.0;
        }

        case BJ_AUDIO_PLAY_SAWTOOTH: {
            double t = phase / (2.0 * BJ_PI);
            return 2.0 * (t - bj_floor(t + 0.5));
        }

        default: return 0;
    }
}

void bj_audio_play_note(
    void* buffer,
    unsigned                   frames,
    const bj_audio_properties* p_audio,
    void* p_user_data,
    uint64_t                   base_sample_index
) {
    bj_audio_play_note_data* data = (bj_audio_play_note_data*)p_user_data;
    double                   freq = data->frequency;
    double                   amplitude = (double)p_audio->amplitude;
    double                   sample_rate = (double)p_audio->sample_rate;
    double                   phase_step = 2.0 * BJ_PI * freq / sample_rate;
    int                      channels = p_audio->channels;

    for (unsigned i = 0; i < frames; i++) {
        double output = make_note_value(i, data->function, base_sample_index, phase_step);

        for (int ch = 0; ch < channels; ++ch) {
            unsigned idx = (i * channels) + ch;

            switch (p_audio->format) {
            case BJ_AUDIO_FORMAT_INT16: {
                int16_t* buf = (int16_t*)buffer;
                buf[idx] = (int16_t)(output * amplitude);
                break;
            }
            case BJ_AUDIO_FORMAT_F32: {
                float* buf = (float*)buffer;
                buf[idx] = (float)output;
                break;
            }
            default:
                // unsupported format—silence
                break;
            }
        }
    }
}


