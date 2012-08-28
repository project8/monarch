#ifndef MONARCHRECORD_HPP_
#define MONARCHRECORD_HPP_

#include "MonarchTypes.hpp"
#include <ctime>

struct MonarchRecord
{
        ChIdType fCId;
        AcqIdType fAId;
        RecIdType fRId;
        ClockType fTick;
        DataType fDataPtr[];
};

#endif
