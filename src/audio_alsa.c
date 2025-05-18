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
#include <math.h>

#define BJ_AUDIO_PERIOD_FRAMES 512
#define BJ_AUDIO_BUFFER_FRAMES (BJ_AUDIO_PERIOD_FRAMES * 4)
#define BJ_AUDIO_FORMAT SND_PCM_FORMAT_S16_LE
#define BJ_AUDIO_CHANNELS 1

typedef struct bj_audio_device_data_t {
    bj_bool    should_stop;
    snd_pcm_t* p_handle;
    pthread_t  playback_thread;
} alsa_device;

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
typedef snd_pcm_sframes_t(*pfn_snd_pcm_avail_update)(snd_pcm_t*);
typedef int(*pfn_snd_pcm_prepare)(snd_pcm_t*);
typedef snd_pcm_sframes_t(*pfn_snd_pcm_writei)(snd_pcm_t*, const void*, snd_pcm_uframes_t);
typedef snd_pcm_sframes_t(*pfn_snd_pcm_writei)(snd_pcm_t*, const void*, snd_pcm_uframes_t);
typedef const char*(*pfn_snd_strerror)(int);


static struct alsa_lib_t {
    void*                                      p_handle;
    pfn_snd_pcm_hw_params                      snd_pcm_hw_params;
    pfn_snd_pcm_hw_params_any                  snd_pcm_hw_params_any;
    pfn_snd_pcm_hw_params_free                 snd_pcm_hw_params_free;
    pfn_snd_pcm_hw_params_malloc               snd_pcm_hw_params_malloc;
    pfn_snd_pcm_hw_params_set_access           snd_pcm_hw_params_set_access;
    pfn_snd_pcm_hw_params_set_buffer_size_near snd_pcm_hw_params_set_buffer_size_near;
    pfn_snd_pcm_hw_params_set_channels         snd_pcm_hw_params_set_channels;
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
} ALSA = {0};

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
    ALSA_BIND(snd_pcm_prepare)
    ALSA_BIND(snd_pcm_writei)
    ALSA_BIND(snd_strerror)
#undef ALSA_BIND

    return BJ_TRUE;
}


// Notes in Hz: A4, C5, E5, G5, A5
double notes[] = { 440.0, 523.25, 659.25, 783.99, 880.0 };
const int note_count = sizeof(notes) / sizeof(notes[0]);

static void* playback_thread(void* p_data) {
    alsa_device* p_alsa_device = ((bj_audio_device*)p_data)->data;
    snd_pcm_t* pcm_handle = p_alsa_device->p_handle;

    int16_t buffer[BJ_AUDIO_PERIOD_FRAMES];
    int sample_index = 0;
    double current_freq = notes[0];
    int last_second = -1;

    while (p_alsa_device->should_stop == BJ_FALSE) {
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

        // Change note every second
        int now = time(NULL);
        if (now != last_second) {
            last_second = now;
            current_freq = notes[now % note_count];
            printf("Switching to frequency: %.2f Hz\n", current_freq);
        }

        if (avail >= BJ_AUDIO_PERIOD_FRAMES) {
            for (int i = 0; i < BJ_AUDIO_PERIOD_FRAMES; ++i) {
                double t = (double)(sample_index++) / BJ_AUDIO_SAMPLE_RATE;
                buffer[i] = (int16_t)(BJ_AUDIO_AMPLITUDE * sin(2 * M_PI * current_freq * t));
            }

            int err = ALSA.snd_pcm_writei(pcm_handle, buffer, BJ_AUDIO_PERIOD_FRAMES);
            if (err == -EPIPE) {
                bj_err("write underrun!");
                ALSA.snd_pcm_prepare(pcm_handle);
            } else if (err < 0) {
                bj_err("write error: %s", ALSA.snd_strerror(err));
                break;
            }
        } else {
            usleep(100); // Yield a bit if not enough space
        }
    }

    return NULL;
}

static void alsa_close_device(bj_audio_layer* p_audio, bj_audio_device* p_device) {
    bj_check(p_audio);
    bj_check(p_device);

    alsa_device* alsa_dev = p_device->data;

    alsa_dev->should_stop = BJ_TRUE;

    if(alsa_dev != 0) {
        if(alsa_dev->playback_thread) {
            pthread_join(alsa_dev->playback_thread, 0);
        }

        if(alsa_dev->p_handle) {
           ALSA.snd_pcm_drain(alsa_dev->p_handle);
           ALSA.snd_pcm_close(alsa_dev->p_handle);
        }
    }

    bj_free(p_device->data);
    bj_free(p_device);
}

static bj_audio_device* alsa_open_device(bj_audio_layer* p_audio, bj_error** p_error) {

    bj_audio_device* p_device = bj_calloc(sizeof(bj_audio_device));
    if(p_device == 0) {
        bj_set_error(p_error, BJ_ERROR_INITIALIZE, "cannot allocate audio device");
        return 0;
    }
    p_device->data = bj_calloc(sizeof(alsa_device));
    if(p_device->data == 0) {
        bj_set_error(p_error, BJ_ERROR_INITIALIZE, "cannot allocate audio device data");
        alsa_close_device(p_audio, p_device);
        return 0;
    }

    snd_pcm_hw_params_t* params   = 0;
    unsigned int sample_rate      = BJ_AUDIO_SAMPLE_RATE;
    snd_pcm_uframes_t period      = BJ_AUDIO_PERIOD_FRAMES;
    snd_pcm_uframes_t buffer_size = BJ_AUDIO_BUFFER_FRAMES;

    if(ALSA.snd_pcm_open(&p_device->data->p_handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
        bj_set_error(p_error, BJ_ERROR_AUDIO, "cannot open 'default' device");
        alsa_close_device(p_audio, p_device);
        return 0;
    }

    ALSA.snd_pcm_hw_params_malloc(&params);
    ALSA.snd_pcm_hw_params_any(p_device->data->p_handle, params);
    ALSA.snd_pcm_hw_params_set_access(p_device->data->p_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    ALSA.snd_pcm_hw_params_set_format(p_device->data->p_handle, params, BJ_AUDIO_FORMAT);
    ALSA.snd_pcm_hw_params_set_channels(p_device->data->p_handle, params, BJ_AUDIO_CHANNELS);
    ALSA.snd_pcm_hw_params_set_rate_near(p_device->data->p_handle, params, &sample_rate, 0);
    ALSA.snd_pcm_hw_params_set_period_size_near(p_device->data->p_handle, params, &period, 0);
    ALSA.snd_pcm_hw_params_set_buffer_size_near(p_device->data->p_handle, params, &buffer_size);

    if(ALSA.snd_pcm_hw_params(p_device->data->p_handle, params) < 0) {
        bj_set_error(p_error, BJ_ERROR_AUDIO, "cannot configure audio device");
        ALSA.snd_pcm_hw_params_free(params);
        alsa_close_device(p_audio, p_device);
        return 0;
    }
    ALSA.snd_pcm_prepare(p_device->data->p_handle);

    ALSA.snd_pcm_hw_params_free(params);

    pthread_t thread;
    pthread_create(&thread, 0, playback_thread, (void*)p_device);

    p_device->data->playback_thread = thread;

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

	p_audio->dispose        = alsa_dispose_audio;
	p_audio->open_device    = alsa_open_device;
	p_audio->close_device   = alsa_close_device;
    p_audio->data = 0;

	return p_audio;
}

bj_audio_layer_create_info alsa_layer_info = {
    .name = "alsa",
    .create = alsa_init_audio,
};

