#include <iostream>
#include <string>

#include "MonarchIO.hpp"
#include "MonarchHeader.hpp"
#include "MonarchEvent.hpp"
#include "MonarchRecord.hpp"
#include "MonarchRead.hpp"
#include "MonarchWrite.hpp"
#include "MonarchSim.hpp"

class Monarch
{
  MonarchIO *eggPTR;
  MonarchHeader *header;
  MonarchEvent *event;
  MonarchRecord *record;

  Monarch();
  ~Monarch();

public:
  //constructors
  Monarch(std::string);
  Monarch(int,int,std::string);
  Monarch(bool,int,int,std::string);
  //destructor
  ~Monarch();

  MonarchHeader* getHeader();
  MonarchEvent* getNextEvent();
  MonarchRecord* getNextRecord();
};

