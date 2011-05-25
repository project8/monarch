#include "MantisBuffer.hh"

#include "MantisBlock.hh"
#include "MantisReadIterator.hh"
#include "MantisWriteIterator.hh"

#include <cstddef>
#include <ctime>

MantisBuffer::MantisBuffer() :
    fBufferLength( 0 ), fRecordLength( 0 ), fBlocks( NULL ), fReadIterator( NULL ), fWriteIterator( NULL )
{
}
~MantisBuffer::MantisBuffer()
{
}

void MantisBuffer::Initialize()
{
    fBlocks = new MantisBlock[fBufferSize];
    fReadIterator = new MantisReadIterator(fBufferLength,fRecordLength,fBlocks);
    fWriteIterator = new MantisWriteIterator(fBufferLength,fRecordLength,fBlocks);
    
    return;
}
void MantisBuffer::Finalize()
{
}

MantisIterator* MantisBuffer::CreateIterator()
{
    if( fMutexArray == NULL )
    {
        fMutexArray = new MantisMutex[fBufferSize];
    }
    if( fBlockArray == NULL )
    {
        fBlockArray = new MantisBlock[fBufferSize];
    }
    
    fIteratorIndex++;
    if( fIteratorIndex == fBufferSize )
    {
        fIteratorIndex == 0;
    }
    return new MantisIterator( this, fIteratorIndex );    
}

void MantisBuffer::SetBufferSize( const size_t& aSize )
{
    fBufferSize = aSize;
    return;
}
const size_t& MantisBuffer::GetBufferSize() const
{
    return fBufferSize;
}

void MantisBuffer::SetDataSize( const size_t& aSize )
{
    fDataSize = aSize;
    return;
}
const size_t& MantisBuffer::GetDataSize() const
{
    return fDataSize;
}
