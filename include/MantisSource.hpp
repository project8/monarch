#ifndef MANTISSOURCE_HH_
#define MANTISSOURCE_HH_

class MantisSource
{
    public:
        MantisSource();
        virtual ~MantisSource();
        
        void Initialize();
        void Execute();
        void Finalize(); 
        
    protected:
        virtual void InitializeSource() = 0;
        virtual void InitializeBlock( MantisBlock& aBlock ) = 0;
        
        virtual void ExecuteBlock() = 0;
        
        virtual void FinalizeBlock( MantisBlock& aBlock ) = 0;
        virtual void FinalizeSource() = 0;
};

#endif
