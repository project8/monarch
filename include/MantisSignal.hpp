#ifndef MANTISSTATUS_HH_
#define MANTISSTATUS_HH_

#include "MantisMutex.hpp"

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
        
    private:
        enum{ eIdle, eRunning, eComplete, eError } fValue;
        MantisMutex fMutex;
}; 

#endif     
