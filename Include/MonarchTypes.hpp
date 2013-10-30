#ifndef __MONARCH_TYPES_HPP
#define __MONARCH_TYPES_HPP

#include <string>

#include "thorax.hh"

template<typename T, typename U>
struct is_same
{
    static const bool value = false;
};

template<typename T>
struct is_same<T,T>  //specialization
{
   static const bool value = true;
};

template<bool> struct staticassert;
template<> struct staticassert<true> {};  //specialization

typedef size_t PreludeType;

typedef uint32_t AccessModeType;
static const AccessModeType sAccessRead = 0;
static const AccessModeType sAccessWrite = 1;

typedef uint32_t InterfaceModeType;
static const AccessModeType sInterfaceInterleaved = 0;
static const AccessModeType sInterfaceSeparate = 1;

typedef uint32_t AcquisitionModeType;
static const AcquisitionModeType sOneChannel = 1;
static const AcquisitionModeType sTwoChannel = 2;

typedef uint32_t RunType;
static const RunType sRunTypeSignal = 0;
static const RunType sRunTypeBackground = 1;
static const RunType sRunTypeOther = 999;

typedef uint32_t RunSourceType;
static const RunSourceType sSourceMantis = 0;
static const RunSourceType sSourceSimulation = 1;

typedef uint32_t FormatModeType;
static const FormatModeType sFormatSingle = 0;
static const FormatModeType sFormatMultiSeparate = 1;
static const FormatModeType sFormatMultiInterleaved = 2;

// re-typdefing for aesthetic purposes
typedef acquisition_id_type AcquisitionIdType; // 8 bytes
typedef record_id_type RecordIdType; // 8 bytes
typedef time_nsec_type TimeType; // 8 bytes
typedef data_type DataType; // 1 byte

#endif // __MONARCH_TYPES_HPP
