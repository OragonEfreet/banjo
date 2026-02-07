

#ifdef BJ_CONFIG_MME_BACKEND

#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/system.h>

#include <audio.h>
#include <check.h>
#include <audio_layer.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmeapi.h>
#include <mmreg.h>

#define MME_BLOCK_COUNT 8
#define MME_SAMPLES_PER_BLOCK 256

struct mme_device {
    struct bj_audio_device common;
    HWAVEOUT               hwDevice;
    WAVEHDR*               p_wave_headers;
    void*                  p_buffer;
    uint64_t               sample_index;
    unsigned               block_count;
    unsigned               frames_per_block;
    HANDLE                 thread;
    HANDLE                 event;
    unsigned               next_block;
    size_t                 bytes_per_sample;
};

typedef UINT(WINAPI* pfn_waveOutGetNumDevs)(void);
typedef MMRESULT(WINAPI* pfn_waveOutGetDevCapsW)(UINT_PTR, LPWAVEOUTCAPSW, UINT);
typedef MMRESULT(WINAPI* pfn_waveOutOpen)(LPHWAVEOUT, UINT, LPCWAVEFORMATEX, DWORD_PTR, DWORD_PTR, DWORD);
typedef MMRESULT(WINAPI* pfn_waveOutGetErrorTextA)(MMRESULT, LPSTR, UINT);
typedef MMRESULT(WINAPI* pfn_waveOutClose)(HWAVEOUT);
typedef MMRESULT(WINAPI* pfn_waveOutPrepareHeader)(HWAVEOUT, LPWAVEHDR, UINT);
typedef MMRESULT(WINAPI* pfn_waveOutWrite)(HWAVEOUT, LPWAVEHDR, UINT);
typedef MMRESULT(WINAPI* pfn_waveOutReset)(HWAVEOUT);
typedef MMRESULT(WINAPI* pfn_waveOutUnprepareHeader)(HWAVEOUT, LPWAVEHDR, UINT);

static struct mme_lib {
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

static void mme_unload_library(void) {
    if (MME.dll) {
        bj_unload_library(MME.dll);
    }
    bj_memzero(&MME, sizeof(MME));
}

static bj_bool mme_load_library(struct bj_error** p_error) {
    bj_check_or_return(MME.dll == 0, BJ_TRUE);

    MME.dll = bj_load_library("winmm.dll");
    if (!MME.dll) {
        bj_set_error(p_error, BJ_ERROR_INITIALIZE, "cannot load winmm.dll");
        return BJ_FALSE;
    }

#define MME_BIND(fn) \
    if (!(MME.fn = (pfn_##fn)bj_library_symbol(MME.dll, #fn))) { \
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
    (void)hWaveOut;
    (void)dwParam1;
    (void)dwParam2;
    if (uMsg == WOM_DONE) {
        struct bj_audio_device* dev = (struct bj_audio_device*)dwInstance;
        struct mme_device* mme = (struct mme_device*)dev;
        SetEvent(mme->event);
    }
}

static void mme_close_device(struct bj_audio_device* dev) {
    struct mme_device* mme_dev= (struct mme_device*)dev;

    dev->should_close = BJ_TRUE;
    if (mme_dev->thread) {
        WaitForSingleObject(mme_dev->thread, INFINITE);
        CloseHandle(mme_dev->thread);
    }

    if (mme_dev->hwDevice) {
        MME.waveOutReset(mme_dev->hwDevice);
        for (unsigned i = 0; i < mme_dev->block_count; ++i) {
            MME.waveOutUnprepareHeader(mme_dev->hwDevice, &mme_dev->p_wave_headers[i], sizeof(WAVEHDR));
        }
        MME.waveOutClose(mme_dev->hwDevice);
    }

    bj_free(mme_dev->p_wave_headers);
    bj_free(mme_dev->p_buffer);
    CloseHandle(mme_dev->event);
    bj_free(mme_dev);

}

static DWORD WINAPI mme_playback_thread(LPVOID param) {
    bj_check_or_0(param);
    struct mme_device* mme_dev = (struct mme_device*)param;
    struct bj_audio_device* dev = (struct bj_audio_device*)param;

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

        if (dev->playing) {
            dev->callback(
                hdr->lpData,
                mme_dev->frames_per_block,
                &dev->properties,
                dev->callback_user_data,
                mme_dev->sample_index
            );
        }
        else {
            size_t total_frames = mme_dev->frames_per_block;
            size_t channels = dev->properties.channels;
            size_t bps = mme_dev->bytes_per_sample;
            size_t block_bytes = total_frames * channels * bps;

            if (dev->silence == 0) {
                bj_memset(hdr->lpData, 0, block_bytes);
            } else {
                uint8_t* dst = (uint8_t*)hdr->lpData;
                for (size_t i = 0; i < total_frames * channels; ++i) {
                    bj_memcpy(dst + i * bps, &dev->silence, bps);
                }
            }
        }

        MME.waveOutWrite(mme_dev->hwDevice, hdr, sizeof(WAVEHDR));
        mme_dev->sample_index += mme_dev->frames_per_block;
        mme_dev->next_block = (mme_dev->next_block + 1) % mme_dev->block_count;
    }

    return 0;
}

static struct bj_audio_device* mme_open_device(
    const struct bj_audio_properties* p_properties,
    bj_audio_callback_fn              p_callback,
    void*                             p_callback_user_data,
    struct bj_error**                 p_error
) {
    //struct bj_audio_device* p_device = bj_calloc(sizeof(struct bj_audio_device));
    struct mme_device* mme_dev = bj_calloc(sizeof(struct mme_device));
    if (mme_dev == 0) {
        bj_set_error(p_error, BJ_ERROR_INITIALIZE, "cannot allocate audio device");
        return 0;
    }

    if (!mme_load_library(p_error)) {
        bj_free(mme_dev);
        return NULL;
    }

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

    HWAVEOUT hwDevice = 0;
    UINT n_devs = MME.waveOutGetNumDevs();
    for (UINT d = 0; d < n_devs; ++d) {
        if (MME.waveOutOpen(&hwDevice, d, &wf,
            (DWORD_PTR)waveOutProcWrap,
            (DWORD_PTR)mme_dev,
            CALLBACK_FUNCTION) == MMSYSERR_NOERROR) {
            break;
        }
    }
    if (!hwDevice) {
        bj_set_error(p_error, BJ_ERROR_AUDIO, "cannot open audio device");
        bj_free(mme_dev);
        return NULL;
    }

    mme_dev->common.callback = p_callback;
    mme_dev->common.callback_user_data = p_callback_user_data;
    mme_dev->common.properties = *p_properties;

    // Compute proper silence value
    if (wf.wFormatTag == WAVE_FORMAT_PCM && wf.wBitsPerSample == 8) {
        mme_dev->common.silence = 0x80; // unsigned 8-bit silence
    } else {
        mme_dev->common.silence = 0; // signed PCM and float32 silence
    }

    mme_dev->hwDevice = hwDevice;
    mme_dev->block_count = MME_BLOCK_COUNT;
    mme_dev->frames_per_block = MME_SAMPLES_PER_BLOCK;
    mme_dev->sample_index = 0;
    mme_dev->next_block = 0;
    mme_dev->event = CreateEvent(NULL, FALSE, FALSE, NULL);
    mme_dev->bytes_per_sample = wf.wBitsPerSample / 8;

    WORD channels = wf.nChannels;
    size_t samples_per_block_allchan = mme_dev->frames_per_block * channels;
    size_t bytes_per_block = samples_per_block_allchan * mme_dev->bytes_per_sample;

    mme_dev->p_wave_headers = bj_calloc(sizeof(WAVEHDR) * mme_dev->block_count);
    mme_dev->p_buffer = bj_calloc(bytes_per_block * mme_dev->block_count);
    if (!mme_dev->p_wave_headers || !mme_dev->p_buffer) {
        bj_set_error(p_error, BJ_ERROR_INITIALIZE, "cannot allocate audio buffer data");
        mme_close_device((struct bj_audio_device*)mme_dev);
        return NULL;
    }

    for (unsigned i = 0; i < mme_dev->block_count; ++i) {
        WAVEHDR* hdr = &mme_dev->p_wave_headers[i];
        hdr->lpData = (LPSTR)((uint8_t*)mme_dev->p_buffer + i * bytes_per_block);
        hdr->dwBufferLength = (DWORD)bytes_per_block;
        MME.waveOutPrepareHeader(hwDevice, hdr, sizeof(WAVEHDR));
    }

    mme_dev->thread = CreateThread(NULL, 0, mme_playback_thread, mme_dev, 0, NULL);
    return (struct bj_audio_device*)mme_dev;
}

static void mme_dispose_audio(struct bj_error** p_error) {
    (void)p_error;
    mme_unload_library();
}

static bj_bool mme_init_audio(
    struct bj_audio_layer* layer,
    struct bj_error** error
) {
    layer->end          = mme_dispose_audio;
    layer->open_device  = mme_open_device;
    layer->close_device = mme_close_device;
    return BJ_TRUE;
}

struct bj_audio_layer_create_info mme_audio_layer_info = {
    .name   = "mme",
    .create = mme_init_audio,
};

#endif
