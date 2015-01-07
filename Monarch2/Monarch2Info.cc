#include "M2Monarch.hh"
#include "M2Logger.hh"

#include <cstring>

using namespace monarch2;

M2LOGGER( mlog, "MonarchInfo" );

int main( const int argc, const char** argv )
{
    if( argc < 2 )
    {
        M2INFO( mlog, "usage:\n"
            << "  MonarchInfo [-h] <input egg file>\n"
            << "      -h: (optional) header only; does not check number of records" );
        return -1;
    }

    unsigned tFileArg = 1;
    bool tCheckRecords = true;
    if( strcmp( argv[1], "-h" ) == 0 )
    {
        if( argc < 3 )
        {
            M2ERROR( mlog, "no filename provided" );
            return -1;
        }
        ++tFileArg;
        tCheckRecords = false;
    }

    const Monarch2* tReadTest = Monarch2::OpenForReading( argv[tFileArg] );
    tReadTest->ReadHeader();

    const M2Header* tReadHeader = tReadTest->GetHeader();
    M2INFO( mlog, *tReadHeader );

    if( ! tCheckRecords )
    {
        tReadTest->Close();
        delete tReadTest;
        return 0;
    }

    unsigned int tRecordCount = 0;
    unsigned int tAcquisiontCount = 0;
    const M2RecordBytes* tReadRecord;
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
        M2ERROR( mlog, "Unable to read a header with acquisition mode <" << tReadHeader->GetAcquisitionMode() << "> and format mode <" << tReadHeader->GetFormatMode() << ">" );
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
    catch (M2Exception& e)
    {
        M2WARN( mlog, "Something went wrong during the reading of records!" << "\n\t" << e.what() );
    }
    M2INFO( mlog, "record count <" << tRecordCount << ">" );
    M2INFO( mlog, "acquisition count <" << tAcquisiontCount << ">" );

    tReadTest->Close();
    delete tReadTest;

    return 0;
}
