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
  std::size_t runLength;
  std::size_t dataWidth;
  std::size_t bufferCount;
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
  void setDataWidth(std::string);
  std::size_t getDataWidth();
  void setBufferCount(std::string);
  std::size_t getBufferCount();

  friend std::ostream& operator << (std::ostream& outstream, 
				    safeEnvPtr&);
};

#endif
