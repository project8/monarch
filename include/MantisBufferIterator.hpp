#ifndef MANTISBUFFERITERATOR_HPP_
#define MANTISBUFFERITERATOR_HPP_

#include "MantisBufferBlock.hpp"

class MantisBufferIterator
{
    public:
        MantisBufferIterator( MantisBufferBlock* aBlockArray, const size_t& aBlockArrayLength );
        ~MantisBufferIterator();

        void Increment();
        bool TryIncrement();
        
        void Decrement();
        bool TryDecrement();
        
        const MantisState PeekNextState();
        const MantisState State();
        const MantisState PeekPreviousState();

        void SetRead();
        void SetReading();
        void SetWritten();
        void SetWriting();
        
        MantisData* Data();
        const size_t& Index()
        {
            return fCurrentIndex;
        }
        
    protected:
        MantisBufferBlock* fBlockArray;
        const size_t fBlockArrayLength;

        void IncrementIndex();
        void DecrementIndex();
        size_t fPreviousIndex;
        size_t fCurrentIndex;
        size_t fNextIndex;
};

inline void MantisBufferIterator::IncrementIndex()
{
    fPreviousIndex++;
    if( fPreviousIndex == fBlockArrayLength )
    {
        fPreviousIndex = 0;
    }
    fCurrentIndex++;
    if( fCurrentIndex == fBlockArrayLength )
    {
        fCurrentIndex = 0;
    }
    fNextIndex++;
    if( fNextIndex == fBlockArrayLength )
    {
        fNextIndex = 0;
    }
    return;
}

inline void MantisBufferIterator::DecrementIndex()
{
    if( fPreviousIndex == 0 )
    {
        fPreviousIndex = fBlockArrayLength;
    }
    fPreviousIndex--;
    if( fCurrentIndex == 0 )
    {
        fCurrentIndex = fBlockArrayLength;
    }
    fCurrentIndex--;
    if( fNextIndex == 0 )
    {
        fNextIndex = fBlockArrayLength;
    }
    fNextIndex--;
    return;
}

#endif
