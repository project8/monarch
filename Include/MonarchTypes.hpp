#ifndef __MONARCH_TYPES_HPP
#define __MONARCH_TYPES_HPP

// We need clock_t
#include <ctime>
#include <string>
#include <stdint.h>

typedef size_t PreludeType;

typedef unsigned int AccessModeType;
static const AccessModeType sAccessRead = 0;
static const AccessModeType sAccessWrite = 1;

typedef unsigned int InterfaceModeType;
static const AccessModeType sInterfaceInterleaved = 0;
static const AccessModeType sInterfaceSeparate = 1;

typedef unsigned int AcquisitionModeType;
static const AcquisitionModeType sOneChannel = 1;
static const AcquisitionModeType sTwoChannel = 2;

typedef unsigned int RunType;
static const RunType sRunTypeSignal = 0;
static const RunType sRunTypeBackground = 1;
static const RunType sRunTypeOther = 999;

typedef unsigned int RunSourceType;
static const RunSourceType sSourceMantis = 0;
static const RunSourceType sSourceSimulation = 1;

typedef unsigned int FormatModeType;
static const FormatModeType sFormatSingle = 0;
static const FormatModeType sFormatMultiSeparate = 1;
static const FormatModeType sFormatMultiInterleaved = 2;

typedef uint32_t AcquisitionIdType; // 4 bytes
typedef uint32_t RecordIdType; // 4 bytes
typedef uint64_t TimeType; // 8 bytes
typedef uint8_t DataType; // 1 byte

static const std::string sDateTimeFormat("%Y-%m-%d %H:%M:%S %z");

#endif // __MONARCH_TYPES_HPP
