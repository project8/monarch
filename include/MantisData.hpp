#ifndef MANTISDATA_HPP_
#define MANTISDATA_HPP_

#include <ctime>
#include "px1500.h"

class MantisData
{
    public:
        typedef unsigned long int IdType;
        typedef clock_t ClockType;
        typedef px4_sample_t DataType;
    
    public:
        MantisData();
        ~MantisData();
    
        IdType fId;
        ClockType fTick;
        DataType* fDataPtr;
};

#endif
