#ifndef MANTISMUTEX_HH_
#define MANTISMUTEX_HH_

#include <pthread.h>

class MantisMutex
{
    public:
        MantisMutex();
        virtual ~MantisMutex();

        int Lock();
        int Trylock();
        int Unlock();

    private:
        pthread_mutex_t fMutex;
};

#endif
