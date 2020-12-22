/*
 * Monarch3MultithreadingTest.cc
 *
 *  Created on: Dec 22, 2016
 *      Author: N. Oblath
 *
 *  Use: Monarch3MultithreadingTest [options]
 *
 *  Options:
 *      filename=[string] -- default: multithreading_test.egg
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

LOGGER( mlog, "M3MultithreadingTest" );

int main( int argc, char** argv )
{
    try
    {
        scarab::main_app theMain;

        theMain.default_config().add( "filename", "multithreading_test.egg" );

        theMain.add_config_option< std::string >( "Filename", "filename", "Test output filename" );

        CLI11_PARSE( theMain, argc, argv );

        std::string tFilename = theMain.primary_config()[ "filename" ]().as_string();

        unsigned tNRecords = 5;
        unsigned tNStreams = 10;
        unsigned tArraySize = 1024;
        unsigned tSampleSize = 1;
        unsigned tDataTypeSize = 1;

        Monarch3* tWriteTest = Monarch3::OpenForWriting( tFilename );

        LINFO( mlog, "Preparing header" );
        M3Header* tHeader = tWriteTest->GetHeader();
        tHeader->Filename() = tFilename;
        tHeader->SetRunDuration( 8675309 );
        tHeader->Timestamp() = "Stardate 33515";
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
                    ::memcpy( tStreamData[ iStream ], tDataMaster, tNBytes );
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

        for( unsigned iStream = 0; iStream < tNStreams; ++iStream )
        {
            tThreads[ iStream ].join();
        }

        LINFO( mlog, "Closing file" );

        tWriteTest->FinishWriting();

        delete tWriteTest;

        delete [] tDataMaster;

        LINFO( mlog, "Test finished" );

    }
    catch( std::exception& e )
    {
        LERROR( mlog, "Exception thrown during write-speed test:\n" << e.what() );
    }

    return 0;
}
