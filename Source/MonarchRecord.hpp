#ifndef MONARCHRECORD_HPP_
#define MONARCHRECORD_HPP_

#include "MonarchException.hpp"
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

    template< typename ReturnType >
    class MonarchRecordDataInterface
    {
        public:
            MonarchRecordDataInterface( const byte_type* aData, unsigned aDataTypeSize ) :
                fByteData( aData )
            {
                SetDataTypeSize( aDataTypeSize );
            }
            ~MonarchRecordDataInterface()
            {
            }

            ReturnType at( unsigned index ) const
            {
                return (this->*fArrayFcn)( index );
            }

            void SetDataTypeSize( unsigned aDataTypeSize )
            {
                if( aDataTypeSize == 1 ) fArrayFcn = &MonarchRecordDataInterface< ReturnType >::at_1_byte;
                else if( aDataTypeSize == 2 )  fArrayFcn = &MonarchRecordDataInterface< ReturnType >::at_2_byte;
                else if( aDataTypeSize == 4 )  fArrayFcn = &MonarchRecordDataInterface< ReturnType >::at_4_byte;
                else if( aDataTypeSize == 8 )  fArrayFcn = &MonarchRecordDataInterface< ReturnType >::at_8_byte;
                else
                {
                    throw MonarchException() << "unable to make a record data interface with data type size " << aDataTypeSize;
                }
                return;
            }

        private:
            ReturnType at_1_byte( unsigned index ) const
            {
                return fByteData[ index ];
            }

            ReturnType at_2_byte( unsigned index ) const
            {
                return fTwoBytesData[ index ];
            }

            ReturnType at_4_byte( unsigned index ) const
            {
                return fFourBytesData[ index ];
            }

            ReturnType at_8_byte( unsigned index ) const
            {
                return fEightBytesData[ index ];
            }

            ReturnType (MonarchRecordDataInterface::*fArrayFcn)( unsigned ) const;

            union
            {
                const byte_type* fByteData;
                const uint16_t* fTwoBytesData;
                const uint32_t* fFourBytesData;
                const uint64_t* fEightBytesData;
            };
    };

}

#endif
