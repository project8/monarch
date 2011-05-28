#ifndef MANTISBUFFERBLOCK_HPP_
#define MANTISBUFFERBLOCK_HPP_

#include "MantisData.hpp"
#include "MantisState.hpp"
#include "MantisMutex.hpp"
#include "MantisCondition.hpp"

class MantisBufferBlock
{
    public:        
        friend class MantisBufferIterator;
    
    public:
        MantisBufferBlock();
        ~MantisBufferBlock();
        
    private:
        MantisState fState;
        MantisMutex fStateMutex;
        MantisData fData;
        MantisMutex fDataMutex;
};

#endif
