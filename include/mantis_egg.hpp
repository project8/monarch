#ifndef __mantis_egg_hpp
#define __mantis_egg_hpp

#include "MantisBlock.hpp"
#include <cstdio>
#include <string>
#include <cstring>
#include <utility>
#include <sstream>
#include <map>

typedef std::string egg_hdr_k_type;
typedef std::string egg_hdr_v_type;
typedef std::pair<egg_hdr_k_type,egg_hdr_v_type> egg_hdr_type;
typedef std::map<egg_hdr_k_type,egg_hdr_v_type> egg_hdr_container;

class mantis_egg {
private:
  FILE* file_ptr;
  bool header_finished;
  egg_hdr_container header_attrs;
  std::string file_name;
  static std::string attr_to_xml(egg_hdr_k_type key,
				 egg_hdr_v_type val);
  static std::string size_to_hex(std::size_t size);
  static std::string xml_vers_header();
  static std::string xml_hdr_open();
  static std::string xml_hdr_close();
  std::size_t data_size;
  std::size_t data_width;
  mantis_egg();
public:
  static mantis_egg* egg_from_env();

  template<typename T> bool add_header_attr(egg_hdr_k_type key, 
					    T value);
  bool write_header();
  bool write_data(MantisBlock* block);
  ~mantis_egg();
};

static union {
  clock_t value;
  char value_bytes[sizeof(clock_t)];
} clock_t_byter;

static union {
  unsigned long value;
  char value_bytes[sizeof(unsigned long)];
} u_long_byter;

template<typename T> bool mantis_egg::add_header_attr(egg_hdr_k_type key,
						      T value)
{
  std::pair<egg_hdr_container::iterator,bool> ret;
  std::stringstream converter;

  converter << value;
  ret = (*this).header_attrs.insert(egg_hdr_type(key,converter.str()));
  return ret.second;
}

#endif
