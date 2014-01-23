#include "MonarchException.hpp"

namespace Monarch
{

    MonarchException::MonarchException() :
                exception(),
                fStream( "" )
    {
    }
    MonarchException::MonarchException( const MonarchException& aCopy ) :
                exception( aCopy ),
                fStream( aCopy.fStream.str() )
    {
    }
    MonarchException::~MonarchException() throw ()
        {
        }

    const char* MonarchException::what() const throw ()
        {
        return fStream.str().c_str();
        }

}
