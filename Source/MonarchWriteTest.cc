#include "MMonarch.hh"
#include "MLogger.hh"

using namespace monarch;

MLOGGER( mlog, "MonarchWriteTest" );

int main( const int argc, const char** argv )
{
    if( strcmp( argv[1], "-h" ) == 0 || argc < 2 )
    {
        MINFO( mlog, "usage:\n"
            << "  MonarchInfo [-h] <output egg file>\n"
            << "      -h: print this usage information" );
        return -1;
    }

    Monarch* tWriteTest = Monarch::OpenForWriting( argv[1] );

    MHeader* tHeader = tWriteTest->GetHeader();
    tHeader->SetSchemaVersion( "???" );
    tHeader->SetFilename( argv[1] );
    tHeader->SetRunDuration( 8675309 );
    tHeader->SetTimestamp( "Stardate 33515" );
    tHeader->SetDescription( "Bigger on the inside" );

    MINFO( mlog, "Adding streams" );
    unsigned tSingleStream = tHeader->AddStream( "HHGTTG", 500, 42, 1, sDigitized, 8 );
    unsigned tDoubleStream = tHeader->AddStream( "Um . . .", 2, 250, 500, 1, sDigitized, 8 );

    tWriteTest->WriteHeader();

    MINFO( mlog, "Wrote header:\n" << *tHeader );

    /*
    unsigned int tRecordCount = 0;
    unsigned int tAcquisiontCount = 0;
    const MonarchRecordBytes* tReadRecord;
    // the FormatMode is ignored for single-channel data
    if( tReadHeader->GetAcquisitionMode() == 1 )
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
        MERROR( mlog, "Unable to read a header with acquisition mode <" << tReadHeader->GetAcquisitionMode() << "> and format mode <" << tReadHeader->GetFormatMode() << ">" );
        return -1;
    }
    try
    {
        while( tReadTest->ReadRecord() != false )
        {
            tRecordCount = tRecordCount + 1;
            if( tReadRecord->fAcquisitionId == tAcquisiontCount )
            {
                tAcquisiontCount = tAcquisiontCount + 1;
            }
            //cout << "  record " << tRecordCount << ": time offset: " << tReadRecord->fTime << " ns" << endl;
        }
    }
    catch (MonarchException& e)
    {
        MWARN( mlog, "Something went wrong during the reading of records!" << "\n\t" << e.what() );
    }
    MINFO( mlog, "record count <" << tRecordCount << ">" );
    MINFO( mlog, "acquisition count <" << tAcquisiontCount << ">" );
*/

    tWriteTest->Close();
    MINFO( mlog, "File closed" );

    delete tWriteTest;

    return 0;
}
