#ifdef _WIN32
#define NOMINMAX
#endif

#include "M3DataInterface.hh"
#include "M3Monarch.hh"
#include "M3Record.hh"

#include "logger.hh"

#include <algorithm>
#include <cstring> // for strcmp
#include <sstream>

using namespace monarch3;
using std::stringstream;

LOGGER( mlog, "M3Info" );

template< typename XDataType >
bool PrintChannelsReal( const M3Stream* aStream, uint32_t aDataFormat );

template< typename XDataType >
bool PrintChannelsComplex( const M3Stream* aStream, uint32_t aDataFormat );

int main( const int argc, const char** argv )
{
    if( argc < 2 || strcmp( argv[1], "-h" ) == 0 )
    {
        LPROG( mlog, "usage:\n"
            << "  M3Info [-Hh] <input egg file>\n"
            << "      -h: print this usage information\n"
            << "      -H: (optional) header only; does not check number of records" );
        return 0;
    }

    unsigned tFileArg = 1;
    bool tCheckRecords = true;
    if( strcmp( argv[1], "-H" ) == 0 )
    {
        if( argc < 3 )
        {
            LERROR( mlog, "no filename provided" );
            return 1;
        }
        ++tFileArg;
        tCheckRecords = false;
    }

    try
    {
        LPROG( mlog, "Opening file <" << argv[tFileArg] << ">" );
        const Monarch3* tReadTest = Monarch3::OpenForReading( argv[tFileArg] );

        LPROG( mlog, "Reading header" );
        tReadTest->ReadHeader();

        const M3Header* tReadHeader = tReadTest->GetHeader();
        LPROG( mlog, *tReadHeader );

        if( ! tCheckRecords )
        {
            tReadTest->FinishReading();
            delete tReadTest;
            return 0;
        }

        LPROG( mlog, "Reading data" );

        unsigned tNStreams = tReadHeader->GetNStreams();
        for( unsigned iStream = 0; iStream < tNStreams; ++iStream )
        {
            const M3StreamHeader& tStrHeader = tReadHeader->StreamHeaders().at( iStream );
            unsigned tNChannels = tStrHeader.GetNChannels();
            //unsigned tRecSize = tStrHeader.GetRecordSize();
            LPROG( mlog, "Stream " << iStream << " has " << tNChannels << " channel(s) stored in format mode " << tStrHeader.GetChannelFormat() );

            const M3Stream* tStream = tReadTest->GetStream( iStream );

            if( tStream->GetNAcquisitions() == 0 )
            {
                LPROG( mlog, "\tThis stream has no acquisitions" );
                continue;
            }

            const unsigned tMaxRecords = 2;
            for( unsigned iRec = 0; iRec < tMaxRecords; ++iRec )
            {
                if( ! tStream->ReadRecord() )
                {
                    throw M3Exception() << "There was a problem reading a record from this stream";
                }
                switch( tStrHeader.GetDataFormat() )
                {
                    case sDigitizedUS:
                        if( ! PrintChannelsReal< uint64_t >( tStream, sDigitizedUS ) )
                        {
                            throw M3Exception() << "Problem printing channels (int)";
                        }
                        break;
                    case sDigitizedS:
                        if( ! PrintChannelsReal< int64_t >( tStream, sDigitizedS ) )
                        {
                            throw M3Exception() << "Problem printing channels (int)";
                        }
                        break;
                    case sAnalog:
                        switch( tStream->GetSampleSize() )
                        {
                            case 1:
                                if( ! PrintChannelsReal< double >( tStream, sAnalog ) )
                                {
                                    throw M3Exception() << "Problem printing channels (float)" ;
                                }
                                break;
                            case 2:
                                switch( tStream->GetDataTypeSize() )
                                {
                                    case 4:
                                        if( ! PrintChannelsComplex< f4_complex >( tStream, sAnalog ) )
                                        {
                                            throw M3Exception() << "Problem printing channels (float-complex)" ;
                                        }
                                        break;
                                    case 8:
                                        if( ! PrintChannelsComplex< f8_complex >( tStream, sAnalog ) )
                                        {
                                            throw M3Exception() << "Problem printing channels (float-complex)";
                                        }
                                        break;
                                    default:
                                        throw M3Exception() << "Invalid data type size: " << tStream->GetDataTypeSize();
                                }
                                break;
                            default:
                                throw M3Exception() << "Invalid sample size: " << tStream->GetSampleSize();
                        }
                        break;
                    default:
                        throw M3Exception() << "Invalid data format: "<<tStrHeader.GetDataFormat();
                }
            }

        }

        tReadTest->FinishReading();
        delete tReadTest;
    }
    catch( M3Exception& e )
    {
        LERROR( mlog, "Exception thrown during file reading:\n" << e.what() );
        return 1;
    }

    return 0;
}

template< typename XDataType >
bool PrintChannelsReal( const M3Stream* aStream, uint32_t aDataFormat )
{
    const unsigned tMaxSamples = 30;
    unsigned tRecSize = aStream->GetChannelRecordSize();
    for( unsigned iChan = 0; iChan < aStream->GetNChannels(); ++iChan )
    {
        const M3DataReader< XDataType > tDataInterface( aStream->GetChannelRecord( iChan )->GetData(),
                aStream->GetDataTypeSize(), aDataFormat );
        stringstream tDataOut;
        for( unsigned iSample = 0; iSample < std::min( tMaxSamples, tRecSize ); ++iSample )
        {
            tDataOut << tDataInterface.at( iSample );
            if( iSample != tRecSize - 1 ) tDataOut << ", ";
        }
        if( tRecSize > tMaxSamples ) tDataOut << " . . .";
        LPROG( mlog, "\tChannel " << iChan << ": " << tDataOut.str() );
    }
    return true;
}

template< typename XDataType >
bool PrintChannelsComplex( const M3Stream* aStream, uint32_t aDataFormat )
{
    const unsigned tMaxSamples = 30;
    unsigned tRecSize = aStream->GetChannelRecordSize();
    for( unsigned iChan = 0; iChan < aStream->GetNChannels(); ++iChan )
    {
        const M3ComplexDataReader< XDataType > tDataInterface( aStream->GetChannelRecord( iChan )->GetData(),
                aStream->GetDataTypeSize(), aDataFormat, aStream->GetSampleSize() );
        stringstream tDataOut;
        for( unsigned iSample = 0; iSample < std::min( tMaxSamples, tRecSize ); ++iSample )
        {
            tDataOut << tDataInterface.at( iSample );
            if( iSample != tRecSize - 1 ) tDataOut << ", ";
        }
        if( tRecSize > tMaxSamples ) tDataOut << " . . .";
        LPROG( mlog, "\tChannel " << iChan << ": " << tDataOut.str() );
    }
    return true;
}
