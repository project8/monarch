#include "MonarchExceptions.hpp"

MonarchExceptions::NoFile::NoFile (std::string const& why) 
  : std::logic_error(why)
{}

MonarchExceptions::EndOfFile::EndOfFile (std::string const& why) 
  : std::logic_error(why)
{}
