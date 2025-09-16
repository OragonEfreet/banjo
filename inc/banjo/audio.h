////////////////////////////////////////////////////////////////////////////////
/// \file audio.h
/// \brief Basic audio library interface.
////////////////////////////////////////////////////////////////////////////////
/// \defgroup audio Audio
///
/// \brief Provide basic PCM audio playback
///
/// The audio component offers 1-channel Pulse-Code Modulation (PCM)
/// playback with a callback-based interface, suitable for retro-style
/// games and basic procedural sound generation.
///
/// \{
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_AUDIO_H
#define BJ_AUDIO_H

#include <banjo/error.h>
#include <banjo/api.h>
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
/// \brief Opaque handle to an audio device instance.
///
/// Instances are created with bj_open_audio_device and must be released
/// with bj_close_audio_device.
///
/// \see bj_open_audio_device
/// \see bj_close_audio_device
////////////////////////////////////////////////////////////////////////////////
typedef struct bj_audio_device_t bj_audio_device;

////////////////////////////////////////////////////////////////////////////////
/// \brief Audio sample format descriptor.
///
/// Encodes sample width, sign, float/integer, and endianness flags.
/// Use the helper macros to inspect properties.
///
/// \see BJ_AUDIO_FORMAT_WIDTH
/// \see BJ_AUDIO_FORMAT_FLOAT
/// \see BJ_AUDIO_FORMAT_INT
/// \see BJ_AUDIO_FORMAT_BIG_ENDIAN
/// \see BJ_AUDIO_FORMAT_SIGNED
////////////////////////////////////////////////////////////////////////////////
typedef enum bj_audio_format_t {
    BJ_AUDIO_FORMAT_UNKNOWN = 0x0000,  ///< Unknown/unspecified format.
    BJ_AUDIO_FORMAT_INT16   = 0x8010,  ///< 16-bit signed integer PCM.
    BJ_AUDIO_FORMAT_F32     = 0x8120,  ///< 32-bit IEEE-754 float PCM.
} bj_audio_format;

////////////////////////////////////////////////////////////////////////////////
/// \def BJ_AUDIO_FORMAT_WIDTH(x)
/// \brief Extract the sample width (in bits) from an audio format code.
/// \param x Audio format code (bj_audio_format).
/// \return Unsigned integer width in bits (e.g., 16 or 32).
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUDIO_FORMAT_WIDTH(x)         ((x) & (0xFFu))

////////////////////////////////////////////////////////////////////////////////
/// \def BJ_AUDIO_FORMAT_FLOAT(x)
/// \brief Test whether the audio format is floating point.
/// \param x Audio format code (bj_audio_format).
/// \return Non-zero if float, zero otherwise.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUDIO_FORMAT_FLOAT(x)         ((x) & (1u<<8))

////////////////////////////////////////////////////////////////////////////////
/// \def BJ_AUDIO_FORMAT_INT(x)
/// \brief Test whether the audio format is integer PCM.
/// \param x Audio format code (bj_audio_format).
/// \return Non-zero if integer PCM, zero otherwise.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUDIO_FORMAT_INT(x)           (!((x) & (1u<<8)))

////////////////////////////////////////////////////////////////////////////////
/// \def BJ_AUDIO_FORMAT_BIG_ENDIAN(x)
/// \brief Test whether the audio format uses big-endian byte order.
/// \param x Audio format code (bj_audio_format).
/// \return Non-zero if big-endian, zero otherwise.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUDIO_FORMAT_BIG_ENDIAN(x)    ((x) & (1u<<12))

////////////////////////////////////////////////////////////////////////////////
/// \def BJ_AUDIO_FORMAT_SIGNED(x)
/// \brief Test whether integer samples are signed.
/// \param x Audio format code (bj_audio_format).
/// \return Non-zero if signed, zero otherwise.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUDIO_FORMAT_SIGNED(x)        ((x) & (1u<<15))

////////////////////////////////////////////////////////////////////////////////
/// \brief Describe properties of an audio device.
///
/// This structure is passed to audio callbacks to inform them about
/// the format and limits of the current playback device.
///
/// \see bj_audio_callback_t
////////////////////////////////////////////////////////////////////////////////
typedef struct bj_audio_properties_t {
    bj_audio_format format;      ///< Sampling format.
    int16_t         amplitude;   ///< Maximum amplitude of the output samples.
    unsigned int    channels;    ///< Number of channels (currently always 1).
    unsigned int    sample_rate; ///< Number of samples per second (Hz).
} bj_audio_properties;

////////////////////////////////////////////////////////////////////////////////
/// \brief Define a callback for generating audio samples.
///
/// This callback is called periodically from a dedicated audio thread to
/// generate PCM audio data.
///
/// \param buffer            Output buffer to write int16_t samples into.
/// \param frames            Number of audio frames to generate.
/// \param audio             Pointer to audio device properties (read-only).
/// \param user_data         User-defined pointer passed at device creation.
/// \param base_sample_index Index of the first sample in the current buffer.
///
/// \see bj_open_audio_device
/// \see bj_audio_properties
////////////////////////////////////////////////////////////////////////////////
typedef void (*bj_audio_callback_t)(
    void*                     buffer,
    unsigned                  frames,
    const bj_audio_properties* audio,
    void*                     user_data,
    uint64_t                  base_sample_index
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Open the default audio device for playback.
///
/// Initializes the audio backend and starts playback immediately using
/// the provided callback.
///
/// \param p_properties         Optional pointer to requested properties, or NULL.
///                             The opened device may differ from the request.
/// \param p_callback           User audio callback used to produce samples.
/// \param p_callback_user_data Opaque pointer passed to the callback on each call.
/// \param p_error              Optional pointer to receive error information on failure.
/// \return A handle to the opened audio device, or NULL on failure.
///
/// \see bj_audio_callback_t
/// \see bj_close_audio_device
/// \see bj_play_audio_device
/// \see bj_pause_audio_device
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_audio_device* bj_open_audio_device(
    const bj_audio_properties* p_properties,
    bj_audio_callback_t        p_callback,
    void*                      p_callback_user_data,
    bj_error**                 p_error
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Close an audio device and release all associated resources.
///
/// Stops playback and joins the audio thread before cleanup.
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
/// \see bj_pause_audio_device
/// \see bj_stop_audio_device
/// \see bj_audio_playing
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_play_audio_device(
    bj_audio_device* p_device
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Pause audio playback.
///
/// While paused, the audio thread continues running and outputs silence.
///
/// \param p_device Pointer to the audio device.
///
/// \see bj_play_audio_device
/// \see bj_stop_audio_device
/// \see bj_audio_playing
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_pause_audio_device(
    bj_audio_device* p_device
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Reset the playback stream sample index to 0.
///
/// Does not stop or pause playback. Only resets timing.
///
/// \param p_device Pointer to the audio device.
///
/// \see bj_stop_audio_device
/// \see bj_audio_callback_t
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_reset_audio_device(
    bj_audio_device* p_device
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Stop playback and reset the sample stream.
///
/// Equivalent to calling pause followed by reset.
///
/// \param p_device Pointer to the audio device.
///
/// \see bj_pause_audio_device
/// \see bj_reset_audio_device
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_stop_audio_device(
    bj_audio_device* p_device
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Query whether the device is currently playing audio.
///
/// \param p_device Pointer to the audio device.
/// \return BJ_TRUE if playing, BJ_FALSE if paused or stopped.
///
/// \see bj_play_audio_device
/// \see bj_pause_audio_device
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bool bj_audio_playing(
    const bj_audio_device* p_device
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Define parameters for generating simple waveforms.
///
/// Used with bj_play_audio_note to synthesize tones such as sine or square.
///
/// \see bj_play_audio_note
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
/// Can be used as a bj_audio_callback_t and uses bj_audio_play_note_data_t.
///
/// \param buffer            Output buffer to write samples into.
/// \param frames            Number of frames to generate.
/// \param audio             Audio device properties.
/// \param user_data         Pointer to a bj_audio_play_note_data_t instance.
/// \param base_sample_index Starting sample index (for phase computation).
///
/// \see bj_audio_play_note_data_t
/// \see bj_audio_callback_t
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_play_audio_note(
    void*                      buffer,
    unsigned                   frames,
    const bj_audio_properties* audio,
    void*                      user_data,
    uint64_t                   base_sample_index
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Represent an audio backend (ALSA, MME, etc).
///
/// Internal structure used to abstract platform-specific device control.
////////////////////////////////////////////////////////////////////////////////
typedef struct bj_audio_layer_t {
    ////////////////////////////////////////////////////////////////////////////
    /// \brief Shutdown and clean up the backend.
    ///
    /// Called during deinitialization to release all backend-related resources.
    ///
    /// \param self    Pointer to the audio layer instance.
    /// \param p_error Optional pointer to receive error information.
    ////////////////////////////////////////////////////////////////////////////
    void (*end)(struct bj_audio_layer_t* self, bj_error** p_error);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Close an audio device managed by this backend.
    ///
    /// Used to stop and destroy a specific audio device instance.
    ///
    /// \param self   Pointer to the audio layer instance.
    /// \param device Pointer to the audio device to close.
    ////////////////////////////////////////////////////////////////////////////
    void (*close_device)(struct bj_audio_layer_t* self, bj_audio_device* device);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Open an audio device through this backend.
    ///
    /// Initializes a new audio device using the given properties and callback.
    ///
    /// \param self         Pointer to the audio layer instance.
    /// \param p_properties Optional requested device properties, or NULL.
    /// \param callback     User-provided callback for audio sample generation.
    /// \param user_data    Pointer passed to the audio callback on each call.
    /// \param p_error      Optional pointer to receive error information.
    /// \return A new audio device instance, or NULL on failure.
    ////////////////////////////////////////////////////////////////////////////
    bj_audio_device* (*open_device)(
        struct bj_audio_layer_t* self,
        const bj_audio_properties* p_properties,
        bj_audio_callback_t callback,
        void* user_data,
        bj_error** p_error
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
/// Used at initialization to register available backends such as ALSA,
/// MME, or WebAudio.
///
/// \see bj_audio_layer
////////////////////////////////////////////////////////////////////////////////
typedef struct {
    const char* name;                          ///< Name of the backend (e.g., "alsa", "mme").
    bj_audio_layer* (*create)(bj_error**);     ///< Factory function to instantiate the backend.
} bj_audio_layer_create_info;

#endif /* BJ_AUDIO_H */
/// \} // end of audio group
