#include <banjo/assert.h>
#include <banjo/math.h>

#include "audio_t.h"
#include "config.h"
#include "check.h"

extern bj_audio_layer_create_info alsa_audio_layer_info;
extern bj_audio_layer_create_info mme_audio_layer_info;
extern bj_audio_layer_create_info noaudio_audio_layer_info;
extern bj_audio_layer_create_info emscripten_audio_layer_info;

static const bj_audio_layer_create_info* layer_infos[] = {
#if BJ_HAS_FEATURE(EMSCRIPTEN)
    &emscripten_audio_layer_info,
#endif
#if BJ_HAS_FEATURE(MME)
    &mme_audio_layer_info,
#endif
#if BJ_HAS_FEATURE(ALSA)
    &alsa_audio_layer_info,
#endif
    &noaudio_audio_layer_info,
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
            bj_log_message(s_audio == 0 ? 0 : 1, 0, 0,
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


void bj_play_audio_device(
    bj_audio_device* p_device
) {
    bj_check(p_device);
    p_device->playing = BJ_TRUE;
}

void bj_pause_audio_device(
    bj_audio_device* p_device
) {
    bj_check(p_device);
    p_device->playing = BJ_FALSE;
}

bj_bool bj_audio_playing(
    const bj_audio_device* p_device
) {
    return p_device ? p_device->playing : BJ_FALSE;
}

void bj_reset_audio_device(
    bj_audio_device* p_device
) {
    bj_check(p_device);
    p_device->should_reset = BJ_TRUE;
}

void bj_stop_audio_device(
    bj_audio_device* p_device
) {
    bj_check(p_device);
    bj_pause_audio_device(p_device);
    bj_reset_audio_device(p_device);
}

inline static double make_note_value(
    unsigned i,
    int      function,
    uint64_t base_sample_index, 
    double   phase_step
) {
    const uint64_t sample_index = base_sample_index + i;
    const double two_pi = BJ_TAU; /* 2PI exactly in double */

    /* Use the double variant explicitly to avoid accidental float truncation
       when BJ_API_FLOAT64 is off. */
    const double phase = bj_fmodd(sample_index * phase_step, two_pi);

    switch (function) {
        case BJ_AUDIO_PLAY_SINE:
            return bj_sind(phase);

        case BJ_AUDIO_PLAY_SQUARE:
            /* Cheap sign of sine; or compare phase against PI for duty=50%. */
            return (bj_sind(phase) > 0.0) ? 0.2 : -0.2;

        case BJ_AUDIO_PLAY_TRIANGLE: {
            const double t = phase / two_pi;               /* 0..1 ramp */
            return 4.0 * bj_absd(t - bj_floord(t + 0.5)) - 1.0;
        }

        case BJ_AUDIO_PLAY_SAWTOOTH: {
            const double t = phase / two_pi;               /* 0..1 ramp */
            return 2.0 * (t - bj_floord(t + 0.5));
        }

        default:
            return 0.0;
    }
}

void bj_play_audio_note(
    void*                        buffer,
    unsigned                     frames,
    const bj_audio_properties*   p_audio,
    void*                        p_user_data,
    uint64_t                     base_sample_index
) {
    bj_audio_play_note_data* data = (bj_audio_play_note_data*)p_user_data;

    const double freq        = (double)data->frequency;
    const double amplitude   = (double)p_audio->amplitude;   /* scale for int16 path */
    const double sample_rate = (double)p_audio->sample_rate;
    const double phase_step  = BJ_TAU * freq / sample_rate;
    const int    channels    = p_audio->channels;

    for (unsigned i = 0; i < frames; ++i) {
        const double output = make_note_value(i, data->function, base_sample_index, phase_step);

        for (int ch = 0; ch < channels; ++ch) {
            const unsigned idx = i * (unsigned)channels + (unsigned)ch;

            switch (p_audio->format) {
            case BJ_AUDIO_FORMAT_INT16: {
                int16_t* buf = (int16_t*)buffer;
                /* clamp to avoid wrap if amplitude drives beyond range */
                double s = output * amplitude;
                if (s >  32767.0) s =  32767.0;
                if (s < -32768.0) s = -32768.0;
                buf[idx] = (int16_t)s;
                break;
            }
            case BJ_AUDIO_FORMAT_F32: {
                float* buf = (float*)buffer;
                buf[idx] = (float)output;  /* amplitude can be baked into output if desired */
                break;
            }
            default:
                /* unsupported -> silence */
                break;
            }
        }
    }
}


