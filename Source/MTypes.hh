/*
 * MTypes.hh
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 */

#ifndef MTYPES_HH_
#define MTYPES_HH_

#include "MException.hh"

#include "H5Cpp.h"

#include <string>

#include "thorax.hh"

namespace monarch
{

    template<typename T, typename U>
    struct is_same
    {
        static const bool value = false;
    };

    template<typename T>
    struct is_same<T,T>  //specialization
    {
       static const bool value = true;
    };

    template<bool> struct staticassert;
    template<> struct staticassert<true> {};  //specialization

    // channel information

    typedef unsigned DataFormatType;
    static const DataFormatType sDigitized = 0;
    static const DataFormatType sAnalog = 1;

    // stream information

    typedef uint32_t MultiChannelFormatType;
    static const MultiChannelFormatType sInterleaved = 0;
    static const MultiChannelFormatType sSeparate = 1;

    /*

    typedef size_t PreludeType;

    typedef uint32_t AccessModeType;
    static const AccessModeType sAccessRead = 0;
    static const AccessModeType sAccessWrite = 1;

    typedef uint32_t InterfaceModeType;
    static const AccessModeType sInterfaceInterleaved = 0;
    static const AccessModeType sInterfaceSeparate = 1;

    typedef uint32_t AcquisitionModeType;
    static const AcquisitionModeType sOneChannel = 1;
    static const AcquisitionModeType sTwoChannel = 2;

    typedef uint32_t RunType;
    static const RunType sRunTypeSignal = 0;
    static const RunType sRunTypeBackground = 1;
    static const RunType sRunTypeOther = 999;

    typedef uint32_t RunSourceType;
    static const RunSourceType sSourceMantis = 0;
    static const RunSourceType sSourceSimulation = 1;

    typedef uint32_t FormatModeType;
    static const FormatModeType sFormatSingle = 0;
    static const FormatModeType sFormatMultiSeparate = 1;
    static const FormatModeType sFormatMultiInterleaved = 2;

    */
    // re-typdefing for aesthetic purposes
    typedef acquisition_id_type AcquisitionIdType; // 8 bytes
    typedef record_id_type RecordIdType; // 8 bytes
    typedef time_nsec_type TimeType; // 8 bytes

    template< typename T >
    struct MH5Type
    {
        static H5::DataType Native( T )
        {
            throw MException() << "Unknown native type requested";
        }
        static H5::DataType H5( T )
        {
            throw MException() << "Unknown H5 type requested";
        }
    };

    template<>
    struct MH5Type< int8_t >
    {
        static H5::DataType Native()
        {
            return H5::PredType::NATIVE_INT8;
        }
        static H5::DataType H5()
        {
            return H5::PredType::STD_I8LE;
        }
    };

    template<>
    struct MH5Type< uint8_t >
    {
        static H5::DataType Native()
        {
            return H5::PredType::NATIVE_UINT8;
        }
        static H5::DataType H5()
        {
            return H5::PredType::STD_U8LE;
        }
    };

    template<>
    struct MH5Type< int16_t >
    {
        static H5::DataType Native()
        {
            return H5::PredType::NATIVE_INT16;
        }
        static H5::DataType H5()
        {
            return H5::PredType::STD_I16LE;
        }
    };

    template<>
    struct MH5Type< uint16_t >
    {
        static H5::DataType Native()
        {
            return H5::PredType::NATIVE_UINT16;
        }
        static H5::DataType H5()
        {
            return H5::PredType::STD_U16LE;
        }
    };

    template<>
    struct MH5Type< int32_t >
    {
        static H5::DataType Native()
        {
            return H5::PredType::NATIVE_INT32;
        }
        static H5::DataType H5()
        {
            return H5::PredType::STD_I32LE;
        }
    };

    template<>
    struct MH5Type< uint32_t >
    {
        static H5::DataType Native()
        {
            return H5::PredType::NATIVE_UINT32;
        }
        static H5::DataType H5()
        {
            return H5::PredType::STD_U32LE;
        }
    };

    template<>
    struct MH5Type< int64_t >
    {
        static H5::DataType Native()
        {
            return H5::PredType::NATIVE_INT64;
        }
        static H5::DataType H5()
        {
            return H5::PredType::STD_I64LE;
        }
    };

    template<>
    struct MH5Type< uint64_t >
    {
        static H5::DataType Native()
        {
            return H5::PredType::NATIVE_UINT64;
        }
        static H5::DataType H5()
        {
            return H5::PredType::STD_U64LE;
        }
    };

    template<>
    struct MH5Type< float >
    {
        static H5::DataType Native()
        {
            return H5::PredType::NATIVE_FLOAT;
        }
        static H5::DataType H5()
        {
            return H5::PredType::IEEE_F32LE;
        }
    };

    template<>
    struct MH5Type< double >
    {
        static H5::DataType Native()
        {
            return H5::PredType::NATIVE_DOUBLE;
        }
        static H5::DataType H5()
        {
            return H5::PredType::IEEE_F64LE;
        }
    };

    template<>
    struct MH5Type< std::string >
    {
        static H5::DataType Native()
        {
            return H5::StrType( H5::PredType::NATIVE_CHAR, 0 );
        }
        static H5::DataType Native( const std::string& aString )
        {
            return H5::StrType( H5::PredType::NATIVE_CHAR, aString.length() + 1 );
        }
        static H5::DataType Native( size_t aSize )
        {
            return H5::StrType( H5::PredType::NATIVE_CHAR, aSize + 1 );
        }
        static H5::DataType H5()
        {
            return H5::StrType( H5::PredType::C_S1, 0 );
        }
        static H5::DataType H5( const std::string& aString )
        {
            return H5::StrType( H5::PredType::C_S1, aString.length() + 1 );
        }
        static H5::DataType H5( size_t aSize )
        {
            return H5::StrType( H5::PredType::C_S1, aSize + 1 );
        }
    };

}

#endif // MTYPES_HH_
