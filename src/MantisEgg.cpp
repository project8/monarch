#include "MantisEgg.hpp"

MantisEgg::MantisEgg() 
  : file_ptr(NULL),
    header_finished(false),
    file_name("default_egg.egg"),
    data_size(sizeof(MantisData::DataType)),
    data_width(4194304)
{ /* no-op */ }

MantisEgg::~MantisEgg()
{
  if(file_ptr) {
    fclose(file_ptr);
  }
}

MantisEgg* MantisEgg::egg_from_env(safeEnvPtr& env)
{
  MantisEgg* egg_ptr = new MantisEgg();
  egg_ptr->file_ptr = fopen((env.get())->getOutName().c_str(),"wb");

  egg_ptr->add_header_attr("clock_ticks_per_second",
			   CLOCKS_PER_SEC);

  if(egg_ptr->file_ptr == NULL) {
    delete egg_ptr;
    egg_ptr = NULL;
  }
  return egg_ptr;
}

std::string MantisEgg::attr_to_xml(egg_hdr_k_type key,
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

std::string MantisEgg::xml_vers_header()
{
  return "<?xml version=\"1.0\"?>";
}

std::string MantisEgg::xml_hdr_open()
{
  return "<header>";
}

std::string MantisEgg::xml_hdr_close()
{
  return "</header>";
}

bool MantisEgg::write_raw_bytes(const void* tgt, 
				 std::size_t tgt_size, 
				 std::size_t tgt_width)
{
  bool res = true;

  std::size_t written = fwrite(tgt,
			       tgt_size,
			       tgt_width,
			       this->file_ptr);
  if ( written != tgt_size * tgt_width ) {
    res = false;
  }

  return res;
}

bool MantisEgg::write_header()
{
  bool ret_val = true;
  std::string hdr_string;
  std::stringstream fuse;
  egg_hdr_container::iterator header_it = this->header_attrs.begin();

  if( fseek(this->file_ptr,0,SEEK_SET) ) {
    ret_val = false;
  }
  
  if( ret_val ) {
    fuse << MantisEgg::xml_vers_header();
    fuse << MantisEgg::xml_hdr_open();

    // Write header attributes
    while( header_it != header_attrs.end() ) {
      std::string xml = MantisEgg::attr_to_xml((*header_it).first,
						(*header_it).second);
      fuse << xml;
      header_it++;
    }

    // Now encode the data size and features, and write it as a string.
    std::stringstream dataDesc;
    dataDesc << "id:"
	     << sizeof(MantisData::IdType) << "|"
	     << "ts:"
	     << sizeof(MantisData::ClockType) << "|"
	     << "data:"
	     << this->data_width;
    fuse << MantisEgg::attr_to_xml("data_format",dataDesc.str());

    // Close the header, we're done.
    fuse << MantisEgg::xml_hdr_close();
  }

  char buffer[2*sizeof(std::size_t)];
  sprintf(buffer,
	  "%x",
	  (unsigned)fuse.str().length());

  this->write_raw_bytes(buffer,
			sizeof(unsigned char),
			strlen(buffer));

  this->write_raw_bytes(fuse.str().c_str(),
			sizeof(unsigned char),
			fuse.str().length());

  if( ret_val ) {
    this->header_finished = true;
  }

  return ret_val;
}

bool MantisEgg::write_data(MantisData* block)
{
  /* fmt: |fID|fTick|fData| */
  static serializer<unsigned long int> u_long_byter;
  static serializer<clock_t> clock_t_byter;
  u_long_byter.value = block->fId;
  clock_t_byter.value = block->fTick;
  this->write_raw_bytes(u_long_byter.value_bytes,
			u_long_byter.value_bytes[0],
			sizeof(u_long_byter));
  this->write_raw_bytes(clock_t_byter.value_bytes,
			clock_t_byter.value_bytes[0],
			sizeof(clock_t_byter));
  this->write_raw_bytes(block->fDataPtr,
			this->data_size,
			this->data_width);
  return true;
}
