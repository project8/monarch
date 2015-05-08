#include "M2Exception.hh"

namespace monarch2
{

    M2Exception::M2Exception() :
                exception(),
                fStream( "" )
    {
    }
    M2Exception::M2Exception( const M2Exception& aCopy ) :
                exception( aCopy ),
                fStream( aCopy.fStream.str() )
    {
    }
    M2Exception::~M2Exception() throw ()
        {
        }

    const char* M2Exception::what() const throw ()
        {
        return fStream.str().c_str();
        }

}
