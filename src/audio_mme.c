#include "config.h"

#if BJ_HAS_FEATURE(MME)

#include <banjo/audio.h>
#include <banjo/log.h>
#include <banjo/system.h>

#include "check.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmeapi.h>

// Things that will change later
#define N_ALLOWED_HEADERS 2
#define BJ_WAVE_SAMPLE_PER_SEC 44100
#define BJ_SAMPLE_T short

typedef UINT(WINAPI* pfn_waveOutGetNumDevs)(void);
typedef MMRESULT(WINAPI* pfn_waveOutGetDevCapsW)(UINT_PTR, LPWAVEOUTCAPSW, UINT);
typedef MMRESULT(WINAPI* pfn_waveOutOpen)(LPHWAVEOUT, UINT, LPCWAVEFORMATEX, DWORD_PTR, DWORD_PTR, DWORD);
typedef MMRESULT(WINAPI* pfn_waveOutGetErrorTextA)(MMRESULT, LPSTR, UINT);


typedef struct bj_audio_layer_data_t {
	void* dll;

	pfn_waveOutGetDevCapsW waveOutGetDevCapsW;
	pfn_waveOutGetNumDevs waveOutGetNumDevs;
	pfn_waveOutOpen waveOutOpen;
	pfn_waveOutGetErrorTextA waveOutGetErrorTextA;
} mme_ctx;

struct bj_audio_device_t {
	HWAVEOUT handle;
	WAVEHDR headers[N_ALLOWED_HEADERS];
};

static void mme_set_error(mme_ctx* mme, bj_error** p_error, MMRESULT result) {
	char msg[256] = { 0 };
	if (mme->waveOutGetErrorTextA(result, msg, 256) == MMSYSERR_NOERROR) {
		bj_set_error(p_error, BJ_ERROR_AUDIO, msg);
	} else {
		bj_set_error(p_error, BJ_ERROR_AUDIO, "cannot open audio device");
	}
}

static void CALLBACK waveOutProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
	// TODO
}

static bj_audio_device* mme_open_device(bj_audio_layer* p_audio, bj_error** p_error) {
	return 0;
}

static void mme_close_device(bj_audio_layer* p_audio, bj_audio_device* p_device) {
	(void)p_audio;
	bj_check(p_device);

	//bj_free(p_device->headers);
	bj_free(p_device);
}

static void mme_dispose_audio(bj_audio_layer* p_audio, bj_error** p_error) {
	bj_check(p_audio);

	bj_unload_library(p_audio->data->dll);

	bj_free(p_audio->data);
	bj_free(p_audio);
}

static bj_audio_layer* mme_init_audio(bj_error** p_error) {
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
	p_audio->open_device = mme_open_device;
	p_audio->close_device = mme_close_device;

	p_audio->data->waveOutGetNumDevs  = bj_get_symbol(p_audio->data->dll, "waveOutGetNumDevs");
	p_audio->data->waveOutGetDevCapsW = bj_get_symbol(p_audio->data->dll, "waveOutGetDevCapsW");
	p_audio->data->waveOutOpen = bj_get_symbol(p_audio->data->dll, "waveOutOpen");
	p_audio->data->waveOutGetErrorTextA = bj_get_symbol(p_audio->data->dll, "waveOutGetErrorTextA");

	return p_audio;
}

bj_audio_layer_create_info mme_layer_info = {
	.name = "mme",
	.create = mme_init_audio,
};

#endif