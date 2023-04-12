/*
 * M4Exception.hh
 *
 *  Created on: Apr 12, 2023
 *      Author: N.S. Oblath
 */

#ifndef M4EXCEPTION_HH_
#define M4EXCEPTION_HH_

#include "M4Constants.hh"

#include <exception>
#include <string>
#include <sstream>

namespace monarch4
{

    /*!
     @class M4Exception
     @author D. Furse

     @brief Specialized exception class for Monarch4

     @details
    */
    class M4_API M4Exception : public std::exception
    {
        public:
            M4Exception();
            M4Exception( const M4Exception& aCopy );
            virtual ~M4Exception() throw();

        public:
            virtual const char* what() const throw();

            template< class XType >
            M4Exception& operator<< ( const XType& aReference )
            {
                std::stringstream tStream;
                tStream << aReference;
                fMessage += tStream.str();
                return *this;
            }

        private:
            std::string fMessage;
    };

}

#endif /* M4EXCEPTION_HH_ */
