#ifndef MONARCHEXCEPTION_HPP_
#define MONARCHEXCEPTION_HPP_

#include <exception>
using std::exception;

#include <sstream>
using std::stringstream;

namespace monarch
{

    class MonarchException :
            public exception
            {
                public:
            MonarchException();
            MonarchException( const MonarchException& aCopy );
            virtual ~MonarchException() throw();

                public:
            virtual const char* what() const throw();

            template< class XType >
            MonarchException& operator<< ( const XType& aReference )
            {
                    fStream << aReference;
                    return *this;
            }

                private:
            stringstream fStream;
            };

}

#endif
