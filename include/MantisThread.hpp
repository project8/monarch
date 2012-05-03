#ifndef MANTISTHREAD_HPP_
#define MANTISTHREAD_HPP_

#include "pthread.h"

class MantisCallable;

class MantisThread
{
    public:
        typedef enum{ eReady, eRunning, eCancelled, eComplete } State;

    public:
        MantisThread(MantisCallable* obj);
        virtual ~MantisThread();

        void Start();
        void Join();
        void Cancel();
        void Reset();

        const State& GetState();

    private:
        static void* ThreadActionFunction( void* voidthread );
        static void ThreadCleanupFunction( void* voidstate );

        pthread_t fThread;
        State fState;
        MantisCallable* fObject;
};

#endif
