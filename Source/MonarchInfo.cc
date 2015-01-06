#include "MMonarch.hh"
#include "MLogger.hh"

#include <sstream>

using namespace monarch;
using std::stringstream;

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

    try
    {
        MINFO( mlog, "Opening file <" << argv[tFileArg] );
        const Monarch* tReadTest = Monarch::OpenForReading( argv[tFileArg] );

        MINFO( mlog, "Reading header" );
        tReadTest->ReadHeader();

        const MHeader* tReadHeader = tReadTest->GetHeader();
        MINFO( mlog, *tReadHeader );

        if( ! tCheckRecords )
        {
            tReadTest->Close();
            delete tReadTest;
            return 0;
        }

        MINFO( mlog, "Reading data" );

        unsigned tNStreams = tReadHeader->GetNStreams();
        for( unsigned iStream = 0; iStream < tNStreams; ++iStream )
        {
            const MStreamHeader& tStrHeader = tReadHeader->GetStreamHeaders().at( iStream );
            unsigned tNChannels = tStrHeader.GetNChannels();
            unsigned tRecSize = tStrHeader.GetRecordSize();
            MINFO( mlog, "Stream " << iStream << " has " << tNChannels << " channel(s) stored in format mode " << tStrHeader.GetChannelFormat() );

            const MStream* tStream = tReadTest->GetStream( iStream );

            if( tStream->GetNAcquisitions() == 0 )
            {
                MINFO( mlog, "\tThis stream has no acquisitions" );
                continue;
            }

            const unsigned tMaxRecords = 2;
            for( unsigned iRec = 0; iRec < tMaxRecords; ++iRec )
            {
                tStream->ReadRecord();

                const unsigned tMaxSamples = 30;
                for( unsigned iChan = 0; iChan < tNChannels; ++iChan )
                {
                    const byte_type* tData = tStream->GetChannelRecord( iChan )->GetData();
                    stringstream tDataOut;
                    for( unsigned iSample = 0; iSample < std::min( tMaxSamples, tRecSize ); ++iSample )
                    {
                        tDataOut << (unsigned)tData[ iSample ];
                        if( iSample != tRecSize - 1 ) tDataOut << ", ";
                    }
                    if( tRecSize > tMaxSamples ) tDataOut << " . . .";
                    MINFO( mlog, "\tChannel " << iChan << ": " << tDataOut.str() );
                }
            }

        }

        tReadTest->Close();
        delete tReadTest;
    }
    catch( MException& e )
    {
        MERROR( mlog, "Exception thrown during file reading:\n" << e.what() );
    }

    return 0;
}
