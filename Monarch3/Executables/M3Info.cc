#ifdef _WIN32
#define NOMINMAX
#endif

#include "M3Monarch.hh"
#include "M3DataInterface.hh"
#include "M3Record.hh"

#include "application.hh"
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
    scarab::main_app theMain( false );

    bool tHeaderOnly;
    std::string tFilename;

    theMain.add_flag( "-H,--header-only", tHeaderOnly, "Only look at header information; does not check number of records" );
    theMain.add_option( "Filename", tFilename, "File to read" )->required();

    CLI11_PARSE( theMain, argc, argv );

    try
    {
        LPROG( mlog, "Opening file <" << tFilename << ">" );
        std::shared_ptr< const Monarch3 > tReadTest( Monarch3::OpenForReading( tFilename ) );

        LPROG( mlog, "Reading header" );
        tReadTest->ReadHeader();

        const M3Header* tReadHeader = tReadTest->GetHeader();
        LPROG( mlog, *tReadHeader );

        if( tHeaderOnly )
        {
            tReadTest->FinishReading();
            STOP_LOGGING;
            return RETURN_SUCCESS;
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
    }
    catch( M3Exception& e )
    {
        LERROR( mlog, "Exception thrown during file reading:\n" << e.what() );
        STOP_LOGGING;
        return RETURN_ERROR;
    }

    STOP_LOGGING;
    return RETURN_SUCCESS;
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
