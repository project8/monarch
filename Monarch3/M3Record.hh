#ifndef M3RECORD_HH_
#define M3RECORD_HH_

#include "M3Exception.hh"
#include "M3MemberVariable.hh"
#include "M3Types.hh"

namespace monarch3
{
    class M3Record
    {
        public:
            M3Record( unsigned aNBytes = 0 );
            M3Record( byte_type* aDataPtr );
            ~M3Record();

            M3MEMBERVARIABLE( RecordIdType, RecordId );
            M3MEMBERVARIABLE( TimeType, Time );

            M3MEMBERVARIABLE_NOSET( bool, OwnsData );

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

    inline const byte_type* M3Record::GetData() const
    {
        return fData;
    }

    inline byte_type* M3Record::GetData()
    {
        return fData;
    }


    template< typename SetType >
    class M3RecordDataSetter
    {
        public:
            M3RecordDataSetter( byte_type* aData, unsigned aDataTypeSize, DataFormatType aDataFormat ) :
                fUByteData( aData )
            {
                SetInterface( aDataTypeSize, aDataFormat );
            }
            ~M3RecordDataSetter()
            {
            }

            void set_at( SetType value, unsigned index )
            {
                (this->*fArrayFcn)( value, index );
            }

            void SetInterface( unsigned aDataTypeSize, DataFormatType aDataFormat )
            {
                if( aDataFormat == sDigitized )
                {
                    if( aDataTypeSize == 1 ) fArrayFcn = &M3RecordDataSetter< SetType >::set_at_u1;
                    else if( aDataTypeSize == 2 )  fArrayFcn = &M3RecordDataSetter< SetType >::set_at_u2;
                    else if( aDataTypeSize == 4 )  fArrayFcn = &M3RecordDataSetter< SetType >::set_at_u4;
                    else if( aDataTypeSize == 8 )  fArrayFcn = &M3RecordDataSetter< SetType >::set_at_u8;
                    else
                    {
                        throw M3Exception() << "Unable to make a digitized data interface with data type size " << aDataTypeSize;
                    }
                }
                else // aDataFormat == sAnalog
                {
                    if( aDataTypeSize == 4 )  fArrayFcn = &M3RecordDataSetter< SetType >::set_at_f4;
                    else if( aDataTypeSize == 8 )  fArrayFcn = &M3RecordDataSetter< SetType >::set_at_f8;
                    else
                    {
                        throw M3Exception() << "Unable to make a analog data interface with data type size " << aDataTypeSize;
                    }                }
                return;
            }

            void SetData( const byte_type* aData )
            {
                fUByteData = aData;
            }

        private:
            void set_at_u1( SetType value, unsigned index )
            {
                fUByteData[ index ] = value;
            }

            void set_at_u2( SetType value, unsigned index )
            {
                fU2BytesData[ index ] = value;
            }

            void set_at_u4( SetType value, unsigned index )
            {
                fU4BytesData[ index ] = value;
            }

            void set_at_u8( SetType value, unsigned index )
            {
                fU8BytesData[ index ] = value;
            }

            void set_at_f4( SetType value, unsigned index )
            {
                fF4BytesData[ index ] = value;
            }

            void set_at_f8( SetType value, unsigned index )
            {
                fF8BytesData[ index ] = value;
            }

            void (M3RecordDataSetter::*fArrayFcn)( SetType, unsigned );

            union
            {
                byte_type* fUByteData;
                uint16_t* fU2BytesData;
                uint32_t* fU4BytesData;
                uint64_t* fU8BytesData;
                float* fF4BytesData;
                double* fF8BytesData;
            };
    };


    template< typename ReturnType >
    class M3RecordDataInterface
    {
        public:
            M3RecordDataInterface( const byte_type* aData, unsigned aDataTypeSize, DataFormatType aDataFormat ) :
                fUByteData( aData )
            {
                SetInterface( aDataTypeSize, aDataFormat );
            }
            ~M3RecordDataInterface()
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
                    if( aDataTypeSize == 1 ) fArrayFcn = &M3RecordDataInterface< ReturnType >::at_u1;
                    else if( aDataTypeSize == 2 )  fArrayFcn = &M3RecordDataInterface< ReturnType >::at_u2;
                    else if( aDataTypeSize == 4 )  fArrayFcn = &M3RecordDataInterface< ReturnType >::at_u4;
                    else if( aDataTypeSize == 8 )  fArrayFcn = &M3RecordDataInterface< ReturnType >::at_u8;
                    else
                    {
                        throw M3Exception() << "Unable to make a digitized data interface with data type size " << aDataTypeSize;
                    }
                }
                else // aDataFormat == sAnalog
                {
                    if( aDataTypeSize == 4 )  fArrayFcn = &M3RecordDataInterface< ReturnType >::at_f4;
                    else if( aDataTypeSize == 8 )  fArrayFcn = &M3RecordDataInterface< ReturnType >::at_f8;
                    else
                    {
                        throw M3Exception() << "Unable to make a analog data interface with data type size " << aDataTypeSize;
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

            ReturnType (M3RecordDataInterface::*fArrayFcn)( unsigned ) const;

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
