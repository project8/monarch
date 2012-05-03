/* MantisLogger.cc
 * implementation of MantisLogger.
 */
#include "MantisLogger.hpp"

time_t MantisLogger::rawTimeBuf;
struct tm *MantisLogger::timeinfo;
char MantisLogger::fmtTimeBuf[23];

MantisLogger::MantisLogger()
{/* no-op */}

MantisLogger::~MantisLogger()
{/* no-op */}

void MantisLogger::updateTime()
{
  time(&MantisLogger::rawTimeBuf);
  MantisLogger::timeinfo = localtime(&MantisLogger::rawTimeBuf);
  strftime(MantisLogger::fmtTimeBuf,23,"[%m/%d/%y %H:%M:%S]",MantisLogger::timeinfo);
}

void MantisLogger::Warn(std::string warning)
{
  MantisLogger::updateTime();
  std::clog << MantisLogger::fmtTimeBuf
	    << " -W- "
	    << warning
	    << std::endl;
}

void MantisLogger::Error(std::string error)
{
  MantisLogger::updateTime();
  std::clog << MantisLogger::fmtTimeBuf
	    << " -E- "
	    << error
	    << std::endl;
}

void MantisLogger::Info(std::string info)
{
  MantisLogger::updateTime();
  std::clog << MantisLogger::fmtTimeBuf
	    << " -I- "
	    << info
	    << std::endl;
}
