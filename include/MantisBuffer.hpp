#ifndef MANTISBUFFER_HH_
#define MANTISBUFFER_HH_

class MantisBlock;
class MantisReadIterator;
class MantisWriteIterator;

class MantisBuffer
{   
    public: 
        MantisBuffer();
        virtual ~MantisBuffer();

        void Initialize();
        void Finalize();
        
        MantisReadIterator* GetReadIterator();
        MantisWriteIterator* GetWriteIterator();
        
        void SetBufferLength( const size_t& aSize );
        const size_t& GetBufferLength();
        
        void SetRecordLength( const size_t& aSize );
        const size_t& GetRecordLength();
        
    private:
        size_t fBufferLength;
        size_t fRecordLength;
        MantisBlock* fBlocks;
        
        MantisReadIterator* fReadIterator;
        MantisWriteIterator* fWriteIterator;
};

#endif
