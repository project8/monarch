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

    ofstream tOutputOne( (string( argv[ 2 ] ) + string( "_ch1.txt" )).c_str() );
    ofstream tOutputTwo( (string( argv[ 2 ] ) + string( "_ch2.txt" )).c_str() );
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
    tReadTest->ReadHeader();
    tReadTest->SetInterface( sInterfaceSeparate );

    const MonarchHeader* tReadHeader = tReadTest->GetHeader();
    cout << *tReadHeader << endl;

    unsigned int tRecordCount = 0;
    unsigned int tAcquisitonCount = 0;

    if( tReadHeader->GetFormatMode() == sFormatSingle )
    {
        const MonarchRecord* tReadRecord = tReadTest->GetRecordSeparateOne();
        while( tReadTest->ReadRecord() != false )
        {
            tRecordCount = tRecordCount + 1;
            if( tReadRecord->fAcquisitionId == tAcquisitonCount )
            {
                tAcquisitonCount = tAcquisitonCount + 1;
                tOutputOne << "\n\n";
            }
            for( size_t tIndex = 0; tIndex < tReadHeader->GetLength(); tIndex++ )
            {
                tOutputOne << tIndex << " " << (unsigned int) ((unsigned char) (tReadRecord->fData[tIndex])) << "\n";
            }
        }
        tOutputTwo << "(empty)\n";
    }
    if( (tReadHeader->GetFormatMode() == sFormatInterleavedDual) || (tReadHeader->GetFormatMode() == sFormatSeparateDual) )
    {
        const MonarchRecord* tReadRecordOne = tReadTest->GetRecordSeparateOne();
        const MonarchRecord* tReadRecordTwo = tReadTest->GetRecordSeparateTwo();
        while( tReadTest->ReadRecord() != false )
        {
            tRecordCount = tRecordCount + 1;
            if( tReadRecordOne->fAcquisitionId == tAcquisitonCount )
            {
                tAcquisitonCount = tAcquisitonCount + 1;
                tOutputOne << "\n\n";
                tOutputTwo << "\n\n";
            }
            for( size_t tIndex = 0; tIndex < tReadHeader->GetLength(); tIndex++ )
            {
                tOutputOne << tIndex << " " << (unsigned int) ((unsigned char) (tReadRecordOne->fData[ tIndex ])) << "\n";
                tOutputTwo << tIndex << " " << (unsigned int) ((unsigned char) (tReadRecordTwo->fData[ tIndex ])) << "\n";
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

