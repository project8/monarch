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

    struct MonarchRecordDataInterface
    {
            MonarchRecordDataInterface( const char* aData, size_t aDataTypeSize )
            {
                fData = aData;
                fDataTypeSize = aDataTypeSize;
            }

            template< typename DataType >
            DataType at( unsigned index ) const
            {
                return (DataType)( fData[ index * fDataTypeSize ] );
            }

            const char* fData;
            size_t fDataTypeSize;
    };

}

#endif
