/*
 * M4Constants.hh
 *
 *  Created on: Apr 12, 2023
 *      Author: N.S. Oblath
 */

#ifndef M4CONSTANTS_HH_
#define M4CONSTANTS_HH_

// API export macros for windows
#ifdef _WIN32
#  ifdef M4_API_EXPORTS
#    define M4_API __declspec(dllexport)
#    define M4_EXPIMP_TEMPLATE
#  else
#    define M4_API __declspec(dllimport)
#    define M4_EXPIMP_TEMPLATE extern
#  endif
#else
#  define M4_API
#endif

#ifdef _WIN32
#include <Windows.h>
#endif

#include <inttypes.h>
#include <limits>

namespace monarch4
{
    // channel information

    /// Data Format
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

}

#endif // M4CONSTANTS_HH_
