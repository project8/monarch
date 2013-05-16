#include "Monarch.hpp"

#include <iostream>
using std::cout;
using std::endl;

int main( const int argc, const char** argv )
{
    if( argc < 2 )
    {
        cout << "usage:" << endl;
        cout << "  MonarchInfo <input egg file>" << endl;
        return -1;
    }

    const Monarch* tReadTest = Monarch::OpenForReading( argv[1] );
    tReadTest->ReadHeader();

    const MonarchHeader* tReadHeader = tReadTest->GetHeader();
    cout << *tReadHeader << endl;

    unsigned int tRecordCount = 0;
    unsigned int tAcquisiontCount = 0;
    const MonarchRecord* tReadRecord;
    if( tReadHeader->GetAcquisitionMode() == 1 /* the FormatMode is ignored for single-channel data */ )
    {
        tReadRecord = tReadTest->GetRecordSeparateOne();
    }
    else if( tReadHeader->GetAcquisitionMode() == 2 && tReadHeader->GetFormatMode() == sFormatMultiSeparate )
    {
        tReadRecord = tReadTest->GetRecordSeparateOne();
    }
    else if( tReadHeader->GetAcquisitionMode() == 2 && tReadHeader->GetFormatMode() == sFormatMultiInterleaved )
    {
        tReadRecord = tReadTest->GetRecordInterleaved();
    }
    else
    {
        cout << "Unable to read a header with acquisition mode <" << tReadHeader->GetAcquisitionMode() << "> and format mode <" << tReadHeader->GetFormatMode() << ">" << endl;
        return -1;
    }
    while( tReadTest->ReadRecord() != false )
    {
        tRecordCount = tRecordCount + 1;
        if( tReadRecord->fAcquisitionId == tAcquisiontCount )
        {
            tAcquisiontCount = tAcquisiontCount + 1;
        }
        cout << "  record " << tRecordCount << ": time offset: " << tReadRecord->fTime << " ns" << endl;
    }
    cout << "record count <" << tRecordCount << ">" << endl;
    cout << "acquisition count <" << tAcquisiontCount << ">" << endl;

    tReadTest->Close();
    delete tReadTest;

    return 0;
}
