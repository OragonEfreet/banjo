////////////////////////////////////////////////////////////////////////////////
/// \file audio.h
/// \brief Basic audio library interface.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup audio Audio
/// \ingroup core
///
/// \brief Provide basic PCM audio playback
///
/// The audio component offers 1-channel Pulse-Code Modulation (PCM)
/// playback with a callback-based interface, suitable for retro-style
/// games and basic procedural sound generation.
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <banjo/error.h>
#include <banjo/api.h>

/// Opaque handle to an audio device instance.
/// \see bj_open_audio_device
/// \see bj_close_audio_device
typedef struct bj_audio_device_t bj_audio_device;

////////////////////////////////////////////////////////////////////////////////
/// \brief Describe properties of an audio device.
///
/// This structure is passed to audio callbacks to inform them about
/// the format and limits of the current playback device.
///
/// \see bj_audio_callback_t
////////////////////////////////////////////////////////////////////////////////
typedef struct bj_audio_properties_t {
    int16_t      amplitude;   ///< Maximum amplitude of the output samples.
    unsigned int channels;    ///< Number of channels (currently always 1).
    unsigned int sample_rate; ///< Number of samples per second (Hz).
    uint16_t     silence;     ///< Sample value that represents silence.
} bj_audio_properties;

////////////////////////////////////////////////////////////////////////////////
/// \brief Define a callback for generating audio samples.
///
/// This callback is called periodically from a dedicated audio thread to
/// generate PCM audio data.
///
/// \param buffer               Output buffer to write `int16_t` samples into.
/// \param frames               Number of audio frames to generate.
/// \param audio                Pointer to audio device properties (readonly).
/// \param user_data            User-defined pointer passed at device creation.
/// \param base_sample_index    Index of the first sample in the current buffer.
///
/// \see bj_open_audio_device
/// \see bj_audio_properties
////////////////////////////////////////////////////////////////////////////////
typedef void (*bj_audio_callback_t)(
    int16_t*                   buffer,
    unsigned                   frames,
    const bj_audio_properties* audio,
    void*                      user_data,
    uint64_t                   base_sample_index
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Open the default audio device for playback.
///
/// This initializes the audio backend and starts playback immediately
/// using the provided callback.
///
/// \param p_error               Pointer to receive error information.
/// \param p_callback            Function pointer to the user-provided audio callback.
/// \param p_callback_user_data  User-defined pointer passed to the callback.
///
/// \return A handle to the opened audio device, or NULL on failure.
///
/// \see bj_audio_callback_t
/// \see bj_close_audio_device
/// \see bj_audio_device_play
/// \see bj_audio_device_pause
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_audio_device* bj_open_audio_device(
    bj_error** p_error,
    bj_audio_callback_t p_callback,
    void* p_callback_user_data
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Close an audio device and release all associated resources.
///
/// This stops playback and joins the audio thread before cleanup.
///
/// \param p_device Pointer to the audio device to close.
///
/// \see bj_open_audio_device
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_close_audio_device(
    bj_audio_device* p_device
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Resume audio playback.
///
/// Playback resumes from where it was previously paused.
///
/// \param p_device Pointer to the audio device.
///
/// \see bj_audio_device_pause
/// \see bj_audio_device_stop
/// \see bj_audio_device_is_playing
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_audio_device_play(
    bj_audio_device* p_device
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Pause audio playback.
///
/// While paused, the audio thread continues running and outputs silence.
///
/// \param p_device Pointer to the audio device.
///
/// \see bj_audio_device_play
/// \see bj_audio_device_stop
/// \see bj_audio_device_is_playing
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_audio_device_pause(
    bj_audio_device* p_device
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Reset the playback stream sample index to 0.
///
/// This does not stop or pause playback, but restarts timing from zero.
///
/// \param p_device Pointer to the audio device.
///
/// \see bj_audio_device_stop
/// \see bj_audio_callback_t
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_audio_device_reset(
    bj_audio_device* p_device
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Stop playback and reset the sample stream.
///
/// This is equivalent to calling pause followed by reset.
///
/// \param p_device Pointer to the audio device.
///
/// \see bj_audio_device_pause
/// \see bj_audio_device_reset
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_audio_device_stop(
    bj_audio_device* p_device
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Query whether the device is currently playing audio.
///
/// \param p_device Pointer to the audio device.
/// \return BJ_TRUE if playing, BJ_FALSE if paused or stopped.
///
/// \see bj_audio_device_play
/// \see bj_audio_device_pause
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bool bj_audio_device_is_playing(
    const bj_audio_device* p_device
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Define parameters for generating simple waveforms.
///
/// Used in conjunction with \ref bj_audio_play_note to synthesize
/// basic musical tones such as sine or square waves.
///
/// \see bj_audio_play_note
////////////////////////////////////////////////////////////////////////////////
typedef struct bj_audio_play_note_data_t {
    enum {
        BJ_AUDIO_PLAY_SINE,      ///< Generate a sine wave.
        BJ_AUDIO_PLAY_SQUARE,    ///< Generate a square wave.
        BJ_AUDIO_PLAY_TRIANGLE,  ///< Generate a triangle wave.
        BJ_AUDIO_PLAY_SAWTOOTH,  ///< Generate a sawtooth wave.
    } function;                  ///< Type of waveform to generate.
    double frequency;            ///< Frequency of the waveform (Hz).
    double phase;                ///< Internal phase accumulator.
} bj_audio_play_note_data;

////////////////////////////////////////////////////////////////////////////////
/// \brief Generate a basic waveform tone using a built-in callback.
///
/// This function can be passed as a \ref bj_audio_callback_t and uses the
/// data provided in \ref bj_audio_play_note_data_t to generate tones.
///
/// \param buffer               Output buffer to write samples into.
/// \param frames               Number of frames to generate.
/// \param audio                Audio device properties.
/// \param user_data            Pointer to a \ref bj_audio_play_note_data_t instance.
/// \param base_sample_index    Starting sample index (used to compute phase).
///
/// \see bj_audio_play_note_data_t
/// \see bj_audio_callback_t
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_audio_play_note(
    int16_t* buffer,
    unsigned frames,
    const bj_audio_properties* audio,
    void* user_data,
    uint64_t base_sample_index
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Represent an audio backend (ALSA, MME, etc).
///
/// This internal structure handles platform-specific device control
/// and is used to abstract away the OS details.
////////////////////////////////////////////////////////////////////////////////
typedef struct bj_audio_layer_t {
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Shutdown and clean up the backend.
    ///
    /// Called during deinitialization to release all backend-related resources.
    ///
    /// \param self Pointer to the audio layer instance.
    /// \param p_error Optional pointer to receive error information.
    ////////////////////////////////////////////////////////////////////////////
    void (*end)(struct bj_audio_layer_t* self, bj_error** p_error);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Close an audio device managed by this backend.
    ///
    /// Used to stop and destroy a specific audio device instance.
    ///
    /// \param self Pointer to the audio layer instance.
    /// \param device Pointer to the audio device to close.
    ////////////////////////////////////////////////////////////////////////////
    void (*close_device)(struct bj_audio_layer_t* self, bj_audio_device* device);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Open an audio device through this backend.
    ///
    /// Initializes a new audio device using the given callback and context.
    ///
    /// \param self Pointer to the audio layer instance.
    /// \param p_error Optional pointer to receive error information.
    /// \param callback User-provided callback for audio sample generation.
    /// \param user_data Pointer to be passed to the audio callback.
    ///
    /// \return A new audio device instance, or NULL on failure.
    ////////////////////////////////////////////////////////////////////////////
    bj_audio_device* (*open_device)(
        struct bj_audio_layer_t* self,
        bj_error** p_error,
        bj_audio_callback_t callback,
        void* user_data
    );

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Backend-specific data pointer.
    ///
    /// Used internally by the backend to store platform-specific state.
    ////////////////////////////////////////////////////////////////////////////
    struct bj_audio_layer_data_t* data;
} bj_audio_layer;

////////////////////////////////////////////////////////////////////////////////
/// \brief Declare a backend and its constructor function.
///
/// Used at initialization time to register available backends such as ALSA,
/// MME, or WebAudio.
///
/// \see bj_audio_layer
////////////////////////////////////////////////////////////////////////////////
typedef struct {
    const char* name;                          ///< Name of the backend (e.g., "alsa", "mme").
    bj_audio_layer* (*create)(bj_error**);     ///< Factory function to instantiate the backend.
} bj_audio_layer_create_info;

/// \} // end of audio group
