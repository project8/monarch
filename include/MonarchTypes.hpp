#ifndef __MONARCH_TYPES_HPP
#define __MONARCH_TYPES_HPP

// We need clock_t
#include <ctime>

typedef unsigned int AccessModeType;
static const AccessModeType sWriteMode = 1;
static const AccessModeType sReadMode = 2;

typedef unsigned int AcquisitionModeType;
static const AcquisitionModeType sOneChannel = 1;
static const AcquisitionModeType sTwoChannel = 2;

typedef size_t PreludeType;

typedef unsigned int AcqIdType;
typedef unsigned int ChIdType;
typedef unsigned long int RecIdType;
typedef clock_t ClockType;
typedef char DataType;

#endif // __MONARCH_TYPES_HPP
