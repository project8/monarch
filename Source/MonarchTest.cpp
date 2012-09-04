#include <cstdlib>
#include <iostream>
using std::cout;
using std::endl;

#include "time.h"
#include "Monarch.hpp"

int main()
{
    static const unsigned int tRecordSize = 20;
    static const unsigned int tRecordCount = 5;


    Monarch* tWriteTest = Monarch::OpenForWriting( "MonarchTest.egg" );

    MonarchHeader* tWriteHeader = tWriteTest->GetHeader();
    tWriteHeader->SetAcqMode( sOneChannel );
    tWriteHeader->SetAcqRate( 500.0 );
    tWriteHeader->SetAcqTime( 100 );
    tWriteHeader->SetRecordSize( tRecordSize );
    if( tWriteTest->WriteHeader() == false )
    {
        cout << "could not read header" << endl;
        return -1;
    }

    MonarchRecord* tWriteRecord = tWriteTest->GetRecordOne();

    for( unsigned int tRecordIndex = 0; tRecordIndex < tRecordCount; tRecordIndex++ )
    {
        tWriteRecord->fRId = tRecordIndex;
        for( unsigned int tDataIndex = 0; tDataIndex < tRecordSize; tDataIndex++ )
        {
            tWriteRecord->fDataPtr[tDataIndex] = 'A' + tDataIndex + tRecordIndex;
        }
        if( tWriteTest->WriteRecord() == false )
        {
            cout << "could not write record " << tRecordIndex << endl;
            return -1;
        }
    }

    tWriteTest->Close();
    delete tWriteTest;


    const Monarch* tReadTest = Monarch::OpenForReading( "MonarchTest.egg" );

    if( tReadTest->ReadHeader() == false )
    {
        return -1;
    }
    const MonarchHeader* tReadHeader = tReadTest->GetHeader();
    cout << "acquisition mode <" << tReadHeader->GetAcqMode() << ">" << endl;
    cout << "acquisition rate <" << tReadHeader->GetAcqRate() << ">" << endl;
    cout << "acquisition time <" << tReadHeader->GetAcqTime() << ">" << endl;
    cout << "record size <" << tReadHeader->GetRecordSize() << ">" << endl;

    const MonarchRecord* tReadRecord = tReadTest->GetRecordOne();

    for( unsigned int tRecordIndex = 0; tRecordIndex < tRecordCount; tRecordIndex++ )
    {
        if( tReadTest->ReadRecord() == false )
        {
            cout << "could not read record " << tRecordIndex << endl;
            return -1;
        }
        cout << "* record <" << tReadRecord->fRId << ">" << endl;
        for( unsigned int tDataIndex = 0; tDataIndex < tRecordSize; tDataIndex++ )
        {
            cout << "  * data <" << (char) (tReadRecord->fDataPtr[tDataIndex]) << ">" << endl;
        }

    }

    tReadTest->Close();
    delete tReadTest;


    return 0;
}
