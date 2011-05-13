#include "mantis_handlers.hpp"

void abort_run(int condition)
{
  switch(condition) {
  case run_end_normal:
    mantis_logger::Info("run ended normally.  cleaning up...");
    break;
  case run_end_killed:
    mantis_logger::Info("run termination requested.  cleaning up...");
    break;
  default:
    break; 
  }

  end_run_and_cleanup(condition);
}
