#include "config.h"

#if BJ_HAS_FEATURE(ALSA)

#include <banjo/audio.h>
#include <banjo/assert.h>
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/math.h>
#include <banjo/memory.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <banjo/video.h>

#include "audio_t.h"
#include "check.h"
#include "window_t.h"

#include <alsa/asoundlib.h>
#include <pthread.h>

typedef struct bj_audio_device_data_t {
    snd_pcm_t*        p_handle;
    pthread_t         playback_thread;
    char*             p_buffer;
    snd_pcm_uframes_t frames_per_period;
} alsa_device;

typedef int(*pfn_snd_pcm_hw_params_any)(snd_pcm_t*, snd_pcm_hw_params_t*);
typedef int(*pfn_snd_pcm_hw_params_malloc)(snd_pcm_hw_params_t**);
typedef int(*pfn_snd_pcm_hw_params)(snd_pcm_t*, snd_pcm_hw_params_t*);
typedef int(*pfn_snd_pcm_hw_params_set_access)(snd_pcm_t*, snd_pcm_hw_params_t*, snd_pcm_access_t);
typedef int(*pfn_snd_pcm_hw_params_set_buffer_size_near)(snd_pcm_t*, snd_pcm_hw_params_t*, snd_pcm_uframes_t*);
typedef int(*pfn_snd_pcm_hw_params_set_channels_near)(snd_pcm_t*, snd_pcm_hw_params_t*, unsigned int*);
typedef int(*pfn_snd_pcm_hw_params_set_format)(snd_pcm_t*, snd_pcm_hw_params_t*, snd_pcm_format_t);
typedef int(*pfn_snd_pcm_hw_params_set_period_size_near)(snd_pcm_t*, snd_pcm_hw_params_t*, snd_pcm_uframes_t*, int*);
typedef int(*pfn_snd_pcm_hw_params_set_rate_near)(snd_pcm_t*, snd_pcm_hw_params_t*, unsigned int*, int*);
typedef int(*pfn_snd_pcm_open)(snd_pcm_t**, const char*, snd_pcm_stream_t, int);
typedef void(*pfn_snd_pcm_hw_params_free)(snd_pcm_hw_params_t*);
typedef int(*pfn_snd_pcm_close)(snd_pcm_t*);
typedef int(*pfn_snd_pcm_drain)(snd_pcm_t*);
typedef snd_pcm_sframes_t(*pfn_snd_pcm_avail_update)(snd_pcm_t*);
typedef int(*pfn_snd_pcm_prepare)(snd_pcm_t*);
typedef snd_pcm_sframes_t(*pfn_snd_pcm_writei)(snd_pcm_t*, const void*, snd_pcm_uframes_t);
typedef const char*(*pfn_snd_strerror)(int);
typedef uint16_t(*pfn_snd_pcm_format_silence_16)(snd_pcm_format_t);
typedef uint32_t(*pfn_snd_pcm_format_silence_32)(snd_pcm_format_t);
typedef ssize_t(*pfn_snd_pcm_format_size)(snd_pcm_format_t, size_t);

static struct alsa_lib_t {
    void*                                      p_handle;
    pfn_snd_pcm_hw_params                      snd_pcm_hw_params;
    pfn_snd_pcm_hw_params_any                  snd_pcm_hw_params_any;
    pfn_snd_pcm_hw_params_free                 snd_pcm_hw_params_free;
    pfn_snd_pcm_hw_params_malloc               snd_pcm_hw_params_malloc;
    pfn_snd_pcm_hw_params_set_access           snd_pcm_hw_params_set_access;
    pfn_snd_pcm_hw_params_set_buffer_size_near snd_pcm_hw_params_set_buffer_size_near;
    pfn_snd_pcm_hw_params_set_channels_near    snd_pcm_hw_params_set_channels_near;
    pfn_snd_pcm_hw_params_set_format           snd_pcm_hw_params_set_format;
    pfn_snd_pcm_hw_params_set_period_size_near snd_pcm_hw_params_set_period_size_near;
    pfn_snd_pcm_hw_params_set_rate_near        snd_pcm_hw_params_set_rate_near;
    pfn_snd_pcm_open                           snd_pcm_open;
    pfn_snd_pcm_close                          snd_pcm_close;
    pfn_snd_pcm_drain                          snd_pcm_drain;
    pfn_snd_pcm_prepare                        snd_pcm_prepare;
    pfn_snd_pcm_avail_update                   snd_pcm_avail_update;
    pfn_snd_pcm_writei                         snd_pcm_writei;
    pfn_snd_strerror                           snd_strerror;
    pfn_snd_pcm_format_silence_16              snd_pcm_format_silence_16;
    pfn_snd_pcm_format_silence_32              snd_pcm_format_silence_32;
    pfn_snd_pcm_format_size                    snd_pcm_format_size;
} ALSA = {0};

/* static bj_audio_format alsa_format_bj(snd_pcm_format_t alsa_format) { */
/*     switch(alsa_format) { */
/*         case SND_PCM_FORMAT_S16_LE:   return BJ_AUDIO_FORMAT_INT16; */
/*         case SND_PCM_FORMAT_FLOAT_LE: return BJ_AUDIO_FORMAT_F32; */
/*         default: break; */
/*     } */
/*     return BJ_AUDIO_FORMAT_UNKNOWN; */
/* } */

static snd_pcm_format_t bj_format_alsa(bj_audio_format format) {
    switch(format) {
        case BJ_AUDIO_FORMAT_INT16:   return SND_PCM_FORMAT_S16_LE;
        case BJ_AUDIO_FORMAT_F32:     return SND_PCM_FORMAT_FLOAT_LE;
        default: break;
    }
    return SND_PCM_FORMAT_UNKNOWN;
}

static void alsa_set_error(bj_error** p_error, int alsa_err) {
    bj_set_error(p_error, BJ_ERROR_AUDIO, ALSA.snd_strerror(alsa_err));
}

static void alsa_unload_library() {
    if(ALSA.p_handle != 0) {
        bj_unload_library(ALSA.p_handle);
    }
    bj_memzero(&ALSA, sizeof(struct alsa_lib_t));
}

static bj_bool alsa_load_library(bj_error** p_error) {
    bj_check_or_return(ALSA.p_handle == 0, BJ_TRUE);

    ALSA.p_handle = bj_load_library("libasound.so");
	if (!ALSA.p_handle) {
		bj_set_error(p_error, BJ_ERROR_INITIALIZE, "cannot load libasound.so");
		return BJ_FALSE;
	}

#define ALSA_BIND(name) if(!(ALSA.name = (pfn_ ## name)bj_get_symbol(ALSA.p_handle, #name))) {bj_set_error(p_error, BJ_ERROR_AUDIO, "cannot load ALSA function " #name); alsa_unload_library(); return 0;}
    ALSA_BIND(snd_pcm_avail_update)
    ALSA_BIND(snd_pcm_close)
    ALSA_BIND(snd_pcm_drain)
    ALSA_BIND(snd_pcm_format_silence_16)
    ALSA_BIND(snd_pcm_format_silence_32)
    ALSA_BIND(snd_pcm_format_size)
    ALSA_BIND(snd_pcm_hw_params)
    ALSA_BIND(snd_pcm_hw_params_any)
    ALSA_BIND(snd_pcm_hw_params_free)
    ALSA_BIND(snd_pcm_hw_params_malloc)
    ALSA_BIND(snd_pcm_hw_params_set_access)
    ALSA_BIND(snd_pcm_hw_params_set_buffer_size_near)
    ALSA_BIND(snd_pcm_hw_params_set_channels_near)
    ALSA_BIND(snd_pcm_hw_params_set_format)
    ALSA_BIND(snd_pcm_hw_params_set_period_size_near)
    ALSA_BIND(snd_pcm_hw_params_set_rate_near)
    ALSA_BIND(snd_pcm_open)
    ALSA_BIND(snd_pcm_prepare)
    ALSA_BIND(snd_pcm_writei)
    ALSA_BIND(snd_strerror)
#undef ALSA_BIND

    return BJ_TRUE;
}


static void* playback_thread(void* p_data) {
    bj_audio_device* p_device           = (bj_audio_device*)p_data;
    alsa_device* p_alsa_device          = (alsa_device*)p_device->data;
    snd_pcm_t* pcm_handle               = p_alsa_device->p_handle;
    char* buffer                        = p_alsa_device->p_buffer;
    snd_pcm_uframes_t frames_per_period = p_alsa_device->frames_per_period;

    uint64_t global_sample_index = 0;

    while (p_device->should_close == BJ_FALSE) {

        if(p_device->should_reset == BJ_TRUE) {
            global_sample_index = 0;
            p_device->should_reset = BJ_FALSE;
        }

        snd_pcm_sframes_t avail = ALSA.snd_pcm_avail_update(pcm_handle);

        if (avail < 0) {
            if (avail == -EPIPE) {
                bj_err("underrun!");
                ALSA.snd_pcm_prepare(pcm_handle);
                continue;
            } else {
                bj_err("avail error: %s", ALSA.snd_strerror(avail));
                break;
            }
        }

        if ((snd_pcm_uframes_t)avail >= frames_per_period) {
            if (p_device->playing == BJ_TRUE) {
                // Generate audio normally
                p_device->p_callback(
                    buffer,
                    frames_per_period,
                    &p_device->properties,
                    p_device->p_callback_user_data,
                    global_sample_index
                );
            } else {
                const size_t format_byte_size = BJ_AUDIO_FORMAT_WIDTH(p_device->properties.format);
                for(size_t s = 0 ; s < frames_per_period * p_device->properties.channels ; ++s) {
                    bj_memcpy(buffer + s * format_byte_size, &p_device->silence, format_byte_size);
                }
            }

            int err = ALSA.snd_pcm_writei(pcm_handle, buffer, frames_per_period);
            if (err == -EPIPE) {
                bj_err("write underrun!");
                ALSA.snd_pcm_prepare(pcm_handle);
            } else if (err < 0) {
                bj_err("write error: %s", ALSA.snd_strerror(err));
                break;
            }

            if (p_device->playing) {
                global_sample_index += frames_per_period;
            }
        } else {
            usleep(100); // Small sleep to avoid busy-looping
        }
    }

    return NULL;
}

static void alsa_close_device(bj_audio_layer* p_audio, bj_audio_device* p_device) {
    bj_check(p_audio);
    bj_check(p_device);

    alsa_device* alsa_dev = p_device->data;

    if(alsa_dev != 0) {
        if(alsa_dev->playback_thread) {
            pthread_join(alsa_dev->playback_thread, 0);
        }

        if(alsa_dev->p_handle) {
           ALSA.snd_pcm_drain(alsa_dev->p_handle);
           ALSA.snd_pcm_drain(alsa_dev->p_handle);
           ALSA.snd_pcm_close(alsa_dev->p_handle);
        }

        bj_free(alsa_dev->p_buffer);
    }

    bj_free(p_device->data);
    bj_free(p_device);
}

static bj_audio_device* alsa_open_device(
    bj_audio_layer*            p_audio,
    const bj_audio_properties* p_properties,
    bj_audio_callback_t        p_callback,
    void*                      p_callback_user_data,
    bj_error**                 p_error
) {
    bj_audio_device* p_device = bj_calloc(sizeof(bj_audio_device));
    if(p_device == 0) {
        bj_set_error(p_error, BJ_ERROR_INITIALIZE, "cannot allocate audio device");
        return 0;
    }
    alsa_device* alsa_dev = bj_calloc(sizeof(alsa_device));
    if(alsa_dev == 0) {
        bj_set_error(p_error, BJ_ERROR_INITIALIZE, "cannot allocate audio device data");
        alsa_close_device(p_audio, p_device);
        return 0;
    }
    p_device->p_callback           = p_callback;
    p_device->p_callback_user_data = p_callback_user_data;
    p_device->data                 = alsa_dev;

    p_device->properties.format      = p_properties ? p_properties->format : BJ_AUDIO_FORMAT_INT16;
    p_device->properties.amplitude   = p_properties ? p_properties->amplitude : BJ_AUDIO_AMPLITUDE;
    p_device->properties.channels    = p_properties ? p_properties->channels : 1;
    p_device->properties.sample_rate = p_properties ? p_properties->sample_rate : BJ_AUDIO_SAMPLE_RATE;
    

    alsa_dev->frames_per_period      = 512;

    snd_pcm_uframes_t total_frames = alsa_dev->frames_per_period * 4;

    int alsa_err = ALSA.snd_pcm_open(&alsa_dev->p_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if(alsa_err < 0) {
        alsa_set_error(p_error, alsa_err);
        alsa_close_device(p_audio, p_device);
        return 0;
    }

    const snd_pcm_format_t alsa_format = bj_format_alsa(p_device->properties.format);

    snd_pcm_hw_params_t* params      = 0;
    ALSA.snd_pcm_hw_params_malloc(&params);
    ALSA.snd_pcm_hw_params_any(alsa_dev->p_handle, params);
    ALSA.snd_pcm_hw_params_set_access(alsa_dev->p_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    ALSA.snd_pcm_hw_params_set_format(alsa_dev->p_handle, params, alsa_format);
    ALSA.snd_pcm_hw_params_set_channels_near(alsa_dev->p_handle, params, &p_device->properties.channels);
    ALSA.snd_pcm_hw_params_set_rate_near(alsa_dev->p_handle, params, &p_device->properties.sample_rate, 0);
    ALSA.snd_pcm_hw_params_set_period_size_near(alsa_dev->p_handle, params, &alsa_dev->frames_per_period, 0);
    ALSA.snd_pcm_hw_params_set_buffer_size_near(alsa_dev->p_handle, params, &total_frames);

    alsa_err = ALSA.snd_pcm_hw_params(alsa_dev->p_handle, params);
    if(alsa_err < 0) {
        alsa_set_error(p_error, alsa_err);
        ALSA.snd_pcm_hw_params_free(params);
        alsa_close_device(p_audio, p_device);
        return 0;
    }
    ALSA.snd_pcm_hw_params_free(params);

    bj_info("format: %d", p_device->properties.format);
    bj_info("amplitude: %d", p_device->properties.amplitude);
    bj_info("channels: %d", p_device->properties.channels);
    bj_info("sample_rate: %d", p_device->properties.sample_rate);

    ssize_t format_byte_size = ALSA.snd_pcm_format_size(alsa_format, 1);
    bj_assert(format_byte_size == BJ_AUDIO_FORMAT_WIDTH(p_device->properties.format) / 8);

    /* switch(format_byte_size) { */
    /*     case 2: */
    /*         p_device->silence = ALSA.snd_pcm_format_silence_16(alsa_format); */
    /*         break; */
    /*     case 4: */
    /*         p_device->silence = ALSA.snd_pcm_format_silence_32(alsa_format); */
    /*         break; */
    /*     default: */
    /*         p_device->silence = 0; */
    /*         break; */
    /* } */

    // Create buffer and fill with silence
    alsa_dev->p_buffer = bj_malloc(format_byte_size * alsa_dev->frames_per_period * p_device->properties.channels);
    for(size_t s = 0 ; s < alsa_dev->frames_per_period * p_device->properties.channels ; ++s) {
        bj_memcpy(alsa_dev->p_buffer + s * format_byte_size, &p_device->silence, format_byte_size);
    }
    
    ALSA.snd_pcm_prepare(alsa_dev->p_handle);
    pthread_create(&alsa_dev->playback_thread, 0, playback_thread, (void*)p_device);

	return p_device;
}


static void alsa_dispose_audio(bj_audio_layer* p_audio, bj_error** p_error) {
    (void)p_error;
    bj_check(p_audio);
    alsa_unload_library();
    bj_free(p_audio);
}

static bj_audio_layer* alsa_init_audio(bj_error** p_error) {

    if(!alsa_load_library(p_error)) {
        return 0;
    }

    // Common Data
	bj_audio_layer* p_audio = bj_malloc(sizeof(bj_audio_layer));
	if (!p_audio) {
		bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "cannot allocate memory for alsa");
		return 0;
	}

	p_audio->end            = alsa_dispose_audio;
	p_audio->open_device    = alsa_open_device;
	p_audio->close_device   = alsa_close_device;
    p_audio->data = 0;

	return p_audio;
}

bj_audio_layer_create_info alsa_layer_info = {
    .name   = "alsa",
    .create = alsa_init_audio,
};

#endif
