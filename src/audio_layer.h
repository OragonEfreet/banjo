#ifndef BJ_AUDIO_LAYER_H
#define BJ_AUDIO_LAYER_H

#include <banjo/audio.h>

////////////////////////////////////////////////////////////////////////////////
/// \brief Represent an audio backend (ALSA, MME, etc).
///
/// Internal structure used to abstract platform-specific device control.
////////////////////////////////////////////////////////////////////////////////
struct bj_audio_layer {
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Shutdown and clean up the backend.
    ///
    /// Called during deinitialization to release all backend-related resources.
    ///
    /// \param error Optional pointer to receive error information.
    ////////////////////////////////////////////////////////////////////////////
    void (*end)(struct bj_error** error);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Close an audio device managed by this backend.
    ///
    /// Used to stop and destroy a specific audio device instance.
    ///
    /// \param device Pointer to the audio device to close.
    ////////////////////////////////////////////////////////////////////////////
    void (*close_device)(struct bj_audio_device* device);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Open an audio device through this backend.
    ///
    /// Initializes a new audio device using the given properties and callback.
    ///
    /// \param properties Optional requested device properties, or NULL.
    /// \param callback     User-provided callback for audio sample generation.
    /// \param user_data    Pointer passed to the audio callback on each call.
    /// \param error      Optional pointer to receive error information.
    /// \return A new audio device instance, or NULL on failure.
    ////////////////////////////////////////////////////////////////////////////
    struct bj_audio_device* (*open_device)(
        const struct bj_audio_properties* properties,
        bj_audio_callback_fn              callback,
        void*                             user_data,
        struct bj_error**                 error
    );

};

////////////////////////////////////////////////////////////////////////////////
/// \brief Declare a backend and its constru    ctor function.
///
/// Used at initialization to register available backends such as ALSA,
/// MME, or WebAudio.
///
/// \see struct bj_audio_layer
////////////////////////////////////////////////////////////////////////////////
struct bj_audio_layer_create_info {
    const char* name;                          ///< Name of the backend (e.g., "alsa", "mme").

    bj_bool (*create)(
        struct bj_audio_layer*,
        struct bj_error**
    );     ///< Factory function to instantiate the backend.
};

bj_bool bj_begin_audio(struct bj_audio_layer* layer, struct bj_error** error);

#endif
