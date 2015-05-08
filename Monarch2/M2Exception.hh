#ifndef M2EXCEPTION_HH_
#define M2EXCEPTION_HH_

#include <exception>
using std::exception;

#include <sstream>
using std::stringstream;

namespace monarch2
{

    class M2Exception :
            public exception
            {
                public:
            M2Exception();
            M2Exception( const M2Exception& aCopy );
            virtual ~M2Exception() throw();

                public:
            virtual const char* what() const throw();

            template< class XType >
            M2Exception& operator<< ( const XType& aReference )
            {
                    fStream << aReference;
                    return *this;
            }

                private:
            stringstream fStream;
            };

}

#endif
