#ifndef __MONARCH_TYPES_HPP
#define __MONARCH_TYPES_HPP

// We need clock_t
#include <ctime>

// Modes for access and data acquisition
typedef char AccessMode;
typedef char AcquisitionMode;

// These types are used in the file format to
// identify fields in the data records.
typedef unsigned short AcqIdType;
typedef unsigned short ChIdType;
typedef unsigned long int RecIdType;
typedef clock_t ClockType;
typedef char DataType;

#endif // __MONARCH_TYPES_HPP
