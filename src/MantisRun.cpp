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

MantisRun* MantisRun::runFromEnv(safeEnvPtr& env, MantisStatus* sts)
{
  MantisRun* res = new MantisRun();
  res->SetDuration((env.get())->getRunLength());
  res->SetStatus(sts);

  return res;
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
        if( !fStatus->IsRunning() )
        {

            if( fStatus->GetWriterCondition()->IsWaiting() == true )
            {
                fStatus->GetWriterCondition()->Release(); 
            }
            if( fStatus->GetReaderCondition()->IsWaiting() == true )
            {
                fStatus->GetReaderCondition()->Release();
            }

            if( fStatus->IsError() == true )
            {
                cout << "stopping on error." << endl;
            }
            return;
        }
    }
    fStatus->SetComplete();
    
    if( fStatus->GetWriterCondition()->IsWaiting() == true )
    {
        fStatus->GetWriterCondition()->Release(); 
    }
    if( fStatus->GetReaderCondition()->IsWaiting() == true )
    {
        fStatus->GetReaderCondition()->Release();
    }
    
    cout << "run complete." << endl;
    
    return;
}

void MantisRun::Finalize()
{
    return;
}
