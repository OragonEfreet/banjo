#include "config.h"

#if BJ_HAS_FEATURE(EMSCRIPTEN)

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <emscripten.h>
#include <banjo/audio.h>
#include <banjo/error.h>
#include <banjo/log.h>
#include <banjo/memory.h>
#include "audio_t.h"
#include "check.h"

// Maximum number of concurrent Emscripten audio devices
#define EMSCRIPTEN_MAX_DEVICES 8

// Per-device data
typedef struct {
    int      id;
    unsigned frames_per_period;
    uint64_t sample_index;
} emscripten_device;

static bj_audio_device* g_emscripten_devices[EMSCRIPTEN_MAX_DEVICES] = {0};

// JavaScript glue to create a ScriptProcessorNode and hook into bj_emscripten_audio_process
EM_JS(void, emscripten_setup_audio, (int id, int frames, int chans), {
    Module.bjAudioNodes = Module.bjAudioNodes || {};
    var ctx = Module.audioContext || (Module.audioContext = new (window.AudioContext || window.webkitAudioContext)());
    // On browsers that block auto-play, resume AudioContext after user gesture
    if (ctx.state === 'suspended') {
        var resumeFunc = function() {
            ctx.resume();
            document.removeEventListener('mousedown', resumeFunc);
            document.removeEventListener('touchstart', resumeFunc);
        };
        document.addEventListener('mousedown', resumeFunc);
        document.addEventListener('touchstart', resumeFunc);
    }
    var node = ctx.createScriptProcessor(frames, 0, chans);
    Module.bjAudioNodes[id] = node;
    node.onaudioprocess = function(ev) {
        var out = ev.outputBuffer;
        var len = out.getChannelData(0).length;
        var ptr = _malloc(len * chans * 4);
        // Call the compiled function directly
        Module._bj_emscripten_audio_process(id, ptr, len);
        for (var c = 0; c < chans; ++c) {
            var data = out.getChannelData(c);
            for (var i = 0; i < len; ++i) {
                data[i] = HEAPF32[(ptr >> 2) + i * chans + c];
            }
        }
        _free(ptr);
    };
    node.connect(ctx.destination);
});

// Close and disconnect the ScriptProcessorNode
static void emscripten_close_device(bj_audio_layer* p_audio, bj_audio_device* dev) {
    (void)p_audio;
    emscripten_device* data = (emscripten_device*)dev->data;
    if (data->id >= 0 && data->id < EMSCRIPTEN_MAX_DEVICES) {
        EM_ASM({
            var id = $0;
            if (Module.bjAudioNodes && Module.bjAudioNodes[id]) {
                Module.bjAudioNodes[id].disconnect();
                delete Module.bjAudioNodes[id];
            }
        }, data->id);
        g_emscripten_devices[data->id] = NULL;
    }
    bj_free(dev->data);
    bj_free(dev);
}

// Open an Emscripten audio device via Web Audio ScriptProcessor
static bj_audio_device* emscripten_open_device(
    bj_audio_layer*            p_audio,
    const bj_audio_properties* p_properties,
    bj_audio_callback_t        p_callback,
    void*                      p_callback_user_data,
    bj_error**                 p_error
) {
    // Allocate device
    bj_audio_device* dev = bj_calloc(sizeof(bj_audio_device));
    if (!dev) {
        bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "cannot allocate audio device");
        return NULL;
    }
    emscripten_device* data = bj_calloc(sizeof(emscripten_device));
    if (!data) {
        bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "cannot allocate device data");
        bj_free(dev);
        return NULL;
    }

    // Initialize bj_audio_device
    dev->p_callback            = p_callback;
    dev->p_callback_user_data  = p_callback_user_data;
    dev->properties            = *p_properties;
    dev->should_reset          = BJ_FALSE;
    dev->playing               = BJ_TRUE; // start playing immediately
    dev->data                  = data;

    // Find a free slot
    int id = -1;
    for (int i = 0; i < EMSCRIPTEN_MAX_DEVICES; ++i) {
        if (!g_emscripten_devices[i]) { id = i; break; }
    }
    if (id < 0) {
        bj_set_error(p_error, BJ_ERROR_INITIALIZE, "no free emscripten device slots");
        bj_free(data);
        bj_free(dev);
        return NULL;
    }
    data->id = id;
    data->frames_per_period = 1024;
    data->sample_index = 0;
    g_emscripten_devices[id] = dev;

    // Setup JS ScriptProcessor
    emscripten_setup_audio(id,
                            data->frames_per_period,
                            dev->properties.channels);

    return dev;
}

// Dispose of the audio layer
static void emscripten_dispose_audio(bj_audio_layer* p_audio, bj_error** p_error) {
    (void)p_error;
    bj_check(p_audio);
    bj_free(p_audio);
}

// C callback invoked from JS to fill float PCM data
EMSCRIPTEN_KEEPALIVE
void bj_emscripten_audio_process(int id, float* buffer, unsigned frames) {
    if (id < 0 || id >= EMSCRIPTEN_MAX_DEVICES) return;
    bj_audio_device* dev = g_emscripten_devices[id];
    if (!dev) return;
    emscripten_device* data = (emscripten_device*)dev->data;
    // Call user-provided callback
    dev->p_callback(buffer, frames, &dev->properties, dev->p_callback_user_data, data->sample_index);
    data->sample_index += frames;
}

// Initialize the Emscripten audio layer
static bj_audio_layer* emscripten_init_audio(bj_error** p_error) {
    bj_audio_layer* layer = bj_malloc(sizeof(bj_audio_layer));
    if (!layer) {
        bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "cannot allocate emscripten layer");
        return NULL;
    }
    layer->end         = emscripten_dispose_audio;
    layer->open_device = emscripten_open_device;
    layer->close_device= emscripten_close_device;
    layer->data        = NULL;
    return layer;
}

bj_audio_layer_create_info emscripten_audio_layer_info = {
    .name   = "emscripten",
    .create = emscripten_init_audio
};

#endif // BJ_HAS_FEATURE(EMSCRIPTEN)
