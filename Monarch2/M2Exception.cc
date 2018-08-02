#include "M2Exception.hh"

namespace monarch2
{

    M2Exception::M2Exception() :
            exception(),
            fMessage( "" )
    {
    }
    M2Exception::M2Exception( const M2Exception& aCopy ) :
                exception( aCopy ),
                fMessage( aCopy.fMessage )
    {
    }
    M2Exception::~M2Exception() throw ()
    {
    }

    const char* M2Exception::what() const throw ()
    {
        return fMessage.c_str();
    }

}
