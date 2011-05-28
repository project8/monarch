#include "MantisEnv.hpp"
#include "MantisRun.hpp"
#include "MantisPX1500.hpp"
#include "MantisFileWriter.hpp"

#include "MantisThread.hpp"

#include <sstream>
using std::stringstream;

#include <iostream>
using std::cout;
using std::endl;

// The global environment variable.
safeEnvPtr runEnvironment;

int main( int argc, char** argv )
{
    if( argc < 4 )
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
    Converter << argv[1];
    Converter >> Rate;
    
    Converter.clear();
    Converter.str("");
    Converter << argv[2];
    Converter >> Duration;    
    
    Converter.clear();
    Converter.str("");
    Converter << argv[3];
    Converter >> FileName;

    runEnvironment = MantisEnv::parseArgs(argc, argv);
    
    MantisStatus* Status = new MantisStatus();
    
    MantisBuffer* Buffer = new MantisBuffer();
    Buffer->SetDataLength(4194304);
    Buffer->SetBufferLength(400);
    
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

    cout << "initializing digitizer..." << endl;
    Reader->Initialize();

    cout << "initializing writer..." << endl;    
    Writer->Initialize();
    
    MantisThread* RunThread = new MantisThread(Run);
    MantisThread* ReadThread = new MantisThread(Reader);
    MantisThread* WriteThread = new MantisThread(Writer);

    cout << "starting threads..." << endl;    
    ReadThread->Start();
    WriteThread->Start();
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
