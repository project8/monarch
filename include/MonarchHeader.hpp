#include "MonarchTypes.hpp"
#include "MonarchHeader.pb.h"

class MonarchHeader {
private:
  MonarchPB::MonarchHeader* pb_hdr;
public:
  MonarchHeader();

  // Getters and setters.  This is basically a wrapper around the
  // protocol buffer object.
  void SetFilename(std::string fname);
  std::string GetFilename();

  void SetAcqMode(const AcquisitionMode& mode);
  AcquisitionMode GetAcqMode();

  void SetRecordSize(const std::size_t& recsize);
  std::size_t GetRecordSize();

  void SetAcqTime(const std::size_t& acqt);
  std::size_t GetAcqTime();

  void SetAcqRate(const std::size_t& acqr);
  std::size_t GetAcqRate();

  // Initialize a MonarchHeader object from a char array.
  static MonarchHeader* FromArray(void* bytes, int size);

  // Pass-thru methods to underlying protocol buffer object
  int ByteSize();
  bool SerializeToArray(void* data, int size);
  
};
