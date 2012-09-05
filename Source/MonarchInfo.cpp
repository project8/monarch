#include "Monarch.hpp"

#include <iostream>
using std::cout;
using std::endl;

int main( const int argc, const char** argv )
{
    const Monarch* tReadTest = Monarch::OpenForReading( argv[1] );

    if( tReadTest->ReadHeader() == false )
    {
        cout << "could not read header!" << endl;
        return -1;
    }
    const MonarchHeader* tReadHeader = tReadTest->GetHeader();
    cout << "acquisition mode <" << tReadHeader->GetAcqMode() << ">" << endl;
    cout << "acquisition rate <" << tReadHeader->GetAcqRate() << ">" << endl;
    cout << "acquisition time <" << tReadHeader->GetAcqTime() << ">" << endl;
    cout << "record size <" << tReadHeader->GetRecordSize() << ">" << endl;

    bool tBigger;

    unsigned int tRecordCount = 0;
    unsigned int tAcquisiontCount = 0;
    const MonarchRecord* tReadRecord = tReadTest->GetRecordOne();
    while( tReadTest->ReadRecord() != false )
    {
        tRecordCount = tRecordCount + 1;
        if( tReadRecord->fAId == tAcquisiontCount )
        {
            tAcquisiontCount = tAcquisiontCount + 1;
        }

        if( tReadRecord->fDataPtr[8192] > 0 )
        {
            tBigger = true;
        }
        else
        {
            tBigger = false;
        }
    }
    cout << "record count <" << tRecordCount << ">" << endl;
    cout << "acquisition count <" << tAcquisiontCount << ">" << endl;

    tReadTest->Close();
    delete tReadTest;

    return 0;
}
