#include "mantis_egg.hpp"

mantis_egg::mantis_egg() 
  : file_ptr(NULL),
    file_name("default_egg.egg"),
    header_finished(false),
    data_size(sizeof(unsigned char)),
    data_width(10)
{ /* no-op */ }

mantis_egg::~mantis_egg()
{
  if(file_ptr) {
    fclose(file_ptr);
  }
}

mantis_egg* mantis_egg::egg_from_env()
{
  mantis_egg* egg_ptr = new mantis_egg();
  egg_ptr->file_ptr = fopen(egg_ptr->file_name.c_str(),"wb");

  egg_ptr->add_header_attr("clock_ticks_per_second",
			   CLOCKS_PER_SEC);

  if(egg_ptr->file_ptr == NULL) {
    delete egg_ptr;
    egg_ptr = NULL;
  }
  return egg_ptr;
}

std::string mantis_egg::attr_to_xml(egg_hdr_k_type key,
				    egg_hdr_v_type val)
{
  std::stringstream fuse;
  fuse << "<key>" 
       << "name=\"" 
       << key 
       << "\" " 
       << "value=\"" 
       << val 
       << "\"</key>";
  return fuse.str();
}

std::string mantis_egg::xml_vers_header()
{
  return "<?xml version=\"1.0\"?>";
}

std::string mantis_egg::xml_hdr_open()
{
  return "<header>";
}

std::string mantis_egg::xml_hdr_close()
{
  return "</header>";
}

std::string mantis_egg::size_to_hex(std::size_t size)
{
  std::stringstream converter;
  converter << std::hex << size;
}

bool mantis_egg::write_header()
{
  bool ret_val = true;
  std::string hdr_string;
  std::stringstream fuse;
  egg_hdr_container::iterator header_it = this->header_attrs.begin();

  if( fseek(this->file_ptr,0,SEEK_SET) ) {
    ret_val = false;
  }
  
  if( ret_val ) {
    fuse << mantis_egg::xml_vers_header();
    fuse << mantis_egg::xml_hdr_open();
    while( header_it != header_attrs.end() ) {
      std::string xml = mantis_egg::attr_to_xml((*header_it).first,
						(*header_it).second);
      fuse << xml;
      header_it++;
    }
    fuse << mantis_egg::xml_hdr_close();
  }

  char buffer[2*sizeof(std::size_t)];
  sprintf(buffer,
	  "%x",
	  (unsigned)fuse.str().length());

  fwrite(buffer,
	 sizeof(char),
	 strlen(buffer),
	 this->file_ptr);

  fwrite(fuse.str().c_str(),
	 sizeof(char),
	 fuse.str().length(),
	 this->file_ptr);

  if( ret_val ) {
    this->header_finished = true;
  }

  return ret_val;
}

bool mantis_egg::write_data(MantisBlock* block)
{
  /* fmt: |fID|fTick|fData| */
  u_long_byter.value = block->fId;
  clock_t_byter.value = block->fTick;
  fwrite(u_long_byter.value_bytes,
	 sizeof(char),
	 sizeof(u_long_byter),
	 this->file_ptr);
  fwrite(clock_t_byter.value_bytes,
	 sizeof(char),
	 sizeof(clock_t_byter),
	 this->file_ptr);
  fwrite(block->fData,
	 this->data_size,
	 this->data_width,
	 this->file_ptr);
  return true;
}
