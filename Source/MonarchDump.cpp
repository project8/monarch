#include "Monarch.hpp"
#include "MonarchLogger.hpp"

#include <cstdlib>

#include <fstream>
using std::ofstream;

using namespace monarch;

MLOGGER( mlog, "MonarchDump" );

int main( const int argc, const char** argv )
{
    if( argc < 3 )
    {
        MINFO( mlog, "usage:\n"
            << "  MonarchDump <input egg file> <output text file> <# of records per channel [optional]>\n"
            << "# of records is optional; the default is 1; use 0 to dump the whole file" );
        return -1;
    }

    ofstream tOutputOne( (string( argv[ 2 ] ) + string( "_ch1.txt" )).c_str() );
    ofstream tOutputTwo( (string( argv[ 2 ] ) + string( "_ch2.txt" )).c_str() );
    if( tOutputOne.is_open() == false )
    {
        MERROR( mlog, "could not open channel one output file!" );
        return -1;
    }
    if( tOutputTwo.is_open() == false )
    {
        MERROR( mlog, "could not open channel two output file!" );
        return -1;
    }

    unsigned int nRecords = 1;
    if( argc >= 4 )
    {
        nRecords = atoi( argv[3] );
    }

    const Monarch* tReadTest = Monarch::OpenForReading( argv[ 1 ] );
    tReadTest->ReadHeader();
    tReadTest->SetInterface( sInterfaceSeparate );

    const MonarchHeader* tReadHeader = tReadTest->GetHeader();
    cout << *tReadHeader << endl;

    unsigned int tRecordCount = 0;
    unsigned int tAcquisitionCount = 0;

    if( tReadHeader->GetFormatMode() == sFormatSingle )
    {
        const MonarchRecordBytes* tReadRecord = tReadTest->GetRecordSeparateOne();
        const MonarchRecordDataInterface tData( tReadRecord->fData, tReadHeader->GetDataTypeSize() );
        unsigned int tRecordsPerChannel = 0;
        while( tReadTest->ReadRecord() != false )
        {
            tRecordCount++;
            tRecordsPerChannel++;
            if( tReadRecord->fAcquisitionId == tAcquisitionCount )
            {
                tAcquisitionCount = tAcquisitionCount + 1;
                tOutputOne << "\n\n";
            }
            for( unsigned int tIndex = 0; tIndex < tReadHeader->GetRecordSize(); tIndex++ )
            {
                tOutputOne << tIndex << " " << tData.at< uint64_t >( tIndex ) << "\n";
            }
            if (nRecords != 0 && tRecordsPerChannel >= nRecords)
                break;
        }
        tOutputTwo << "(empty)\n";
    }
    if( (tReadHeader->GetFormatMode() == sFormatMultiInterleaved) || (tReadHeader->GetFormatMode() == sFormatMultiSeparate) )
    {
        const MonarchRecordBytes* tReadRecordOne = tReadTest->GetRecordSeparateOne();
        const MonarchRecordDataInterface tDataOne( tReadRecordOne->fData, tReadHeader->GetDataTypeSize() );
        const MonarchRecordBytes* tReadRecordTwo = tReadTest->GetRecordSeparateTwo();
        const MonarchRecordDataInterface tDataTwo( tReadRecordTwo->fData, tReadHeader->GetDataTypeSize() );
        unsigned int tRecordsPerChannel = 0;
        while( tReadTest->ReadRecord() != false )
        {
            tRecordCount = tRecordCount + 1;
            tRecordsPerChannel++;
            if( tReadRecordOne->fAcquisitionId == tAcquisitionCount )
            {
                tAcquisitionCount = tAcquisitionCount + 1;
                tOutputOne << "\n\n";
                tOutputTwo << "\n\n";
            }
            for( unsigned int tIndex = 0; tIndex < tReadHeader->GetRecordSize(); tIndex++ )
            {
                tOutputOne << tIndex << " " << tDataOne.at< uint64_t >( tIndex ) << "\n";
                tOutputTwo << tIndex << " " << tDataTwo.at< uint64_t >( tIndex ) << "\n";
            }
            if (nRecords != 0 && tRecordsPerChannel >= nRecords)
                break;
        }
    }

    MINFO( mlog, "record count <" << tRecordCount << ">" );
    MINFO( mlog, "acquisition count <" << tAcquisitionCount << ">" );

    tReadTest->Close();
    delete tReadTest;

    tOutputOne.close();
    tOutputTwo.close();

    return 0;
}

