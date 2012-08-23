#ifndef __MONARCH_EXC_HPP
#define __MONARCH_EXC_HPP

#include <stdexcept>
#include <string>

namespace MonarchExceptions
{
    class NoFile :
        public std::logic_error
    {
        public:
            NoFile( std::string const& why );
    };

    class EndOfFile :
        public std::logic_error
    {
        public:
            EndOfFile( std::string const& why );
    };

}
;

#endif  // __MONARCH_EXC_HPP
