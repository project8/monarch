#include "MantisRun.hpp"

#include <unistd.h>

#include <iostream>
using std::cout;
using std::endl;

MantisRun::MantisRun() :
    fStatus( NULL ), fDuration( 0 )
{
}
MantisRun::~MantisRun()
{
}

void MantisRun::SetStatus( MantisStatus* aStatus )
{
    fStatus = aStatus;
    return;
}
void MantisRun::SetDuration( const unsigned int& aDuration )
{
    fDuration = aDuration;
    return;
}

void MantisRun::Initialize()
{
    return;
}

void MantisRun::Execute()
{
    fStatus->SetRunning();
    for( unsigned int Seconds = 1; Seconds <= fDuration; Seconds++ )
    {
        sleep( 1 );
        cout << fDuration - Seconds << " seconds remaining..." << endl;
        if( !fStatus->IsRunning() )
        {
            return;
        }
    }
    fStatus->SetComplete();
    return;
}

void MantisRun::Finalize()
{
    return;
}
