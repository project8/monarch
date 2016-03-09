#ifndef M2EXCEPTION_HH_
#define M2EXCEPTION_HH_

#include <exception>
#include <sstream>

namespace monarch2
{

    class M2Exception :
            public std::exception
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
            std::stringstream fStream;
            };

}

#endif
