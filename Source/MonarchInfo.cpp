#include "Monarch.hpp"

#include <iostream>
using std::cout;
using std::endl;

int main( const int argc, const char** argv )
{
    const Monarch* tReadTest = Monarch::OpenForReading( argv[1] );
    tReadTest->ReadHeader();

    const MonarchHeader* tReadHeader = tReadTest->GetHeader();
    cout << *tReadHeader << endl;

    unsigned int tRecordCount = 0;
    unsigned int tAcquisiontCount = 0;
    const MonarchRecord* tReadRecord;
    if( tReadHeader->GetFormatMode() == sFormatSingle )
    {
        tReadRecord = tReadTest->GetRecordSeparateOne();
    }
    if( tReadHeader->GetFormatMode() == sFormatSeparateDual )
    {
        tReadRecord = tReadTest->GetRecordSeparateOne();
    }
    if( tReadHeader->GetFormatMode() == sFormatInterleavedDual )
    {
        tReadRecord = tReadTest->GetRecordInterleaved();
    }
    while( tReadTest->ReadRecord() != false )
    {
        tRecordCount = tRecordCount + 1;
        if( tReadRecord->fAcquisitionId == tAcquisiontCount )
        {
            tAcquisiontCount = tAcquisiontCount + 1;
        }
    }
    cout << "record count <" << tRecordCount << ">" << endl;
    cout << "acquisition count <" << tAcquisiontCount << ">" << endl;

    tReadTest->Close();
    delete tReadTest;

    return 0;
}
