#include "M2Monarch.hh"

#include "application.hh"
#include "logger.hh"

#include <cstdlib>

#include <fstream>
using std::ofstream;

using namespace monarch2;

LOGGER( mlog, "Monarch2Dump" );

int main( const int argc, const char** argv )
{
    scarab::main_app theMain( false );

    unsigned tNRecords;
    std::string tInputFilename, tOutputFilebase;

    theMain.add_option( "-n,--n-records", tNRecords, "Number of records to dump; use 0 to dump the whole file" )->default_val(0);
    theMain.add_option( "InputFilename", tInputFilename, "Input egg file" )->required();
    theMain.add_option( "OutputFilename", tOutputFilebase, "Filename base for output files (will have ch[#].txt appended)" )->required();

    CLI11_PARSE( theMain, argc, argv );

    std::shared_ptr< const Monarch2 > tReadTest( Monarch2::OpenForReading( tInputFilename ) );
    tReadTest->ReadHeader();
    tReadTest->SetInterface( sInterfaceSeparate );

    const M2Header* tReadHeader = tReadTest->GetHeader();
    LINFO( mlog, *tReadHeader );

    unsigned int tRecordCount = 0;
    unsigned int tAcquisitionCount = 0;

    if( tReadHeader->GetFormatMode() == sFormatSingle )
    {
        ofstream tOutputOne( (tOutputFilebase + std::string( "_ch1.txt" )).c_str() );
        if( tOutputOne.is_open() == false )
        {
            LERROR( mlog, "could not open channel one output file!" );
            tReadTest->Close();
            return RETURN_ERROR;
        }

        const unsigned tDataTypeSize = tReadHeader->GetDataTypeSize();
        const M2RecordBytes* tReadRecord = tReadTest->GetRecordSeparateOne();
        const M2RecordDataInterface< uint64_t > tData( tReadRecord->fData, tDataTypeSize );
        unsigned int tRecordsPerChannel = 0;
        while( tReadTest->ReadRecord() != false )
        {
            ++tRecordCount;
            ++tRecordsPerChannel;
            if( tReadRecord->fAcquisitionId == tAcquisitionCount )
            {
                tAcquisitionCount = tAcquisitionCount + 1;
                tOutputOne << "\n\n";
            }
            for( unsigned int tIndex = 0; tIndex < tReadHeader->GetRecordSize(); tIndex++ )
            {
                tOutputOne << tIndex << " " << tData.at( tIndex ) << "\n";
            }
            if (tNRecords != 0 && tRecordsPerChannel >= tNRecords) break;
        }

        tOutputOne.close();
    }
    if( (tReadHeader->GetFormatMode() == sFormatMultiInterleaved) || (tReadHeader->GetFormatMode() == sFormatMultiSeparate) )
    {
        ofstream tOutputOne( (tOutputFilebase + std::string( "_ch1.txt" )).c_str() );
        ofstream tOutputTwo( (tOutputFilebase + std::string( "_ch2.txt" )).c_str() );
        if( tOutputOne.is_open() == false )
        {
            LERROR( mlog, "could not open channel one output file!" );
            tReadTest->Close();
            return RETURN_ERROR;
        }
        if( tOutputTwo.is_open() == false )
        {
            LERROR( mlog, "could not open channel two output file!" );
            tReadTest->Close();
            return RETURN_ERROR;
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
            ++tRecordsPerChannel;
            if( tReadRecordOne->fAcquisitionId == tAcquisitionCount )
            {
                tAcquisitionCount = tAcquisitionCount + 1;
                tOutputOne << "\n\n";
                tOutputTwo << "\n\n";
            }
            for( unsigned int tIndex = 0; tIndex < tReadHeader->GetRecordSize(); ++tIndex )
            {
                tOutputOne << tIndex << " " << tDataOne.at( tIndex ) << "\n";
                tOutputTwo << tIndex << " " << tDataTwo.at( tIndex ) << "\n";
            }
            if (tNRecords != 0 && tRecordsPerChannel >= tNRecords) break;
        }

        tOutputOne.close();
        tOutputTwo.close();
    }

    LINFO( mlog, "record count <" << tRecordCount << ">" );
    LINFO( mlog, "acquisition count <" << tAcquisitionCount << ">" );

    tReadTest->Close();

    return RETURN_SUCCESS;
}

