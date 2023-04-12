/*
 * M4Exception.cc
 *
 *  Created on: Apr 12, 2023
 *      Author: N.S. Oblath
 */
#define M4_API_EXPORTS

#include "M4Exception.hh"

namespace monarch4
{

    M4Exception::M4Exception() :
            exception(),
            fMessage()
    {
    }
    M4Exception::M4Exception( const M4Exception& aCopy ) :
            exception( aCopy ),
            fMessage( aCopy.fMessage )
    {
    }
    M4Exception::~M4Exception() throw ()
    {
    }

    //void M4Exception::SetWhat( const char* aWhat )
    //{
    //    fMessage = std::string( aWhat );
    //    //fStream << aWhat;
    //}

    const char* M4Exception::what() const throw ()
    {
        return fMessage.c_str();
    }

}
