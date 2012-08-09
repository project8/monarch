#include "Monarch.hpp"


Monarch::Monarch()
  : filename(""), io(NULL), mem(NULL), rec(NULL)
{
  //empty, just here to make the compiler throw a
  //fit if someone tries to call a default constructor
}

Monarch::~Monarch() 
{
  if(io != NULL) delete io;
  if(mem != NULL) delete mem;
}


Monarch::Monarch(std::string filename, 
		 AccessMode iomode)
  : 
  filename(filename),
  io( new MonarchIO(filename,iomode) ),
  mem( NULL ),
  rec( NULL )
{ }

bool Monarch::AllocateRec(std::size_t nbytes) {
  bool res = true;

  if(mem) {
    delete mem;
  }
  mem = new char[nbytes];
  rec = new (mem) MonarchRecord();
  if(mem == NULL || rec == NULL) {
    res = false;
  }

  return res;
}

Monarch* Monarch::Open(std::string filename, 
		       AccessMode iomode) {
  // First check for readmode 
  if(iomode == ReadMode) {
    return Monarch::OpenForReading(filename);
  }
  else if(iomode == WriteMode) {
    return Monarch::OpenForWriting(filename);
  }

  return NULL;
}

Monarch* Monarch::Open(MonarchHeader& hdr) {
  Monarch* ret = new Monarch(hdr.GetFilename(),
			     WriteMode);
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

    // Cache the size of records.
    ret->recsize = sizeof(ChIdType) 
      + sizeof(AcqIdType) 
      + sizeof(RecIdType)
      + sizeof(ClockType)
      + hdr.GetRecordSize();

    // OK, now we are ready for writing.
    

  }

  return ret;
}

Monarch* Monarch::OpenForReading(std::string filename) {
  // This should really be fixed, it makes no goddamned sense.  You can't know these
  // parameters before you read the header!!!
  Monarch* ret = new Monarch(filename, ReadMode);

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

    // Set the recsize to be correct from the header.  This will be fixed
    // once the constructor for reading is less goofy.
    ret->recsize = sizeof(ChIdType) 
      + sizeof(AcqIdType) 
      + sizeof(RecIdType)
      + sizeof(ClockType)
      + ret->hdr->GetRecordSize();

    // Allocate memory for the record we keep.
    ret->AllocateRec(ret->recsize);
  }

  return ret;
}

Monarch* Monarch::OpenForWriting(std::string filename) {
  std::size_t rsize = sizeof(ChIdType) 
    + sizeof(AcqIdType) 
    + sizeof(RecIdType) 
    + sizeof(ClockType) 
    + 1024;
  return new Monarch(filename, WriteMode);
}

bool Monarch::Close() {
  return (this->io->Close());
}

MonarchRecord* Monarch::NewRecord(std::size_t dsize) {
  std::size_t rsize = sizeof(ChIdType) 
    + sizeof(AcqIdType) 
    + sizeof(RecIdType) 
    + sizeof(ClockType) 
    + dsize;
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
