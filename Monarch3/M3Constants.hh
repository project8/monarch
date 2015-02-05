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
#    define M3_EXPIMP_TEMPLATE
#  endif
#else
#  define M3_API
#endif

#include "thorax.hh"

namespace monarch3
{
    // channel information

    /// Specifies whether the data is digitized or analog
    static const uint32_t sDigitized = 0;
    static const uint32_t sAnalog = 1;

    // stream information

    /// Specifies whether the data channels are interleaved or separate in a stream
    static const uint32_t sInterleaved = 0;
    static const uint32_t sSeparate = 1;

    // re-typdefing for aesthetic purposes
    typedef acquisition_id_type AcquisitionIdType; // 8 bytes
    typedef record_id_type RecordIdType; // 8 bytes
    typedef time_nsec_type TimeType; // 8 bytes

}

#endif // M3CONSTANTS_HH_
