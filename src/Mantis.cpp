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
    
    MantisStatus* Status = new MantisStatus();    
    MantisBuffer* Buffer = MantisBuffer::bufferFromEnv(runEnvironment);    
    MantisRun* Run = MantisRun::runFromEnv(runEnvironment,Status);    
    MantisPX1500* Reader = MantisPX1500::digFromEnv(runEnvironment,
						    Status,
						    Buffer);
    
    MantisFileWriter* Writer = MantisFileWriter::writerFromEnv(runEnvironment,
							       Status,
							       Buffer);
    
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
