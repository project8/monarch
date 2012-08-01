#include <iostream>
#include <cassert>
#include "time.h"
#include "Monarch.hpp"

int main() {
  /*
   * Test making a header in one channel mode at 500 MHz with
   * a 4MB record size.  Then write some data.
   */
  std::cout << "Testing that we can write and read back a header..." 
	    << std::endl; 
  MonarchHeader hdr;
  hdr.SetFilename(std::string("test_egg.egg"));
  hdr.SetAcqMode(Monarch::OneChannel);
  hdr.SetAcqRate(500);
  hdr.SetRecordSize(4194304);
  hdr.SetAcqTime(100);

  std::cout << "\tOpening a file (test.egg) based on header...";
  Monarch* wr_test_0 = Monarch::Open(hdr);
  wr_test_0->Close();
  std::cout << "ok" << std::endl;

  // Now we should be able to de-serialize the header and read
  // back those attributes.
  std::cout << "\tRe-opening the file and parsing the header...";
  Monarch* rb_test_0 = Monarch::Open("test_egg.egg",MonarchIO::ReadMode);
  MonarchHeader* rb_hdr = rb_test_0->GetHeader();
  assert(rb_hdr->GetFilename() == "test_egg.egg");
  assert(rb_hdr->GetAcqMode() == Monarch::OneChannel);
  assert(rb_hdr->GetAcqRate() == 500);
  assert(rb_hdr->GetRecordSize() == 4194304);
  assert(rb_hdr->GetAcqTime() == 100);
  std::cout << "ok" << std::endl;
  std::cout << "Success.  Cleaning up." << std::endl;
  
  /*
   * Nonexistent file test
   */
  std::cout << std::endl << "Testing for NoFile throw on missing file...";
  Monarch* readtest;
  try {
    readtest = Monarch::Open("test.egg",MonarchIO::ReadMode);
  }
  catch(MonarchExceptions::NoFile &e) {
    std::cout << "ok" << std::endl;
  }
  std::cout << "Success.  Cleaning up." << std::endl;

  /*
   * Generate a new record using Monarch::NewRecord.  We should be 
   * able to allocate it, play with it, and delete it with no problem.
   */
  std::cout << std::endl << "Testing Monarch::NewRecord" << std::endl;

  std::cout << "\tAllocating...";
  MonarchRecord* ftest = Monarch::NewRecord(4094304);
  std::cout << "ok" << std::endl << "\tSetting fields...";
  ftest->fCId = 1;
  ftest->fAId = 1;
  ftest->fRId = 0;
  ftest->fTick = 213;
  std::cout << "ok" << std::endl << "\tAssigning data...";
  for(std::size_t i = 0; i < 4094304; i++) {
    ftest->fDataPtr[i] = (DataType)i;
  }
  std::cout << "ok" << std::endl << "Success.  Cleaning up." << std::endl;
  delete ftest;

  /*
   * Write a test file.  Open a new file, write 100 records each of
   * 1024 data points, where data[i] == i.  We should then be able
   * to open the file for reading and read back the data, verifying
   * that the records start at index 0 and are sequentially numbered
   * up to 100, and that for each record, data[i] == i.
   */
  std::cout << std::endl << "Writing a test file (w_test_1.egg) "
	    << "in one channel mode" << std::endl;

  std::cout << "\tgenerating header...";
  MonarchHeader w_hdr_0;
  w_hdr_0.SetFilename(std::string("w_test_1.egg"));
  w_hdr_0.SetAcqMode(Monarch::OneChannel);
  w_hdr_0.SetAcqRate(500);
  w_hdr_0.SetRecordSize(1024);
  w_hdr_0.SetAcqTime(100);
  std::cout << "ok" << std::endl;

  Monarch* wtest_1 = Monarch::Open(w_hdr_0);
  MonarchRecord* rtest_1 = Monarch::NewRecord(1024);
  std::cout << "\twriting records...";
  rtest_1->fCId = 1;
  rtest_1->fAId = 2;
  for(std::size_t i = 0; i < 1024; i++) {
    rtest_1->fRId = (RecIdType)(i+3);
    rtest_1->fTick = (ClockType)(i+4);
    for(std::size_t j=0; j<1024; j++) {
      rtest_1->fDataPtr[j] = (DataType)i;
    }
    wtest_1->WriteRecord(rtest_1);
  }
  std::cout << "ok" << std::endl;

  std::cout << "Success.  Cleaning up." << std::endl;
  wtest_1->Close();
  delete wtest_1;

  /*
   * Read back the test file we created and verify that it contains the
   * correct data.
   */
  std::cout << std::endl << "Reading back test file (w_test_1.egg)" << std::endl;
  Monarch* rtest_2 = Monarch::Open("w_test_1.egg",MonarchIO::ReadMode);
  MonarchRecord* ev;
  for(std::size_t i = 0; i < 1024; i++) {
    try {
      ev = rtest_2->GetNextEvent();
    }
    catch(MonarchExceptions::EndOfFile &e) {
      std::cout << "\tUh oh, shouldn't be the end of the file yet..." 
		<< "(event " << i << ")"
		<< std::endl;
      std::cout << "FAILED." << std::endl;
      exit(2);
    }
    // Assert that things are correct.
    assert(ev->fCId == 1);
    assert(ev->fAId == 2);
    assert(ev->fRId == (RecIdType)(i+3));
    assert(ev->fTick == (ClockType)(i+4));
    for(std::size_t j=0; j<1024; j++) {
      if(ev->fDataPtr[j] != (DataType)i) {
      	std::cout << "Error reading back data in event #"
      		  << i << ": expected "
      		  << i << " but got " 
      		  << (int)ev->fDataPtr[j] 
      		  << " at byte "
      		  << j
      		  << std::endl;
      	std::cout << "FAILED." << std::endl;
      }
    }
  }
  std::cout << "Success.  Cleaning up." << std::endl;

  /*
   *  Time for a speed test.  Measure the time it takes to write
   *  1M records to disk.
   */
  std::cout << std::endl << "Preparing for throughput test." << std::endl;
  std::cout << "\tCreating header...";
  MonarchHeader bnch_hdr;
  bnch_hdr.SetFilename(std::string("bench.egg"));
  bnch_hdr.SetAcqMode(Monarch::OneChannel);
  bnch_hdr.SetAcqRate(500);
  bnch_hdr.SetRecordSize(1024);
  bnch_hdr.SetAcqTime(100);
  std::cout << "ok" << std::endl;

  Monarch* bnch_file = Monarch::Open(bnch_hdr);
  MonarchRecord* bnch_rec = Monarch::NewRecord(1024);
  std::cout << "\twriting records...";
  bnch_rec->fCId = 1;
  bnch_rec->fAId = 2;
  
  time_t start,end;
  time (&start);

  for(std::size_t i = 0; i < 1000000; i++) {
    bnch_rec->fRId = (RecIdType)(i+3);
    bnch_rec->fTick = (ClockType)(i+4);
    for(std::size_t j=0; j<1024; j++) {
      bnch_rec->fDataPtr[j] = (DataType)i;
    }
    bnch_file->WriteRecord(bnch_rec);
  }
  time (&end);
  double dif = difftime (end,start);
  std::cout << "ok (" << dif << "s elapsed)" << std::endl;
  std::cout << "\tApprox. throughput: " 
	    << (1000000*1048)/(1024*1024*dif) 
	    << "MBps"
	    << std::endl;

  std::cout << "Success.  Cleaning up." << std::endl;
  bnch_file->Close();
  delete bnch_file;

  // Clean up.
  return 0;
}
