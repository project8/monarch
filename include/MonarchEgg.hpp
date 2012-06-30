#include <iostream>
#include <cstring>
#include <mxml.hpp>

class MonarchEgg {

  static int prelude_size = 8;  //currently restricted to 8 bytes

  struct event{
    int timestamp_size;
    int frameID_size;
    int record_size;
    int event_size;
    int sample_rate;
    int sample_length;
    unsigned char *ts;
    unsigned char *frameID;
    unsigned char *record;    
  };

  FILE *eggptr;
  unsigned char *prelude;
  unsigned char *header;
  struct event *data;

};
