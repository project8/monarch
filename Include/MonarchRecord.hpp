#ifndef MONARCHRECORD_HPP_
#define MONARCHRECORD_HPP_

#include "MonarchTypes.hpp"
#include <ctime>

class MonarchRecord
{
    public:
        MonarchRecord();
        ~MonarchRecord();

        AcqIdType fAId;
        RecIdType fRId;
        ClockType fTick;
        DataType fDataPtr[];
};

#endif
