#include "M3Monarch.hh"
#include "M3Logger.hh"

using namespace monarch3;

M3LOGGER( mlog, "Monarch3WriteTest" );

int main( const int argc, const char** argv )
{
    if( strcmp( argv[1], "-h" ) == 0 || argc < 2 )
    {
        M3INFO( mlog, "usage:\n"
            << "  MonarchInfo [-h] <output egg file>\n"
            << "      -h: print this usage information" );
        return -1;
    }

    try
    {
        Monarch3* tWriteTest = Monarch3::OpenForWriting( argv[1] );

        M3Header* tHeader = tWriteTest->GetHeader();
        tHeader->SetFilename( argv[1] );
        tHeader->SetRunDuration( 8675309 );
        tHeader->SetTimestamp( "Stardate 33515" );
        tHeader->SetDescription( "Bigger on the inside" );

        // number of samples in each stream
        unsigned tSSSamples = 10;
        unsigned tDSSamples = 5;
        unsigned tTSSamples = 5;

        M3INFO( mlog, "Adding streams" );
        unsigned tSingleStreamNum = tHeader->AddStream( "1-channel device", 500, tSSSamples, 1, sDigitized, 8 );
        unsigned tDoubleStreamNum = tHeader->AddStream( "2-channel device", 2, sInterleaved, 250, tDSSamples, 2, sDigitized, 16 );
        unsigned tTripleStreamNum = tHeader->AddStream( "3-channel device", 3, sSeparate, 100, tTSSamples, 1, sDigitized, 8 );

        tWriteTest->WriteHeader();

        M3INFO( mlog, "Wrote header:\n" << *tHeader );

        M3INFO( mlog, "Writing data" );

        // Stream 0
        M3Stream* tSingleStream = tWriteTest->GetStream( tSingleStreamNum );
        byte_type* tSSData = tSingleStream->GetStreamRecord()->GetData();
        for( unsigned iSample = 0; iSample < tSSSamples; ++iSample )
        {
            tSSData[ iSample ] = 1;
        }
        tSingleStream->WriteRecord( true );

        for( unsigned iSample = 0; iSample < tSSSamples; ++iSample )
        {
            tSSData[ iSample ] = 10;
        }
        tSingleStream->WriteRecord( false );


        // Stream 1
        M3Stream* tDoubleStream = tWriteTest->GetStream( tDoubleStreamNum );
        M3RecordDataSetter< uint16_t > tDSData0( tDoubleStream->GetChannelRecord( 0 )->GetData(), 2, sDigitized );
        M3RecordDataSetter< uint16_t > tDSData1( tDoubleStream->GetChannelRecord( 1 )->GetData(), 2, sDigitized );
        for( unsigned iSample = 0; iSample < tDSSamples; ++iSample )
        {
            tDSData0.set_at( 1, iSample );
            tDSData1.set_at( 2, iSample );
        }
        tDoubleStream->WriteRecord( true );

        for( unsigned iSample = 0; iSample < tDSSamples; ++iSample )
        {
            tDSData0.set_at( 1000, iSample );
            tDSData1.set_at( 2000, iSample );
        }
        tDoubleStream->WriteRecord( true );

        for( unsigned iSample = 0; iSample < tDSSamples; ++iSample )
        {
            tDSData0.set_at( 10000, iSample );
            tDSData1.set_at( 20000, iSample );
        }
        tDoubleStream->WriteRecord( false );


        // Stream 2
        M3Stream* tTripleStream = tWriteTest->GetStream( tTripleStreamNum );
        byte_type* tTSData0 = tTripleStream->GetChannelRecord( 0 )->GetData();
        byte_type* tTSData1 = tTripleStream->GetChannelRecord( 1 )->GetData();
        byte_type* tTSData2 = tTripleStream->GetChannelRecord( 2 )->GetData();
        for( unsigned iSample = 0; iSample < tTSSamples; ++iSample )
        {
            tTSData0[ iSample ] = 1;
            tTSData1[ iSample ] = 2;
            tTSData2[ iSample ] = 3;
        }
        tTripleStream->WriteRecord( true );

        for( unsigned iSample = 0; iSample < tTSSamples; ++iSample )
        {
            tTSData0[ iSample ] = 10;
            tTSData1[ iSample ] = 20;
            tTSData2[ iSample ] = 30;
        }
        tTripleStream->WriteRecord( false );

        tWriteTest->FinishWriting();
        M3INFO( mlog, "File closed" );

        delete tWriteTest;

    }
    catch( M3Exception& e )
    {
        M3ERROR( mlog, "Exception thrown during write test:\n" << e.what() );
    }

    return 0;
}
