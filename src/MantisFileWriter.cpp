#include "MantisFileWriter.hpp"

#include <iostream>
using std::cout;
using std::endl;

MantisFileWriter::MantisFileWriter() :
    fCondition(), fRecordCount( 0 ), fLiveMicroseconds( 0 ), fEgg( NULL ), fStatus( NULL ), fBuffer( NULL )
{
}
MantisFileWriter::~MantisFileWriter()
{
}

MantisFileWriter* MantisFileWriter::writerFromEnv(safeEnvPtr& env,
						  MantisEgg* egg,
						  MantisStatus* sts,
						  MantisBuffer* buf)
{
  MantisFileWriter* res = new MantisFileWriter();
  res->SetOutputEgg(egg);
  res->SetStatus(sts);
  res->SetBuffer(buf);
  return res;
}

void MantisFileWriter::SetStatus( MantisStatus* aStatus )
{
    fStatus = aStatus;
    return;
}
void MantisFileWriter::SetBuffer( MantisBuffer* aBuffer )
{
    fBuffer = aBuffer;
    return;
}
void MantisFileWriter::SetOutputEgg( MantisEgg* anEgg )
{
    fEgg = anEgg;
    return;
}

void MantisFileWriter::Initialize()
{
    fStatus->SetReaderCondition( &fCondition );    
    return;
}

void MantisFileWriter::Execute()
{
    //allocate some local variables
    int Result;
    timeval StartTime;
    timeval EndTime;
    
    //get an iterator and pull it up to right behind the read iterator
    MantisBufferIterator* Iterator = fBuffer->CreateIterator();
    while( Iterator->TryIncrement() == true );
    
    //wait for run to release me
    fCondition.Wait();
    if( fStatus->IsRunning() == false )
    {
        delete Iterator;
        return;
    }
    
    //start timing
    gettimeofday( &StartTime, NULL );

    //go go go
    while( true )
    {
        //check the run status
        if( !fStatus->IsRunning() )
        {
            //get the time and update the number of live microseconds
            gettimeofday( &EndTime, NULL );
            fLiveMicroseconds += (1000000 * EndTime.tv_sec + EndTime.tv_usec) - (1000000 * StartTime.tv_sec + StartTime.tv_usec);
            
            delete Iterator;
            return;
        }
        
        if( Iterator->TryIncrement() == false )
        {
            if( fStatus->GetWriterCondition()->IsWaiting() == true )
            {
                fStatus->GetWriterCondition()->Release();
            }
            Iterator->Increment();
        }
        
        if( !Iterator->State().IsWritten() )
        {
            cout << "found an unwritten block, might possibly fail?\n";
        }
        Iterator->SetReading();
        Result = fEgg->write_data( Iterator->Data() );
        fRecordCount++;
        Iterator->SetRead();
    }
    return;
}
 
void MantisFileWriter::Finalize()
{
    double LiveTime = fLiveMicroseconds / 1000000.;
    double MegabytesWritten = fRecordCount * ( ((double)(fBuffer->GetDataLength())) / (1048576.) );
    double WriteRate = MegabytesWritten / LiveTime;
    
    cout << "\nwriter statistics:\n";
    cout << "  * records written: " << fRecordCount << "\n";
    cout << "  * live time: " << LiveTime << "(sec)\n";
    cout << "  * total data written: " << MegabytesWritten << "(Mb)\n";
    cout << "  * average write rate: " << WriteRate << "(Mb/sec)\n";
    cout.flush();
    
    return;
}
