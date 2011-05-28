#ifndef MANTISMUTEX_HPP_
#define MANTISMUTEX_HPP_

#include <pthread.h>

class MantisMutex
{
    public:
        MantisMutex();
        virtual ~MantisMutex();

        bool Trylock();

        void Lock();
        void Unlock();

    private:
        pthread_mutex_t fMutex;
};

#endif
