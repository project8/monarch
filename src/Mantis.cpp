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
    runEnvironment = MantisEnv::parseArgs(argc, argv);

    std::cout << runEnvironment;
    
    MantisStatus* Status = new MantisStatus();    
    MantisBuffer* Buffer = MantisBuffer::bufferFromEnv(runEnvironment);    
    MantisRun* Run = MantisRun::runFromEnv(runEnvironment,Status);    
    MantisPX1500* Reader = MantisPX1500::digFromEnv(runEnvironment,
						    Status,
						    Buffer);
    MantisEgg* OutputFile = MantisEgg::egg_from_env(runEnvironment);
    OutputFile->write_header();
    
    MantisFileWriter* Writer = MantisFileWriter::writerFromEnv(runEnvironment,
							       OutputFile,
							       Status,
							       Buffer);
    
    Buffer->Initialize();
    Run->Initialize();

    Reader->Initialize();
    Writer->Initialize();
    
    MantisThread* RunThread = new MantisThread(Run);
    MantisThread* ReadThread = new MantisThread(Reader);
    MantisThread* WriteThread = new MantisThread(Writer);
    
    cout << "starting threads..." << endl;

    ReadThread->Start();
    while( Status->GetWriterCondition()->IsWaiting() == false );

    WriteThread->Start();    
    while( Status->GetReaderCondition()->IsWaiting() == false );
    
    RunThread->Start();

    cout << "threads started..." << endl;

    RunThread->Join();
    WriteThread->Join();
    ReadThread->Join();
    
    cout << "threads returned..." << endl;
    
    delete ReadThread;
    delete WriteThread;
    delete RunThread;
    
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
