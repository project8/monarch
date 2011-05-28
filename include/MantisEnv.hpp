#ifndef __env_hpp
#define __env_hpp

#include "MantisExceptions.hpp"
#include <sstream>
#include <memory>
#include <string>
#include <iostream>
#include <ostream>
#include <getopt.h>
#include <ctime>

class MantisEnv;
typedef std::auto_ptr<MantisEnv> safeEnvPtr;

class MantisEnv
{

private:
  std::string outName;
  double clockRate;
  std::size_t bufferCount;
  std::size_t runLength;
public:
  MantisEnv();
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
