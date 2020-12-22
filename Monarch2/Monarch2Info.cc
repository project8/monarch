#include "M2Monarch.hh"

#include "application.hh"
#include "logger.hh"

#include <cstring>
#include <memory>

using namespace monarch2;

LOGGER( mlog, "Monarch2Info" );

int main( const int argc, const char** argv )
{
    scarab::main_app theMain( true );

    bool tHeaderOnly;
    std::string tFilename;

    theMain.add_flag( "-H,--header-only", tHeaderOnly, "Only look at header information; does not check number of records" );
    theMain.add_option( "Filename", tFilename, "Input egg file" )->required();

    CLI11_PARSE( theMain, argc, argv );

    std::shared_ptr< const Monarch2 > tReadTest( Monarch2::OpenForReading( tFilename ) );
    tReadTest->ReadHeader();

    const M2Header* tReadHeader = tReadTest->GetHeader();
    LINFO( mlog, *tReadHeader );

    if( tHeaderOnly )
    {
        tReadTest->Close();
        return RETURN_SUCCESS;
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
        LERROR( mlog, "Unable to read a header with acquisition mode <" << tReadHeader->GetAcquisitionMode() << "> and format mode <" << tReadHeader->GetFormatMode() << ">" );
        return RETURN_ERROR;
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
        LWARN( mlog, "Something went wrong during the reading of records!" << "\n\t" << e.what() );
        return RETURN_ERROR;
    }
    LINFO( mlog, "record count <" << tRecordCount << ">" );
    LINFO( mlog, "acquisition count <" << tAcquisiontCount << ">" );

    tReadTest->Close();

    return RETURN_SUCCESS;
}
