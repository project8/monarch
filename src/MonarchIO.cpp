#include "MonarchIO.hpp"

MonarchIO::MonarchIO(std::string filename, 
		     AccessMode iomode)
  : filePTR(NULL),
    _iomode(iomode)
{
  if(iomode == WriteMode) {
    filePTR = fopen(filename.c_str(), "wb");
  }
  else if (iomode == ReadMode) {
    filePTR = fopen(filename.c_str(), "rb");
  }

  if( filePTR == NULL ) {
    throw MonarchExceptions::NoFile("file could not be opened!");
  }
}

MonarchIO::~MonarchIO() {
  if(filePTR) {
    fclose(filePTR);
  }
}

bool MonarchIO::Write(unsigned char* wbuf, std::size_t nbytes) {
  std::size_t written = fwrite(wbuf,
			       sizeof(DataType),
			       nbytes,
			       this->filePTR);

  return (written == nbytes);
}

bool MonarchIO::Read(unsigned char* rbuf, std::size_t nbytes) {
  std::size_t read = fread(rbuf,
			   sizeof(DataType),
			   nbytes,
			   this->filePTR);
  return (read == nbytes);
}
