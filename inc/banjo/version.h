////////////////////////////////////////////////////////////////////////////////
/// \file version.h
/// \brief 32-bit packed version storage following the SemVer standard.
///
/// Banjo versions are stored in a single 32-bit integer, with each byte 
/// representing a different component in order from most significant to least 
/// significant: major version, minor version, patch version, and stage 
/// (pre-release type and number).
///
/// For example, the version 3.4.0-rc.2 is represented as 0x030400C2:
/// - 0x03000000 → Major version 3
/// - 0x00040000 → Minor version 4
/// - 0x00000000 → Patch version 0
/// - 0x000000C2 → Release Candidate, second iteration
///
/// This header provides macros to construct packed version integers, extract 
/// each component, and define stage flags. The encoding respects SemVer 
/// precedence, so comparing two packed versions using standard integer 
/// comparison operators correctly reflects SemVer ordering. 
/// Pre-release stages always compare lower than stable releases, and the 
/// stage byte allows internal numbering for alpha, beta, and RC builds.
///
/// SemVer Standard: https://semver.org/
////////////////////////////////////////////////////////////////////////////////
#ifndef BJ_VERSION_H
#define BJ_VERSION_H

#include <banjo/api.h>

////////////////////////////////////////////////////////////////////////////////
/// \brief Version stage flags.
///
/// The least significant byte of the 32-bit version integer encodes the stage 
/// of the release. 
///
/// The two most significant bits indicate the stage type: alpha, beta, rc, or 
/// stable. 
/// The remaining six bits store the stage number, for example rc.2 would be 
/// encoded as BJ_VERSION_RC | 0x02.
///
/// Certain values are treated specially: 0xFF always represents a stable 
/// release and has the highest precedence. 
/// Any 0b00xxxxxx value is implementation-defined.
///
/// This encoding cannot distinguish between, for example, rc and rc.0. 
/// Version precedence follows SemVer rules, so all pre-release stages are 
/// considered lower than stable releases.
/// \{
////////////////////////////////////////////////////////////////////////////////
#define BJ_VERSION_DEV      0x00 ///< Alpha release
#define BJ_VERSION_ALPHA    0x40 ///< Alpha release
#define BJ_VERSION_BETA     0x80 ///< Beta release
#define BJ_VERSION_RC       0xC0 ///< Release Candidate

#define BJ_VERSION_STABLE   0xFF ///< Stable Release
/// \}


////////////////////////////////////////////////////////////////////////////////
/// \brief Extract the major version from a packed 32-bit version.
///
/// The most significant byte of the version represents the major version.
/// \param version The packed 32-bit version value.
/// \return Major version in the range [0, 255].
////////////////////////////////////////////////////////////////////////////////
#define BJ_VERSION_MAJOR(version) (((version) >> 24U) & 0xFFU)

////////////////////////////////////////////////////////////////////////////////
/// \brief Extract the minor version from a packed 32-bit version.
///
/// The second byte of the packed version represents the minor version.
/// \param version The packed 32-bit version value.
/// \return Minor version in the range [0, 255].
////////////////////////////////////////////////////////////////////////////////
#define BJ_VERSION_MINOR(version) (((version) >> 16U) & 0xFFU)

////////////////////////////////////////////////////////////////////////////////
/// \brief Extract the patch version from a packed 32-bit version.
///
/// The third byte of the packed version represents the patch version.
/// \param version The packed 32-bit version value.
/// \return Patch version in the range [0, 255].
////////////////////////////////////////////////////////////////////////////////
#define BJ_VERSION_PATCH(version) (((version) >> 8U) & 0xFFU)

////////////////////////////////////////////////////////////////////////////////
/// \brief Extract the stage byte from a packed 32-bit version.
///
/// The least significant byte of the packed version contains the stage type
/// and stage number.
/// \param version The packed 32-bit version value.
/// \return Stage byte in the range [0, 255].
////////////////////////////////////////////////////////////////////////////////
#define BJ_VERSION_STAGE(version) ((version) & 0xFFU)

////////////////////////////////////////////////////////////////////////////////
/// \brief Construct a packed 32-bit version.
///
/// The 32-bit version layout is [major:8 | minor:8 | patch:8 | stage:8].
/// \param major Major version in the range [0, 255].
/// \param minor Minor version in the range [0, 255].
/// \param patch Patch version in the range [0, 255].
/// \param stage Stage byte, typically one of BJ_VERSION_ALPHA, BJ_VERSION_BETA,
///              BJ_VERSION_RC, or BJ_VERSION_STABLE optionally OR'ed with a 
///              stage number.
/// \return Packed 32-bit version suitable for BJ_VERSION_* macros.
////////////////////////////////////////////////////////////////////////////////
#define BJ_MAKE_VERSION(major, minor, patch, stage) \
    ((((uint32_t)(major)) << 24U) | (((uint32_t)(minor)) << 16U) | \
     (((uint32_t)(patch)) << 8U) | ((uint32_t)(stage)))

////////////////////////////////////////////////////////////////////////////////
/// \brief Library name string.
///
/// This macro can be changed to modify the name of the library in case of
/// fork, for example.
////////////////////////////////////////////////////////////////////////////////
#define BJ_NAME "Banjo"

////////////////////////////////////////////////////////////////////////////////
/// \brief Variant name for Banjo binary
///
/// The variant is an additional name set after the version number.
/// It can be used in a fork project to specify a non-mainstream version of
/// banjo, while still keeping the name Banjo.
////////////////////////////////////////////////////////////////////////////////
#define BJ_NAME_VARIANT ""

#define BJ_VERSION_MAJOR_NUMBER 0 ///< Current major version number.
#define BJ_VERSION_MINOR_NUMBER 1 ///< Current minor version number.
#define BJ_VERSION_PATCH_NUMBER 0 ///< Current patch version number.
#define BJ_VERSION_STAGE_NUMBER (BJ_VERSION_DEV | 0x00) ///< Current stage version specifier.

////////////////////////////////////////////////////////////////////////////////
/// \brief Current API version as a packed 32-bit value.
////////////////////////////////////////////////////////////////////////////////
#define BJ_VERSION BJ_MAKE_VERSION(BJ_VERSION_MAJOR_NUMBER, BJ_VERSION_MINOR_NUMBER, BJ_VERSION_PATCH_NUMBER, BJ_VERSION_STAGE_NUMBER)

////////////////////////////////////////////////////////////////////////////////
/// \brief Format a packed version number as a SemVer-compatible string.
///
/// Writes a human-readable Semantic Version string into the provided buffer,
/// using the packed 32-bit version representation defined in version.h.
/// The output follows the form:
///
///     major.minor.patch[-stage[.number]]
///
/// Stable releases are printed without a pre-release suffix. Pre-release
/// stages (alpha, beta, rc, or implementation-defined unstable stages) are
/// formatted according to the encoded stage type and stage number.
///
/// This function behaves like snprintf: it writes at most \p bufsize bytes
/// (including the null terminator), never overflows the buffer, and returns
/// the number of characters that would have been written if the buffer were
/// sufficiently large.
///
/// \param buffer  Destination buffer for the formatted version string.
/// \param bufsize Size of the destination buffer in bytes, including the
///                terminating null character.
/// \param version Packed 32-bit version value to format.
///
/// \return The number of characters that would have been written, excluding
///         the terminating null character. If the return value is greater
///         than or equal to \p bufsize, the output has been truncated.
////////////////////////////////////////////////////////////////////////////////
BANJO_EXPORT size_t bj_format_version(
    char*    buffer,
    size_t   bufsize,
    uint32_t version
);

#endif
