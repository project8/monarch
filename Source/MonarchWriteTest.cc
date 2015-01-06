#include "MMonarch.hh"
#include "MLogger.hh"

using namespace monarch;

MLOGGER( mlog, "MonarchWriteTest" );

int main( const int argc, const char** argv )
{
    if( strcmp( argv[1], "-h" ) == 0 || argc < 2 )
    {
        MINFO( mlog, "usage:\n"
            << "  MonarchInfo [-h] <output egg file>\n"
            << "      -h: print this usage information" );
        return -1;
    }

    try
    {
        Monarch* tWriteTest = Monarch::OpenForWriting( argv[1] );

        MHeader* tHeader = tWriteTest->GetHeader();
        tHeader->SetSchemaVersion( "???" );
        tHeader->SetFilename( argv[1] );
        tHeader->SetRunDuration( 8675309 );
        tHeader->SetTimestamp( "Stardate 33515" );
        tHeader->SetDescription( "Bigger on the inside" );

        // number of samples in each stream
        unsigned tSSSamples = 10;
        unsigned tDSSamples = 5;
        unsigned tTSSamples = 5;

        MINFO( mlog, "Adding streams" );
        unsigned tSingleStreamNum = tHeader->AddStream( "1-channel device", 500, tSSSamples, 1, sDigitized, 8 );
        unsigned tDoubleStreamNum = tHeader->AddStream( "2-channel device", 2, sInterleaved, 250, tDSSamples, 1, sDigitized, 8 );
        unsigned tTripleStreamNum = tHeader->AddStream( "3-channel device", 3, sSeparate, 100, tTSSamples, 1, sDigitized, 8 );

        tWriteTest->WriteHeader();

        MINFO( mlog, "Wrote header:\n" << *tHeader );

        MINFO( mlog, "Writing data" );

        // Stream 0
        MStream* tSingleStream = tWriteTest->GetStream( tSingleStreamNum );
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
        MStream* tDoubleStream = tWriteTest->GetStream( tDoubleStreamNum );
        byte_type* tDSData0 = tDoubleStream->GetChannelRecord( 0 )->GetData();
        byte_type* tDSData1 = tDoubleStream->GetChannelRecord( 1 )->GetData();
        for( unsigned iSample = 0; iSample < tDSSamples; ++iSample )
        {
            tDSData0[ iSample ] = 1;
            tDSData1[ iSample ] = 2;
        }
        tDoubleStream->WriteRecord( true );

        for( unsigned iSample = 0; iSample < tDSSamples; ++iSample )
        {
            tDSData0[ iSample ] = 10;
            tDSData1[ iSample ] = 20;
        }
        tDoubleStream->WriteRecord( true );

        for( unsigned iSample = 0; iSample < tDSSamples; ++iSample )
        {
            tDSData0[ iSample ] = 100;
            tDSData1[ iSample ] = 200;
        }
        tDoubleStream->WriteRecord( false );


        // Stream 2
        MStream* tTripleStream = tWriteTest->GetStream( tTripleStreamNum );
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

        tWriteTest->Close();
        MINFO( mlog, "File closed" );

        delete tWriteTest;

    }
    catch( MException& e )
    {
        MERROR( mlog, "Exception thrown during write test:\n" << e.what() );
    }

    return 0;
}
