#include "MonarchHeader.hpp"

MonarchHeader::MonarchHeader() 
  : pb_hdr(new MonarchPB::MonarchHeader)
{}

void MonarchHeader::SetFilename(std::string fname) {
  pb_hdr->set_filename(fname);
}

std::string MonarchHeader::GetFilename() {
  return pb_hdr->filename();
}

void MonarchHeader::SetAcqMode(const AcquisitionMode& mode) {
  (pb_hdr->mutable_diginfo())->set_acqmode(mode);
}

AcquisitionMode MonarchHeader::GetAcqMode() {
  return (pb_hdr->diginfo()).acqmode();
}

void MonarchHeader::SetRecordSize(const std::size_t& recsize) {
  (pb_hdr->mutable_diginfo())->set_recsize(recsize);
}

std::size_t MonarchHeader::GetRecordSize() {
  return (pb_hdr->diginfo()).recsize();
}

void MonarchHeader::SetAcqTime(const std::size_t& acqt) {
  pb_hdr->set_acqtime(acqt);
}

std::size_t MonarchHeader::GetAcqTime() {
  return pb_hdr->acqtime();
}

void MonarchHeader::SetAcqRate(const std::size_t& acqr) {
  (pb_hdr->mutable_diginfo())->set_digrate(acqr);
}

std::size_t MonarchHeader::GetAcqRate() {
  return (pb_hdr->diginfo()).digrate();
}



int MonarchHeader::ByteSize() {
  return pb_hdr->ByteSize();
}

bool MonarchHeader::SerializeToArray(void* data, int size) {
  return pb_hdr->SerializeToArray(data,size);
}

bool MonarchHeader::SerializeToOstream(std::ostream* output) {
  return pb_hdr->SerializeToOstream(output);
}

MonarchHeader* MonarchHeader::FromArray(void* bytes, int size) {
  MonarchHeader* ret = new MonarchHeader();
  if((ret->pb_hdr->ParseFromArray(bytes, size)) == false) {
    // uh oh
    delete ret;
    ret = NULL;
  }

  return ret;
}

std::ostream& operator<<(std::ostream& out, MonarchHeader& hdr) {
  out << "Here's the header: " << std::endl << std::endl;
  out << "\tFilename: "<< hdr.GetFilename() << std::endl;
  out << "\tAcquisition Mode: " << hdr.GetAcqMode() << std::endl;
  out << "\tRecord Size: " << hdr.GetRecordSize() << std::endl;
  out << "\tAcquisition Time: " << hdr.GetAcqTime() << std::endl;
  out << "\tDigitization Rate: " << hdr.GetAcqRate() << std::endl;
  return out;
}
