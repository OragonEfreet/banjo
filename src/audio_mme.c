#include "config.h"

#if BJ_HAS_FEATURE(MME)

#include <banjo/log.h>
#include <banjo/system.h>

#include "audio_t.h"
#include "check.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmeapi.h>

#define MME_BLOCK_COUNT 8
#define MME_SAMPLES_PER_BLOCK 256

typedef struct bj_audio_device_data_t {
	HWAVEOUT hwDevice;
	WAVEHDR* p_wave_headers;
	int16_t* p_buffer;
	uint64_t sample_index;
	unsigned block_count;
	unsigned samples_per_block;
	HANDLE   thread;
	HANDLE   event;
	unsigned next_block;
} mme_device;

typedef UINT(WINAPI* pfn_waveOutGetNumDevs)(void);
typedef MMRESULT(WINAPI* pfn_waveOutGetDevCapsW)(UINT_PTR, LPWAVEOUTCAPSW, UINT);
typedef MMRESULT(WINAPI* pfn_waveOutOpen)(LPHWAVEOUT, UINT, LPCWAVEFORMATEX, DWORD_PTR, DWORD_PTR, DWORD);
typedef MMRESULT(WINAPI* pfn_waveOutGetErrorTextA)(MMRESULT, LPSTR, UINT);
typedef MMRESULT(WINAPI* pfn_waveOutClose)(HWAVEOUT);
typedef MMRESULT(WINAPI* pfn_waveOutPrepareHeader)(HWAVEOUT, LPWAVEHDR, UINT);
typedef MMRESULT(WINAPI* pfn_waveOutWrite)(HWAVEOUT, LPWAVEHDR, UINT);
typedef MMRESULT(WINAPI* pfn_waveOutReset)(HWAVEOUT);
typedef MMRESULT(WINAPI* pfn_waveOutUnprepareHeader)(HWAVEOUT, LPWAVEHDR, UINT);

static struct mme_lib_t {
	void* dll;
	pfn_waveOutGetDevCapsW waveOutGetDevCapsW;
	pfn_waveOutGetNumDevs waveOutGetNumDevs;
	pfn_waveOutOpen waveOutOpen;
	pfn_waveOutClose waveOutClose;
	pfn_waveOutWrite waveOutWrite;
	pfn_waveOutReset waveOutReset;
	pfn_waveOutGetErrorTextA waveOutGetErrorTextA;
	pfn_waveOutPrepareHeader waveOutPrepareHeader;
	pfn_waveOutUnprepareHeader waveOutUnprepareHeader;
} MME = { 0 };

static void mme_set_error(bj_error** p_error, MMRESULT result) {
	char msg[256] = { 0 };
	if (MME.waveOutGetErrorTextA(result, msg, 256) == MMSYSERR_NOERROR) {
		bj_set_error(p_error, BJ_ERROR_AUDIO, msg);
	} else {
		bj_set_error(p_error, BJ_ERROR_AUDIO, "MME error");
	}
}

static void mme_unload_library(void) {
	if (MME.dll != 0) {
		bj_unload_library(MME.dll);
	}
	bj_memzero(&MME, sizeof(struct mme_lib_t));
}

static bj_bool mme_load_library(bj_error** p_error) {
	bj_check_or_return(MME.dll == 0, BJ_TRUE);

	MME.dll = bj_load_library("winmm.dll");
	if (!MME.dll) {
		bj_set_error(p_error, BJ_ERROR_INITIALIZE, "cannot load winmm.dll");
		return BJ_FALSE;
	}

#define MME_BIND(name) if(!(MME.name = (pfn_ ## name)bj_get_symbol(MME.dll, #name))) {bj_set_error(p_error, BJ_ERROR_AUDIO, "cannot load MME function " #name); mme_unload_library(); return 0;}
	MME_BIND(waveOutGetNumDevs)
	MME_BIND(waveOutGetDevCapsW);
	MME_BIND(waveOutOpen);
	MME_BIND(waveOutClose);
	MME_BIND(waveOutGetErrorTextA);
	MME_BIND(waveOutPrepareHeader);
	MME_BIND(waveOutWrite);
	MME_BIND(waveOutReset);
	MME_BIND(waveOutUnprepareHeader);
#undef MME_BIND

	return BJ_TRUE;
}

static void CALLBACK waveOutProcWrap(HWAVEOUT hWaveOut, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	if (uMsg == WOM_DONE) {
		mme_device* mme_dev = (mme_device*)((bj_audio_device*)dwInstance)->data;
		SetEvent(mme_dev->event);
	}
}

static void mme_close_device(bj_audio_layer* p_audio, bj_audio_device* p_device) {
	(void)p_audio;
	mme_device* mme = (mme_device*)p_device->data;

	p_device->should_close = BJ_TRUE;
	if (mme->thread) {
		WaitForSingleObject(mme->thread, INFINITE);
		CloseHandle(mme->thread);
	}

	if (mme->hwDevice) {
		MME.waveOutReset(mme->hwDevice);
		for (int i = 0; i < mme->block_count; ++i) {
			MME.waveOutUnprepareHeader(mme->hwDevice, &mme->p_wave_headers[i], sizeof(WAVEHDR));
		}
		MME.waveOutClose(mme->hwDevice);
	}

	bj_free(mme->p_wave_headers);
	bj_free(mme->p_buffer);
	CloseHandle(mme->event);
	bj_free(mme);
	bj_free(p_device);
}

static DWORD WINAPI mme_playback_thread(LPVOID param) {
	bj_check(param);
	bj_audio_device* dev = (bj_audio_device*)param;
	mme_device* mme_dev = (mme_device*)dev->data;

	while (!dev->should_close) {
		if (dev->should_reset) {
			mme_dev->sample_index = 0;
			dev->should_reset = BJ_FALSE;
		}

		WAVEHDR* hdr = &mme_dev->p_wave_headers[mme_dev->next_block];

		if (hdr->dwFlags & WHDR_INQUEUE) {
			WaitForSingleObject(mme_dev->event, 10);
			continue;
		}

		if (dev->playing == BJ_TRUE) {
			dev->p_callback(
				(int16_t*)hdr->lpData, mme_dev->samples_per_block, &dev->properties,
				dev->p_callback_user_data, mme_dev->sample_index
			);
		} else {
			const int16_t val = dev->properties.silence;
			for (unsigned i = 0; i < mme_dev->samples_per_block; ++i) {
				((int16_t*)hdr->lpData)[i] = val;
			}
		}

		MME.waveOutWrite(mme_dev->hwDevice, hdr, sizeof(WAVEHDR));
		mme_dev->sample_index += mme_dev->samples_per_block;
		mme_dev->next_block = (mme_dev->next_block + 1) % mme_dev->block_count;
	}

	return 0;
}

static bj_audio_device* mme_open_device(
	bj_audio_layer* p_audio,
	bj_error** p_error,
	bj_audio_callback_t p_callback,
	void* p_callback_user_data
) {
	bj_audio_device* p_device = bj_calloc(sizeof(bj_audio_device));
	mme_device* mme_dev = bj_calloc(sizeof(mme_device));

	if (p_device == 0 || mme_dev == 0) {
		bj_set_error(p_error, BJ_ERROR_INITIALIZE, "cannot allocate audio device");
		return 0;
	}

	// Candidates for device field:
	HWAVEOUT hwDevice = 0;

	const WAVEFORMATEX wave_format = {
		.wFormatTag = WAVE_FORMAT_PCM,
		.nChannels = BJ_AUDIO_CHANNELS,
		.nSamplesPerSec = BJ_AUDIO_SAMPLE_RATE,
		.wBitsPerSample = sizeof(int16_t) * 8,
		.nBlockAlign = sizeof(int16_t) * BJ_AUDIO_CHANNELS,
		.nAvgBytesPerSec = sizeof(int16_t) * BJ_AUDIO_CHANNELS * BJ_AUDIO_SAMPLE_RATE,
		.cbSize = 0
	};

	// Loop through devices to take the first available
	const UINT n_devices = MME.waveOutGetNumDevs();
	for (UINT d = 0; d < n_devices; ++d) {
		if (MME.waveOutOpen(&hwDevice, d, &wave_format, waveOutProcWrap, p_device, CALLBACK_FUNCTION) == MMSYSERR_NOERROR) {
			break;
		}
	}

	if (hwDevice == 0) {
		bj_set_error(p_error, BJ_ERROR_AUDIO, "cannot find suitable audio driver");
		return 0;
	}



	// TODO maybe read from the open device?
	p_device->p_callback             = p_callback;
	p_device->p_callback_user_data   = p_callback_user_data;
	p_device->properties.amplitude   = BJ_AUDIO_AMPLITUDE;
	p_device->properties.channels    = BJ_AUDIO_CHANNELS;
	p_device->properties.sample_rate = BJ_AUDIO_SAMPLE_RATE;
	p_device->properties.silence     = 0;
	p_device->data                   = mme_dev;

	mme_dev->hwDevice          = hwDevice;
	mme_dev->block_count       = MME_BLOCK_COUNT;
	mme_dev->samples_per_block = MME_SAMPLES_PER_BLOCK;
	mme_dev->sample_index      = 0;
	mme_dev->event             = CreateEvent(NULL, FALSE, FALSE, NULL);
	mme_dev->p_wave_headers    = bj_calloc(sizeof(WAVEHDR) * mme_dev->block_count);
	mme_dev->p_buffer          = bj_calloc(sizeof(int16_t) * mme_dev->block_count * mme_dev->samples_per_block);

	if (mme_dev->p_buffer == 0 || mme_dev->p_wave_headers == 0) {
		bj_set_error(p_error, BJ_ERROR_INITIALIZE, "cannot allocate audio buffer data");
		mme_close_device(p_audio, p_device);
		return 0;
	}

	for (unsigned int i = 0; i < mme_dev->block_count; ++i) {
		WAVEHDR* hdr = &mme_dev->p_wave_headers[i];
		hdr->lpData = (LPSTR)(mme_dev->p_buffer + i * mme_dev->samples_per_block);
		hdr->dwBufferLength = sizeof(int16_t) * mme_dev->samples_per_block;
		MME.waveOutPrepareHeader(hwDevice, hdr, sizeof(WAVEHDR));
	}

	mme_dev->thread = CreateThread(NULL, 0, mme_playback_thread, p_device, 0, NULL);
	return p_device;
}




static void mme_dispose_audio(bj_audio_layer* p_audio, bj_error** p_error) {
	(void)p_error;
	bj_check(p_audio);
	mme_unload_library();
	bj_free(p_audio);
}

static bj_audio_layer* mme_init_audio(bj_error** p_error) {
	
	if (!mme_load_library(p_error)) {
		return 0;
	}

	bj_audio_layer* p_audio = bj_malloc(sizeof(bj_audio_layer));
	if (!p_audio) {
		bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "cannot allocate memory for mme");
		return 0;
	}

	p_audio->end          = mme_dispose_audio;
	p_audio->open_device  = mme_open_device;
	p_audio->close_device = mme_close_device;
	p_audio->data         = 0;
	return p_audio;
}

bj_audio_layer_create_info mme_layer_info = {
	.name = "mme",
	.create = mme_init_audio,
};

#endif
