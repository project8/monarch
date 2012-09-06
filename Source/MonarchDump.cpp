#include "Monarch.hpp"

#include <fstream>
using std::ofstream;

#include <iostream>
using std::cout;
using std::endl;

int main( const int argc, const char** argv )
{
    if( argc < 3 )
    {
        cout << "usage:" << endl;
        cout << "  MonarchDump <input monarch file> <output text file>" << endl;
        return -1;
    }

    ofstream tOutput( argv[2] );
    if( tOutput.is_open() == false )
    {
        cout << "could not open output file!" << endl;
        return -1;
    }

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

    unsigned int tRecordCount = 0;
    unsigned int tAcquisiontCount = 0;
    const MonarchRecord* tReadRecord = tReadTest->GetRecordOne();
    while( tReadTest->ReadRecord() != false )
    {
        tRecordCount = tRecordCount + 1;
        if( tReadRecord->fAId == tAcquisiontCount )
        {
            tAcquisiontCount = tAcquisiontCount + 1;
            tOutput << "\n\n";
        }
        for( size_t tIndex = 0; tIndex < tReadHeader->GetRecordSize(); tIndex++ )
        {
            tOutput << tIndex << " " << (unsigned int)( (unsigned char) (tReadRecord->fDataPtr[tIndex]) ) << "\n";
        }
    }
    cout << "record count <" << tRecordCount << ">" << endl;
    cout << "acquisition count <" << tAcquisiontCount << ">" << endl;

    tReadTest->Close();
    delete tReadTest;

    tOutput.close();

    return 0;
}

