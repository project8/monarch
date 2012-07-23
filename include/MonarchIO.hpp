#include <iostream>
#include <cstring>
#include <cstdio>

#include "MonarchTypes.hpp"

class MonarchIO {
  FILE *filePTR;
  AccessMode _iomode;
public:
  // Access Modes
  static const AccessMode WriteMode = 0x01;
  static const AccessMode ReadMode  = 0x02;

  MonarchIO(std::string filename, AccessMode iomode);
  ~MonarchIO();
};
