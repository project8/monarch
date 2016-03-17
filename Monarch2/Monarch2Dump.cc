#include "M2Monarch.hh"
#include "logger.hh"

#include <cstdlib>

#include <fstream>
using std::ofstream;

using namespace monarch2;

LOGGER( mlog, "Monarch2Dump" );

int main( const int argc, const char** argv )
{
    if( argc < 3 )
    {
        INFO( mlog, "usage:\n"
            << "  Monarch2Dump <input egg file> <output text file> <# of records per channel [optional]>\n"
            << "# of records is optional; the default is 1; use 0 to dump the whole file" );
        return -1;
    }

    unsigned int nRecords = 1;
    if( argc >= 4 )
    {
        nRecords = atoi( argv[3] );
    }

    const Monarch2* tReadTest = Monarch2::OpenForReading( argv[ 1 ] );
    tReadTest->ReadHeader();
    tReadTest->SetInterface( sInterfaceSeparate );

    const M2Header* tReadHeader = tReadTest->GetHeader();
    INFO( mlog, *tReadHeader );

    unsigned int tRecordCount = 0;
    unsigned int tAcquisitionCount = 0;

    if( tReadHeader->GetFormatMode() == sFormatSingle )
    {
        ofstream tOutputOne( (std::string( argv[ 2 ] ) + std::string( "_ch1.txt" )).c_str() );
        if( tOutputOne.is_open() == false )
        {
            ERROR( mlog, "could not open channel one output file!" );
            tReadTest->Close();
            delete tReadTest;
            return -1;
        }

        const unsigned tDataTypeSize = tReadHeader->GetDataTypeSize();
        const M2RecordBytes* tReadRecord = tReadTest->GetRecordSeparateOne();
        const M2RecordDataInterface< uint64_t > tData( tReadRecord->fData, tDataTypeSize );
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
        ofstream tOutputOne( (std::string( argv[ 2 ] ) + std::string( "_ch1.txt" )).c_str() );
        ofstream tOutputTwo( (std::string( argv[ 2 ] ) + std::string( "_ch2.txt" )).c_str() );
        if( tOutputOne.is_open() == false )
        {
            ERROR( mlog, "could not open channel one output file!" );
            tReadTest->Close();
            delete tReadTest;
            return -1;
        }
        if( tOutputTwo.is_open() == false )
        {
            ERROR( mlog, "could not open channel two output file!" );
            tReadTest->Close();
            delete tReadTest;
            return -1;
        }

        const unsigned tDataTypeSize = tReadHeader->GetDataTypeSize();
        const M2RecordBytes* tReadRecordOne = tReadTest->GetRecordSeparateOne();
        const M2RecordDataInterface< uint64_t > tDataOne( tReadRecordOne->fData, tDataTypeSize );
        const M2RecordBytes* tReadRecordTwo = tReadTest->GetRecordSeparateTwo();
        const M2RecordDataInterface< uint64_t > tDataTwo( tReadRecordTwo->fData , tDataTypeSize);
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

    INFO( mlog, "record count <" << tRecordCount << ">" );
    INFO( mlog, "acquisition count <" << tAcquisitionCount << ">" );

    tReadTest->Close();
    delete tReadTest;

    return 0;
}

