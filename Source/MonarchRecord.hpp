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

    typedef MonarchRecord< byte_type > MonarchRecordBytes;

    struct MonarchRecordDataInterface
    {
            MonarchRecordDataInterface( const byte_type* aData, size_t aDataTypeSize )
            {
                fData = aData;
                fDataTypeSize = aDataTypeSize;
            }

            template< typename DataType >
            DataType at( unsigned index ) const
            {
                return (DataType)( fData[ index * fDataTypeSize ] );
            }

            const byte_type* fData;
            size_t fDataTypeSize;
    };

}

#endif
