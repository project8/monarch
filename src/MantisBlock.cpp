#include "MantisBlock.hpp"

MantisBlock::MantisBlock() :
    fData( NULL ), fId( 0 ), fTick( 0 ), fReadMutex(), fWriteMutex()
{
}
MantisBlock::~MantisBlock()
{
}
