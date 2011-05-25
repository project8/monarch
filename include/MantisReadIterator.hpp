#ifndef MANTISREADITERATOR_HPP_
#define MANTISREADITERATOR_HPP_

#include "MantisBlock.hh"

class MantisReadIterator
{
    public:
        MantisReadIterator( const size_t& aBufferLength, const size_t& aRecordLength, MantisBlock* aBlock );
        ~MantisIterator();

        bool Increment();
        bool TryIncrement();
        
        bool Decrement();
        bool TryDecrement();
        
        const MantisBlock::IdType& GetId();
        const MantisBlock::ClockType& GetTick();
        
        const MantisBlock::DataType* Data();
        
    private:
        size_t fBufferLength;
        size_t fRecordLength;
        MantisBlock* fBlocks;

        size_t fIndex;        
};

#endif
