#include <iostream>
#include "Monarch.hpp"

int main() {
  // Test opening a file in one channel mode, with some attributes
  // set in a key-value list.
  Monarch* test = new Monarch("test.egg",
			      Monarch::WriteMode,
			      Monarch::OneChannel);

  // Clean up.
  delete test;

  return 0;
}
