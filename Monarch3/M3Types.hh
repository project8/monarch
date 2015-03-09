/*
 * MTypes.hh
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 */

#ifndef M3TYPES_HH_
#define M3TYPES_HH_

#include "M3Exception.hh"

#include "H5Cpp.h"

#include <string>

namespace monarch3
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

    template< typename T >
    struct MH5Type
    {
        static H5::DataType Native( T )
        {
            throw M3Exception() << "Unknown native type requested";
        }
        static H5::DataType H5( T )
        {
            throw M3Exception() << "Unknown H5 type requested";
        }
    };

    template<>
    struct MH5Type< bool >
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
