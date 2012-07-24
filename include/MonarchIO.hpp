#include <iostream>
#include <cstring>
#include <cstdio>

#include "MonarchTypes.hpp"
#include "MonarchRecord.hpp"
#include "MonarchSerializer.hpp"
#include "MonarchExceptions.hpp"

class MonarchIO {
  FILE *filePTR;
  AccessMode _iomode;
  std::size_t _recWidth;
public:
  // Access Modes
  static const AccessMode WriteMode = 0x01;
  static const AccessMode ReadMode  = 0x02;
  
  // Constructors and Destructors
  MonarchIO(std::string filename, 
	    AccessMode iomode,
	    std::size_t nbytes);
  ~MonarchIO();

  // API
  bool WriteRecord(MonarchRecord* data);
};
