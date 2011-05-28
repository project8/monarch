#ifndef __mantis_logger_hpp
#define __mantis_logger_hpp
/* MantisLogger.hpp
 * started by jared kofron <jared.kofron@gmail.com> 3/14/2011
 * provides an abstract logging interface for the project 8 daq.
 */

#include <string>
#include <iostream>
#include <memory>
#include <time.h>

class MantisLogger
{
private:
  MantisLogger();
  ~MantisLogger();
  static void updateTime();
  static char fmtTimeBuf[23];
  static time_t rawTimeBuf;
  static struct tm *timeinfo;
public:
  static void Warn(std::string);
  static void Error(std::string);
  static void Info(std::string);
};

#endif //__MantisLogger_hpp
