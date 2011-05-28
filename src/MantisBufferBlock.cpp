#include "MantisBufferBlock.hpp"

MantisBufferBlock::MantisBufferBlock() :
    fState(), fStateMutex(), fData(), fDataMutex()
{
}
MantisBufferBlock::~MantisBufferBlock()
{
}
