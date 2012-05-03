#include "MantisExceptions.hpp"

const char* argument_exception::what()
{
  return "unknown error occurred while parsing arguments.";
}

clock_rate_exception::clock_rate_exception(std::string badRate) :
  bad_clock_rate(badRate)
{ /* no-op */ }

const char* clock_rate_exception::what()
{
  return ("invalid choice of clock rate: "+(*this).bad_clock_rate).c_str();
}

clock_rate_exception::~clock_rate_exception() throw ()
{ /* no-op */ }

run_length_exception::run_length_exception(std::string badLength) :
  bad_run_length(badLength)
{ /* no-op */ }

const char* run_length_exception::what()
{
  return ("invalid choice of run length: "+(*this).bad_run_length).c_str();
}

run_length_exception::~run_length_exception() throw ()
{ /* no-op */ }
