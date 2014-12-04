/*
 * MException.hh
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 */

#ifndef MEXCEPTION_HH_
#define MEXCEPTION_HH_

#include <exception>
#include <sstream>

namespace monarch
{

    /*!
     @class MException
     @author D. Furse

     @brief Specialized exception class for Monarch

     @details
    */
    class MException : public std::exception
    {
        public:
            MException();
            MException( const MException& aCopy );
            virtual ~MException() throw();

        public:
            virtual const char* what() const throw();

            template< class XType >
            MException& operator<< ( const XType& aReference )
            {
                fStream << aReference;
                return *this;
            }

        private:
            std::stringstream fStream;
    };

}

#endif /* MEXCEPTION_HH_ */
