#include "Monarch.hpp"

Monarch::Monarch()
  : io(NULL), mem(NULL), rec(NULL)
{
  //empty, just here to make the compiler throw a
  //fit if someone tries to call a default constructor
}

Monarch::Monarch(std::string filename, 
		 AccessMode iomode,
		 AcquisitionMode datamode,
		 std::size_t DataWidth) 
  : 
  io( new MonarchIO(filename,iomode,DataWidth) ),
  mem(new char[DataWidth]), 
  rec( new (mem) MonarchSerializer<MonarchRecord>() )
{ }

Monarch* Monarch::Open(std::string filename, 
		       AccessMode iomode, 
		       AcquisitionMode datamode) {
  // First check for readmode 
  if(iomode == MonarchIO::ReadMode) {
    return Monarch::OpenForReading(filename);
  }
}

Monarch* Monarch::Open(std::string filename, 
		       AccessMode iomode) {
  return Monarch::Open(filename, iomode, OneChannel);
}

Monarch* Monarch::OpenForReading(std::string filename) {
  return new Monarch(filename, MonarchIO::ReadMode, OneChannel, 1024);
}
