#ifndef MANTISITERATOR_HH_
#define MANTISITERATOR_HH_

class MantisIterator
{
    public:
        MantisIterator( MantisBuffer* aBuffer, const size_t anIndex );
        ~MantisIterator();

        void Increment();
        void Decrement();
        
        MantisBlock* Block() const;
        
    private:
        MantisBuffer* fBuffer;
        size_t fIndex;
        
};

#endif
