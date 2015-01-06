#ifndef MRECORD_HH_
#define MRECORD_HH_

#include "MException.hh"
#include "MMemberVariable.hh"
#include "MTypes.hh"

namespace monarch
{
    class MRecord
    {
        public:
            MRecord( unsigned aNBytes = 0 );
            MRecord( byte_type* aDataPtr );
            ~MRecord();

            MEMBERVARIABLE( RecordIdType, RecordId );
            MEMBERVARIABLE( TimeType, Time );

            MEMBERVARIABLE_NOSET( bool, OwnsData );

        public:
            const byte_type* GetData() const;
            byte_type* GetData();

            /// Allocate no memory for the record; data pointer is to NULL
            void SetData();
            /// Allocate aNBytes of memory for the record
            void SetData( unsigned aNBytes );
            /// Do not allocate memory for the record; instead point to someone else's memory.
            /// Warning: if the memory pointed to is de-allocated, use of this record should be discontinued
            void SetData( byte_type* aDataPtr );

            void ClearData();

        private:
            byte_type* fData;

    };

    inline const byte_type* MRecord::GetData() const
    {
        return fData;
    }

    inline byte_type* MRecord::GetData()
    {
        return fData;
    }

    template< typename ReturnType >
    class MRecordDataInterface
    {
        public:
            MRecordDataInterface( const byte_type* aData, unsigned aDataTypeSize, DataFormatType aDataFormat ) :
                fUByteData( aData )
            {
                SetInterface( aDataTypeSize, aDataFormat );
            }
            ~MRecordDataInterface()
            {
            }

            ReturnType at( unsigned index ) const
            {
                return (this->*fArrayFcn)( index );
            }

            void SetInterface( unsigned aDataTypeSize, DataFormatType aDataFormat )
            {
                if( aDataFormat == sDigitized )
                {
                    if( aDataTypeSize == 1 ) fArrayFcn = &MRecordDataInterface< ReturnType >::at_u1;
                    else if( aDataTypeSize == 2 )  fArrayFcn = &MRecordDataInterface< ReturnType >::at_u2;
                    else if( aDataTypeSize == 4 )  fArrayFcn = &MRecordDataInterface< ReturnType >::at_u4;
                    else if( aDataTypeSize == 8 )  fArrayFcn = &MRecordDataInterface< ReturnType >::at_u8;
                    else
                    {
                        throw MException() << "Unable to make a digitized data interface with data type size " << aDataTypeSize;
                    }
                }
                else // aDataFormat == sAnalog
                {
                    if( aDataTypeSize == 4 )  fArrayFcn = &MRecordDataInterface< ReturnType >::at_f4;
                    else if( aDataTypeSize == 8 )  fArrayFcn = &MRecordDataInterface< ReturnType >::at_f8;
                    else
                    {
                        throw MException() << "Unable to make a analog data interface with data type size " << aDataTypeSize;
                    }                }
                return;
            }

            void SetData( const byte_type* aData )
            {
                fUByteData = aData;
            }

        private:
            ReturnType at_u1( unsigned index ) const
            {
                return fUByteData[ index ];
            }

            ReturnType at_u2( unsigned index ) const
            {
                return fU2BytesData[ index ];
            }

            ReturnType at_u4( unsigned index ) const
            {
                return fU4BytesData[ index ];
            }

            ReturnType at_u8( unsigned index ) const
            {
                return fU8BytesData[ index ];
            }

            ReturnType at_f4( unsigned index ) const
            {
                return fF4BytesData[ index ];
            }

            ReturnType at_f8( unsigned index ) const
            {
                return fF8BytesData[ index ];
            }

            ReturnType (MRecordDataInterface::*fArrayFcn)( unsigned ) const;

            union
            {
                const byte_type* fUByteData;
                const uint16_t* fU2BytesData;
                const uint32_t* fU4BytesData;
                const uint64_t* fU8BytesData;
                const float* fF4BytesData;
                const double* fF8BytesData;
            };
    };

}

#endif
