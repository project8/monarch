#ifndef __MONARCH_TYPES_HPP
#define __MONARCH_TYPES_HPP

// We need clock_t
#include <ctime>

// Modes for access and data acquisition
// The integer equivalents are hard-coded to preserve backwards compatibility with files written before
// these flags were enums. Please do not change them!
enum AccessMode
{
    WriteMode = 1,
    ReadMode = 2
};
enum AcquisitionMode
{
    OneChannel = 3,
    TwoChannel = 4
};

// These types are used in the file format to
// identify fields in the data records.
typedef unsigned int AcqIdType;
typedef unsigned int ChIdType;
typedef unsigned long int RecIdType;
typedef clock_t ClockType;
typedef char DataType;

#endif // __MONARCH_TYPES_HPP
