#include "MantisPX1500.hpp"

#include <cstdlib>
#include <unistd.h>

#include <sstream>
using std::stringstream;

#include <iostream>
using std::cout;
using std::endl;

MantisPX1500::MantisPX1500() :
     fHandle(), fAcquisitionCount( 0 ), fRecordCount( 0 ), fDeadTickCount( 0 ), fStatus( NULL ), fBuffer( NULL ), fDigitizationRate( 0. )
{
}
MantisPX1500::~MantisPX1500()
{
}

void MantisPX1500::SetStatus( MantisStatus* aStatus )
{
    fStatus = aStatus;
    return;
}
void MantisPX1500::SetBuffer( MantisBuffer* aBuffer )
{
    fBuffer = aBuffer;
    return;
}
void MantisPX1500::SetDigitizationRate( const double& aRate )
{
    fDigitizationRate = aRate;
    return;
}

void MantisPX1500::Initialize()
{
    int PX4Result;
    
    PX4Result = ConnectToDevicePX4( &fHandle, 1 );
    if( PX4Result != SIG_SUCCESS )
    {
        DumpLibErrorPX4( PX4Result, "failed to connect to digitizer card: " );
        exit(-1);
    }

    PX4Result = SetPowerupDefaultsPX4( fHandle );
    if( PX4Result != SIG_SUCCESS )
    {
        DumpLibErrorPX4( PX4Result, "failed to enter default state: " );
        exit(-1);
    }

    PX4Result = SetActiveChannelsPX4( fHandle, PX4CHANSEL_SINGLE_CH1 );
    if( PX4Result != SIG_SUCCESS )
    {
        DumpLibErrorPX4( PX4Result, "failed to activate channel 1: " );
        exit(-1);
    }

    PX4Result = SetInternalAdcClockRatePX4( fHandle, fDigitizationRate );
    if( PX4Result != SIG_SUCCESS )
    {
        DumpLibErrorPX4( PX4Result, "failed to set sampling rate: " );
        exit(-1);
    }
    
    MantisBufferIterator* Iterator = fBuffer->CreateIterator();
    for( size_t Count = 0; Count < fBuffer->GetBufferLength(); Count++ )
    {
        PX4Result = AllocateDmaBufferPX4( fHandle, fBuffer->GetDataLength(), &Iterator->Data()->fDataPtr );
        if( PX4Result != SIG_SUCCESS )
        {
            stringstream Converter;
            Converter << "failed to allocate DMA buffer " << Count << ": " ;
            DumpLibErrorPX4( PX4Result, Converter.str().c_str() );
            exit(-1);
        }
        Iterator->Increment();
    }
    delete Iterator;
    
    return;
}

void MantisPX1500::Execute()
{   
    MantisBufferIterator* Iterator = fBuffer->CreateIterator();
    
    int PX4Result;
    clock_t StartTick;
    clock_t EndTick;
    
    PX4Result = BeginBufferedPciAcquisitionPX4( fHandle, PX4_FREE_RUN );
    fAcquisitionCount++;
    
    while( true )
    {
        if( fStatus->IsRunning() == false )
        {
            EndBufferedPciAcquisitionPX4( fHandle );
            delete Iterator;
            return;
        }
        
        Iterator->Data()->fId = fAcquisitionCount;
        Iterator->Data()->fTick = clock();
        PX4Result = GetPciAcquisitionDataFastPX4( fHandle, fBuffer->GetDataLength(), Iterator->Data()->fDataPtr, 0 );
        fRecordCount++;
        
        if( Iterator->PeekNextState().IsRead() == false )
        {
            PX4Result = EndBufferedPciAcquisitionPX4( fHandle );

            StartTick = clock();
            Iterator->Wait();
            EndTick = clock();
            fDeadTickCount += (EndTick - StartTick);

            PX4Result = BeginBufferedPciAcquisitionPX4( fHandle, PX4_FREE_RUN );
            fAcquisitionCount++;
            
            Iterator->Increment();
        }              
    }
    return;
}  

void MantisPX1500::Finalize()
{
    int PX4Result;
    
    MantisBufferIterator* Iterator = fBuffer->CreateIterator();
    for( size_t Count = 0; Count < fBuffer->GetBufferLength(); Count++ )
    {
        PX4Result = FreeDmaBufferPX4( fHandle, Iterator->Data()->fDataPtr );
        if( PX4Result != SIG_SUCCESS )
        {
            DumpLibErrorPX4( PX4Result, "failed to deallocate DMA buffer: " );
            exit(-1);
        }
        Iterator->Increment();
    }
    delete Iterator;
    
    PX4Result = DisconnectFromDevicePX4( fHandle );
    if( PX4Result != SIG_SUCCESS )
    {
        DumpLibErrorPX4( PX4Result, "failed to disconnect from digitizer card: " );
        exit(-1);
    }
    
    cout << "aquisitions taken: " << fAcquisitionCount << endl;
    cout << "records taken: " << fRecordCount << endl;
    cout << "dead time: " << (double)(fDeadTickCount) / (double)(CLOCKS_PER_SEC) << endl;

    return;
}
