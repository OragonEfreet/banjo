#include <banjo/audio.h>
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/video.h>
#include <banjo/system.h>
#include <banjo/time.h>

#include "audio_t.h"
#include "check.h"
#include "window_t.h"

#include <alsa/asoundlib.h>
#include <pthread.h>

#define BJ_AUDIO_PERIOD_FRAMES 512
#define BJ_AUDIO_BUFFER_FRAMES (BJ_AUDIO_PERIOD_FRAMES * 4)
#define BJ_AUDIO_FORMAT SND_PCM_FORMAT_S16_LE
#define BJ_AUDIO_CHANNELS 1

typedef int(*pfn_snd_pcm_hw_params_any)(snd_pcm_t*, snd_pcm_hw_params_t*);
typedef int(*pfn_snd_pcm_hw_params_malloc)(snd_pcm_hw_params_t**);
typedef int(*pfn_snd_pcm_hw_params)(snd_pcm_t*, snd_pcm_hw_params_t*);
typedef int(*pfn_snd_pcm_hw_params_set_access)(snd_pcm_t*, snd_pcm_hw_params_t*, snd_pcm_access_t);
typedef int(*pfn_snd_pcm_hw_params_set_buffer_size_near)(snd_pcm_t*, snd_pcm_hw_params_t*, snd_pcm_uframes_t*);
typedef int(*pfn_snd_pcm_hw_params_set_channels)(snd_pcm_t*, snd_pcm_hw_params_t*, unsigned int);
typedef int(*pfn_snd_pcm_hw_params_set_format)(snd_pcm_t*, snd_pcm_hw_params_t*, snd_pcm_format_t);
typedef int(*pfn_snd_pcm_hw_params_set_period_size_near)(snd_pcm_t*, snd_pcm_hw_params_t*, snd_pcm_uframes_t*, int*);
typedef int(*pfn_snd_pcm_hw_params_set_rate_near)(snd_pcm_t*, snd_pcm_hw_params_t*, unsigned int*, int*);
typedef int(*pfn_snd_pcm_open)(snd_pcm_t**, const char*, snd_pcm_stream_t, int);
typedef void(*pfn_snd_pcm_hw_params_free)(snd_pcm_hw_params_t*);
typedef int(*pfn_snd_pcm_close)(snd_pcm_t*);
typedef int(*pfn_snd_pcm_drain)(snd_pcm_t*);

typedef struct bj_audio_layer_data_t {
    void* p_handle;

    pfn_snd_pcm_hw_params snd_pcm_hw_params;
    pfn_snd_pcm_hw_params_any snd_pcm_hw_params_any;
    pfn_snd_pcm_hw_params_free snd_pcm_hw_params_free;
    pfn_snd_pcm_hw_params_malloc snd_pcm_hw_params_malloc;
    pfn_snd_pcm_hw_params_set_access snd_pcm_hw_params_set_access;
    pfn_snd_pcm_hw_params_set_buffer_size_near snd_pcm_hw_params_set_buffer_size_near;
    pfn_snd_pcm_hw_params_set_channels snd_pcm_hw_params_set_channels;
    pfn_snd_pcm_hw_params_set_format snd_pcm_hw_params_set_format;
    pfn_snd_pcm_hw_params_set_period_size_near snd_pcm_hw_params_set_period_size_near;
    pfn_snd_pcm_hw_params_set_rate_near snd_pcm_hw_params_set_rate_near;
    pfn_snd_pcm_open snd_pcm_open;
    pfn_snd_pcm_close snd_pcm_close;
    pfn_snd_pcm_drain snd_pcm_drain;
} alsa_ctx;

typedef struct bj_audio_device_data_t {
    snd_pcm_t* p_handle;
    pthread_t  playback_thread;
} alsa_device;

static void* playback_thread(void* data) {
    (void)data;
    for(size_t i = 0 ; i < 10 ; ++i) {
        bj_info("#%d", i);
        bj_sleep(100 * (10-i));
    }
    return 0;
}

static void alsa_close_device(bj_audio_layer* p_audio, bj_audio_device* p_device) {
    bj_check(p_audio);
    bj_check(p_device);

    alsa_ctx* alsa = p_audio->data;
    alsa_device* alsa_dev = p_device->data;

    if(alsa_dev != 0) {
        if(alsa_dev->playback_thread) {
            pthread_join(alsa_dev->playback_thread, 0);
        }

        if(alsa_dev->p_handle) {
           alsa->snd_pcm_drain(alsa_dev->p_handle);
           alsa->snd_pcm_close(alsa_dev->p_handle);
        }
    }

    bj_free(p_device->data);
    bj_free(p_device);
}

static bj_audio_device* alsa_open_device(bj_audio_layer* p_audio, bj_error** p_error) {
    alsa_ctx* alsa = p_audio->data;

    bj_audio_device* p_device = bj_malloc(sizeof(bj_audio_device));
    if(p_device == 0) {
        bj_set_error(p_error, BJ_ERROR_INITIALIZE, "cannot allocate audio device");
        return 0;
    }
    p_device->data = bj_malloc(sizeof(alsa_device));
    if(p_device->data == 0) {
        bj_set_error(p_error, BJ_ERROR_INITIALIZE, "cannot allocate audio device data");
        alsa_close_device(p_audio, p_device);
        return 0;
    }


    snd_pcm_hw_params_t* params   = 0;
    unsigned int sample_rate      = BJ_AUDIO_SAMPLE_RATE;
    snd_pcm_uframes_t period      = BJ_AUDIO_PERIOD_FRAMES;
    snd_pcm_uframes_t buffer_size = BJ_AUDIO_BUFFER_FRAMES;

    if(alsa->snd_pcm_open(&p_device->data->p_handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        bj_set_error(p_error, BJ_ERROR_AUDIO, "cannot open 'default' device");
        alsa_close_device(p_audio, p_device);
        return 0;
    }

    alsa->snd_pcm_hw_params_malloc(&params);
    alsa->snd_pcm_hw_params_any(p_device->data->p_handle, params);
    alsa->snd_pcm_hw_params_set_access(p_device->data->p_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    alsa->snd_pcm_hw_params_set_format(p_device->data->p_handle, params, BJ_AUDIO_FORMAT);
    alsa->snd_pcm_hw_params_set_channels(p_device->data->p_handle, params, BJ_AUDIO_CHANNELS);
    alsa->snd_pcm_hw_params_set_rate_near(p_device->data->p_handle, params, &sample_rate, 0);
    alsa->snd_pcm_hw_params_set_period_size_near(p_device->data->p_handle, params, &period, 0);
    alsa->snd_pcm_hw_params_set_buffer_size_near(p_device->data->p_handle, params, &buffer_size);

    if(alsa->snd_pcm_hw_params(p_device->data->p_handle, params) < 0) {
        bj_set_error(p_error, BJ_ERROR_AUDIO, "cannot configure audio device");
        alsa->snd_pcm_hw_params_free(params);
        alsa_close_device(p_audio, p_device);
        return 0;
    }

    alsa->snd_pcm_hw_params_free(params);

    pthread_t thread;
    pthread_create(&thread, 0, playback_thread, 0);

    p_device->data->playback_thread = thread;

	return p_device;
}


static void alsa_dispose_audio(bj_audio_layer* p_audio, bj_error** p_error) {
    (void)p_error;
    bj_check(p_audio);
    alsa_ctx* alsa = (alsa_ctx*)p_audio->data;

    bj_unload_library(alsa->p_handle);

    bj_free(alsa);
    bj_free(p_audio);
}

static bj_audio_layer* alsa_init_audio(bj_error** p_error) {

	alsa_ctx* p_ctx = bj_malloc(sizeof(alsa_ctx));
	if (!p_ctx) {
		bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "cannot allocate memory for alsa");
		return 0;
	}

    p_ctx->p_handle = bj_load_library("libasound.so");
	if (!p_ctx->p_handle) {
		bj_set_error(p_error, BJ_ERROR_INITIALIZE, "cannot load libasound.so");
		return 0;
	}

#define ALSA_BIND(name) if(!(p_ctx->name = (pfn_ ## name)bj_get_symbol(p_ctx->p_handle, #name))) {bj_set_error(p_error, BJ_ERROR_AUDIO, "cannot load ALSA function " #name); bj_free(p_ctx); return 0;}
    ALSA_BIND(snd_pcm_close)
    ALSA_BIND(snd_pcm_drain)
    ALSA_BIND(snd_pcm_hw_params)
    ALSA_BIND(snd_pcm_hw_params_any)
    ALSA_BIND(snd_pcm_hw_params_free)
    ALSA_BIND(snd_pcm_hw_params_malloc)
    ALSA_BIND(snd_pcm_hw_params_set_access)
    ALSA_BIND(snd_pcm_hw_params_set_buffer_size_near)
    ALSA_BIND(snd_pcm_hw_params_set_channels)
    ALSA_BIND(snd_pcm_hw_params_set_format)
    ALSA_BIND(snd_pcm_hw_params_set_period_size_near)
    ALSA_BIND(snd_pcm_hw_params_set_rate_near)
    ALSA_BIND(snd_pcm_open)
#undef ALSA_BIND

    // Common Data
	bj_audio_layer* p_audio = bj_malloc(sizeof(bj_audio_layer));
	if (!p_audio) {
		bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "cannot allocate memory for alsa");
		return 0;
	}

	p_audio->dispose        = alsa_dispose_audio;
	p_audio->open_device    = alsa_open_device;
	p_audio->close_device   = alsa_close_device;
    p_audio->data = p_ctx;

	return p_audio;
}

bj_audio_layer_create_info alsa_layer_info = {
    .name = "alsa",
    .create = alsa_init_audio,
};

