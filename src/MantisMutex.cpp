#include "MantisMutex.hh"

MantisMutex::MantisMutex()
{
    pthread_mutex_init(&fMutex,0);
}
MantisMutex::~MantisMutex()
{
    pthread_mutex_destroy(&fMutex);
}

int MantisMutex::Lock()
{
    return pthread_mutex_lock(&fMutex);
}
int MantisMutex::Trylock()
{
    return pthread_mutex_trylock(&fMutex);
}
int MantisMutex::Unlock()
{
    return pthread_mutex_unlock(&fMutex);
}
