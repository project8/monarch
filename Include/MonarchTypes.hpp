#ifndef __MONARCH_TYPES_HPP
#define __MONARCH_TYPES_HPP

// We need clock_t
#include <ctime>

typedef unsigned int AccessModeType;
static const AccessModeType sWriteMode = 1;
static const AccessModeType sReadMode = 2;

typedef unsigned int AcquisitionModeType;
static const AcquisitionModeType sOneChannel = 3;
static const AcquisitionModeType sTwoChannel = 4;

typedef size_t PreludeType;

/*
 * very important note -- the first three pieces are
 * purposefully 8 bytes to keep things from crossing the 8 byte boundary.
 * even if it seems inefficient to use a long int for something,
 * please don't change it, as it's this way for good reason.
 */

typedef unsigned long int AcqIdType; // 8 bytes
typedef unsigned long int RecIdType; // 8 bytes
typedef clock_t ClockType; // 8 bytes (alias to long int in most systems)
typedef unsigned char DataType;

#endif // __MONARCH_TYPES_HPP
