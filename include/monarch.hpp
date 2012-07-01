#include <iostream>
#include <string>

#include "MonarchIO.hpp"
#include "MonarchEgg.hpp"
#include "MonarchRead.hpp"
#include "MonarchWrite.hpp"
#include "MonarchSim.hpp"

class Monarch
{
  MonarchEgg *egg;
  MonarchIO *eggPTR;
  Monarch();
  ~Monarch();

public:
  //constructors
  Monarch(std::string);
  Monarch(int,int,std::string);
  Monarch(bool,int,int,std::string);
  //destructors
  ~Monarch(std::string);
  ~Monarch(int,int,std::string);
  ~Monarch(bool,int,int,std::string);

  mPrintHeader();
  mPrintNextEvent();
  mPrintNextRecord();
};

