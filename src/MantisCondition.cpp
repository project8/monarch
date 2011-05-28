#include "MantisCondition.hpp"

#include <iostream>
using std::cout;
using std::endl;

MantisCondition::MantisCondition() :
    fState( false ), fMutex(), fCondition()
{
    pthread_mutex_init( &fMutex, NULL );
    pthread_cond_init( &fCondition, NULL );
}
MantisCondition::~MantisCondition()
{
    pthread_cond_destroy( &fCondition );
    pthread_mutex_destroy( &fMutex );
}

bool MantisCondition::IsWaiting()
{
    bool StateCopy;
    pthread_mutex_lock( &fMutex );
    StateCopy = fState;
    cout << "<" << this << "> polled: " << StateCopy << endl;
    pthread_mutex_unlock( &fMutex );
    return StateCopy;
};

void MantisCondition::Wait()
{
    pthread_mutex_lock( &fMutex );
    fState = true;
    while( fState == true )
    {
        pthread_cond_wait( &fCondition, &fMutex );
    }
    pthread_mutex_unlock( &fMutex );
    return;
}
void MantisCondition::Release()
{
    pthread_mutex_lock( &fMutex );
    fState = false;
    pthread_cond_signal( &fCondition );
    pthread_mutex_unlock( &fMutex );
    return;
}    
