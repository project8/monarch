#include "MantisStatus.hpp"

MantisStatus::MantisStatus() :
    fValue( eIdle ), fMutex(), fReaderCondition( NULL ), fWriterCondition( NULL )
{
}
MantisStatus::~MantisStatus()
{
}

void MantisStatus::SetIdle()
{
    fMutex.Lock();
    fValue = eIdle;
    fMutex.Unlock();
    return;
}
bool MantisStatus::IsIdle()
{
    fMutex.Lock();
    bool Value = (fValue == eIdle);
    fMutex.Unlock();
    return Value;
}

void MantisStatus::SetRunning()
{
    fMutex.Lock();
    fValue = eRunning;
    fMutex.Unlock();
    return;
}
bool MantisStatus::IsRunning()
{
    fMutex.Lock();
    bool Value = (fValue == eRunning);
    fMutex.Unlock();
    return Value;
}

void MantisStatus::SetError()
{
    fMutex.Lock();
    fValue = eError;
    fMutex.Unlock();
    return;
}
bool MantisStatus::IsError()
{
    fMutex.Lock();
    bool Value = (fValue == eError);
    fMutex.Unlock();
    return Value;
}

void MantisStatus::SetComplete()
{
    fMutex.Lock();
    fValue = eComplete;
    fMutex.Unlock();
    return;
}
bool MantisStatus::IsComplete()
{
    fMutex.Lock();
    bool Value = (fValue == eComplete);
    fMutex.Unlock();
    return Value;
}

void MantisStatus::SetReaderCondition( MantisCondition* aCondition )
{
    fReaderCondition = aCondition;
    return;
}
MantisCondition* MantisStatus::GetReaderCondition()
{
    return fReaderCondition;
}

void MantisStatus::SetWriterCondition( MantisCondition* aCondition )
{
    fWriterCondition = aCondition;
    return;
}
MantisCondition* MantisStatus::GetWriterCondition()
{
    return fWriterCondition;
}
