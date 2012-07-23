#include "MonarchIO.hpp"

MonarchIO::MonarchIO(std::string filename, AccessMode iomode) 
  : filePTR(NULL),
    _iomode(iomode)
{
  if(iomode == WriteMode) {
    filePTR = fopen(filename.c_str(), "wb");
  }
  else if (iomode == ReadMode) {
    filePTR = fopen(filename.c_str(), "rb");
  }
}

MonarchIO::~MonarchIO() {
  if(filePTR) {
    fclose(filePTR);
  }
}
