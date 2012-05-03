#include "MantisPX1500.hpp"

#include <cstdlib>
#include <unistd.h>

#include <sstream>
using std::stringstream;

#include <iostream>
using std::cout;
using std::endl;

MantisPX1500::MantisPX1500() :
     fCondition(), fHandle(), fAcquisitionCount( 0 ), fRecordCount( 0 ), fLiveMicroseconds( 0 ), fDeadMicroseconds( 0 ), fStatus( NULL ), fBuffer( NULL ), fDigitizationRate( 0. )
{
}
MantisPX1500::~MantisPX1500()
{
}

MantisPX1500* MantisPX1500::digFromEnv(safeEnvPtr& env, 
				       MantisStatus* sts,
				       MantisBuffer* buf)
{
  MantisPX1500* res = new MantisPX1500();
  res->SetDigitizationRate((env.get())->getClockRate());
  res->SetStatus(sts);
  res->SetBuffer(buf);

  return res;
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
    fStatus->SetWriterCondition( &fCondition );
    
    int Result;
    
    Result = ConnectToDevicePX4( &fHandle, 1 );
    if( Result != SIG_SUCCESS )
    {
        DumpLibErrorPX4( Result, "failed to connect to digitizer card: " );
        exit(-1);
    }

    Result = SetPowerupDefaultsPX4( fHandle );
    if( Result != SIG_SUCCESS )
    {
        DumpLibErrorPX4( Result, "failed to enter default state: " );
        exit(-1);
    }

    Result = SetActiveChannelsPX4( fHandle, PX4CHANSEL_SINGLE_CH1 );
    if( Result != SIG_SUCCESS )
    {
        DumpLibErrorPX4( Result, "failed to activate channel 1: " );
        exit(-1);
    }

    Result = SetInternalAdcClockRatePX4( fHandle, fDigitizationRate );
    if( Result != SIG_SUCCESS )
    {
        DumpLibErrorPX4( Result, "failed to set sampling rate: " );
        exit(-1);
    }
    
    MantisBufferIterator* Iterator = fBuffer->CreateIterator();
    for( size_t Count = 0; Count < fBuffer->GetBufferLength(); Count++ )
    {
        Result = AllocateDmaBufferPX4( fHandle, fBuffer->GetDataLength(), &Iterator->Data()->fDataPtr );
        if( Result != SIG_SUCCESS )
        {
            stringstream Converter;
            Converter << "failed to allocate DMA buffer " << Count << ": " ;
            DumpLibErrorPX4( Result, Converter.str().c_str() );
            exit(-1);
        }
        Iterator->Increment();
    }
    delete Iterator;
    
    return;
}

void MantisPX1500::Execute()
{   
    //allocate some local variables
    int Result;
    timeval StartTime;
    timeval EndTime;
    timeval DeadTime;
    
    //grab an iterator
    MantisBufferIterator* Iterator = fBuffer->CreateIterator();
    
    //wait for run to release me
    fCondition.Wait();
    if( fStatus->IsRunning() == false )
    {
        delete Iterator;
        return;
    }
    
    //start acquisition
    Result = BeginBufferedPciAcquisitionPX4( fHandle, PX4_FREE_RUN );
    if( Result != SIG_SUCCESS )
    {
        DumpLibErrorPX4( Result, "failed to begin dma acquisition: " );
        fStatus->SetError();
        return;
    }
    fAcquisitionCount++;
    
    //start timing
    gettimeofday( &StartTime, NULL );
    
    //go go go go
    while( true )
    {
        if( fStatus->IsRunning() == false )
        {
            //this exit occurs during active data acquisition
            
            //get the time and update the number of live microseconds
            gettimeofday( &EndTime, NULL );
            fLiveMicroseconds += (1000000 * EndTime.tv_sec + EndTime.tv_usec) - (1000000 * StartTime.tv_sec + StartTime.tv_usec);
            
            //halt the pci acquisition
            Result = EndBufferedPciAcquisitionPX4( fHandle );
            
            delete Iterator;
            return;
        }
        
        Iterator->SetWriting();
        
        Iterator->Data()->fId = fAcquisitionCount;
        Iterator->Data()->fTick = clock();
        Result = GetPciAcquisitionDataFastPX4( fHandle, fBuffer->GetDataLength(), Iterator->Data()->fDataPtr, 0 );
        if( Result != SIG_SUCCESS )
        {
            DumpLibErrorPX4( Result, "failed to acquire dma data over pci: " );
            Result = EndBufferedPciAcquisitionPX4( fHandle );
            fStatus->SetError();
            delete Iterator;
            return;
        }
        
        fRecordCount++;
        
        Iterator->SetWritten();
        
        if( Iterator->TryIncrement() == false )
        {
            //get the time and update the number of live microseconds
            gettimeofday( &EndTime, NULL );
            fLiveMicroseconds += (1000000 * EndTime.tv_sec + EndTime.tv_usec) - (1000000 * StartTime.tv_sec + StartTime.tv_usec);
            
            //halt the pci acquisition
            Result = EndBufferedPciAcquisitionPX4( fHandle );
            
            //wait
            fCondition.Wait();
            
            //get the time and update the number of dead microseconds
            gettimeofday( &DeadTime, NULL );
            fDeadMicroseconds += (1000000 * DeadTime.tv_sec + DeadTime.tv_usec) - (1000000 * EndTime.tv_sec + EndTime.tv_usec);
            
            if( fStatus->IsRunning() == false )
            {
                delete Iterator;
                return;
            }

            //start acquisition
            Result = BeginBufferedPciAcquisitionPX4( fHandle, PX4_FREE_RUN );
            if( Result != SIG_SUCCESS )
            {
                DumpLibErrorPX4( Result, "failed to begin dma acquisition: " );
                fStatus->SetError();
                return;
            }
            fAcquisitionCount++;
            
            //start timing
            gettimeofday( &StartTime, NULL );
            
            Iterator->Increment();
        }              
    }
    return;
}  

void MantisPX1500::Finalize()
{
    int Result;
    
    MantisBufferIterator* Iterator = fBuffer->CreateIterator();
    for( size_t Count = 0; Count < fBuffer->GetBufferLength(); Count++ )
    {
        Result = FreeDmaBufferPX4( fHandle, Iterator->Data()->fDataPtr );
        if( Result != SIG_SUCCESS )
        {
            DumpLibErrorPX4( Result, "failed to deallocate DMA buffer: " );
            exit(-1);
        }
        Iterator->Increment();
    }
    delete Iterator;
    
    Result = DisconnectFromDevicePX4( fHandle );
    if( Result != SIG_SUCCESS )
    {
        DumpLibErrorPX4( Result, "failed to disconnect from digitizer card: " );
        exit(-1);
    }
    
    double LiveTime = fLiveMicroseconds / 1000000.;
    double DeadTime = fDeadMicroseconds / 1000000.;
    double MegabytesRead = fRecordCount * ( ((double)(fBuffer->GetDataLength())) / (1048576.) );;
    double ReadRate = MegabytesRead / LiveTime;
    
    cout << "\nreader statistics:\n";
    cout << "  * records taken: " << fRecordCount << "\n";
    cout << "  * aquisitions taken: " << fAcquisitionCount << "\n";
    cout << "  * live time: " << LiveTime << "(sec)\n";
    cout << "  * dead time: " << DeadTime << "(sec)\n";
    cout << "  * total data read: " << MegabytesRead << "(Mb)\n";
    cout << "  * average read rate: " << ReadRate << "(Mb/sec)\n";
    cout.flush();

    return;
}
