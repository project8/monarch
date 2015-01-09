#include "M3Monarch.hh"
#include "M3Logger.hh"

#include <sstream>

using namespace monarch3;
using std::stringstream;

M3LOGGER( mlog, "Monarch3Info" );

bool PrintChannelsInt( const M3Stream* aStream );
bool PrintChannelsFloat( const M3Stream* aStream );

int main( const int argc, const char** argv )
{
    if( strcmp( argv[1], "-h" ) == 0 || argc < 2 )
    {
        M3INFO( mlog, "usage:\n"
            << "  Monarch3Info [-Hh] <input egg file>\n"
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
            tReadTest->FinishReading();
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
                switch( tStrHeader.GetDataFormat() )
                {
                    case sDigitized:
                        if( ! PrintChannelsInt( tStream ) )
                        {
                            M3ERROR( "Problem printing channels (int)" );
                            return 0;
                        }
                        break;
                    case sAnalog:
                        if( ! PrintChannelsFloat( tStream ) )
                        {
                            M3ERROR( "Problem printing channels (float)" );
                            return 0;
                        }
                        break;
                    default:
                        M3ERROR( "Invalid data format: " << tStrHeader.GetDataFormat() );
                        break;
                }
            }

        }

        tReadTest->FinishReading();
        delete tReadTest;
    }
    catch( M3Exception& e )
    {
        M3ERROR( mlog, "Exception thrown during file reading:\n" << e.what() );
    }

    return 0;
}

bool PrintChannelsInt( const M3Stream* aStream )
{
    const unsigned tMaxSamples = 30;
    unsigned tRecSize = aStream->GetChannelRecordSize();
    for( unsigned iChan = 0; iChan < aStream->GetNChannels(); ++iChan )
    {
        const M3RecordDataInterface< uint64_t > tDataInterface( aStream->GetChannelRecord( iChan )->GetData(),
                aStream->GetDataTypeSize(), sDigitized );
        stringstream tDataOut;
        for( unsigned iSample = 0; iSample < std::min( tMaxSamples, tRecSize ); ++iSample )
        {
            tDataOut << tDataInterface.at( iSample );
            if( iSample != tRecSize - 1 ) tDataOut << ", ";
        }
        if( tRecSize > tMaxSamples ) tDataOut << " . . .";
        M3INFO( mlog, "\tChannel " << iChan << ": " << tDataOut.str() );
    }
    return true;
}

bool PrintChannelsFloat( const M3Stream* aStream )
{
    const unsigned tMaxSamples = 30;
    unsigned tRecSize = aStream->GetChannelRecordSize();
    for( unsigned iChan = 0; iChan < aStream->GetNChannels(); ++iChan )
    {
        const M3RecordDataInterface< double > tDataInterface( aStream->GetChannelRecord( iChan )->GetData(),
                aStream->GetDataTypeSize(), sAnalog );
        stringstream tDataOut;
        for( unsigned iSample = 0; iSample < std::min( tMaxSamples, tRecSize ); ++iSample )
        {
            tDataOut << tDataInterface.at( iSample );
            if( iSample != tRecSize - 1 ) tDataOut << ", ";
        }
        if( tRecSize > tMaxSamples ) tDataOut << " . . .";
        M3INFO( mlog, "\tChannel " << iChan << ": " << tDataOut.str() );
    }
    return true;
}

