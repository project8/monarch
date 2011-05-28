#include "MantisBuffer.hpp"

MantisBuffer::MantisBuffer() :
     fBuffer( NULL ), fBufferLength( 0 ), fDataLength( 0 )
{
}
MantisBuffer::~MantisBuffer()
{
}

MantisBuffer* MantisBuffer::bufferFromEnv(safeEnvPtr& env)
{
  MantisBuffer* res = new MantisBuffer();
  
  res->SetDataLength((env.get())->getDataWidth());
  res->SetBufferLength((env.get())->getBufferCount());

  return res;
}

void MantisBuffer::Initialize()
{
    fBuffer = new MantisBufferBlock[fBufferLength];
    return;
}
void MantisBuffer::Finalize()
{
    if( fBuffer != NULL )
    {
        delete[] fBuffer;
        fBuffer = NULL;
    }
    return;
}

MantisBufferIterator* MantisBuffer::CreateIterator() const
{
    return new MantisBufferIterator( fBuffer, fBufferLength );    
}

void MantisBuffer::SetBufferLength( const size_t& aLength )
{
    fBufferLength = aLength;
    return;
}
const size_t& MantisBuffer::GetBufferLength() const
{
    return fBufferLength;
}

void MantisBuffer::SetDataLength( const size_t& aLength )
{
    fDataLength = aLength;
    return;
}
const size_t& MantisBuffer::GetDataLength() const
{
    return fDataLength;
}
