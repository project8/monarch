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

bool MonarchIO::Write(char* wbuf, std::size_t nbytes) {
  std::size_t written = fwrite(wbuf,
			       sizeof(DataType),
			       nbytes,
			       this->filePTR);

  return (written == nbytes);
}

std::size_t MonarchIO::Read(char* rbuf, std::size_t nbytes) {
  return fread(rbuf,
	       sizeof(DataType),
	       nbytes,
	       this->filePTR);
}

bool MonarchIO::Close() {
  if(this->filePTR) {
    fclose(this->filePTR);
    return true;
  }
  return false;
}
