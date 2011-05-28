#include "MantisBufferIterator.hpp"

#include <iostream>
using std::cout;
using std::endl;

MantisBufferIterator::MantisBufferIterator( MantisBufferBlock* aBlockArray, const size_t& aBlockArrayLength ) :
    fBlockArray( aBlockArray ), fBlockArrayLength( aBlockArrayLength ), fPreviousIndex( aBlockArrayLength - 1 ),fCurrentIndex( 0 ), fNextIndex( 1 )
{
    while( fBlockArray[fCurrentIndex].fDataMutex.Trylock() == false)
    {
        IncrementIndex();
    }
}
MantisBufferIterator::~MantisBufferIterator()
{
    fBlockArray[fCurrentIndex].fDataMutex.Unlock();
}

bool MantisBufferIterator::TryIncrement()
{       
    if( fBlockArray[fNextIndex].fDataMutex.Trylock() == true )
    {
        fBlockArray[fCurrentIndex].fDataMutex.Unlock();
        IncrementIndex();
        return true;
    }
    else
    {
        return false;
    }
}
void MantisBufferIterator::Increment()
{   
    fBlockArray[fNextIndex].fDataMutex.Lock();
    fBlockArray[fCurrentIndex].fDataMutex.Unlock();
    IncrementIndex();
    return;
}

bool MantisBufferIterator::TryDecrement()
{   
    if( fBlockArray[fPreviousIndex].fDataMutex.Trylock() == true )
    {
        fBlockArray[fCurrentIndex].fDataMutex.Unlock();
        DecrementIndex();
        return true;
    }
    else
    {
        return false;
    }
}
void MantisBufferIterator::Decrement()
{   
    fBlockArray[fPreviousIndex].fDataMutex.Lock();
    fBlockArray[fCurrentIndex].fDataMutex.Unlock();
    DecrementIndex();
    return;
}

const MantisState MantisBufferIterator::PeekNextState()
{
    fBlockArray[fNextIndex].fStateMutex.Lock();
    MantisState Copy(fBlockArray[fNextIndex].fState); 
    fBlockArray[fNextIndex].fStateMutex.Unlock();
    return Copy;
}
const MantisState MantisBufferIterator::State()
{
    fBlockArray[fCurrentIndex].fStateMutex.Lock();
    MantisState Copy(fBlockArray[fCurrentIndex].fState);
    fBlockArray[fCurrentIndex].fStateMutex.Unlock();
    return Copy;
}
const MantisState MantisBufferIterator::PeekPreviousState()
{
    fBlockArray[fPreviousIndex].fStateMutex.Lock();
    MantisState Copy(fBlockArray[fPreviousIndex].fState); 
    fBlockArray[fPreviousIndex].fStateMutex.Unlock();
    return Copy;
}

void MantisBufferIterator::SetRead()
{
    fBlockArray[fCurrentIndex].fStateMutex.Lock();
    fBlockArray[fCurrentIndex].fState.SetRead();
    fBlockArray[fCurrentIndex].fStateMutex.Unlock();
    return;
}
void MantisBufferIterator::SetReading()
{
    fBlockArray[fCurrentIndex].fStateMutex.Lock();
    fBlockArray[fCurrentIndex].fState.SetReading();
    fBlockArray[fCurrentIndex].fStateMutex.Unlock();
    return;
}
void MantisBufferIterator::SetWritten()
{
    fBlockArray[fCurrentIndex].fStateMutex.Lock();
    fBlockArray[fCurrentIndex].fState.SetWritten();
    fBlockArray[fCurrentIndex].fStateMutex.Unlock();
    return;
}
void MantisBufferIterator::SetWriting()
{
    fBlockArray[fCurrentIndex].fStateMutex.Lock();
    fBlockArray[fCurrentIndex].fState.SetWriting();
    fBlockArray[fCurrentIndex].fStateMutex.Unlock();
    return;
}


MantisData* MantisBufferIterator::Data()
{
    return &fBlockArray[fCurrentIndex].fData;
}
