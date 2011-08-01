#ifndef __mantis_egg_hpp
#define __mantis_egg_hpp

#include "MantisData.hpp"
#include "MantisEnv.hpp"
#include <cstdio>
#include <list>
#include <string>
#include <cstring>
#include <utility>
#include <sstream>
#include <map>

typedef std::string egg_hdr_elem_k_t;
typedef std::string egg_hdr_attr_k_t;
typedef std::string egg_hdr_attr_v_t;
typedef std::pair<egg_hdr_attr_k_t,egg_hdr_attr_v_t> egg_hdr_attr_t;
typedef std::list<egg_hdr_attr_t> egg_hdr_attr_list;
typedef std::map<egg_hdr_elem_k_t,egg_hdr_attr_list> egg_hdr_container;
typedef std::pair<egg_hdr_elem_k_t,egg_hdr_attr_list> egg_hdr_elem_t;

class MantisEgg {
private:
  FILE* file_ptr;
  bool header_finished;
  egg_hdr_container header_attrs;
  std::string file_name;
  static std::string attr_to_xml(egg_hdr_attr_k_t key,
				 egg_hdr_attr_v_t val);
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

  template<typename T1,typename T2> 
  bool add_header_attr(egg_hdr_elem_k_t key, 
		       T1 value1,
		       T2 value2);
  bool write_header();
  bool write_data(MantisData* block);
  ~MantisEgg();
};

template<typename T>
union serializer {
  T value;
  unsigned char value_bytes[sizeof(T)];
};

template<typename T1, typename T2>
bool MantisEgg::add_header_attr(egg_hdr_elem_k_t key,
				T1 value1,
				T2 value2)
{
  egg_hdr_attr_t new_attr;
  egg_hdr_attr_list new_attr_list;

  std::stringstream converter;
  converter << value1;
  new_attr.first = converter.str();
  converter.str(std::string());
  converter << value2;
  new_attr.second = converter.str();

  new_attr_list.push_back(new_attr);

  // Search the map for the key.  If it exists, merge
  // our new list with the existing list.  Otherwise,
  // insert the new value.
  if( (*this).header_attrs.count(key) == 0 ) {
    (*this).header_attrs[key] = new_attr_list;
  }
  else {
    (*this).header_attrs[key].merge(new_attr_list);
  }

  return true;
}

#endif
