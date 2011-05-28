#include "MantisFileWriter.hpp"

#include <cstdio>

#include <iostream>
using std::cout;
using std::endl;

MantisFileWriter::MantisFileWriter() :
    fFile( NULL ), fRecordCount( 0 ), fStatus( NULL ), fBuffer( NULL ), fFileName("")
{
}
MantisFileWriter::~MantisFileWriter()
{
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
void MantisFileWriter::SetFileName( const string& aName )
{
    fFileName = aName;
    return;
}

void MantisFileWriter::Initialize()
{
    fFile = fopen( fFileName.c_str(), "w" );
    return;
}

void MantisFileWriter::Execute()
{
    MantisBufferIterator* Iterator = fBuffer->CreateIterator();

    int WriteResult;
    
    fRecordCount = 0;

    while( true )
    {
        if( !fStatus->IsRunning() )
        {
            delete Iterator;
            return;
        }
        
        if( Iterator->State().IsWritten() == true )
        {
            Iterator->SetReading();
            WriteResult = fwrite( Iterator->Data()->fDataPtr, sizeof( MantisData::DataType ), fBuffer->GetDataLength(), fFile );
            fRecordCount++;
            Iterator->SetRead();
        }
        
        Iterator->Increment();
    }
    return;
}
 
void MantisFileWriter::Finalize()
{
    fclose( fFile );
    
    cout << "records written: " << fRecordCount << endl;
    
    return;
}       
