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
    fStatus->GetWriterCondition()->Release();
    fStatus->GetReaderCondition()->Release(); 
    for( unsigned int Seconds = 1; Seconds <= fDuration; Seconds++ )
    {
        sleep( 1 );
        cout << fDuration - Seconds << " seconds remaining..." << endl;
        if( !fStatus->IsRunning() )
        {
            if( fStatus->IsError() == true )
            {
                cout << "stopping on error." << endl;
            }
            if( fStatus->IsComplete() == true )
            {
                cout << "run complete." << endl;
            }
            
            if( fStatus->GetReaderCondition()->IsWaiting() == true )
            {
                fStatus->GetReaderCondition()->Release();
            }
            if( fStatus->GetWriterCondition()->IsWaiting() == true )
            {
                fStatus->GetWriterCondition()->Release(); 
            }

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
