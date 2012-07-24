#include "MonarchIO.hpp"

MonarchIO::MonarchIO(std::string filename, 
		     AccessMode iomode,
		     std::size_t nbytes) 
  : filePTR(NULL),
    _iomode(iomode),
    _recWidth(nbytes)
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

bool MonarchIO::WriteRecord(MonarchRecord* data) {
  static MonarchSerializer<MonarchRecord> ser;
  ser.value = (*data);
  std::size_t written = fwrite(ser.value_bytes,
			       sizeof(DataType),
			       this->_recWidth,
			       this->filePTR);

  return (written == this->_recWidth * sizeof(DataType));
}
