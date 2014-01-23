/*
 * MonarchTimeCheck.cpp
 *
 *  Created on: May 16, 2013
 *      Author: nsoblath
 */

#include "Monarch.hpp"
#include "MonarchLogger.hpp"

#include <fstream>
using std::ofstream;

using namespace monarch;

MLOGGER( mlog, "MonarchTimeCheck" );

int main( const int argc, const char** argv )
{
    if( argc < 3 )
    {
        MINFO( mlog, "usage:\n"
            << "  MonarchTimeCheck <input egg file> <output text file>" );
        return -1;
    }

    ofstream tOutput( argv[ 2 ] );
    if( tOutput.is_open() == false )
    {
        MERROR( mlog, "could not open output file!" );
        return -1;
    }

    const Monarch* tReadTest = Monarch::OpenForReading( argv[1] );
    tReadTest->ReadHeader();

    const MonarchHeader* tReadHeader = tReadTest->GetHeader();
    cout << *tReadHeader << endl;

    TimeType tRecordSize = (TimeType)tReadHeader->GetRecordSize();
    TimeType tBinWidthNS = (TimeType)(1000. / tReadHeader->GetAcquisitionRate()); // in ns

    const MonarchRecord* tReadRecord;
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

    unsigned long long tRecordCount = 0;
    unsigned long long tAcquisitionCount = 0;

    TimeType tNSTimeInRunClock = 0;
    TimeType tNSTimeInRunBins = 0;
    TimeType tNSTimeInRunBinsCorr = 0; // corrected to the Clock time when there's a new acquisition

    // read first record
    if (! tReadTest->ReadRecord())
    {
        MERROR( mlog, "No records in the file" );
        return -1;
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
            cout << "new acquisition" << endl;
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
    MINFO( mlog, "record count <" << tRecordCount << ">" );
    MINFO( mlog, "acquisition count <" << tAcquisitionCount << ">" );

    tReadTest->Close();
    delete tReadTest;

    tOutput.close();

    return 0;
}


