#include <iostream>
#include "Monarch.hpp"

int main() {
  char* mem = new char[4096];
  MonarchSerializer<MonarchRecord>* ptr = new (mem) MonarchSerializer<MonarchRecord>();

  for(int i = 0; i < 255; i++) {
    (ptr->value).fDataPtr[i] = (DataType)i;
  }

  // Open a file that doesn't exist for reading.  This should barf and we should catch it.
  std::cout << "Testing for NoFile throw on missing file...";
  Monarch* readtest;
  try {
    readtest = Monarch::Open("test.egg",MonarchIO::ReadMode);
  }
  catch(MonarchExceptions::NoFile &e) {
    std::cout << "\tok, success." << std::endl;
  }

  // Clean up.

  return 0;
}
