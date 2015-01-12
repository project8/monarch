#ifndef M3DATAINTERFACE_HH_
#define M3DATAINTERFACE_HH_

#include "M3Exception.hh"
#include "M3Types.hh"

namespace monarch3
{

    typedef float f4_complex[ 2 ];
    typedef double f8_complex[ 2 ];

    template< typename SetType >
    class M3DataWriter
    {
        public:
            M3DataWriter( byte_type* aData, unsigned aDataTypeSize, DataFormatType aDataFormat ) :
                fUByteData( aData )
            {
                SetInterface( aDataTypeSize, aDataFormat );
            }
            ~M3DataWriter()
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
                    if( aDataTypeSize == 1 ) fArrayFcn = &M3DataWriter< SetType >::set_at_u1;
                    else if( aDataTypeSize == 2 )  fArrayFcn = &M3DataWriter< SetType >::set_at_u2;
                    else if( aDataTypeSize == 4 )  fArrayFcn = &M3DataWriter< SetType >::set_at_u4;
                    else if( aDataTypeSize == 8 )  fArrayFcn = &M3DataWriter< SetType >::set_at_u8;
                    else
                    {
                        throw M3Exception() << "Unable to make a digitized data interface with data type size " << aDataTypeSize;
                    }
                }
                else if( aDataFormat == sAnalog )
                {
                    if( aDataTypeSize == 4 )  fArrayFcn = &M3DataWriter< SetType >::set_at_f4;
                    else if( aDataTypeSize == 8 )  fArrayFcn = &M3DataWriter< SetType >::set_at_f8;
                    else
                    {
                        throw M3Exception() << "Unable to make a analog data interface with data type size " << aDataTypeSize;
                    }
                }
                else
                {
                    throw M3Exception() << "Invalid combination of data format <" << aDataFormat << ">, data type size <" << aDataTypeSize << ">";
                }
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

            void (M3DataWriter::*fArrayFcn)( SetType, unsigned );

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

    template< typename SetType >
    class M3ComplexDataWriter
    {
        public:
            M3ComplexDataWriter( byte_type* aData, unsigned aDataTypeSize, DataFormatType aDataFormat, unsigned aSampleSize = 2 ) :
                fUByteData( aData )
            {
                SetInterface( aDataTypeSize, aDataFormat, aSampleSize );
            }
            ~M3ComplexDataWriter()
            {
            }

            void set_at( SetType value, unsigned index )
            {
                (this->*fArrayFcn)( value, index );
            }

            void SetInterface( unsigned aDataTypeSize, DataFormatType aDataFormat, unsigned aSampleSize = 2 )
            {
                if( aDataFormat == sDigitized && aSampleSize == 1 )
                {
                    if( aDataTypeSize == 1 ) fArrayFcn = &M3ComplexDataWriter< SetType >::set_at_u1;
                    else
                    {
                        throw M3Exception() << "Unable to make a digitized data interface with data type size " << aDataTypeSize;
                    }
                }
                else if( aDataFormat == sAnalog && aSampleSize == 2 )
                {
                    if( aDataTypeSize == 4 )  fArrayFcn = &M3ComplexDataWriter< SetType >::set_at_f4_comp;
                    else if( aDataTypeSize == 8 )  fArrayFcn = &M3ComplexDataWriter< SetType >::set_at_f8_comp;
                    else
                    {
                        throw M3Exception() << "Unable to make a analog data interface with data type size " << aDataTypeSize;
                    }
                }
                else
                {
                    throw M3Exception() << "Invalid combination of data format <" << aDataFormat << ">, data type size <" << aDataTypeSize << "> and sample size <" << aSampleSize << ">";
                }
                return;
            }

            void SetData( const byte_type* aData )
            {
                fUByteData = aData;
            }

        private:
            void set_at_u1( SetType value, unsigned index )
            {
                fUByteData[ index ] = value[ 0 ];
            }

            void set_at_f4_comp( SetType value, unsigned index )
            {
                fF4CompBytesData[ index ][ 0 ] = value[ 0 ];
                fF4CompBytesData[ index ][ 1 ] = value[ 1 ];
            }

            void set_at_f8_comp( SetType value, unsigned index )
            {
                fF8CompBytesData[ index ][ 0 ] = value[ 0 ];
                fF8CompBytesData[ index ][ 1 ] = value[ 1 ];
            }

            void (M3ComplexDataWriter::*fArrayFcn)( SetType, unsigned );

            union
            {
                byte_type* fUByteData;
                f4_complex* fF4CompBytesData;
                f8_complex* fF8CompBytesData;
            };
    };


    template< typename ReturnType >
    class M3DataReader
    {
        public:
            M3DataReader( const byte_type* aData, unsigned aDataTypeSize, DataFormatType aDataFormat ) :
                fUByteData( aData )
            {
                SetInterface( aDataTypeSize, aDataFormat );
            }
            ~M3DataReader()
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
                    if( aDataTypeSize == 1 ) fArrayFcn = &M3DataReader< ReturnType >::at_u1;
                    else if( aDataTypeSize == 2 )  fArrayFcn = &M3DataReader< ReturnType >::at_u2;
                    else if( aDataTypeSize == 4 )  fArrayFcn = &M3DataReader< ReturnType >::at_u4;
                    else if( aDataTypeSize == 8 )  fArrayFcn = &M3DataReader< ReturnType >::at_u8;
                    else
                    {
                        throw M3Exception() << "Unable to make a digitized data interface with data type size " << aDataTypeSize;
                    }
                }
                else if( aDataFormat == sAnalog )
                {
                    if( aDataTypeSize == 4 )  fArrayFcn = &M3DataReader< ReturnType >::at_f4;
                    else if( aDataTypeSize == 8 )  fArrayFcn = &M3DataReader< ReturnType >::at_f8;
                    else
                    {
                        throw M3Exception() << "Unable to make a analog data interface with data type size " << aDataTypeSize;
                    }
                }
                else
                {
                    throw M3Exception() << "Invalid combination of data format <" << aDataFormat << ">, data type size <" << aDataTypeSize << ">";
                }
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

            ReturnType (M3DataReader::*fArrayFcn)( unsigned ) const;

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

    template< typename ReturnType >
    class M3ComplexDataReader
    {
        public:
            M3ComplexDataReader( const byte_type* aData, unsigned aDataTypeSize, DataFormatType aDataFormat, unsigned aSampleSize = 2 ) :
                fUByteData( aData )
            {
                SetInterface( aDataTypeSize, aDataFormat, aSampleSize );
            }
            ~M3ComplexDataReader()
            {
            }

            const ReturnType& at( unsigned index ) const
            {
                return (this->*fArrayFcn)( index );
            }

            void SetInterface( unsigned aDataTypeSize, DataFormatType aDataFormat, unsigned aSampleSize = 2 )
            {
                if( aDataFormat == sDigitized && aSampleSize == 1 )
                {
                    if( aDataTypeSize == 1 ) fArrayFcn = &M3ComplexDataReader< ReturnType >::at_u1;
                    else
                    {
                        throw M3Exception() << "Unable to make a digitized data interface with data type size " << aDataTypeSize;
                    }
                }
                else if( aDataFormat == sAnalog && aSampleSize == 2 )
                {
                    if( aDataTypeSize == 4 )  fArrayFcn = &M3ComplexDataReader< ReturnType >::at_f4_comp;
                    else if( aDataTypeSize == 8 )  fArrayFcn = &M3ComplexDataReader< ReturnType >::at_f8_comp;
                    else
                    {
                        throw M3Exception() << "Unable to make a analog data interface with data type size " << aDataTypeSize;
                    }
                }
                else
                {
                    throw M3Exception() << "Invalid combination of data format <" << aDataFormat << ">, data type size <" << aDataTypeSize << "> and sample size <" << aSampleSize << ">";
                }
                return;
            }

            void SetData( const byte_type* aData )
            {
                fUByteData = aData;
            }

        private:
            const ReturnType& at_u1( unsigned index ) const
            {
                fBuffer[ 0 ] = fUByteData[ index ];
                fBuffer[ 1 ] = fUByteData[ index ];
                return fBuffer;
            }

            const ReturnType& at_f4_comp( unsigned index ) const
            {
                fBuffer[ 0 ] = fF4CompBytesData[ index ][ 0 ];
                fBuffer[ 1 ] = fF4CompBytesData[ index ][ 1 ];
                return fBuffer;
            }

            const ReturnType& at_f8_comp( unsigned index ) const
            {
                fBuffer[ 0 ] = fF8CompBytesData[ index ][ 0 ];
                fBuffer[ 1 ] = fF8CompBytesData[ index ][ 1 ];
                return fBuffer;
            }

            const ReturnType& (M3ComplexDataReader::*fArrayFcn)( unsigned ) const;

            union
            {
                const byte_type* fUByteData;
                const f4_complex* fF4CompBytesData;
                const f8_complex* fF8CompBytesData;
            };

            mutable ReturnType fBuffer;
    };

}

#endif /* M3DATAINTERFACE_HH_ */
