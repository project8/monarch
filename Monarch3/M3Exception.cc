/*
 * M3Exception.cc
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 */
#define M3_API_EXPORTS

#include "M3Exception.hh"

namespace monarch3
{

    M3Exception::M3Exception() :
            exception(),
            fMessage()
    {
    }
    M3Exception::M3Exception( const M3Exception& aCopy ) :
            exception( aCopy ),
            fMessage( aCopy.fMessage )
    {
    }
    M3Exception::~M3Exception() throw ()
    {
    }

    //void M3Exception::SetWhat( const char* aWhat )
    //{
    //    fMessage = std::string( aWhat );
    //    //fStream << aWhat;
    //}

    const char* M3Exception::what() const throw ()
    {
        return fMessage.c_str();
    }

}
