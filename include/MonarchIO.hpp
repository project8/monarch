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
public:
  // Access Modes
  static const AccessMode WriteMode = 0x01;
  static const AccessMode ReadMode  = 0x02;
  
  // Constructors and Destructors
  MonarchIO(std::string filename,
	    AccessMode iomode);
  ~MonarchIO();

  // API
  
  // Write nbytes of data from the byte array wbuf to the
  // current position of the file pointer.
  bool Write(unsigned char* wbuf, std::size_t nbytes);

  // Read nbytes of data from the file pointer and store
  // the result in the byte array rbuf.
  std::size_t Read(unsigned char* rbuf, std::size_t nbytes);

  // Close the file handle owned by this IO object
  bool Close();
};
