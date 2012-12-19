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

    ofstream tOutputOne( string( "ch1_" ) + string( argv[ 2 ] ) );
    ofstream tOutputTwo( string( "ch2_" ) + string( argv[ 2 ] ) );
    if( tOutputOne.is_open() == false )
    {
        cout << "could not open channel one output file!" << endl;
        return -1;
    }
    if( tOutputTwo.is_open() == false )
    {
        cout << "could not open channel two output file!" << endl;
        return -1;
    }

    const Monarch* tReadTest = Monarch::OpenForReading( argv[ 1 ] );
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
    unsigned int tAcquisitonCount = 0;

    if( tReadHeader->GetAcqMode() == sOneChannel )
    {
        const MonarchRecord* tReadRecord = tReadTest->GetRecordOne();
        while( tReadTest->ReadRecord() != false )
        {
            tRecordCount = tRecordCount + 1;
            if( tReadRecord->fAId == tAcquisitonCount )
            {
                tAcquisitonCount = tAcquisitonCount + 1;
                tOutputOne << "\n\n";
            }
            for( size_t tIndex = 0; tIndex < tReadHeader->GetRecordSize(); tIndex++ )
            {
                tOutputOne << tIndex << " " << (unsigned int) ((unsigned char) (tReadRecord->fDataPtr[ tIndex ])) << "\n";
            }
        }
        tOutputTwo << "(empty)\n";
    }
    if( tReadHeader->GetAcqMode() == sTwoChannel )
    {
        const MonarchRecord* tReadRecordOne = tReadTest->GetRecordOne();
        const MonarchRecord* tReadRecordTwo = tReadTest->GetRecordTwo();
        while( tReadTest->ReadRecord() != false )
        {
            tRecordCount = tRecordCount + 1;
            if( tReadRecordOne->fAId == tAcquisitonCount )
            {
                tAcquisitonCount = tAcquisitonCount + 1;
                tOutputOne << "\n\n";
                tOutputTwo << "\n\n";
            }
            for( size_t tIndex = 0; tIndex < tReadHeader->GetRecordSize(); tIndex++ )
            {
                tOutputOne << tIndex << " " << (unsigned int) ((unsigned char) (tReadRecordOne->fDataPtr[ tIndex ])) << "\n";
                tOutputTwo << tIndex << " " << (unsigned int) ((unsigned char) (tReadRecordTwo->fDataPtr[ tIndex ])) << "\n";
            }
        }
    }

    cout << "record count <" << tRecordCount << ">" << endl;
    cout << "acquisition count <" << tAcquisitonCount << ">" << endl;

    tReadTest->Close();
    delete tReadTest;

    tOutputOne.close();
    tOutputTwo.close();

    return 0;
}

