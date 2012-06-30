#include <iostream>
#include <cstring>

#include "MonarchEgg.hpp"
#include "MonarchIO.hpp"

class MonarchRead {
protected:
  MonarchEgg *mBreakEgg(MonarchIO *,MonarchEgg *);
  MonarchEgg *mParseEggHeader(MonarchIO *,MonarchEgg *);
  int mHatchNextEvent(MonarchIO *,MonarchEgg *);
};
