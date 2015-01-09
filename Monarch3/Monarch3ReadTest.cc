#include "M3Monarch.hh"
#include "M3Logger.hh"

#include <sstream>

using namespace monarch3;
using std::stringstream;

M3LOGGER( mlog, "Monarch3ReadTest" );

bool ReadRecordCheck( const M3Stream* aStream, int aOffset, unsigned aDataFormat );
bool PrintChannelsInt( const M3Stream* aStream );
bool PrintChannelsFloat( const M3Stream* aStream );
bool PrintChannelsFloatComplex( const M3Stream* aStream );

int main( const int argc, const char** argv )
{
    if( strcmp( argv[1], "-h" ) == 0 || argc < 2 )
    {
        M3INFO( mlog, "usage:\n"
            << "  Monarch3ReadTest [-Hh] <input egg file>\n"
            << "      -h: print this usage information\n"
            << "      -H: (optional) header only; does not check records" );
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

        M3INFO( mlog, "Test 1: reading 2 sequential 1-channel records from stream 0");
        M3INFO( mlog, "\tRecord 0 has values '1'" );
        M3INFO( mlog, "\tRecord 1 has values '10'" );

        const M3Stream* tStream0 = tReadTest->GetStream( 0 );
        unsigned tNAcquisitions0 = tStream0->GetNAcquisitions();
        unsigned tNRecords0 = tStream0->GetNRecordsInFile();
        M3INFO( mlog, "Stream 0 has " << tNAcquisitions0 << " acquisitions and " << tNRecords0 << " records" );
        const M3StreamHeader& tStrHeader0 = tReadHeader->GetStreamHeaders().at( 0 );
        unsigned tNChannels0 = tStrHeader0.GetNChannels();
        unsigned tRecSize0 = tStrHeader0.GetRecordSize();
        M3INFO( mlog, "Stream 0 has " << tNChannels0 << " channel(s) stored in format mode " << tStrHeader0.GetChannelFormat() );
        if( tNAcquisitions0 != 1 || tNChannels0 != 1 || tNRecords0 != 2 )
        {
            M3ERROR( mlog, "Invalid number of acquisitions (1 expected), channels (1 expected), or records (2 expected)" );
            delete tReadTest;
            return 0;
        }

        for( unsigned iRec = 0; iRec < tNRecords0; ++iRec )
        {
            M3INFO( mlog, "Checking record " << iRec );
            if( ! ReadRecordCheck( tStream0, 0, tStrHeader0.GetDataFormat() ) )
            {
                M3ERROR( mlog, "Failed read record check" );
                delete tReadTest;
                return 0;
            }
        }

        M3INFO( mlog, "Test 1 complete\n" );



        M3INFO( mlog, "Test 2: using non-zero reading offsets");
        M3INFO( mlog, "\tRecord 0 has values '1' and '2'" );
        M3INFO( mlog, "\tRecord 1 has values '1000' and '2000'" );
        M3INFO( mlog, "\tRecord 2 has values '10000' and '20000'" );

        const M3Stream* tStream1 = tReadTest->GetStream( 1 );
        unsigned tNAcquisitions1 = tStream1->GetNAcquisitions();
        unsigned tNRecords1 = tStream1->GetNRecordsInFile();
        M3INFO( mlog, "Stream 1 has " << tNAcquisitions1 << " acquisitions and " << tNRecords1 << " records" );
        const M3StreamHeader& tStrHeader1 = tReadHeader->GetStreamHeaders().at( 1 );
        unsigned tNChannels1 = tStrHeader1.GetNChannels();
        unsigned tRecSize1 = tStrHeader1.GetRecordSize();
        M3INFO( mlog, "Stream 1 has " << tNChannels1 << " channel(s) stored in format mode " << tStrHeader1.GetChannelFormat() );
        if( tNAcquisitions1 != 2 || tNChannels1 != 2 || tNRecords1 != 3 )
        {
            M3ERROR( mlog, "Invalid number of acquisitions (2 expected), channels (2 expected), or records (3 expected)" );
            delete tReadTest;
            return 0;
        }

        M3INFO( mlog, "Read the first record (record 0)" );
        if( ! ReadRecordCheck( tStream1, 0, tStrHeader1.GetDataFormat() ) )
        {
            M3ERROR( mlog, "Failed read record check" );
            delete tReadTest;
            return 0;
        }

        M3INFO( mlog, "Skip to the third record, crossing to the next acquisition (record 2; acquisition 1)" );
        if( ! ReadRecordCheck( tStream1, 1, tStrHeader1.GetDataFormat() ) )
        {
            M3ERROR( mlog, "Failed read record check" );
            delete tReadTest;
            return 0;
        }

        M3INFO( mlog, "Reread the third record (record 2; acquisition 1)" );
        if( ! ReadRecordCheck( tStream1, -1, tStrHeader1.GetDataFormat() ) )
        {
            M3ERROR( mlog, "Failed read record check" );
            delete tReadTest;
            return 0;
        }

        M3INFO( mlog, "Go backwards to the second record (record 1; acquisition 1)" );
        if( ! ReadRecordCheck( tStream1, -2, tStrHeader1.GetDataFormat() ) )
        {
            M3ERROR( mlog, "Failed read record check" );
            delete tReadTest;
            return 0;
        }

        M3INFO( mlog, "Go backwards to the first record (record 1; acquisition 0)" );
        if( ! ReadRecordCheck( tStream1, -2, tStrHeader1.GetDataFormat() ) )
        {
            M3ERROR( mlog, "Failed read record check" );
            delete tReadTest;
            return 0;
        }

        M3INFO( mlog, "Reread the first record (record 1; acquisition 0)" );
        if( ! ReadRecordCheck( tStream1, -1, tStrHeader1.GetDataFormat() ) )
        {
            M3ERROR( mlog, "Failed read record check" );
            delete tReadTest;
            return 0;
        }

        M3INFO( mlog, "Request record beyond the end of the file" );
        if( tStream1->ReadRecord( 5 ) )
        {
            M3ERROR( mlog, "Record read did not fail" );
            delete tReadTest;
            return 0;
        }

        M3INFO( mlog, "Test 2 complete\n" );



        M3INFO( mlog, "Test 3: using non-zero reading offset to skip the first record");
        M3INFO( mlog, "\tRecord 0 has values '1', '2', and '3'" );
        M3INFO( mlog, "\tRecord 1 has values '10', '20', and '30'" );

        const M3Stream* tStream2 = tReadTest->GetStream( 2 );
        unsigned tNAcquisitions2 = tStream2->GetNAcquisitions();
        unsigned tNRecords2 = tStream2->GetNRecordsInFile();
        M3INFO( mlog, "Stream 2 has " << tNAcquisitions2 << " acquisitions and " << tNRecords2 << " records" );
        const M3StreamHeader& tStrHeader2 = tReadHeader->GetStreamHeaders().at( 2 );
        unsigned tNChannels2 = tStrHeader2.GetNChannels();
        unsigned tRecSize2 = tStrHeader2.GetRecordSize();
        M3INFO( mlog, "Stream 2 has " << tNChannels2 << " channel(s) stored in format mode " << tStrHeader2.GetChannelFormat() );
        if( tNAcquisitions2 != 1 || tNChannels2 != 3 || tNRecords2 != 2 )
        {
            M3ERROR( mlog, "Invalid number of acquisitions (1 expected), channels (3 expected), or records (2 expected)" );
            delete tReadTest;
            return 0;
        }

        M3INFO( mlog, "Skipping immediately to the second record (record 1)" );
        if( ! ReadRecordCheck( tStream2, 1, tStrHeader2.GetDataFormat() ) )
        {
            M3ERROR( mlog, "Failed read record check" );
            delete tReadTest;
            return 0;
        }

        M3INFO( mlog, "Request record before the beginning of the file" );
        if( tStream2->ReadRecord( -3 ) )
        {
            M3ERROR( mlog, "Record read did not fail" );
            delete tReadTest;
            return 0;
        }

        M3INFO( mlog, "Test 3 complete\n" );


        M3INFO( mlog, "Test 4: reading 2 sequential 1-channel floating-point records from stream 3");
        M3INFO( mlog, "\tRecord 0 has values '3.1415926535898'" );
        M3INFO( mlog, "\tRecord 1 has values '2.71828182846'" );

        const M3Stream* tStream3 = tReadTest->GetStream( 3 );
        unsigned tNAcquisitions3 = tStream3->GetNAcquisitions();
        unsigned tNRecords3 = tStream3->GetNRecordsInFile();
        M3INFO( mlog, "Stream 3 has " << tNAcquisitions3 << " acquisitions and " << tNRecords3 << " records" );
        const M3StreamHeader& tStrHeader3 = tReadHeader->GetStreamHeaders().at( 3 );
        unsigned tNChannels3 = tStrHeader3.GetNChannels();
        unsigned tRecSize3 = tStrHeader3.GetRecordSize();
        M3INFO( mlog, "Stream 3 has " << tNChannels3 << " channel(s) stored in format mode " << tStrHeader3.GetChannelFormat() );
        if( tNAcquisitions3 != 2 || tNChannels3 != 1 || tNRecords3 != 2 )
        {
            M3ERROR( mlog, "Invalid number of acquisitions (2 expected), channels (1 expected), or records (2 expected)" );
            delete tReadTest;
            return 0;
        }

        for( unsigned iRec = 0; iRec < tNRecords3; ++iRec )
        {
            M3INFO( mlog, "Checking record " << iRec );
            if( ! ReadRecordCheck( tStream3, 0, tStrHeader3.GetDataFormat() ) )
            {
                M3ERROR( mlog, "Failed read record check" );
                delete tReadTest;
                return 0;
            }
        }



        M3INFO( mlog, "Test 5: reading 2 sequential 5-channel interleaved complex floating-point records from stream 4");
        M3INFO( mlog, "\tRecord 0 always has values '(0, 0)'" );
        M3INFO( mlog, "\tRecord 1 has values '(1.1, 1.001) and (-1.1, -1.001)'" );
        M3INFO( mlog, "\tRecord 2 has values '(2.2, 2.002) and (-2.2, -2.002)'" );
        M3INFO( mlog, "\tRecord 3 has values '(3.3, 3.003) and (-3.3, -3.003)'" );
        M3INFO( mlog, "\tRecord 4 has values '(4.4, 4.004) and (-4.4, -4.004)'" );

        const M3Stream* tStream4 = tReadTest->GetStream( 4 );
        unsigned tNAcquisitions4 = tStream4->GetNAcquisitions();
        unsigned tNRecords4 = tStream4->GetNRecordsInFile();
        M3INFO( mlog, "Stream 4 has " << tNAcquisitions4 << " acquisitions and " << tNRecords4 << " records" );
        const M3StreamHeader& tStrHeader4 = tReadHeader->GetStreamHeaders().at( 3 );
        unsigned tNChannels4 = tStrHeader4.GetNChannels();
        unsigned tRecSize4 = tStrHeader4.GetRecordSize();
        M3INFO( mlog, "Stream 4 has " << tNChannels4 << " channel(s) stored in format mode " << tStrHeader4.GetChannelFormat() );
        if( tNAcquisitions4 != 1 || tNChannels4 != 5 || tNRecords4 != 2 )
        {
            M3ERROR( mlog, "Invalid number of acquisitions (1 expected), channels (5 expected), or records (2 expected)" );
            delete tReadTest;
            return 0;
        }

        for( unsigned iRec = 0; iRec < tNRecords4; ++iRec )
        {
            M3INFO( mlog, "Checking record " << iRec );
            if( ! ReadRecordCheck( tStream4, 0, tStrHeader4.GetDataFormat() ) )
            {
                M3ERROR( mlog, "Failed read record check" );
                delete tReadTest;
                return 0;
            }
        }

        M3INFO( mlog, "Test 5 complete\n" );




        tReadTest->FinishReading();
        delete tReadTest;
    }
    catch( M3Exception& e )
    {
        M3ERROR( mlog, "Exception thrown during file reading:\n" << e.what() );
    }

    return 0;
}

bool ReadRecordCheck( const M3Stream* aStream, int aOffset, unsigned aDataFormat )
{
    if( ! aStream->ReadRecord( aOffset ) )
    {
        M3ERROR( mlog, "Failed to read record" );
        return false;
    }

    switch( aDataFormat )
    {
        case sDigitized:
            if( ! PrintChannelsInt( aStream ) )
            {
                M3ERROR( mlog, "Failed to print channels (digitized)" );
                return false;
            }
            break;
        case sAnalog:
            switch( aStream->GetSampleSize() )
            {
                case 1:
                    if( ! PrintChannelsFloat( aStream ) )
                    {
                        M3ERROR( "Problem printing channels (float)" );
                        return 0;
                    }
                    break;
                default:
                    if( ! PrintChannelsFloatComplex( aStream ) )
                    {
                        M3ERROR( "Problem printing channels (float-complex)" );
                        return 0;
                    }
                    break;
            }
            break;
        default:
            M3ERROR( mlog, "Invalid data format" );
            return false;
    }

    return true;
}

bool PrintChannelsInt( const M3Stream* aStream )
{
    unsigned tRecSize = aStream->GetChannelRecordSize();
    const unsigned tMaxSamples = 30;
    for( unsigned iChan = 0; iChan < aStream->GetNChannels(); ++iChan )
    {
        const M3DataReader< uint64_t > tDataInterface( aStream->GetChannelRecord( iChan )->GetData(),
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
    unsigned tRecSize = aStream->GetChannelRecordSize();
    const unsigned tMaxSamples = 30;
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
        M3INFO( mlog, "\tChannel " << iChan << ": " << tDataOut.str() );
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
                M3INFO( mlog, "\tChannel " << iChan << ": " << tDataOut.str() );
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
                M3INFO( mlog, "\tChannel " << iChan << ": " << tDataOut.str() );
            }
            break;
        default:
            M3ERROR( mlog, "Cannot print channels for complex floating-point data with type size " << aStream->GetDataTypeSize() );
            return false;
            break;
    }
    return true;
}
