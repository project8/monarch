#ifndef M2RECORD_HH_
#define M2RECORD_HH_

#include "M2Exception.hh"
#include "M2Types.hh"

namespace monarch2
{
    template< typename DataType >
    struct M2Record
    {
            AcquisitionIdType fAcquisitionId;
            RecordIdType fRecordId;
            TimeType fTime;
            DataType fData[];
    };

    typedef M2Record< byte_type > M2RecordBytes;

    template< typename ReturnType >
    class M2RecordDataInterface
    {
        public:
            M2RecordDataInterface( const byte_type* aData, unsigned aDataTypeSize ) :
                fByteData( aData )
            {
                SetDataTypeSize( aDataTypeSize );
            }
            ~M2RecordDataInterface()
            {
            }

            ReturnType at( unsigned index ) const
            {
                return (this->*fArrayFcn)( index );
            }

            void SetDataTypeSize( unsigned aDataTypeSize )
            {
                if( aDataTypeSize == 1 ) fArrayFcn = &M2RecordDataInterface< ReturnType >::at_1_byte;
                else if( aDataTypeSize == 2 )  fArrayFcn = &M2RecordDataInterface< ReturnType >::at_2_byte;
                else if( aDataTypeSize == 4 )  fArrayFcn = &M2RecordDataInterface< ReturnType >::at_4_byte;
                else if( aDataTypeSize == 8 )  fArrayFcn = &M2RecordDataInterface< ReturnType >::at_8_byte;
                else
                {
                    throw M2Exception() << "unable to make a record data interface with data type size " << aDataTypeSize;
                }
                return;
            }

            void SetData( const byte_type* aData )
            {
                fByteData = aData;
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

            ReturnType (M2RecordDataInterface::*fArrayFcn)( unsigned ) const;

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
