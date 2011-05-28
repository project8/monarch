#ifndef __mantis_run_ctrl_hpp
#define __mantis_run_ctrl_hpp

#include "MantisStatus.hpp"
#include <cstdlib>
static const int env_arg_error = 1;
static const int run_end_normal = 0;
static const int run_end_killed = 2;
static const int run_end_error  = 3;
void end_run_and_cleanup(int);

#endif //__mantis_run_ctrl_hpp
