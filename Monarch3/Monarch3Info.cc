#include "M3Monarch.hh"
#include "M3Logger.hh"

#include <sstream>

using namespace monarch3;
using std::stringstream;

M3LOGGER( mlog, "Monarch3Info" );

int main( const int argc, const char** argv )
{
    if( strcmp( argv[1], "-h" ) == 0 || argc < 2 )
    {
        M3INFO( mlog, "usage:\n"
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
            M3ERROR( mlog, "no filename provided" );
            return -1;
        }
        ++tFileArg;
        tCheckRecords = false;
    }

    try
    {
        M3INFO( mlog, "Opening file <" << argv[tFileArg] );
        const Monarch3* tReadTest = Monarch3::OpenForReading( argv[tFileArg] );

        M3INFO( mlog, "Reading header" );
        tReadTest->ReadHeader();

        const M3Header* tReadHeader = tReadTest->GetHeader();
        M3INFO( mlog, *tReadHeader );

        if( ! tCheckRecords )
        {
            tReadTest->Close();
            delete tReadTest;
            return 0;
        }

        M3INFO( mlog, "Reading data" );

        unsigned tNStreams = tReadHeader->GetNStreams();
        for( unsigned iStream = 0; iStream < tNStreams; ++iStream )
        {
            const M3StreamHeader& tStrHeader = tReadHeader->GetStreamHeaders().at( iStream );
            unsigned tNChannels = tStrHeader.GetNChannels();
            unsigned tRecSize = tStrHeader.GetRecordSize();
            M3INFO( mlog, "Stream " << iStream << " has " << tNChannels << " channel(s) stored in format mode " << tStrHeader.GetChannelFormat() );

            const M3Stream* tStream = tReadTest->GetStream( iStream );

            if( tStream->GetNAcquisitions() == 0 )
            {
                M3INFO( mlog, "\tThis stream has no acquisitions" );
                continue;
            }

            const unsigned tMaxRecords = 2;
            for( unsigned iRec = 0; iRec < tMaxRecords; ++iRec )
            {
                tStream->ReadRecord();

                const unsigned tMaxSamples = 30;
                for( unsigned iChan = 0; iChan < tNChannels; ++iChan )
                {
                    const M3RecordDataInterface< uint64_t > tDataInterface( tStream->GetChannelRecord( iChan )->GetData(),
                            tStream->GetDataTypeSize(), tStrHeader.GetDataFormat() );
                    stringstream tDataOut;
                    for( unsigned iSample = 0; iSample < std::min( tMaxSamples, tRecSize ); ++iSample )
                    {
                        tDataOut << tDataInterface.at( iSample );
                        if( iSample != tRecSize - 1 ) tDataOut << ", ";
                    }
                    if( tRecSize > tMaxSamples ) tDataOut << " . . .";
                    M3INFO( mlog, "\tChannel " << iChan << ": " << tDataOut.str() );
                }
            }

        }

        tReadTest->Close();
        delete tReadTest;
    }
    catch( M3Exception& e )
    {
        M3ERROR( mlog, "Exception thrown during file reading:\n" << e.what() );
    }

    return 0;
}
