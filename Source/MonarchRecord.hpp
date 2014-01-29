#ifndef MONARCHRECORD_HPP_
#define MONARCHRECORD_HPP_

#include "MonarchTypes.hpp"

namespace monarch
{
    template< typename DataType >
    struct MonarchRecord
    {
            AcquisitionIdType fAcquisitionId;
            RecordIdType fRecordId;
            TimeType fTime;
            DataType fData[];
    };

    typedef MonarchRecord< char > MonarchRecordBytes;

}

#endif
