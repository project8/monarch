 #include "MantisFileWriter.hpp"

#include <cstdio>

#include <iostream>
using std::cout;
using std::endl;

MantisFileWriter::MantisFileWriter() :
    fCondition(), fRecordCount( 0 ), fStatus( NULL ), fBuffer( NULL )
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
  this->egg_ptr = anEgg;
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
    int WriteResult;
    
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

    //go go go
    while( true )
    {
        //check the run status
        if( !fStatus->IsRunning() )
        {
            delete Iterator;
            return;
        }
        
        Iterator->SetReading();
        WriteResult = this->egg_ptr->write_data( Iterator->Data() );
        fRecordCount++;
        Iterator->SetRead();
        
        if( Iterator->TryIncrement() == false )
        {
            if( fStatus->GetWriterCondition()->IsWaiting() == true )
            {
                fStatus->GetWriterCondition()->Release();
            }
            Iterator->Increment();
        }
    }
    return;
}
 
void MantisFileWriter::Finalize()
{
    cout << "records written: " << fRecordCount << endl;
    
    return;
}
