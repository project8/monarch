#ifndef __env_hpp
#define __env_hpp

#include "mantis_exceptions.hpp"
#include <sstream>
#include <memory>
#include <string>
#include <iostream>
#include <ostream>
#include <getopt.h>
#include <ctime>

class mantis_env;
typedef std::auto_ptr<mantis_env> safeEnvPtr;

class mantis_env
{

private:
  std::string outName;
  double clockRate;
  std::size_t runLength;
public:
  mantis_env();
  static safeEnvPtr parseArgs(int argc,char** argv);
  static void verifyEnvironment(safeEnvPtr);
  void setOutName(std::string);
  std::string getOutName();
  void setClockRate(std::string);
  double getClockRate();
  void setRunLength(std::string);
  std::size_t getRunLength();

  friend std::ostream& operator << (std::ostream& outstream, 
				    safeEnvPtr&);
};

#endif
