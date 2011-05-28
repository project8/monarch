#ifndef MANTIS_CONDITION_HPP_
#define MANTIS_CONDITION_HPP_

#include <pthread.h>

class MantisCondition
{
    public:
        MantisCondition();
        ~MantisCondition();
        
        bool IsWaiting();
        
        void Wait();
        void Release();
        
    private:
        bool fState;
        pthread_mutex_t fMutex;
        pthread_cond_t fCondition;
};

#endif
