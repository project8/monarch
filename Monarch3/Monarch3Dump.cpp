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

    unsigned int nRecords = 1;
    if( argc >= 4 )
    {
        nRecords = atoi( argv[3] );
    }

    const Monarch* tReadTest = Monarch::OpenForReading( argv[ 1 ] );
    tReadTest->ReadHeader();
    tReadTest->SetInterface( sInterfaceSeparate );

    const MonarchHeader* tReadHeader = tReadTest->GetHeader();
    MINFO( mlog, *tReadHeader );

    unsigned int tRecordCount = 0;
    unsigned int tAcquisitionCount = 0;

    if( tReadHeader->GetFormatMode() == sFormatSingle )
    {
        ofstream tOutputOne( (string( argv[ 2 ] ) + string( "_ch1.txt" )).c_str() );
        if( tOutputOne.is_open() == false )
        {
            MERROR( mlog, "could not open channel one output file!" );
            tReadTest->Close();
            delete tReadTest;
            return -1;
        }

        const unsigned tDataTypeSize = tReadHeader->GetDataTypeSize();
        const MonarchRecordBytes* tReadRecord = tReadTest->GetRecordSeparateOne();
        const MonarchRecordDataInterface< uint64_t > tData( tReadRecord->fData, tDataTypeSize );
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
                tOutputOne << tIndex << " " << tData.at( tIndex ) << "\n";
            }
            if (nRecords != 0 && tRecordsPerChannel >= nRecords)
                break;
        }

        tOutputOne.close();
    }
    if( (tReadHeader->GetFormatMode() == sFormatMultiInterleaved) || (tReadHeader->GetFormatMode() == sFormatMultiSeparate) )
    {
        ofstream tOutputOne( (string( argv[ 2 ] ) + string( "_ch1.txt" )).c_str() );
        ofstream tOutputTwo( (string( argv[ 2 ] ) + string( "_ch2.txt" )).c_str() );
        if( tOutputOne.is_open() == false )
        {
            MERROR( mlog, "could not open channel one output file!" );
            tReadTest->Close();
            delete tReadTest;
            return -1;
        }
        if( tOutputTwo.is_open() == false )
        {
            MERROR( mlog, "could not open channel two output file!" );
            tReadTest->Close();
            delete tReadTest;
            return -1;
        }

        const unsigned tDataTypeSize = tReadHeader->GetDataTypeSize();
        const MonarchRecordBytes* tReadRecordOne = tReadTest->GetRecordSeparateOne();
        const MonarchRecordDataInterface< uint64_t > tDataOne( tReadRecordOne->fData, tDataTypeSize );
        const MonarchRecordBytes* tReadRecordTwo = tReadTest->GetRecordSeparateTwo();
        const MonarchRecordDataInterface< uint64_t > tDataTwo( tReadRecordTwo->fData , tDataTypeSize);
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
                tOutputOne << tIndex << " " << tDataOne.at( tIndex ) << "\n";
                tOutputTwo << tIndex << " " << tDataTwo.at( tIndex ) << "\n";
            }
            if (nRecords != 0 && tRecordsPerChannel >= nRecords)
                break;
        }

        tOutputOne.close();
        tOutputTwo.close();
    }

    MINFO( mlog, "record count <" << tRecordCount << ">" );
    MINFO( mlog, "acquisition count <" << tAcquisitionCount << ">" );

    tReadTest->Close();
    delete tReadTest;

    return 0;
}

