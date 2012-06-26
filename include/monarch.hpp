#include <iostream>
#include <string>

#include "MonarchRead.hpp"
#include "MonarchWrite.hpp"
#include "MonarchSim.hpp"
#include "MonarchEgg.hpp"

class Monarch
{
  MonarchEgg *egg;
public:
  Monarch(std::string);
  Monarch(int,int,std::string);
  Monarch(bool,int,int,std::string);
};

//read constructor
Monarch::Monarch(std::string filename){

  //fix me

}

//write constructor
Monarch::Monarch(int sample_length,int sample_rate,std::string filename){

  //fix me

}

//simulation constructor
Monarch::Monarch(bool signal,int sample_length,int sample_rate,std::string filename){

  //fix me

}

Monarch::~Monarch(){

  //fix me

}

