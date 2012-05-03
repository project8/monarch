#include "MantisHandlers.hpp"

void abort_run(int condition)
{
  switch(condition) {
  case run_end_normal:
    MantisLogger::Info("run ended normally.  cleaning up...");
    break;
  case run_end_killed:
    MantisLogger::Info("run termination requested.  cleaning up...");
    break;
  default:
    break; 
  }

  end_run_and_cleanup(condition);
}
