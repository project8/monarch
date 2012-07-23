#include "Monarch.hpp"

Monarch::Monarch(){

  //empty, just here to make the compiler throw a
  //fit if someone tries to call a default constructor

}

Monarch::Monarch(std::string filename, 
		 AccessMode iomode,
		 AcquisitionMode datamode) :
  io(new MonarchIO(filename, iomode))
{
}
