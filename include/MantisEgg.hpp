#ifndef __mantis_egg_hpp
#define __mantis_egg_hpp

#include "MantisData.hpp"
#include "MantisEnv.hpp"
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

class MantisEgg {
private:
  FILE* file_ptr;
  bool header_finished;
  egg_hdr_container header_attrs;
  std::string file_name;
  static std::string attr_to_xml(egg_hdr_k_type key,
				 egg_hdr_v_type val);
  static std::string xml_vers_header();
  static std::string xml_hdr_open();
  static std::string xml_hdr_close();
  std::size_t data_size;
  std::size_t data_width;
  bool write_raw_bytes(const void* tgt, 
		       std::size_t tgt_size, 
		       std::size_t tgt_width);
  MantisEgg();
public:
  static MantisEgg* egg_from_env(safeEnvPtr& env);

  template<typename T> bool add_header_attr(egg_hdr_k_type key, 
					    T value);
  bool write_header();
  bool write_data(MantisData* block);
  ~MantisEgg();
};

template<typename T>
union serializer {
  T value;
  unsigned char value_bytes[sizeof(T)];
};

template<typename T>
bool MantisEgg::add_header_attr(egg_hdr_k_type key,
				 T value)
{
  std::pair<egg_hdr_container::iterator,bool> ret;
  std::stringstream converter;

  converter << value;
  ret = (*this).header_attrs.insert(egg_hdr_type(key,converter.str()));
  return ret.second;
}

#endif
