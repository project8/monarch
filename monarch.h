#ifndef __monarch_h
#define __monarch_h

#include <stdio.h>
#include <stdlib.h>
#include <mxml.h>

static const int prelude_size = 8;  /*the prelude size is currently restricted
                                    to eight bytes*/ 
struct event {
  int timestamp_size;
  int frameID_size;
  int record_size;
  int event_size;
  int sample_rate;  //in MHz
  int sample_length;  //in sec
  char *ts;
  char *fID;
  char *record;
};

struct egg { 
  FILE *eggptr;
  char *prelude;
  char *header;
  struct event *data;
};

struct egg *mBreakEgg(const char *, struct egg *);
struct egg *mParseEggHeader(struct egg *);
int mHatchNextEvent(struct egg *);
void mCleanUp(struct egg *);

#endif
