/*
 * MonarchTimeCheck.cpp
 *
 *  Created on: May 16, 2013
 *      Author: nsoblath
 */

#include "M2Monarch.hh"

#include "application.hh"
#include "logger.hh"
#include "macros.hh"

#include <fstream>
using std::ofstream;

using namespace monarch2;

LOGGER( mlog, "Monarch2TimeCheck" );

int main( const int argc, const char** argv )
{
    scarab::main_app theMain( false );

    std::string tInputFilename, tOutputFilename;

    theMain.add_option( "InputFilename", tInputFilename, "Input egg file" )->required();
    theMain.add_option( "OutputFilename", tOutputFilename, "Output text file" )->required();

    CLI11_PARSE( theMain, argc, argv );

    ofstream tOutput( tOutputFilename );
    if( tOutput.is_open() == false )
    {
        LERROR( mlog, "could not open output file!" );
        STOP_LOGGING;
        return RETURN_ERROR;
    }

    std::shared_ptr< const Monarch2 > tReadTest( Monarch2::OpenForReading( tInputFilename ) );
    tReadTest->ReadHeader();

    const M2Header* tReadHeader = tReadTest->GetHeader();
    LINFO( mlog, *tReadHeader );

    TimeType tRecordSize = (TimeType)tReadHeader->GetRecordSize();
    TimeType tBinWidthNS = (TimeType)(1000. / tReadHeader->GetAcquisitionRate()); // in ns

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
        STOP_LOGGING;
        return RETURN_ERROR;
    }

    unsigned long long tRecordCount = 0;
    unsigned long long tAcquisitionCount = 0;

    TimeType tNSTimeInRunClock = 0;
    TimeType tNSTimeInRunBins = 0;
    TimeType tNSTimeInRunBinsCorr = 0; // corrected to the Clock time when there's a new acquisition

    // read first record
    if (! tReadTest->ReadRecord())
    {
        LERROR( mlog, "No records in the file" );
        STOP_LOGGING;
        return RETURN_ERROR;
    }
    tRecordCount = 1;
    tAcquisitionCount = 1;
    tNSTimeInRunClock = tReadRecord->fTime;
    tNSTimeInRunBinsCorr = tNSTimeInRunClock;
    tNSTimeInRunBins = tNSTimeInRunClock;

    tOutput << tRecordCount << '\t' << tNSTimeInRunClock << '\t' << tNSTimeInRunBins << '\t' << tNSTimeInRunBinsCorr << '\n';

    while( tReadTest->ReadRecord() != false )
    {
        tRecordCount = tRecordCount + 1;

        tNSTimeInRunClock = tReadRecord->fTime;

        if( tReadRecord->fAcquisitionId == tAcquisitionCount )
        {
            tAcquisitionCount = tAcquisitionCount + 1;
            tNSTimeInRunBinsCorr = tNSTimeInRunClock;
        }
        else
        {
            tNSTimeInRunBinsCorr += tRecordSize * tBinWidthNS;
        }
        tNSTimeInRunBins += tRecordSize * tBinWidthNS;

        tOutput << tRecordCount << '\t' << tNSTimeInRunClock << '\t' << tNSTimeInRunBins << '\t' << tNSTimeInRunBinsCorr << '\n';

        //cout << "  record " << tRecordCount << ": time offset: " << tReadRecord->fTime << " ns" << endl;
    }
    LINFO( mlog, "record count <" << tRecordCount << ">" );
    LINFO( mlog, "acquisition count <" << tAcquisitionCount << ">" );

    tReadTest->Close();

    tOutput.close();

    STOP_LOGGING;
    return RETURN_SUCCESS;
}


