#ifndef MANTISBUFFER_HPP_
#define MANTISBUFFER_HPP_

#include "MantisBufferBlock.hpp"
#include "MantisBufferIterator.hpp"

class MantisBuffer
{   
    public: 
        MantisBuffer();
        virtual ~MantisBuffer();

        void Initialize();
        void Finalize();
        
        MantisBufferIterator* CreateIterator() const;
        
        void SetBufferLength( const size_t& aLength );
        const size_t& GetBufferLength() const;
        
        void SetDataLength( const size_t& aLength );
        const size_t& GetDataLength() const;
        
    private:
        MantisBufferBlock* fBuffer;
        size_t fBufferLength;
        size_t fDataLength;
};

#endif
