#ifndef __MONARCH_SER_HPP
#define __MONARCH_SER_HPP

template<typename T>
union MonarchSerializer {
  T value;
  unsigned char value_bytes[sizeof(T)];
};

#endif
