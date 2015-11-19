/*
 * M3Exception.hh
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 */

#ifndef M3EXCEPTION_HH_
#define M3EXCEPTION_HH_

#include "M3Constants.hh"

#include <exception>
#include <string>
#include <sstream>

namespace monarch3
{

    /*!
     @class M3Exception
     @author D. Furse

     @brief Specialized exception class for Monarch3

     @details
    */
    class M3_API M3Exception : public std::exception
    {
        public:
            M3Exception();
            M3Exception( const M3Exception& aCopy );
            virtual ~M3Exception() throw();

        public:
            virtual const char* what() const throw();

            template< class XType >
            M3Exception& operator<< ( const XType& aReference )
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

#endif /* M3EXCEPTION_HH_ */
