/* mantis_logger.cc
 * implementation of mantis_logger.
 */
#include "mantis_logger.hpp"

time_t mantis_logger::rawTimeBuf;
struct tm *mantis_logger::timeinfo;
char mantis_logger::fmtTimeBuf[23];

mantis_logger::mantis_logger()
{/* no-op */}

mantis_logger::~mantis_logger()
{/* no-op */}

void mantis_logger::updateTime()
{
  time(&mantis_logger::rawTimeBuf);
  mantis_logger::timeinfo = localtime(&mantis_logger::rawTimeBuf);
  strftime(mantis_logger::fmtTimeBuf,23,"[%m/%d/%y %H:%M:%S]",mantis_logger::timeinfo);
}

void mantis_logger::Warn(std::string warning)
{
  mantis_logger::updateTime();
  std::clog << mantis_logger::fmtTimeBuf
	    << " -W- "
	    << warning
	    << std::endl;
}

void mantis_logger::Error(std::string error)
{
  mantis_logger::updateTime();
  std::clog << mantis_logger::fmtTimeBuf
	    << " -E- "
	    << error
	    << std::endl;
}

void mantis_logger::Info(std::string info)
{
  mantis_logger::updateTime();
  std::clog << mantis_logger::fmtTimeBuf
	    << " -I- "
	    << info
	    << std::endl;
}
