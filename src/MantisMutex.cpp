#include "MantisMutex.hpp"

MantisMutex::MantisMutex()
{
    pthread_mutex_init(&fMutex,NULL);
}
MantisMutex::~MantisMutex()
{
    pthread_mutex_destroy(&fMutex);
}

void MantisMutex::Lock()
{
    pthread_mutex_lock(&fMutex);
    return;
}
bool MantisMutex::Trylock()
{
    if( pthread_mutex_trylock(&fMutex) == 0 )
    {
        return true;
    }
    return false;
}
void MantisMutex::Unlock()
{
    pthread_mutex_unlock(&fMutex);
    return;
}
