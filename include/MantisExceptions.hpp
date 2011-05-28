#ifndef __mantis_exception_hpp
#define __mantis_exception_hpp

#include <exception>
#include <string>

class argument_exception : public std::exception
{
public:
  virtual const char* what();
};

class clock_rate_exception : public argument_exception
{
private:
  std::string bad_clock_rate;
  clock_rate_exception();
public:
  ~clock_rate_exception() throw ();
  clock_rate_exception(std::string);
  const char* what();
};

class run_length_exception : public argument_exception
{
private:
  std::string bad_run_length;
  run_length_exception();
public:
  ~run_length_exception() throw ();
  run_length_exception(std::string);

  const char* what();
};

#endif //__mantis_exception_hpp
