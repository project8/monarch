#include "Monarch.hpp"
#include "MonarchLogger.hpp"

using namespace monarch;

MLOGGER( mlog, "MonarchInfo" );

int main( const int argc, const char** argv )
{
    if( argc < 2 )
    {
        MINFO( mlog, "usage:\n"
            << "  MonarchInfo <input egg file>" );
        return -1;
    }

    const Monarch* tReadTest = Monarch::OpenForReading( argv[1] );
    tReadTest->ReadHeader();

    const MonarchHeader* tReadHeader = tReadTest->GetHeader();
    MINFO( mlog, *tReadHeader );

    unsigned int tRecordCount = 0;
    unsigned int tAcquisiontCount = 0;
    const MonarchRecordBytes* tReadRecord;
    if( tReadHeader->GetAcquisitionMode() == 1 /* the FormatMode is ignored for single-channel data */ )
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

    tReadTest->Close();
    delete tReadTest;

    return 0;
}
