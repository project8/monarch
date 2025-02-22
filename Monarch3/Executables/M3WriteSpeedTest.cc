/*
 * Monarch3WriteSpeedTest.cc
 *
 *  Created on: Dec 21, 2016
 *      Author: N. Oblath
 *
 *  Description: Measures the speed of writing a specified amount of data to disk.
 *
 *  Single- or multi-threaded operation; in single-threaded operation all streams are written sequentially for each record.
 *
 *  Use: Monarch3WriteSpeedTest [options]
 *
 *  Options:
 *      multithreaded=(true | false) -- default: false
 *      n-records=[unsigned int] -- default: 10000
 *      n-streams=[unsigned int] -- default: 1
 *      array-size=[unsigned int] -- default: 1024
 *      data-type-size=[unsigned int] -- default: 1
 */

#include "M3DataInterface.hh"
#include "M3Monarch.hh"

#include "application.hh"
#include "logger.hh"
#include "param.hh"

#include <boost/filesystem.hpp>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>

using namespace monarch3;

LOGGER( mlog, "M3WriteSpeedTest" );

int main( int argc, char** argv )
{
    scarab::main_app theMain;

    theMain.default_config().add( "multithreaded", new scarab::param_value( false ) );
    theMain.default_config().add( "n-records", new scarab::param_value( 10000U ) );
    theMain.default_config().add( "n-streams", new scarab::param_value( 1U ) );
    theMain.default_config().add( "array-size", new scarab::param_value( 1024U ) );
    theMain.default_config().add( "data-type-size", new scarab::param_value( 1U ) );

    theMain.add_config_option< std::string >( "Filename", "filename", "Test output filename" );
    theMain.add_config_flag< bool >( "-m,--multithreaded", "multithreaded", "Use multithreaded write" );
    theMain.add_config_option< unsigned >( "-n,--n-records", "n-records", "Number of records to write" );
    theMain.add_config_option< unsigned >( "-N,--n-streams", "n-streams", "Number of streams to write" );
    theMain.add_config_option< unsigned >( "-a,--array-size", "array-size", "Array size" );
    theMain.add_config_option< unsigned >( "-d,--data-type-size", "data-type-size", "Data-type size" );

    CLI11_PARSE( theMain, argc, argv );

    try
    {
        bool tMultithreaded = theMain.primary_config()[ "multithreaded" ]().as_bool();
        unsigned tNRecords = theMain.primary_config()[ "n-records" ]().as_uint();
        unsigned tNStreams = theMain.primary_config()[ "n-streams" ]().as_uint();
        unsigned tArraySize = theMain.primary_config()[ "array-size" ]().as_uint();
        unsigned tSampleSize = 1; // currently not configurable
        unsigned tDataTypeSize = theMain.primary_config()[ "data-type-size" ]().as_uint();

        double tMBToWrite = (double)(tNRecords * tNStreams * tArraySize * tSampleSize * tDataTypeSize) * 10.e-6;

        if( tNStreams == 0 )
        {
            LERROR( mlog, "Please specify a number of streams > 0" );
            STOP_LOGGING;
            return RETURN_ERROR;
        }


        boost::filesystem::path tFilePath = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
        std::shared_ptr< Monarch3 > tWriteTest( Monarch3::OpenForWriting( tFilePath.native() ) );
        LINFO( mlog, "Temp file is " << tFilePath );

        LINFO( mlog, "Preparing header" );
        M3Header* tHeader = tWriteTest->GetHeader();
        tHeader->Filename() = "tempfile";
        tHeader->SetRunDuration( 8675309 );
        tHeader->Timestamp( ) ="Stardate 33515";
        tHeader->Description() = "Bigger on the inside";

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
        std::vector< byte_type > tDataMaster( tNBytes );

        M3DataWriter< uint8_t > tDMWriter( tDataMaster.data(), tDataTypeSize, sDigitizedUS );
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

        using std::chrono::steady_clock;
        using std::chrono::duration;
        using std::chrono::duration_cast;

        if( tMultithreaded )
        {

            std::mutex tRunMutex;
            std::condition_variable tRunRelease;

            std::vector< std::thread > tThreads;
            std::atomic< unsigned > tNThreadsReady( 0 );

            for( unsigned iStream = 0; iStream < tNStreams; ++iStream )
            {
                tThreads.push_back( std::thread( [&, iStream]{
                    //LDEBUG( mlog, "Starting thread " << iStream << "; waiting for start signal" );
                    bool tIsNewAcq = true;
                    std::unique_lock< std::mutex > tRunLock( tRunMutex );
                    tNThreadsReady++;
                    tRunRelease.wait( tRunLock );
                    for( unsigned iRecord = 0; iRecord < tNRecords; ++iRecord )
                    {
                        ::memcpy( tStreamData[ iStream ], tDataMaster.data(), tNBytes );
                        if( ! tStreams[ iStream ]->WriteRecord( tIsNewAcq ) )
                        {
                            LERROR( mlog, "Unable to write record <" << iRecord << "> for stream <" << iStream << ">" );
                            return;
                        }
                        tIsNewAcq = false;
                    }
                    //LDEBUG( mlog, "Thread " << iStream << " is finished" );
                } ) );
            }

            // Synchronize threads
            LINFO( mlog, "Waiting for threads to be ready" );
            while( tNThreadsReady.load() != tNStreams )
            {
                std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
            }
            std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

            LINFO( mlog, "Releasing threads" );
            tRunRelease.notify_all();

            steady_clock::time_point tStartTime = steady_clock::now();

            for( unsigned iStream = 0; iStream < tNStreams; ++iStream )
            {
                tThreads[ iStream ].join();
            }

            steady_clock::time_point tEndTime = steady_clock::now();

            duration< double > tDuration = duration_cast< duration< double > >( tEndTime - tStartTime );

            LINFO( mlog, "Processing time: " << tDuration.count() << " sec" );
            LINFO( mlog, "Size of data written: " << tMBToWrite << " MB" );
            LINFO( mlog, "Write speed: " << tMBToWrite / tDuration.count() << " MB/s" );

        }
        else
        {

            bool tIsNewAcq = true;

            steady_clock::time_point tStartTime = steady_clock::now();

            for( unsigned iRecord = 0; iRecord < tNRecords; ++iRecord )
            {
                for( unsigned iStream = 0; iStream < tNStreams; ++iStream )
                {
                    ::memcpy( tStreamData[ iStream ], tDataMaster.data(), tNBytes );
                    if( ! tStreams[ iStream ]->WriteRecord( tIsNewAcq ) )
                    {
                        LERROR( mlog, "Unable to write record <" << iRecord << "> for stream <" << iStream << ">" );
                        STOP_LOGGING;
                        return RETURN_ERROR;
                    }
                }
                tIsNewAcq = false;
            }

            steady_clock::time_point tEndTime = steady_clock::now();

            duration< double > tDuration = duration_cast< duration< double > >( tEndTime - tStartTime );

            LINFO( mlog, "Processing time: " << tDuration.count() << " sec" );
            LINFO( mlog, "Size of data written: " << tMBToWrite << " MB" );
            LINFO( mlog, "Write speed: " << tMBToWrite / tDuration.count() << " MB/s" );

        }

        LINFO( mlog, "Closing file" );

        tWriteTest->FinishWriting();

        boost::filesystem::remove( tFilePath );

        LINFO( mlog, "Test finished" );

    }
    catch( std::exception& e )
    {
        LERROR( mlog, "Exception thrown during write-speed test:\n" << e.what() );
        STOP_LOGGING;
        return RETURN_ERROR;
    }

    STOP_LOGGING;
    return RETURN_SUCCESS;
}
