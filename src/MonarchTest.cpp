#include <iostream>
#include <cassert>
#include "Monarch.hpp"

int main() {
  /*
   * Nonexistent file test
   */
  std::cout << "Testing for NoFile throw on missing file...";
  Monarch* readtest;
  try {
    readtest = Monarch::Open("test.egg",MonarchIO::ReadMode);
  }
  catch(MonarchExceptions::NoFile &e) {
    std::cout << "\tok, success." << std::endl;
  }

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
  std::cout << std::endl << "Writing a test file (testw1.egg) "
	    << "in one channel mode" << std::endl;
  Monarch* wtest_1 = Monarch::Open("testw1.egg",MonarchIO::WriteMode);
  MonarchRecord* rtest_1 = Monarch::NewRecord(1024);

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

  std::cout << "Success.  Cleaning up." << std::endl;
  delete wtest_1;

  /*
   * Read back the test file we created and verify that it contains the
   * correct data.
   */
  std::cout << std::endl << "Reading back test file (testw1.egg)" << std::endl;
  Monarch* rtest_2 = Monarch::Open("testw1.egg",MonarchIO::ReadMode);
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
	exit(3);
      }
    }
  }
  std::cout << "Success.  Cleaning up." << std::endl;

  // Clean up.
  return 0;
}
