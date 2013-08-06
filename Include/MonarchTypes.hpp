#ifndef __MONARCH_TYPES_HPP
#define __MONARCH_TYPES_HPP

// We need clock_t
#include <stdint.h>
#include <ctime>
#include <string>

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

template<bool> struct static_assert {};
template<> struct static_assert<true> {};  //specialization

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

typedef uint64_t AcquisitionIdType; // 8 bytes
typedef uint64_t RecordIdType; // 8 bytes
typedef uint64_t TimeType; // 8 bytes
typedef unsigned char DataType;

static const std::string sDateTimeFormat("%Y-%m-%d %H:%M:%S %z");
//static const char sRecordTimeCalSep = '#';

#endif // __MONARCH_TYPES_HPP
