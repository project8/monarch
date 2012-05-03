#ifndef MANTISSTATUS_HH_
#define MANTISSTATUS_HH_

#include "MantisMutex.hpp"
#include "MantisCondition.hpp"

class MantisStatus
{
    public:
        MantisStatus();
        ~MantisStatus();
        
        void SetIdle();
        bool IsIdle();
        
        void SetRunning();
        bool IsRunning();
        
        void SetComplete();
        bool IsComplete();
        
        void SetError();
        bool IsError();
        
        void SetReaderCondition( MantisCondition* aCondition );
        MantisCondition* GetReaderCondition();
        
        void SetWriterCondition( MantisCondition* aCondition );
        MantisCondition* GetWriterCondition();
        
    private:
        enum{ eIdle, eRunning, eComplete, eError } fValue;
        MantisMutex fMutex;
        
        MantisCondition* fReaderCondition;
        MantisCondition* fWriterCondition;
}; 

#endif     
