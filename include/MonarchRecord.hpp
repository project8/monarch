#ifndef MONARCHRECORD_HPP_
#define MONARCHRECORD_HPP_

#include <ctime>

class MonarchRecord
{
    public:
        typedef unsigned long int IdType;
        typedef clock_t ClockType;
        typedef char DataType;
    
    public:
        MonarchRecord();
        ~MonarchRecord();
    
        IdType fId;
        ClockType fTick;
        DataType* fDataPtr;
};

#endif
