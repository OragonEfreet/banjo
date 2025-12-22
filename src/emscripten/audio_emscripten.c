

#ifdef BJ_CONFIG_EMSCRIPTEN_BACKEND

// Note: This file is AI-generated.
// TODO: I'll remake it myself later on.

#include <emscripten.h>
#include <banjo/audio.h>
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include <audio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <audio_layer.h>

// Internal per-device data for Emscripten/WebAudio
struct emscripten_device {
    struct   bj_audio_device common;
    float*   p_buffer;         ///< Interleaved float buffer
    unsigned frames_per_block; ///< Number of frames per JS callback
    uint64_t sample_index;     ///< Total frames generated so far
    int      channels;         ///< Number of output channels
    int      silence_timer;    ///< ID for fallback silence timer
};

// Forward declarations
EMSCRIPTEN_KEEPALIVE void audio_emscripten_process(uintptr_t device_ptr);

static void emscripten_close_device(struct bj_audio_device* p_device);

// JS initialization: set up AudioContext and ScriptProcessorNode
EM_JS(int, js_audio_init, (uintptr_t device_ptr, float* buffer, int frames, int channels), {
    if (typeof(Module['BJ_EM_AUDIO']) === 'undefined') Module['BJ_EM_AUDIO'] = {};
    var BJ_EM = Module['BJ_EM_AUDIO'];
    var AudioContext = window.AudioContext || window.webkitAudioContext;
    if (!BJ_EM.audioContext) BJ_EM.audioContext = new AudioContext();
    var ctx = BJ_EM.audioContext;

    // Create ScriptProcessorNode
    BJ_EM.scriptNode = ctx.createScriptProcessor(frames, 0, channels);
    BJ_EM.scriptNode.onaudioprocess = function(e) {
        if (ctx.state === 'suspended') return;
        // Generate fresh samples in C
        Module._audio_emscripten_process(device_ptr);
        // Copy interleaved samples into output
        var ptr = buffer >>> 2;
        for (var ch = 0; ch < channels; ++ch) {
            var out = e.outputBuffer.getChannelData(ch);
            for (var i = 0; i < frames; ++i) {
                out[i] = HEAPF32[ptr + i*channels + ch];
            }
        }
    };
    BJ_EM.scriptNode.connect(ctx.destination);

    // Handle suspended context: play silence until resumed by user
    if (ctx.state === 'suspended') {
        BJ_EM.silenceBuffer = ctx.createBuffer(channels, frames, ctx.sampleRate);
        for (var c = 0; c < channels; ++c) {
            BJ_EM.silenceBuffer.getChannelData(c).fill(0.0);
        }
        BJ_EM.silenceTimer = setInterval(function() {
            Module._audio_emscripten_process(device_ptr);
        }, (frames / ctx.sampleRate) * 1000);
        document.addEventListener('click', function resumeHandler() {
            ctx.resume();
            clearInterval(BJ_EM.silenceTimer);
            document.removeEventListener('click', resumeHandler);
        });
    }
    return 1;
});

static struct bj_audio_device* emscripten_open_device(
    const struct bj_audio_properties* p_properties,
    bj_audio_callback_fn              p_callback,
    void*                             p_callback_user_data,
    struct bj_error**                 p_error
) {
    struct emscripten_device* em_dev = bj_calloc(sizeof(struct emscripten_device));
    if (!em_dev) { 
        bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "alloc device");
        return 0; 
    }

    struct bj_audio_device* dev = &em_dev->common;
    dev->callback               = p_callback;
    dev->callback_user_data     = p_callback_user_data;
    dev->properties.format      = BJ_AUDIO_FORMAT_F32;
    dev->properties.amplitude   = 1;
    dev->properties.channels    = p_properties ? p_properties->channels : 1;
    dev->properties.sample_rate = p_properties ? p_properties->sample_rate : 44100;

    em_dev->frames_per_block = 512;
    em_dev->channels         = dev->properties.channels;
    em_dev->sample_index     = 0;
    em_dev->silence_timer    = -1;

    size_t samples = em_dev->frames_per_block * em_dev->channels;
    em_dev->p_buffer = bj_malloc(sizeof(float) * samples);
    if (!em_dev->p_buffer) { 
        bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "alloc buffer"); 
        bj_free(em_dev); 
        return 0; 
    }
    memset(em_dev->p_buffer, 0, sizeof(float) * samples);

    if (!js_audio_init((uintptr_t)em_dev, em_dev->p_buffer, em_dev->frames_per_block, em_dev->channels)) {
        bj_set_error(p_error, BJ_ERROR_AUDIO, "WebAudio init failed");
        emscripten_close_device(dev);
        return NULL;
    }

    return dev;
}

static void emscripten_close_device(struct bj_audio_device* p_device) {
    struct emscripten_device* dev = (struct emscripten_device*)p_device;
    EM_ASM({
        var BJ_EM = Module['BJ_EM_AUDIO'];
        if (BJ_EM.scriptNode) { BJ_EM.scriptNode.disconnect(); BJ_EM.scriptNode = null; }
        if (BJ_EM.silenceTimer) { clearInterval(BJ_EM.silenceTimer); BJ_EM.silenceTimer = null; }
    });
    bj_free(dev->p_buffer);
    bj_free(dev);
}

static void emscripten_dispose_audio(struct bj_error** p_error) {
    (void)p_error;
}

static bj_bool emscripten_init_audio(
    struct bj_audio_layer* layer,
    struct bj_error** p_error
) {
    (void)p_error;
    layer->open_device  = emscripten_open_device;
    layer->close_device = emscripten_close_device;
    layer->end          = emscripten_dispose_audio;
    return BJ_TRUE;
}

EMSCRIPTEN_KEEPALIVE void audio_emscripten_process(uintptr_t device_ptr) {
    struct bj_audio_device*   p_device = (struct bj_audio_device*)device_ptr;
    struct emscripten_device* dev      = (struct emscripten_device*)p_device;

    size_t samples = (size_t)dev->frames_per_block * (size_t)dev->channels;
    // Always start with silence so JS never repeats old audio.
    memset(dev->p_buffer, 0, samples * sizeof(float));

    if (p_device->playing && p_device->callback) {
        p_device->callback(
            dev->p_buffer,
            dev->frames_per_block,
            &p_device->properties,
            p_device->callback_user_data,
            dev->sample_index
        );
        dev->sample_index += dev->frames_per_block;
    }
}

struct bj_audio_layer_create_info emscripten_audio_layer_info = {
    .name   = "emscripten",
    .create = emscripten_init_audio,
};

#endif
