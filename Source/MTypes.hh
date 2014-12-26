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
    struct MH5TypeAccess
    {
        static H5::DataType GetType( T )
        {
            throw MException() << "Unknown type requested";
        }
    };

    template<>
    struct MH5TypeAccess< unsigned >
    {
        static H5::DataType GetType()
        {
            return H5::PredType::NATIVE_UINT;
        }
    };

    template<>
    struct MH5TypeAccess< int >
    {
        static H5::DataType GetType()
        {
            return H5::PredType::NATIVE_INT;
        }
    };

    template<>
    struct MH5TypeAccess< unsigned long >
    {
        static H5::DataType GetType()
        {
            return H5::PredType::NATIVE_ULONG;
        }
    };

    template<>
    struct MH5TypeAccess< long >
    {
        static H5::DataType GetType()
        {
            return H5::PredType::NATIVE_LONG;
        }
    };

    template<>
    struct MH5TypeAccess< float >
    {
        static H5::DataType GetType()
        {
            return H5::PredType::NATIVE_FLOAT;
        }
    };

    template<>
    struct MH5TypeAccess< double >
    {
        static H5::DataType GetType()
        {
            return H5::PredType::NATIVE_DOUBLE;
        }
    };

    template<>
    struct MH5TypeAccess< std::string >
    {
        static H5::DataType GetType()
        {
            return H5::StrType( H5::PredType::C_S1, 0 );
        }
        static H5::DataType GetType( const std::string& aString )
        {
            return H5::StrType( H5::PredType::C_S1, aString.length() + 1 );
        }
        static H5::DataType GetType( size_t aSize )
        {
            return H5::StrType( H5::PredType::C_S1, aSize + 1 );
        }
    };

}

#endif // MTYPES_HH_
