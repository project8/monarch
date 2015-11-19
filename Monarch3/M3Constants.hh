/*
 * MConstants.hh
 *
 *  Created on: Jan 31, 2015
 *      Author: nsoblath
 */

#ifndef M3CONSTANTS_HH_
#define M3CONSTANTS_HH_

// API export macros for windows
#ifdef _WIN32
#  ifdef M3_API_EXPORTS
#    define M3_API __declspec(dllexport)
#    define M3_EXPIMP_TEMPLATE
#  else
#    define M3_API __declspec(dllimport)
#    define M3_EXPIMP_TEMPLATE extern
#  endif
#else
#  define M3_API
#endif

#ifdef _WIN32
// to be able to use numeric_limits<>:max in windows
#define NOMINMAX
#include <Windows.h>
#endif

#include <inttypes.h>
#include <limits>

namespace monarch3
{
    // channel information

    /// Specifies whether the data is digitized (unsigned or signed) or analog
    static const uint32_t sInvalidFormat = std::numeric_limits< uint32_t >::max();
    static const uint32_t sDigitizedUS = 0;
    static const uint32_t sDigitizedS = 1;
    static const uint32_t sAnalog = 2;

    /// Specifies whether the meaningful bits for each sample are aligned to the left or right in each sample word
    /// e.g. For 14-bit digitizer bit depth, represented by a 16-bit sample word, are the 14 bits aligned to the left or right in the 16-bit word?
    static const uint32_t sBitsAlignedLeft = 0;
    static const uint32_t sBitsAlignedRight = 1;

    // stream information

    /// Specifies whether the data channels are interleaved or separate in a stream
    static const uint32_t sInterleaved = 0;
    static const uint32_t sSeparate = 1;

    // re-typdefing for aesthetic purposes
    typedef uint64_t AcquisitionIdType; // 8 bytes
    typedef uint64_t RecordIdType; // 8 bytes
    typedef uint64_t TimeType; // 8 bytes

}

#endif // M3CONSTANTS_HH_
