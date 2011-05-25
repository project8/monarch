#ifndef MANTISBLOCK_HPP_
#define MANTISBLOCK_HPP_

#include "MantisMutex.hpp"

#include "px1500.h"

#include <ctime>

class MantisBlock
{
    public:
        typedef unsigned long int IdType;
        typedef clock_t ClockType;
        typedef px4_sample_t DataType;
        
        friend class MantisReadIterator;
        friend class MantisWriteIterator;
    
    public:
        MantisBlock();
        ~MantisBlock();
        
    private:
        IdType fId;
        ClockType fTick;
        DataType* fData;
        MantisMutex fReadMutex;
        MantisMutex fWriteMutex;
};

#endif
