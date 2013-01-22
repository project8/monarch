#ifndef __MONARCH_TYPES_HPP
#define __MONARCH_TYPES_HPP

// We need clock_t
#include <ctime>

typedef size_t PreludeType;

typedef unsigned int AccessModeType;
static const AccessModeType sAccessRead = 0;
static const AccessModeType sAccessWrite = 1;

typedef unsigned int InterfaceModeType;
static const AccessModeType sInterfaceInterleaved = 0;
static const AccessModeType sInterfaceSeparate = 1;

typedef unsigned int ContentModeType;
static const ContentModeType sContentSignal = 0;
static const ContentModeType sContentBackground = 1;
static const ContentModeType sContentOther = 2;

typedef unsigned int SourceModeType;
static const SourceModeType sSourceMantis = 0;
static const SourceModeType sSourceSimulation = 1;

typedef unsigned int FormatModeType;
static const FormatModeType sFormatSingle = 0;
static const FormatModeType sFormatSeparateDual = 1;
static const FormatModeType sFormatInterleavedDual = 2;

typedef unsigned long int AcquisitionIdType; // 8 bytes
typedef unsigned long int RecordIdType; // 8 bytes
typedef long int TimeType; // 8 bytes (alias to long int in most systems)
typedef unsigned char DataType;

#endif // __MONARCH_TYPES_HPP
