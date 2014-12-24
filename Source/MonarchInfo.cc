#include "MMonarch.hh"
#include "MLogger.hh"

using namespace monarch;

MLOGGER( mlog, "MonarchInfo" );

int main( const int argc, const char** argv )
{
    if( strcmp( argv[1], "-h" ) == 0 || argc < 2 )
    {
        MINFO( mlog, "usage:\n"
            << "  MonarchInfo [-Hh] <input egg file>\n"
            << "      -h: print this usage information\n"
            << "      -H: (optional) header only; does not check number of records" );
        return -1;
    }

    unsigned tFileArg = 1;
    bool tCheckRecords = true;
    if( strcmp( argv[1], "-H" ) == 0 )
    {
        if( argc < 3 )
        {
            MERROR( mlog, "no filename provided" );
            return -1;
        }
        ++tFileArg;
        tCheckRecords = false;
    }

    const Monarch* tReadTest = Monarch::OpenForReading( argv[tFileArg] );
    tReadTest->ReadHeader();

    const MHeader* tReadHeader = tReadTest->GetHeader();
    MINFO( mlog, *tReadHeader );

    if( ! tCheckRecords )
    {
        tReadTest->Close();
        delete tReadTest;
        return 0;
    }

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

    tReadTest->Close();
    delete tReadTest;

    return 0;
}
