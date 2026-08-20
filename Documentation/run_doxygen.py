# This script sets environment variables that are used by Doxygen

import os
from subprocess import call, check_output

# version
this_version = 'v?.?.?'
try:
    this_version = check_output(['git', 'describe', '--abbrev=0', '--tags']).decode('utf-8').strip()
except:
    pass

# environment variables used by Doxygen
os.environ['PROJECT_NAME'] = 'Monarch'
os.environ['PROJECT_NUMBER'] = this_version
os.environ['PROJECT_BRIEF_DESC'] = 'Library for reading and writing egg files'
# located in your documentation directory, or give the relative path from the documentation directory
#os.environ['PROJECT_LOGO'] = './documentation/images/Logo_55x55.png'

# directories in which doxygen should look for source files; if you have a `doxfiles` directory in your documentation, that should go here; string with space-separated directories
os.environ['DOXYGEN_INPUT'] = './Documentation/DoxFiles ./Monarch2 ./Monarch3'
# directories within DOXYGEN_INPUT that you want to exclude from doxygen (e.g. if there's  a submodule included that you don't want to index); string with space-separated directories
os.environ['DOXYGEN_EXCLUDE'] = ''
# directories outside of DOXYGEN_INPUT that you want the C preprocessor to look in for macro definitions (e.g. if there's a submodule not included that has relevant macros); string with space-separated directories
os.environ['PREPROC_INCLUDE_PATH'] = './Scarab/library/utility ./Scarab/library/logger'

# Doxygen
call(['doxygen', './Scarab/documentation/Doxyfile'])
