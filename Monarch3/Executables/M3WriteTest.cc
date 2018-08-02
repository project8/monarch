#include "M3DataInterface.hh"
#include "M3Monarch.hh"
#include "logger.hh"

#include <cstring> // for strcmp

using namespace monarch3;

LOGGER( mlog, "M3WriteTest" );

int main( const int argc, const char** argv )
{
    if( argc < 2 || strcmp( argv[1], "-h" ) == 0 )
    {
        LINFO( mlog, "usage:\n"
            << "  M3WriteTest [-h] <output egg file>\n"
            << "      -h: print this usage information" );
        return -1;
    }

    try
    {
        Monarch3* tWriteTest = Monarch3::OpenForWriting( argv[1] );

        M3Header* tHeader = tWriteTest->GetHeader();
        tHeader->Filename() = argv[1];
        tHeader->SetRunDuration( 8675309 );
        tHeader->Timestamp() = "Stardate 33515";
        tHeader->Description() = "Bigger on the inside";

        // number of samples in each stream
        unsigned tSSSamples = 10;
        unsigned tDSSamples = 5;
        unsigned tTSSamples = 5;
        unsigned tFlSSamples = 10;
        //unsigned tFlCompSamples = 5;

        LINFO( mlog, "Adding streams" );
        unsigned tSingleStreamNum = tHeader->AddStream( "1-channel device", 500, tSSSamples, 1, 1, sDigitizedUS, 8, sBitsAlignedLeft );
        unsigned tDoubleStreamNum = tHeader->AddStream( "2-channel device", 2, sInterleaved, 250, tDSSamples, 1, 2, sDigitizedUS, 16, sBitsAlignedLeft );
        unsigned tTripleStreamNum = tHeader->AddStream( "3-channel device", 3, sSeparate, 100, tTSSamples, 1, 1, sDigitizedUS, 8, sBitsAlignedLeft );
        unsigned tFloatStreamNum = tHeader->AddStream( "Floating-point device", 100, tFlSSamples, 1, 4, sAnalog, 8, sBitsAlignedLeft );
        // multi-channel multi-sample writing commented out until fixed
        //unsigned tFlCompStreamNum = tHeader->AddStream( "Complex Floating-point device", 5, sInterleaved, 100, tFlCompSamples, 2, 8, sAnalog, 16 );

        tWriteTest->WriteHeader();

        LINFO( mlog, "Wrote header:\n" << *tHeader );

        LINFO( mlog, "Writing data" );

        // Stream 0
        M3Stream* tSingleStream = tWriteTest->GetStream( tSingleStreamNum );
        byte_type* tSSData = tSingleStream->GetStreamRecord()->GetData();
        for( unsigned iSample = 0; iSample < tSSSamples; ++iSample )
        {
            tSSData[ iSample ] = 1;
        }
        if( ! tSingleStream->WriteRecord( true ) )
        {
            LERROR( mlog, "Unable to write the record!" );
            delete tWriteTest;
            return -1;
        }

        for( unsigned iSample = 0; iSample < tSSSamples; ++iSample )
        {
            tSSData[ iSample ] = 10;
        }
        if( ! tSingleStream->WriteRecord( false ) )
        {
            LERROR( mlog, "Unable to write the record!" );
            delete tWriteTest;
            return -1;
        }


        // Stream 1
        M3Stream* tDoubleStream = tWriteTest->GetStream( tDoubleStreamNum );
        M3DataWriter< uint16_t > tDSData0( tDoubleStream->GetChannelRecord( 0 )->GetData(), 2, sDigitizedUS );
        M3DataWriter< uint16_t > tDSData1( tDoubleStream->GetChannelRecord( 1 )->GetData(), 2, sDigitizedUS );
        for( unsigned iSample = 0; iSample < tDSSamples; ++iSample )
        {
            tDSData0.set_at( 1, iSample );
            tDSData1.set_at( 2, iSample );
        }
        if( ! tDoubleStream->WriteRecord( true ) )
        {
            LERROR( mlog, "Unable to write the record!" );
            delete tWriteTest;
            return -1;
        }

        for( unsigned iSample = 0; iSample < tDSSamples; ++iSample )
        {
            tDSData0.set_at( 1000, iSample );
            tDSData1.set_at( 2000, iSample );
        }
        if( ! tDoubleStream->WriteRecord( true ) )
        {
            LERROR( mlog, "Unable to write the record!" );
            delete tWriteTest;
            return -1;
        }

        for( unsigned iSample = 0; iSample < tDSSamples; ++iSample )
        {
            tDSData0.set_at( 10000, iSample );
            tDSData1.set_at( 20000, iSample );
        }
        if( ! tDoubleStream->WriteRecord( false ) )
        {
            LERROR( mlog, "Unable to write the record!" );
            delete tWriteTest;
            return -1;
        }


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
        if( ! tTripleStream->WriteRecord( true ) )
        {
            LERROR( mlog, "Unable to write the record!" );
            delete tWriteTest;
            return -1;
        }

        for( unsigned iSample = 0; iSample < tTSSamples; ++iSample )
        {
            tTSData0[ iSample ] = 10;
            tTSData1[ iSample ] = 20;
            tTSData2[ iSample ] = 30;
        }
        if( ! tTripleStream->WriteRecord( false ) )
        {
            LERROR( mlog, "Unable to write the record!" );
            delete tWriteTest;
            return -1;
        }



        // Stream 3
        M3Stream* tFloatStream = tWriteTest->GetStream( tFloatStreamNum );
        M3DataWriter< float > tFlSData( tFloatStream->GetChannelRecord( 0 )->GetData(), 4, sAnalog );
        for( unsigned iSample = 0; iSample < tFlSSamples; ++iSample )
        {
            tFlSData.set_at( 3.1415926535898, iSample );
        }
        if( ! tFloatStream->WriteRecord( true ) )
        {
            LERROR( mlog, "Unable to write the record!" );
            delete tWriteTest;
            return -1;
        }

        for( unsigned iSample = 0; iSample < tFlSSamples; ++iSample )
        {
            tFlSData.set_at( 2.71828182846, iSample );
        }
        if( ! tFloatStream->WriteRecord( true ) )
        {
            LERROR( mlog, "Unable to write the record!" );
            delete tWriteTest;
            return -1;
        }




        // Stream 4
        /*
        M3Stream* tFlCompStream = tWriteTest->GetStream( tFlCompStreamNum );
        M3ComplexDataWriter< f8_complex > tFlCompSData0( tFlCompStream->GetChannelRecord( 0 )->GetData(), 8, sAnalog, 2 );
        M3ComplexDataWriter< f8_complex > tFlCompSData1( tFlCompStream->GetChannelRecord( 1 )->GetData(), 8, sAnalog, 2 );
        M3ComplexDataWriter< f8_complex > tFlCompSData2( tFlCompStream->GetChannelRecord( 2 )->GetData(), 8, sAnalog, 2 );
        M3ComplexDataWriter< f8_complex > tFlCompSData3( tFlCompStream->GetChannelRecord( 3 )->GetData(), 8, sAnalog, 2 );
        M3ComplexDataWriter< f8_complex > tFlCompSData4( tFlCompStream->GetChannelRecord( 3 )->GetData(), 8, sAnalog, 2 );
        f8_complex value0, value1, value2, value3, value4;
        value0[ 0 ] = 0.0; value0[ 1 ] = 0.0;
        value1[ 0 ] = 1.1; value1[ 1 ] = 1.001;
        value2[ 0 ] = 2.2; value1[ 1 ] = 2.002;
        value3[ 0 ] = 3.3; value1[ 1 ] = 3.003;
        value4[ 0 ] = 4.4; value1[ 1 ] = 4.004;
        for( unsigned iSample = 0; iSample < tFlCompSamples; ++iSample )
        {
            tFlCompSData0.set_at( value0, iSample );
            tFlCompSData1.set_at( value1, iSample );
            tFlCompSData2.set_at( value2, iSample );
            tFlCompSData3.set_at( value3, iSample );
            tFlCompSData4.set_at( value4, iSample );
        }
        if( ! tFlCompStream->WriteRecord( true ) )
        {
            LERROR( mlog, "Unable to write the record!" );
            delete tWriteTest;
            return -1;
        }

        value0[ 0 ] = -0.0; value0[ 1 ] = -0.0;
        value1[ 0 ] = -1.1; value1[ 1 ] = -1.001;
        value2[ 0 ] = -2.2; value1[ 1 ] = -2.002;
        value3[ 0 ] = -3.3; value1[ 1 ] = -3.003;
        value4[ 0 ] = -4.4; value1[ 1 ] = -4.004;
        for( unsigned iSample = 0; iSample < tFlCompSamples; ++iSample )
        {
            tFlCompSData0.set_at( value0, iSample );
            tFlCompSData1.set_at( value1, iSample );
            tFlCompSData2.set_at( value2, iSample );
            tFlCompSData3.set_at( value3, iSample );
            tFlCompSData4.set_at( value4, iSample );
        }
        if( ! tFlCompStream->WriteRecord( false ) )
        {
            LERROR( mlog, "Unable to write the record!" );
            delete tWriteTest;
            return -1;
        }
        */


        tWriteTest->FinishWriting();
        LINFO( mlog, "File closed" );

        delete tWriteTest;

    }
    catch( M3Exception& e )
    {
        LERROR( mlog, "Exception thrown during write test:\n" << e.what() );
    }

    return 0;
}
