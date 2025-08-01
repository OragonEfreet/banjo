#include "config.h"

#if BJ_HAS_FEATURE(MME)

#include <banjo/log.h>
#include <banjo/system.h>

#include "audio_t.h"
#include "check.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmeapi.h>
#include <mmreg.h>

#define MME_BLOCK_COUNT 8
#define MME_SAMPLES_PER_BLOCK 256

typedef struct bj_audio_device_data_t {
    HWAVEOUT  hwDevice;
    WAVEHDR*  p_wave_headers;
    void*     p_buffer;
    uint64_t  sample_index;
    unsigned  block_count;
    unsigned  frames_per_block;
    HANDLE    thread;
    HANDLE    event;
    unsigned  next_block;
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
    if (MME.waveOutGetErrorTextA(result, msg, sizeof(msg)) == MMSYSERR_NOERROR) {
        bj_set_error(p_error, BJ_ERROR_AUDIO, msg);
    }
    else {
        bj_set_error(p_error, BJ_ERROR_AUDIO, "MME error");
    }
}

static void mme_unload_library(void) {
    if (MME.dll) {
        bj_unload_library(MME.dll);
    }
    bj_memzero(&MME, sizeof(MME));
}

static bj_bool mme_load_library(bj_error** p_error) {
    bj_check_or_return(MME.dll == 0, BJ_TRUE);

    MME.dll = bj_load_library("winmm.dll");
    if (!MME.dll) {
        bj_set_error(p_error, BJ_ERROR_INITIALIZE, "cannot load winmm.dll");
        return BJ_FALSE;
    }

#define MME_BIND(fn) \
        if (!(MME.fn = (pfn_##fn)bj_get_symbol(MME.dll, #fn))) { \
            bj_set_error(p_error, BJ_ERROR_AUDIO, "cannot load MME function " #fn); \
            mme_unload_library(); \
            return BJ_FALSE; \
        }

    MME_BIND(waveOutGetNumDevs)
        MME_BIND(waveOutGetDevCapsW)
        MME_BIND(waveOutOpen)
        MME_BIND(waveOutGetErrorTextA)
        MME_BIND(waveOutPrepareHeader)
        MME_BIND(waveOutWrite)
        MME_BIND(waveOutReset)
        MME_BIND(waveOutUnprepareHeader)

#undef MME_BIND
        return BJ_TRUE;
}

static void CALLBACK waveOutProcWrap(
    HWAVEOUT hWaveOut,
    UINT     uMsg,
    DWORD_PTR dwInstance,
    DWORD_PTR dwParam1,
    DWORD_PTR dwParam2
) {
    if (uMsg == WOM_DONE) {
        bj_audio_device* dev = (bj_audio_device*)dwInstance;
        mme_device* mme = (mme_device*)dev->data;
        SetEvent(mme->event);
    }
}

static void mme_close_device(bj_audio_layer* p_audio, bj_audio_device* p_device) {
    (void)p_audio;
    mme_device* mme = (mme_device*)p_device->data;

    // Signal thread to exit
    p_device->should_close = BJ_TRUE;
    if (mme->thread) {
        WaitForSingleObject(mme->thread, INFINITE);
        CloseHandle(mme->thread);
    }

    // Stop and clean up waveOut
    if (mme->hwDevice) {
        MME.waveOutReset(mme->hwDevice);
        for (unsigned i = 0; i < mme->block_count; ++i) {
            MME.waveOutUnprepareHeader(mme->hwDevice, &mme->p_wave_headers[i], sizeof(WAVEHDR));
        }
        MME.waveOutClose(mme->hwDevice);
    }

    // Free resources
    bj_free(mme->p_wave_headers);
    bj_free(mme->p_buffer);
    CloseHandle(mme->event);
    bj_free(mme);
    bj_free(p_device);
}

static DWORD WINAPI mme_playback_thread(LPVOID param) {
    bj_check(param);
    bj_audio_device* dev = (bj_audio_device*)param;
    mme_device* mme = (mme_device*)dev->data;

    while (!dev->should_close) {
        if (dev->should_reset) {
            mme->sample_index = 0;
            dev->should_reset = BJ_FALSE;
        }

        WAVEHDR* hdr = &mme->p_wave_headers[mme->next_block];
        if (hdr->dwFlags & WHDR_INQUEUE) {
            WaitForSingleObject(mme->event, 10);
            continue;
        }

        if (dev->playing) {
            dev->p_callback(
                hdr->lpData,
                mme->frames_per_block,
                &dev->properties,
                dev->p_callback_user_data,
                mme->sample_index
            );
        }
        else {
            // Fill silence
            size_t samples = mme->frames_per_block * dev->properties.channels;
            int16_t* buf = (int16_t*)hdr->lpData;
            for (size_t i = 0; i < samples; ++i) {
                buf[i] = dev->silence;
            }
        }

        MME.waveOutWrite(mme->hwDevice, hdr, sizeof(WAVEHDR));
        mme->sample_index += mme->frames_per_block;
        mme->next_block = (mme->next_block + 1) % mme->block_count;
    }

    return 0;
}

static bj_audio_device* mme_open_device(
    bj_audio_layer* p_audio,
    const bj_audio_properties* p_properties,
    bj_audio_callback_t        p_callback,
    void* p_callback_user_data,
    bj_error** p_error
) {
    bj_audio_device* p_device = bj_calloc(sizeof(bj_audio_device));
    mme_device* mme = bj_calloc(sizeof(mme_device));
    if (!p_device || !mme) {
        bj_set_error(p_error, BJ_ERROR_INITIALIZE, "cannot allocate audio device");
        return NULL;
    }

    // Load WinMM functions
    if (!mme_load_library(p_error)) {
        bj_free(mme);
        bj_free(p_device);
        return NULL;
    }

    // Prepare wave format
    WAVEFORMATEX wf = { 0 };
    wf.wFormatTag = BJ_AUDIO_FORMAT_FLOAT(p_properties->format)
        ? WAVE_FORMAT_IEEE_FLOAT
        : WAVE_FORMAT_PCM;
    wf.nChannels = (WORD)p_properties->channels;
    wf.nSamplesPerSec = p_properties->sample_rate;
    wf.wBitsPerSample = (WORD)BJ_AUDIO_FORMAT_WIDTH(p_properties->format);
    wf.nBlockAlign = (WORD)(wf.nChannels * (wf.wBitsPerSample / 8));
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
    wf.cbSize = 0;

    // Open device
    HWAVEOUT hwDevice = 0;
    UINT n_devs = MME.waveOutGetNumDevs();
    for (UINT d = 0; d < n_devs; ++d) {
        if (MME.waveOutOpen(&hwDevice, d, &wf,
            (DWORD_PTR)waveOutProcWrap,
            (DWORD_PTR)p_device,
            CALLBACK_FUNCTION) == MMSYSERR_NOERROR) {
            break;
        }
    }
    if (!hwDevice) {
        bj_set_error(p_error, BJ_ERROR_AUDIO, "cannot open audio device");
        bj_free(mme);
        bj_free(p_device);
        return NULL;
    }

    // Initialize device fields
    p_device->p_callback = p_callback;
    p_device->p_callback_user_data = p_callback_user_data;
    p_device->properties = *p_properties;
    p_device->silence = 0;
    p_device->data = mme;

    // Setup mme device struct
    mme->hwDevice = hwDevice;
    mme->block_count = MME_BLOCK_COUNT;
    mme->frames_per_block = MME_SAMPLES_PER_BLOCK;
    mme->sample_index = 0;
    mme->next_block = 0;
    mme->event = CreateEvent(NULL, FALSE, FALSE, NULL);

    // Compute sizes
    WORD bytes_per_sample = wf.wBitsPerSample / 8;
    WORD channels = wf.nChannels;
    size_t samples_per_block_allchan = mme->frames_per_block * channels;
    size_t bytes_per_block = samples_per_block_allchan * bytes_per_sample;

    // Allocate buffers
    mme->p_wave_headers = bj_calloc(sizeof(WAVEHDR) * mme->block_count);
    mme->p_buffer = bj_calloc(bytes_per_block * mme->block_count);
    if (!mme->p_wave_headers || !mme->p_buffer) {
        bj_set_error(p_error, BJ_ERROR_INITIALIZE, "cannot allocate audio buffer data");
        mme_close_device(p_audio, p_device);
        return NULL;
    }

    // Prepare headers
    for (unsigned i = 0; i < mme->block_count; ++i) {
        WAVEHDR* hdr = &mme->p_wave_headers[i];
        hdr->lpData = (LPSTR)((uint8_t*)mme->p_buffer + i * bytes_per_block);
        hdr->dwBufferLength = (DWORD)bytes_per_block;
        MME.waveOutPrepareHeader(hwDevice, hdr, sizeof(WAVEHDR));
    }

    // Start playback thread
    mme->thread = CreateThread(NULL, 0, mme_playback_thread, p_device, 0, NULL);
    return p_device;
}

static void mme_dispose_audio(bj_audio_layer* p_audio, bj_error** p_error) {
    (void)p_error;
    bj_check(p_audio);
    mme_unload_library();
    bj_free(p_audio);
}

static bj_audio_layer* mme_init_audio(bj_error** p_error) {
    bj_audio_layer* layer = bj_malloc(sizeof(bj_audio_layer));
    if (!layer) {
        bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "cannot allocate mme layer");
        return NULL;
    }
    layer->end = mme_dispose_audio;
    layer->open_device = mme_open_device;
    layer->close_device = mme_close_device;
    layer->data = NULL;
    return layer;
}

bj_audio_layer_create_info mme_layer_info = {
    .name = "mme",
    .create = mme_init_audio,
};

#endif // BJ_HAS_FEATURE(MME)
