#include "config.h"

#if BJ_HAS_FEATURE(MME)

#include <banjo/audio.h>
#include <banjo/log.h>
#include <banjo/system.h>

#include "check.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmeapi.h>


typedef UINT(WINAPI *pfn_waveOutGetNumDevs)(void);

typedef struct bj_audio_layer_data_t {
	void* dll;

	pfn_waveOutGetNumDevs waveOutGetNumDevs;
} mme_ctx;

static void mme_detect_devices(bj_audio_layer* p_audio) {
	mme_ctx* mme = p_audio->data;
	
	UINT num_devices = mme->waveOutGetNumDevs();

	bj_trace("found %d devices", num_devices);


}

void mme_dispose_audio(bj_audio_layer* p_audio, bj_error** p_error) {
	bj_check(p_audio);

	bj_unload_library(p_audio->data->dll);

	bj_free(p_audio->data);
	bj_free(p_audio);
}

bj_audio_layer* mme_init_audio(bj_error** p_error) {
	bj_audio_layer* p_audio = bj_malloc(sizeof(bj_audio_layer));

	if (!p_audio) {
		bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "cannot allocate memory for MME");
		return 0;
	}

	p_audio->data = bj_malloc(sizeof(mme_ctx));
	if (!p_audio->data) {
		bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "cannot allocate memory for MME");
		mme_dispose_audio(p_audio, 0);
		return 0;
	}

	p_audio->data->dll = bj_load_library("winmm.dll");
	if (!p_audio->data->dll) {
		bj_set_error(p_error, BJ_ERROR_INITIALIZE, "cannot load winmm.dll");
		mme_dispose_audio(p_audio, 0);
		return 0;
	}
	
	p_audio->dispose = mme_dispose_audio;

	p_audio->data->waveOutGetNumDevs = bj_get_symbol(p_audio->data->dll, "waveOutGetNumDevs");

	mme_detect_devices(p_audio);

	return p_audio;
}

bj_audio_layer_create_info mme_layer_info = {
	.name = "mme_ctx",
	.create = mme_init_audio,
};

#endif