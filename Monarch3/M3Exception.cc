/*
 * MException.cc
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 */

#include "MException.hh"

namespace monarch
{

    MException::MException() :
            exception(),
            fStream( "" )
    {
    }
    MException::MException( const MException& aCopy ) :
            exception( aCopy ),
            fStream( aCopy.fStream.str() )
    {
    }
    MException::~MException() throw ()
    {
    }

    const char* MException::what() const throw ()
    {
        return fStream.str().c_str();
    }

}
