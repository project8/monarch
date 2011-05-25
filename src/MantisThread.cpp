#include "MantisThread.hh"

#include "MantisCallable.hh"

MantisThread::MantisThread(MantisCallable* obj) :
    fThread(),
    fThreadState(eReady),
    fObject(obj)
{
}

MantisThread::~MantisThread()
{
    if( fThreadState == eRunning )
    {
        Cancel();
    }
}

void MantisThread::Start()
{
    if( fThreadState == eReady )
    {
        pthread_create(&fThread, 0, &MantisThread::ThreadActionFunction, this);
        fThreadState = eRunning;
    }
    return;
}
void MantisThread::Join()
{
    if( fThreadState == eRunning )
    {
        pthread_join(fThread,0);
    }
    return;
}
void MantisThread::Cancel()
{
    if( fThreadState == eRunning )
    {
        pthread_cancel(fThread);
        fThreadState = eCancelled;
    }
    return;
}
void MantisThread::Reset()
{
    fThreadState = eReady;
    return;
}

const MantisThread::State& MantisThread::GetState()
{
    return fThreadState;
}

void* MantisThread::ThreadActionFunction( void* voidthread )
{
    pthread_cleanup_push( &MantisThread::ThreadCleanupFunction, voidthread );
    MantisThread* thread = (MantisThread*)(voidthread);
    MantisThread::State state = thread->fThreadState;
    MantisCallable* object = thread->fObject;
    object->Execute();
    state = eComplete;
    pthread_cleanup_pop(0);
    return 0;
}

void MantisThread::ThreadCleanupFunction(void* voidthread )
{
    MantisThread* thread = (MantisThread*)(voidthread);
    MantisThread::State state = thread->fThreadState;
    state = eCancelled;
    return;
}
