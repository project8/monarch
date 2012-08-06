#ifndef __MONARCH_UTILS_HPP
#define __MONARCH_UTILS_HPP

// Yay for bytes
typedef char byte;

// Bitmasks for options
#if __GNUC__ == 4 && __GNUC_MINOR__ < 3 

// Was the input file specified?
byte ifile_set = 0x01;

// Are we supposed to pretty print the header?
byte pp_header = 0x01 << 1;

// Are we supposed to extract the header?
byte ex_header = 0x01 << 2;

// Are we dumping the header to a file?
byte hdr_o_file = 0x01 << 3;

#else
// Was the input file specified?
byte ifile_set = 0b00000001;

// Are we supposed to pretty print the header?
byte pp_header = 0b00000010;

// Are we supposed to extract the header?
byte ex_header = 0b00000100;

// Are we dumping the header to a file?
byte hdr_o_file = 0b00001000;

#endif

// Check if a bit is set in a bitmask
bool bit_set(byte tgt, byte query) {
  return ((tgt & query) != 0);
}

#endif //__MONARCH_UTILS_HPP
