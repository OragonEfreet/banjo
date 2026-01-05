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
/// Banjo supports audio manipulation for Windows, GNU/Linux and WebAssembly.
///
/// \todo Add support for audio push-based API
/// \todo Add support for audio WAVE format
/// \todo Add support for audio MIDI format
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
struct bj_audio_device;

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
enum bj_audio_format {
    BJ_AUDIO_FORMAT_UNKNOWN = 0x0000,  ///< Unknown/unspecified format.
    BJ_AUDIO_FORMAT_INT16   = 0x8010,  ///< 16-bit signed integer PCM.
    BJ_AUDIO_FORMAT_F32     = 0x8120,  ///< 32-bit IEEE-754 float PCM.
};

////////////////////////////////////////////////////////////////////////////////
/// \def BJ_AUDIO_FORMAT_WIDTH(x)
/// \brief Extract the sample width (in bits) from an audio format code.
/// \param x Audio format code (enum bj_audio_format).
/// \return Unsigned integer width in bits (e.g., 16 or 32).
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUDIO_FORMAT_WIDTH(x)         ((x) & (0xFFu))

////////////////////////////////////////////////////////////////////////////////
/// \def BJ_AUDIO_FORMAT_FLOAT(x)
/// \brief Test whether the audio format is floating point.
/// \param x Audio format code (enum bj_audio_format).
/// \return Non-zero if float, zero otherwise.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUDIO_FORMAT_FLOAT(x)         ((x) & (1u<<8))

////////////////////////////////////////////////////////////////////////////////
/// \def BJ_AUDIO_FORMAT_INT(x)
/// \brief Test whether the audio format is integer PCM.
/// \param x Audio format code (enum bj_audio_format).
/// \return Non-zero if integer PCM, zero otherwise.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUDIO_FORMAT_INT(x)           (!((x) & (1u<<8)))

////////////////////////////////////////////////////////////////////////////////
/// \def BJ_AUDIO_FORMAT_BIG_ENDIAN(x)
/// \brief Test whether the audio format uses big-endian byte order.
/// \param x Audio format code (enum bj_audio_format).
/// \return Non-zero if big-endian, zero otherwise.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUDIO_FORMAT_BIG_ENDIAN(x)    ((x) & (1u<<12))

////////////////////////////////////////////////////////////////////////////////
/// \def BJ_AUDIO_FORMAT_SIGNED(x)
/// \brief Test whether integer samples are signed.
/// \param x Audio format code (enum bj_audio_format).
/// \return Non-zero if signed, zero otherwise.
////////////////////////////////////////////////////////////////////////////////
#define BJ_AUDIO_FORMAT_SIGNED(x)        ((x) & (1u<<15))

////////////////////////////////////////////////////////////////////////////////
/// \brief Describe properties of an audio device.
///
/// This structure is passed to audio callbacks to inform them about
/// the format and limits of the current playback device.
///
/// \see bj_audio_callback_fn
////////////////////////////////////////////////////////////////////////////////
struct bj_audio_properties {
    enum bj_audio_format format;      ///< Sampling format.
    int16_t         amplitude;   ///< Maximum amplitude of the output samples.
    unsigned int    channels;    ///< Number of channels (currently always 1).
    unsigned int    sample_rate; ///< Number of samples per second (Hz).
};

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
/// \see struct bj_audio_properties
////////////////////////////////////////////////////////////////////////////////
typedef void (*bj_audio_callback_fn)(
    void*                     buffer,
    unsigned                  frames,
    const struct bj_audio_properties* audio,
    void*                     user_data,
    uint64_t                  base_sample_index
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Open the default audio device for playback.
///
/// Initializes the audio backend and starts playback immediately using
/// the provided callback.
///
/// \param properties         Optional pointer to requested properties, or NULL.
///                             The opened device may differ from the request.
/// \param callback           User audio callback used to produce samples.
/// \param callback_user_data Opaque pointer passed to the callback on each call.
/// \param error              Optional pointer to receive error information on failure.
/// \return A handle to the opened audio device, or NULL on failure.
///
/// \see bj_audio_callback_fn
/// \see bj_close_audio_device
/// \see bj_play_audio_device
/// \see bj_pause_audio_device
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT struct bj_audio_device* bj_open_audio_device(
    const struct bj_audio_properties* properties,
    bj_audio_callback_fn        callback,
    void*                      callback_user_data,
    struct bj_error**                 error
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Close an audio device and release all associated resources.
///
/// Stops playback and joins the audio thread before cleanup.
///
/// \param device Pointer to the audio device to close.
///
/// \see bj_open_audio_device
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_close_audio_device(
    struct bj_audio_device* device
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Resume audio playback.
///
/// Playback resumes from where it was previously paused.
///
/// \param device Pointer to the audio device.
///
/// \see bj_pause_audio_device
/// \see bj_stop_audio_device
/// \see bj_audio_playing
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_play_audio_device(
    struct bj_audio_device* device
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Pause audio playback.
///
/// While paused, the audio thread continues running and outputs silence.
///
/// \param device Pointer to the audio device.
///
/// \see bj_play_audio_device
/// \see bj_stop_audio_device
/// \see bj_audio_playing
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_pause_audio_device(
    struct bj_audio_device* device
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Reset the playback stream sample index to 0.
///
/// Does not stop or pause playback. Only resets timing.
///
/// \param device Pointer to the audio device.
///
/// \see bj_stop_audio_device
/// \see bj_audio_callback_fn
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_reset_audio_device(
    struct bj_audio_device* device
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Stop playback and reset the sample stream.
///
/// Equivalent to calling pause followed by reset.
///
/// \param device Pointer to the audio device.
///
/// \see bj_pause_audio_device
/// \see bj_reset_audio_device
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_stop_audio_device(
    struct bj_audio_device* device
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Query whether the device is currently playing audio.
///
/// \param device Pointer to the audio device.
/// \return BJ_TRUE if playing, BJ_FALSE if paused or stopped.
///
/// \see bj_play_audio_device
/// \see bj_pause_audio_device
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT bj_bool bj_audio_playing(
    const struct bj_audio_device* device
);

////////////////////////////////////////////////////////////////////////////////
/// \brief Define parameters for generating simple waveforms.
///
/// Used with bj_play_audio_note to synthesize tones such as sine or square.
///
/// \see bj_play_audio_note
////////////////////////////////////////////////////////////////////////////////
struct bj_audio_play_note_data {
    enum {
        BJ_AUDIO_PLAY_SINE,      ///< Generate a sine wave.
        BJ_AUDIO_PLAY_SQUARE,    ///< Generate a square wave.
        BJ_AUDIO_PLAY_TRIANGLE,  ///< Generate a triangle wave.
        BJ_AUDIO_PLAY_SAWTOOTH,  ///< Generate a sawtooth wave.
    } function;                  ///< Type of waveform to generate.
    double frequency;            ///< Frequency of the waveform (Hz).
    double phase;                ///< Internal phase accumulator.
};

////////////////////////////////////////////////////////////////////////////////
/// \brief Generate a basic waveform tone using a built-in callback.
///
/// Can be used as a bj_audio_callback_fn and uses struct bj_audio_play_note_data.
///
/// \param buffer            Output buffer to write samples into.
/// \param frames            Number of frames to generate.
/// \param audio             Audio device properties.
/// \param user_data         Pointer to a struct bj_audio_play_note_data instance.
/// \param base_sample_index Starting sample index (for phase computation).
///
/// \see struct bj_audio_play_note_data
/// \see bj_audio_callback_fn
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT void bj_play_audio_note(
    void*                      buffer,
    unsigned                   frames,
    const struct bj_audio_properties* audio,
    void*                      user_data,
    uint64_t                   base_sample_index
);

#endif /* BJ_AUDIO_H */
/// \} // end of audio group
