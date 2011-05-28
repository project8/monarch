#include "MantisThread.hpp"

#include "MantisCallable.hpp"

MantisThread::MantisThread(MantisCallable* obj) :
    fThread(),
    fState(eReady),
    fObject(obj)
{
}

MantisThread::~MantisThread()
{
    if( fState == eRunning )
    {
        Cancel();
    }
}

void MantisThread::Start()
{
    if( fState == eReady )
    {
        pthread_create(&fThread, 0, &MantisThread::ThreadActionFunction, this);
        fState = eRunning;
    }
    return;
}
void MantisThread::Join()
{
    if( fState == eRunning )
    {
        pthread_join(fThread,0);
    }
    return;
}
void MantisThread::Cancel()
{
    if( fState == eRunning )
    {
        pthread_cancel(fThread);
        fState = eCancelled;
    }
    return;
}
void MantisThread::Reset()
{
    fState = eReady;
    return;
}

const MantisThread::State& MantisThread::GetState()
{
    return fState;
}

void* MantisThread::ThreadActionFunction( void* voidthread )
{
    pthread_cleanup_push( &MantisThread::ThreadCleanupFunction, voidthread );
    MantisThread* thread = (MantisThread*)(voidthread);
    MantisCallable* object = thread->fObject;
    object->Execute();
    thread->fState = eComplete;
    pthread_cleanup_pop(0);
    return 0;
}

void MantisThread::ThreadCleanupFunction(void* voidthread )
{
    MantisThread* thread = (MantisThread*)(voidthread);
    MantisThread::State state = thread->fState;
    state = eCancelled;
    return;
}
