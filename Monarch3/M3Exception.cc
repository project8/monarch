/*
 * M3Exception.cc
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 */

#include "M3Exception.hh"

namespace monarch3
{

    M3Exception::M3Exception() :
            exception(),
            fStream( "" )
    {
    }
    M3Exception::M3Exception( const M3Exception& aCopy ) :
            exception( aCopy ),
            fStream( aCopy.fStream.str() )
    {
    }
    M3Exception::~M3Exception() throw ()
    {
    }

    const char* M3Exception::what() const throw ()
    {
        return fStream.str().c_str();
    }

}
