#include "config.h"

#if BJ_HAS_FEATURE(EMSCRIPTEN)

#include <banjo/audio.h>

#include "check.h"

#include <emscripten/emscripten.h>

static void emscripten_dispose_audio(bj_audio_layer* p_audio, bj_error** p_error) {
    (void)p_error;
    bj_check(p_audio);
    bj_free(p_audio);
}

static bj_audio_device* emscripten_open_device(
    bj_audio_layer*             p_audio,
    const bj_audio_properties*  p_properties,
    bj_audio_callback_t         p_callback,
    void*                       p_callback_user_data,
    bj_error**                  p_error
) {
    (void)p_audio;
    (void)p_properties;
    (void)p_callback;
    (void)p_callback_user_data;
    (void)p_error;
    
    const bj_bool result = MAIN_THREAD_EM_ASM_INT({
        if (typeof(Module['banjo']) === 'undefined') {
            Module['banjo'] = {};
        }
        var banjo = Module['banjo'];
        banjo.audio_playback = {};

        if (!banjo.audioContext) {
            if (typeof(AudioContext) !== 'undefined') {
                banjo.audioContext = new AudioContext();
            } else if (typeof(webkitAudioContext) !== 'undefined') {
                banjo.audioContext = new webkitAudioContext();
            }
            if (banjo.audioContext) {
                if ((typeof navigator.userActivation) === 'undefined') {
                    autoResumeAudioContext(banjo.audioContext);
                }
            }
        }
        return (banjo.audioContext !== undefined);
    });

	if (!result) {
		bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "cannot open audio device");
		return 0;
	}

	return 0;
}

static void emscripten_close_device(bj_audio_layer* p_audio, bj_audio_device* p_device) {
	(void)p_audio;
	(void)p_device;
}


static bj_audio_layer* emscripten_init_audio(bj_error** p_error) {
	bj_audio_layer* p_audio = bj_malloc(sizeof(bj_audio_layer));

    const bj_bool available = MAIN_THREAD_EM_ASM_INT({
        if (typeof(AudioContext) !== 'undefined') {
            return true;
        } 
        if (typeof(webkitAudioContext) !== 'undefined') {
            return true;
        }
        return false;
    });
    if(available == BJ_FALSE) {
		bj_set_error(p_error, BJ_ERROR_INITIALIZE, "No available audio device");
		return 0;
    }

	if (!p_audio) {
		bj_set_error(p_error, BJ_ERROR_CANNOT_ALLOCATE, "cannot allocate memory for emscripten");
		return 0;
	}

	p_audio->end          = emscripten_dispose_audio;
	p_audio->open_device  = emscripten_open_device;
	p_audio->close_device = emscripten_close_device;
    p_audio->data         = 0;

	return p_audio;
}

bj_audio_layer_create_info emscripten_audio_layer_info = {
    .name = "emscripten",
    .create = emscripten_init_audio,
};

#endif
