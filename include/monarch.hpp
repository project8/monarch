#ifndef __MONARCH_HPP
#define __MONARCH_HPP

#include "MonarchIO.hpp"
#include "MonarchTypes.hpp"

class Monarch {
private:
  // Default constructor is private, naturally
  Monarch();

  // The MonarchIO class is the file pointer abstraction that we use
  // to read and write data.
  MonarchIO* io;

public:
  // Some bits for configuration flags.
  static const AcquisitionMode OneChannel = 0x03;
  static const AcquisitionMode TwoChannel = 0x04;

  // The simplest constructor takes a string filename, an access mode, and
  // an acquisition mode which determines the number of channels in the data
  // stream.
  Monarch(std::string filename, 
	  AccessMode iomode, 
	  AcquisitionMode datamode);
};

#endif
