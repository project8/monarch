#include "Monarch.hpp"

Monarch::Monarch()
  : filename(""), io(NULL), mem(NULL), rec(NULL)
{
  //empty, just here to make the compiler throw a
  //fit if someone tries to call a default constructor
}
Monarch::~Monarch() 
{
  delete io;
  rec->~MonarchRecord();
  delete mem;
}


Monarch::Monarch(std::string filename, 
		 AccessMode iomode,
		 AcquisitionMode datamode,
		 std::size_t DataWidth) 
  : 
  filename(filename),
  recsize(DataWidth),
  io( new MonarchIO(filename,iomode) ),
  mem(new char[DataWidth]), 
  rec( new (mem) MonarchRecord() )
{ }

Monarch* Monarch::Open(std::string filename, 
		       AccessMode iomode, 
		       AcquisitionMode datamode) {
  // First check for readmode 
  if(iomode == MonarchIO::ReadMode) {
    return Monarch::OpenForReading(filename);
  }
  else if(iomode == MonarchIO::WriteMode) {
    return Monarch::OpenForWriting(filename);
  }

  return NULL;
}

Monarch* Monarch::Open(std::string filename, 
		       AccessMode iomode) {
  return Monarch::Open(filename, iomode, OneChannel);
}

Monarch* Monarch::OpenForReading(std::string filename) {
  std::size_t rsize = sizeof(ChIdType) + sizeof(AcqIdType) + sizeof(RecIdType) + sizeof(ClockType) + 1024;
  return new Monarch(filename, MonarchIO::ReadMode, OneChannel, rsize);
}

Monarch* Monarch::OpenForWriting(std::string filename) {
  std::size_t rsize = sizeof(ChIdType) + sizeof(AcqIdType) + sizeof(RecIdType) + sizeof(ClockType) + 1024;
  return new Monarch(filename, MonarchIO::WriteMode, OneChannel, rsize);
}

bool Monarch::Close() {
  return (this->io->Close());
}

MonarchRecord* Monarch::NewRecord(std::size_t dsize) {
  std::size_t rsize = sizeof(ChIdType) + sizeof(AcqIdType) + sizeof(RecIdType) + sizeof(ClockType) + dsize;
  unsigned char* mem = new unsigned char[rsize]();
  return new (mem) MonarchRecord();
}

bool Monarch::WriteRecord(MonarchRecord* newrec) {
  return (this->io)->Write((unsigned char*)(newrec), this->recsize);
}

MonarchRecord* Monarch::GetNextEvent() {
  std::size_t res = (this->io)->Read((unsigned char*)(this->rec), this->recsize);
  if( res == this->recsize ) {
    return (this->rec);
  }
  else {
    throw MonarchExceptions::EndOfFile(this->filename);
  }
}
