/*
 * Monarch3WriteSpeedTest.cc
 *
 *  Created on: Dec 21, 2016
 *      Author: obla999
 */

#include "M3DataInterface.hh"
#include "M3Monarch.hh"

#include "configurator.hh"
#include "logger.hh"
#include "param.hh"

#include <boost/filesystem.hpp>

#include <chrono>
#include <stdlib.h>
#include <string>

using namespace monarch3;

LOGGER( mlog, "Monarch3WriteSpeedTest" );

int main( int argc, char** argv )
{
    try
    {
        scarab::param_node tDefaultConfig;
        tDefaultConfig.add( "n-records", new scarab::param_value( 10000U ) );
        tDefaultConfig.add( "n-streams", new scarab::param_value( 1U ) );
        tDefaultConfig.add( "array-size", new scarab::param_value( 1024U ) );
        tDefaultConfig.add( "data-type-size", new scarab::param_value( 1U ) );

        scarab::configurator t_configurator( argc, argv, &tDefaultConfig );

        unsigned tNRecords = t_configurator.get< unsigned >( "n-records" );
        unsigned tNStreams = t_configurator.get< unsigned >( "n-streams" );
        unsigned tArraySize = t_configurator.get< unsigned >( "array-size" );
        unsigned tSampleSize = 1; // currently not configurable
        unsigned tDataTypeSize = t_configurator.get< unsigned >( "data-type-size" );

        double tMBToWrite = (double)(tNRecords * tNStreams * tArraySize * tSampleSize * tDataTypeSize) * 10.e-6;

        // For now only allow 1 stream because Monarch is not threadsafe
        if( tNStreams != 1 )
        {
            LERROR( mlog, "Unable to run with <" << tNStreams << "> streams" );
            return -1;
        }


        boost::filesystem::path tFilePath = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
        Monarch3* tWriteTest = Monarch3::OpenForWriting( tFilePath.native() );
        LINFO( mlog, "Temp file is " << tFilePath );

        LINFO( mlog, "Preparing header" );
        M3Header* tHeader = tWriteTest->GetHeader();
        tHeader->SetFilename( "tempfile" );
        tHeader->SetRunDuration( 8675309 );
        tHeader->SetTimestamp( "Stardate 33515" );
        tHeader->SetDescription( "Bigger on the inside" );

        LINFO( mlog, "Adding stream(s)" );

        std::vector< unsigned > tStreamNums( tNStreams );
        for( unsigned iStream = 0; iStream < tNStreams; ++iStream )
        {
            std::stringstream tStr;
            tStr << "Channel " << iStream;
            unsigned tSingleStreamNum = tHeader->AddStream( tStr.str(), 100, tArraySize, tSampleSize, tDataTypeSize, sDigitizedUS, tDataTypeSize * 8, sBitsAlignedLeft );
            tStreamNums[ iStream ] = tSingleStreamNum;
        }

        tWriteTest->WriteHeader();

        LINFO( mlog, "Wrote header:\n" << *tHeader );


        LINFO( mlog, "Creating fake data array" );

        unsigned tNBytes = tArraySize * tDataTypeSize * tSampleSize;
        byte_type* tDataMaster = new byte_type[tNBytes];

        M3DataWriter< uint8_t > tDMWriter( tDataMaster, tDataTypeSize, sDigitizedUS );
        for( unsigned iBin = 0; iBin < tArraySize; ++iBin )
        {
            tDMWriter.set_at( 42, iBin );
        }


        LINFO( mlog, "Getting stream pointers" );

        std::vector< M3Stream* > tStreams( tNStreams );
        std::vector< byte_type* > tStreamData( tNStreams );
        for( unsigned iStream = 0; iStream < tNStreams; ++iStream )
        {
            tStreams[ iStream ] = tWriteTest->GetStream( tStreamNums[ iStream ] );
            tStreamData[ iStream ] = tStreams[ iStream ]->GetStreamRecord()->GetData();
        }

        LINFO( mlog, "Writing data" );

        bool tIsNewAcq = true;

        using std::chrono::steady_clock;
        using std::chrono::duration;
        using std::chrono::duration_cast;

        steady_clock::time_point tStartTime = steady_clock::now();

        for( unsigned iRecord = 0; iRecord < tNRecords; ++iRecord )
        {
            for( unsigned iStream = 0; iStream < tNStreams; ++iStream )
            {
                ::memcpy( tStreamData[ iStream ], tDataMaster, tNBytes );
                if( ! tStreams[ iStream ]->WriteRecord( tIsNewAcq ) )
                {
                    LERROR( mlog, "Unable to write record <" << iRecord << "> for stream <" << iStream << ">" );
                    delete tWriteTest;
                    delete [] tDataMaster;
                    return -1;
                }
                tIsNewAcq = false;
            }
        }

        steady_clock::time_point tEndTime = steady_clock::now();

        duration< double > tDuration = duration_cast< duration< double > >( tEndTime - tStartTime );

        LINFO( mlog, "Processing time: " << tDuration.count() << " sec" );
        LINFO( mlog, "Size of data written: " << tMBToWrite << " MB" );
        LINFO( mlog, "Write speed: " << tMBToWrite / tDuration.count() << " MB/s" );

        LINFO( mlog, "Closing file" );

        tWriteTest->FinishWriting();

        delete tWriteTest;
        boost::filesystem::remove( tFilePath );

        LINFO( mlog, "Test finished" );

    }
    catch( std::exception& e )
    {
        LERROR( mlog, "Exception thrown during write-speed test:\n" << e.what() );
    }

    return 0;
}
