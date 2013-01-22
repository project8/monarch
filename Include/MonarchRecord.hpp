#ifndef MONARCHRECORD_HPP_
#define MONARCHRECORD_HPP_

#include "MonarchTypes.hpp"
#include <ctime>

class MonarchRecord
{
    public:
        MonarchRecord();
        ~MonarchRecord();

    public:
        AcquisitionIdType fAcquisitionId;
        RecordIdType fRecordId;
        TimeType fTime;
        DataType fData[];
};

#endif
