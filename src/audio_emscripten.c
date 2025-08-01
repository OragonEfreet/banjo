#include "config.h"

#if BJ_HAS_FEATURE(EMSCRIPTEN)

// Note: This file is AI-generated.
// I'll remake it myself later on.

#include <emscripten.h>
#include <banjo/audio.h>
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>
#include <banjo/system.h>
#include <banjo/time.h>
#include "audio_t.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Internal per-device data for Emscripten/WebAudio
typedef struct emscripten_device_t {
    float*    p_buffer;         ///< Interleaved float buffer
    unsigned  frames_per_block; ///< Number of frames per JS callback
    uint64_t  sample_index;     ///< Total frames generated so far
    int       channels;         ///< Number of output channels
    int       silence_timer;    ///< ID for fallback silence timer
} emscripten_device;

// Forward declarations
EMSCRIPTEN_KEEPALIVE void audio_emscripten_process(uintptr_t device_ptr);
static void emscripten_close_device(bj_audio_layer* layer, bj_audio_device* p_device);

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

static bj_audio_device* emscripten_open_device(
    bj_audio_layer*            layer,
    const bj_audio_properties* p_properties,
    bj_audio_callback_t        p_callback,
    void*                      p_callback_user_data,
    bj_error**                 p_error
) {
    (void)layer;
    bj_audio_device* p_device = bj_calloc(sizeof(bj_audio_device));
    if (!p_device) { bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "alloc device"); return NULL; }
    emscripten_device* dev = bj_calloc(sizeof(emscripten_device));
    if (!dev) { bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "alloc data"); bj_free(p_device); return NULL; }

    p_device->p_callback = p_callback;
    p_device->p_callback_user_data = p_callback_user_data;
    p_device->properties.format    = BJ_AUDIO_FORMAT_F32;
    p_device->properties.amplitude = 1;
    p_device->properties.channels  = p_properties ? p_properties->channels : 1;
    p_device->properties.sample_rate = p_properties ? p_properties->sample_rate : 44100;

    dev->frames_per_block = 512;
    dev->channels         = p_device->properties.channels;
    dev->sample_index     = 0;
    dev->silence_timer    = -1;

    size_t samples = dev->frames_per_block * dev->channels;
    dev->p_buffer = bj_malloc(sizeof(float) * samples);
    if (!dev->p_buffer) { bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "alloc buffer"); bj_free(dev); bj_free(p_device); return NULL; }
    memset(dev->p_buffer, 0, sizeof(float) * samples);

    p_device->data = (bj_audio_device_data*)dev;

    if (!js_audio_init((uintptr_t)p_device, dev->p_buffer, dev->frames_per_block, dev->channels)) {
        bj_set_error(p_error, BJ_ERROR_AUDIO, "WebAudio init failed");
        emscripten_close_device(layer, p_device);
        return NULL;
    }

    return p_device;
}

static void emscripten_close_device(bj_audio_layer* layer, bj_audio_device* p_device) {
    (void)layer;
    emscripten_device* dev = (emscripten_device*)p_device->data;
    EM_ASM({
        var BJ_EM = Module['BJ_EM_AUDIO'];
        if (BJ_EM.scriptNode) { BJ_EM.scriptNode.disconnect(); BJ_EM.scriptNode = null; }
        if (BJ_EM.silenceTimer) { clearInterval(BJ_EM.silenceTimer); BJ_EM.silenceTimer = null; }
    });
    bj_free(dev->p_buffer);
    bj_free(dev);
    bj_free(p_device);
}

static void emscripten_dispose_audio(bj_audio_layer* layer, bj_error** p_error) {
    (void)layer; (void)p_error;
}

static bj_audio_layer* emscripten_init_audio(bj_error** p_error) {
    (void)p_error;
    bj_audio_layer* layer = bj_malloc(sizeof(bj_audio_layer));
    if (!layer) return NULL;
    layer->open_device  = emscripten_open_device;
    layer->close_device = emscripten_close_device;
    layer->end          = emscripten_dispose_audio;
    layer->data         = NULL;
    return layer;
}

EMSCRIPTEN_KEEPALIVE void audio_emscripten_process(uintptr_t device_ptr) {
    bj_audio_device*   p_device = (bj_audio_device*)device_ptr;
    emscripten_device* dev      = (emscripten_device*)p_device->data;
    p_device->p_callback(
        dev->p_buffer,
        dev->frames_per_block,
        &p_device->properties,
        p_device->p_callback_user_data,
        dev->sample_index
    );
    dev->sample_index += dev->frames_per_block;
}

bj_audio_layer_create_info emscripten_audio_layer_info = {
    .name   = "emscripten",
    .create = emscripten_init_audio,
};

#endif // BJ_HAS_FEATURE(EMSCRIPTEN)
