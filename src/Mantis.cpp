#include "MantisRun.hpp"
#include "MantisPX1500.hpp"
#include "MantisFileWriter.hpp"

#include "MantisThread.hpp"

#include <sstream>
using std::stringstream;

#include <iostream>
using std::cout;
using std::endl;

#include <unistd.h>

int main( int argv, char** argc )
{
    if( argv < 4 )
    {
        cout << "usage:" << endl;
        cout << "\tMantis <rate> <duration> <output file name>" << endl;
        return -1;
    }
    stringstream Converter;
    double Rate;
    unsigned int Duration;
    string FileName;
    
    Converter.clear();
    Converter.str("");
    Converter << argc[1];
    Converter >> Rate;
    
    Converter.clear();
    Converter.str("");
    Converter << argc[2];
    Converter >> Duration;    
    
    Converter.clear();
    Converter.str("");
    Converter << argc[3];
    Converter >> FileName;
    
    MantisStatus* Status = new MantisStatus();
    
    MantisBuffer* Buffer = new MantisBuffer();
    Buffer->SetDataLength( 4194304 );
    Buffer->SetBufferLength( 630 );
    
    MantisRun* Run = new MantisRun();
    Run->SetStatus(Status);
    Run->SetDuration(Duration);
    
    MantisPX1500* Reader = new MantisPX1500();
    Reader->SetStatus(Status);
    Reader->SetBuffer(Buffer);
    Reader->SetDigitizationRate(Rate);
    
    MantisFileWriter* Writer = new MantisFileWriter();
    Writer->SetStatus(Status);
    Writer->SetBuffer(Buffer);
    Writer->SetFileName(FileName);
    
    Buffer->Initialize();
    Run->Initialize();

    cout << "initializing writer..." << endl;
    Reader->Initialize();

    cout << "initializing reader..." << endl;    
    Writer->Initialize();
    
    MantisThread* RunThread = new MantisThread(Run);
    MantisThread* ReadThread = new MantisThread(Reader);
    MantisThread* WriteThread = new MantisThread(Writer);

    cout << "starting read thread..." << endl;
    ReadThread->Start();
    while( Status->GetWriterCondition()->IsWaiting() == false );

    cout << "starting write thread..." << endl;
    WriteThread->Start();    
    while( Status->GetReaderCondition()->IsWaiting() == false );
    
    cout << "starting run thread..." << endl;
    RunThread->Start();

    RunThread->Join();
    WriteThread->Join();
    ReadThread->Join();
    
    delete ReadThread;
    delete WriteThread;
    delete RunThread;
    cout << "threads finished..." << endl;
    
    Writer->Finalize();
    Reader->Finalize();
    Run->Finalize();
    Buffer->Finalize();
    
    delete Status;
    delete Buffer;
    delete Run;
    delete Reader;
    delete Writer;
    
    return 0;
}   
