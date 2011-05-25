#include "MantisIterator.hh"

MantisIterator::MantisIterator( MantisBuffer* aBuffer, const size_t anIndex ) :
    fBuffer( aBuffer ), fIndex( anIndex )
{
    fBuffer->fMutexArray[fIndex].Lock();
}
MantisIterator::~MantisIterator()
{
    fBuffer->fMutexArray[fIndex].Unlock();
}

void MantisIterator::Increment()
{
    fBuffer->fMutexArray[fIndex].Unlock();
    
    fIndex++;
    if( fIndex == fBuffer->fBufferSize )
    {
        fIndex = 0;
    }
    
    fBuffer->fMutexArray[fIndex].Lock();
    
    return;
}
void MantisIterator::Decrement()
{
    fBuffer->fMutexArray[fIndex].Unlock();
    
    if( fIndex == 0 )
    {
        fIndex = fBuffer->fBufferSize;
    }
    fIndex--;
    
    fBuffer->fMutexArray[fIndex].Unlock();
    
    return;
}

MantisBlock* MantisIterator::Block()
{
    return &(fBuffer->fBlockArray[fIndex]);
}
