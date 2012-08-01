#include "Monarch.hpp"

const AcquisitionMode Monarch::OneChannel = 0x03;

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

Monarch* Monarch::Open(MonarchHeader& hdr) {
  Monarch* ret = new Monarch(hdr.GetFilename(),
			     MonarchIO::WriteMode,
			     hdr.GetAcqMode(),
			     hdr.GetRecordSize());
  if(ret) {
    // Now just write the header & prelude on the file.  First we need to know
    // just how big the header is in bytes
    int hdr_size = hdr.ByteSize();
    
    // Now write the prelude.  Jesus christ this is ugly... not sure what I'm
    // doing wrong here but it seemed the only way to drop the null termination
    // reliably.
    char prelude_buf_tmp[9];
    char prelude_buf[8];
    snprintf(prelude_buf_tmp, 9, "%08x", hdr_size);
    memcpy(prelude_buf, prelude_buf_tmp, 8);
    (ret->io)->Write(prelude_buf, 8);

    // Now write the header on the stream.
    char hdr_buf[hdr_size];
    if(hdr.SerializeToArray(hdr_buf, hdr_size) == false) {
      std::cout << "couldn't serialize header!!!" << std::endl;
    }
    (ret->io)->Write(hdr_buf, hdr_size);

    // OK, now we are ready for reading.
  }

  return ret;
}

Monarch* Monarch::OpenForReading(std::string filename) {
  // This should really be fixed, it makes no goddamned sense.  You can't know these
  // parameters before you read the header!!!
  Monarch* ret = new Monarch(filename, MonarchIO::ReadMode, OneChannel, 0);

  if(ret) {
    // We need an array for the prelude and then another for the header to read in.
    char prelude[8];
    (ret->io)->Read(prelude, 8);

    unsigned int hdr_size;
    sscanf((char*)prelude, "%08x", &hdr_size);
    
    // OK now we should know how big it is...
    char hdr_buf[hdr_size];
    (ret->io)->Read(hdr_buf, hdr_size);
    
    // Now deserialize.
    ret->hdr = MonarchHeader::FromArray(hdr_buf, hdr_size);
  }

  return ret;
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
  char* mem = new char[rsize]();
  return new (mem) MonarchRecord();
}

bool Monarch::WriteRecord(MonarchRecord* newrec) {
  return (this->io)->Write((char*)(newrec), this->recsize);
}

MonarchRecord* Monarch::GetNextEvent() {
  std::size_t res = (this->io)->Read((char*)(this->rec), this->recsize);
  if( res == this->recsize ) {
    return (this->rec);
  }
  else {
    throw MonarchExceptions::EndOfFile(this->filename);
  }
}

MonarchHeader* Monarch::GetHeader() {
  return this->hdr;
}
