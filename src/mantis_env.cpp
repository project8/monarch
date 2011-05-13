#include "mantis_env.hpp"

mantis_env::mantis_env()
  : outName("mantis_out.h5"), // default output name
    clockRate(500.0),         // default ADC clock rate (MHz)
    runLength(600)            // default run length in seconds
{ /* no-op */ }

void mantis_env::setOutName(std::string newOutName)
{
  (*this).outName = newOutName;
  return;
}

std::string mantis_env::getOutName()
{
  return (*this).outName;
}

void mantis_env::setClockRate(std::string clockRateStr)
{
  try {
    std::istringstream(clockRateStr) >> (*this).clockRate;
  }
  catch (std::exception& e) {
    throw new clock_rate_exception(clockRateStr);
  }
  return;
}

double mantis_env::getClockRate()
{
  return (*this).clockRate;
}

void mantis_env::setRunLength(std::string runLengthStr)
{
  try {
    std::istringstream(runLengthStr) >> (*this).runLength;
  }
  catch (std::exception& e) {
    throw new run_length_exception(runLengthStr);
  }

  return;
}

std::size_t mantis_env::getRunLength()
{
  return (*this).runLength;
}

safeEnvPtr mantis_env::parseArgs(int argc, char** argv)
{
  safeEnvPtr result(new mantis_env());
 
  int c;
  try {
    while((c = getopt(argc,argv,"o:r:d:")) != -1) {    
      switch(c) {
      case 'o':
	result->setOutName(optarg);
	break;
      case 'r':
	result->setClockRate(optarg);
	break;
      case 'd':
	result->setRunLength(optarg);
	break;
      default:
	throw new argument_exception();
      }
    }
  }
  catch(argument_exception* e) {
    throw e;
  }
  return result;
}

void mantis_env::verifyEnvironment(safeEnvPtr someEnvironment)
{
  return;
}

std::ostream& operator << (std::ostream& outstream, safeEnvPtr obj)
{
  outstream << obj->getOutName();
  return outstream;
}
