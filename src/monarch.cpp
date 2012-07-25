#include "Monarch.hpp"

Monarch::Monarch()
  : filename(""), io(NULL), mem(NULL), rec(NULL)
{
  //empty, just here to make the compiler throw a
  //fit if someone tries to call a default constructor
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
  rec( new (mem) MonarchSerializer<MonarchRecord>() )
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
  std::size_t rsize = 1024 + sizeof(MonarchRecord) - sizeof(DataType*);
  return new Monarch(filename, MonarchIO::ReadMode, OneChannel, rsize);
}

Monarch* Monarch::OpenForWriting(std::string filename) {
  std::size_t rsize = 1024 + sizeof(MonarchRecord) - sizeof(DataType*);
  return new Monarch(filename, MonarchIO::WriteMode, OneChannel, rsize);
}

bool Monarch::Close() {
  return true;
}

MonarchRecord* Monarch::NewRecord(std::size_t dsize) {
  std::size_t rsize = dsize + sizeof(MonarchRecord) - sizeof(DataType*);
  unsigned char* mem = new unsigned char[rsize]();
  return new (mem) MonarchRecord();
}

bool Monarch::WriteRecord(MonarchRecord* newrec) {
  (this->rec->value_bytes) = (unsigned char*)(newrec);
  return (this->io)->Write((this->rec->value_bytes), this->recsize);
}

MonarchRecord* Monarch::GetNextEvent() {
  bool res = (this->io)->Read(this->rec->value_bytes,this->recsize);
  if( res == true ) {
    return &(this->rec->value);
  }
  else {
    throw MonarchExceptions::EndOfFile(this->filename);
  }
}
