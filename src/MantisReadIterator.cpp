#include "MantisReadIterator.hpp"

MantisReadIterator::MantisReadIterator( const size_t& aBufferLength, const size_t& aRecordLength, MantisBlock* aBlock ) :
    fBufferLength( aBufferLength ), fRecordLength( aRecordLength ), fBlocks( aBlock )
{
    
