#ifdef _WIN32
#define NOMINMAX
#endif

#include "M3DataInterface.hh"
#include "M3Monarch.hh"
#include "logger.hh"
#include "M3Record.hh"

#include <algorithm>
#include <cstring> // for strcmp
#include <sstream>

using namespace monarch3;
using std::stringstream;

LOGGER( mlog, "Monarch3Info" );

bool PrintChannelsUInt( const M3Stream* aStream );
bool PrintChannelsInt( const M3Stream* aStream );
bool PrintChannelsFloat( const M3Stream* aStream );
bool PrintChannelsFloatComplex( const M3Stream* aStream );

int main( const int argc, const char** argv )
{
    if( argc < 2 || strcmp( argv[1], "-h" ) == 0 )
    {
        INFO( mlog, "usage:\n"
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
            ERROR( mlog, "no filename provided" );
            return -1;
        }
        ++tFileArg;
        tCheckRecords = false;
    }

    try
    {
        INFO( mlog, "Opening file <" << argv[tFileArg] << ">" );
        const Monarch3* tReadTest = Monarch3::OpenForReading( argv[tFileArg] );

        INFO( mlog, "Reading header" );
        tReadTest->ReadHeader();

        const M3Header* tReadHeader = tReadTest->GetHeader();
        INFO( mlog, *tReadHeader );

        if( ! tCheckRecords )
        {
            tReadTest->FinishReading();
            delete tReadTest;
            return 0;
        }

        INFO( mlog, "Reading data" );

        unsigned tNStreams = tReadHeader->GetNStreams();
        for( unsigned iStream = 0; iStream < tNStreams; ++iStream )
        {
            const M3StreamHeader& tStrHeader = tReadHeader->GetStreamHeaders().at( iStream );
            unsigned tNChannels = tStrHeader.GetNChannels();
            //unsigned tRecSize = tStrHeader.GetRecordSize();
            INFO( mlog, "Stream " << iStream << " has " << tNChannels << " channel(s) stored in format mode " << tStrHeader.GetChannelFormat() );

            const M3Stream* tStream = tReadTest->GetStream( iStream );

            if( tStream->GetNAcquisitions() == 0 )
            {
                INFO( mlog, "\tThis stream has no acquisitions" );
                continue;
            }

            const unsigned tMaxRecords = 2;
            for( unsigned iRec = 0; iRec < tMaxRecords; ++iRec )
            {
                if( ! tStream->ReadRecord() )
                {
                    ERROR( mlog, "There was a problem reading a record from this stream" );
                    break;
                }
                switch( tStrHeader.GetDataFormat() )
                {
                    case sDigitizedUS:
                        if( ! PrintChannelsUInt( tStream ) )
                        {
                            ERROR( mlog, "Problem printing channels (int)" );
                            return 0;
                        }
                        break;
                    case sDigitizedS:
                        if( ! PrintChannelsInt( tStream ) )
                        {
                            ERROR( mlog, "Problem printing channels (int)" );
                            return 0;
                        }
                        break;
                    case sAnalog:
                        switch( tStream->GetSampleSize() )
                        {
                            case 1:
                                if( ! PrintChannelsFloat( tStream ) )
                                {
                                    ERROR( mlog, "Problem printing channels (float)" );
                                    return 0;
                                }
                                break;
                            default:
                                if( ! PrintChannelsFloatComplex( tStream ) )
                                {
                                    ERROR( mlog, "Problem printing channels (float-complex)" );
                                    return 0;
                                }
                                break;
                        }
                        break;
                    default:
                        ERROR( mlog, "Invalid data format: "<<tStrHeader.GetDataFormat() );
                        break;
                }
            }

        }

        tReadTest->FinishReading();
        delete tReadTest;
    }
    catch( M3Exception& e )
    {
        ERROR( mlog, "Exception thrown during file reading:\n" << e.what() );
    }

    return 0;
}

bool PrintChannelsUInt( const M3Stream* aStream )
{
    const unsigned tMaxSamples = 30;
    unsigned tRecSize = aStream->GetChannelRecordSize();
    for( unsigned iChan = 0; iChan < aStream->GetNChannels(); ++iChan )
    {
        const M3DataReader< uint64_t > tDataInterface( aStream->GetChannelRecord( iChan )->GetData(),
                aStream->GetDataTypeSize(), sDigitizedUS );
        stringstream tDataOut;
        for( unsigned iSample = 0; iSample < std::min( tMaxSamples, tRecSize ); ++iSample )
        {
            tDataOut << tDataInterface.at( iSample );
            if( iSample != tRecSize - 1 ) tDataOut << ", ";
        }
        if( tRecSize > tMaxSamples ) tDataOut << " . . .";
        INFO( mlog, "\tChannel " << iChan << ": " << tDataOut.str() );
    }
    return true;
}

bool PrintChannelsInt( const M3Stream* aStream )
{
    const unsigned tMaxSamples = 30;
    unsigned tRecSize = aStream->GetChannelRecordSize();
    for( unsigned iChan = 0; iChan < aStream->GetNChannels(); ++iChan )
    {
        const M3DataReader< int64_t > tDataInterface( aStream->GetChannelRecord( iChan )->GetData(),
                aStream->GetDataTypeSize(), sDigitizedS );
        stringstream tDataOut;
        for( unsigned iSample = 0; iSample < std::min( tMaxSamples, tRecSize ); ++iSample )
        {
            tDataOut << tDataInterface.at( iSample );
            if( iSample != tRecSize - 1 ) tDataOut << ", ";
        }
        if( tRecSize > tMaxSamples ) tDataOut << " . . .";
        INFO( mlog, "\tChannel " << iChan << ": " << tDataOut.str() );
    }
    return true;
}

bool PrintChannelsFloat( const M3Stream* aStream )
{
    const unsigned tMaxSamples = 30;
    unsigned tRecSize = aStream->GetChannelRecordSize();
    for( unsigned iChan = 0; iChan < aStream->GetNChannels(); ++iChan )
    {
        const M3DataReader< double > tDataInterface( aStream->GetChannelRecord( iChan )->GetData(),
                aStream->GetDataTypeSize(), sAnalog );
        stringstream tDataOut;
        for( unsigned iSample = 0; iSample < std::min( tMaxSamples, tRecSize ); ++iSample )
        {
            tDataOut << tDataInterface.at( iSample );
            if( iSample != tRecSize - 1 ) tDataOut << ", ";
        }
        if( tRecSize > tMaxSamples ) tDataOut << " . . .";
        INFO( mlog, "\tChannel " << iChan << ": " << tDataOut.str() );
    }
    return true;
}

bool PrintChannelsFloatComplex( const M3Stream* aStream )
{
    // NOTE: this implementation is specific to f4_complex and f8_complex, and therefore assumes only 2 elements in each sample
    const unsigned tMaxSamples = 30;
    unsigned tRecSize = aStream->GetChannelRecordSize();
    switch( aStream->GetDataTypeSize() )
    {
        case 4:
            for( unsigned iChan = 0; iChan < aStream->GetNChannels(); ++iChan )
            {
                const M3ComplexDataReader< f4_complex > tDataInterface( aStream->GetChannelRecord( iChan )->GetData(),
                        aStream->GetDataTypeSize(), sAnalog, aStream->GetSampleSize() );
                stringstream tDataOut;
                for( unsigned iSample = 0; iSample < std::min( tMaxSamples, tRecSize ); ++iSample )
                {
                    tDataOut << "(" << tDataInterface.at( iSample )[ 0 ] << ", " << tDataInterface.at( iSample )[ 1 ] << ")";
                    if( iSample != tRecSize - 1 ) tDataOut << ", ";
                }
                if( tRecSize > tMaxSamples ) tDataOut << " . . .";
                INFO( mlog, "\tChannel " << iChan << ": " << tDataOut.str() );
            }
            break;
        case 8:
            for( unsigned iChan = 0; iChan < aStream->GetNChannels(); ++iChan )
            {
                const M3ComplexDataReader< f8_complex > tDataInterface( aStream->GetChannelRecord( iChan )->GetData(),
                        aStream->GetDataTypeSize(), sAnalog, aStream->GetSampleSize() );
                stringstream tDataOut;
                for( unsigned iSample = 0; iSample < std::min( tMaxSamples, tRecSize ); ++iSample )
                {
                    tDataOut << "(" << tDataInterface.at( iSample )[ 0 ] << ", " << tDataInterface.at( iSample )[ 1 ] << ")";
                    if( iSample != tRecSize - 1 ) tDataOut << ", ";
                }
                if( tRecSize > tMaxSamples ) tDataOut << " . . .";
                INFO( mlog, "\tChannel " << iChan << ": " << tDataOut.str() );
            }
            break;
        default:
            ERROR( mlog, "Cannot print channels for complex floating-point data with type size " << aStream->GetDataTypeSize() );
            return false;
            break;
    }
    return true;
}

