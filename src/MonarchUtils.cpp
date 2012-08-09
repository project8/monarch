#include "Monarch.hpp"
#include "MonarchUtils.hpp"
#include <stdlib.h>
#include <unistd.h>
#include <fstream>

int main(int argc, char** argv) {

  // Filename of input file
  std::string iname, hdr_oname;

  // Keep track of what has been selected
  byte flags;

  // Now parse options
  int c;

  while((c = getopt(argc, argv, "i:o:pe")) != -1 ) {
    switch (c)
      {
      case 'i':
	flags |= ifile_set;
	iname = optarg;
	break;
      case 'p':
	flags |= pp_header;
	break;
      case 'e':
	flags |= ex_header;
	break;
      case 'o':
	flags |= hdr_o_file;
	hdr_oname = optarg;
	break;
      }
  }

  // If the filename hasn't been set, we die b/c this makes
  // no sense.
  if( !bit_set(flags, ifile_set) ) {
    std::cout << "What input file should I be reading?" << std::endl;
    exit(2);
  }

  // Otherwise we have an input file.  Let's try to open it!
  Monarch* in_egg;
  try {
    in_egg = Monarch::Open(iname, ReadMode);
  }

  catch (MonarchExceptions::NoFile &e) {
    std::cout << "Sorry, but there isn't a file called " 
	      << iname 
	      << " for me to open."
	      << std::endl;
    exit(3);
  }

  // OK, should we pretty print the header?
  if( bit_set(flags, pp_header) ) {
    std::cout << *(in_egg->GetHeader()) << std::endl;
  }  

  // Are we supposed to extract the header?
  if( bit_set(flags, ex_header) ) {
    std::ostream* dest_stream = &std::cout;
    std::ofstream hdr_file_stream;
    
    std::cout << "Dumping raw header to ";
    if( bit_set(flags, hdr_o_file) ) {
      std::cout << hdr_oname << std::endl;
      hdr_file_stream.open(hdr_oname.c_str(), 
			   std::ios::out | std::ios::trunc | std::ios::binary);
      dest_stream = &hdr_file_stream;
    }
    else {
      std::cout << "stdout for some reason." << std::endl;
    }
    if( (in_egg->GetHeader()->SerializeToOstream(dest_stream)) != true ) {
      std::cout << "Hmm, I seem to have had a problem serializing the header "
		<< "to the stream you wanted... sorry about that" 
		<< std::endl;
    }
  }

  return 0;
}
