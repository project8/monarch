/*
 * Joelle.cpp
 *
 * created on: May 9, 2011
 * author: dlfurse
 */

#include <cstdlib>
#include <cstring>
#include <signal.h>

#include <sstream>
using std::stringstream;

#include "mantis_env.hpp"
#include "mantis_exceptions.hpp"
#include "mantis_status.hpp"
#include "mantis_handlers.hpp"
#include "mantis_logger.hpp"

#include <iostream>
using std::cout;
using std::endl;

#include <pthread.h>
#include <px1500.h>
#include <hdf5.h>
#include <unistd.h>

#define JOELLE_SAMPLE_SIZE (4 * 1048576)

typedef unsigned long int data_id_t;
typedef enum
{
    eComplete, eAvailable
} data_status_t;

typedef unsigned int run_duration_t;
typedef enum
{
    eRun, eHalt
} run_status_t;

typedef enum {
  eNormal, eError
} run_condition_t;

struct shared_block
{
  pthread_mutex_t fDataMutexA;
  px4_sample_t* fDataA;
  data_id_t fDataIdA;
  data_status_t fDataStatusA;

  pthread_mutex_t fDataMutexB;
  px4_sample_t* fDataB;
  data_id_t fDataIdB;
  data_status_t fDataStatusB;

  pthread_mutex_t fRunMutex;
  run_duration_t fRunDuration;
  run_status_t fRunStatus;
  run_condition_t fRunCondition;

  HPX4 fDigitizerHandle;
  hid_t fFileHandle;
  hid_t fDataSpaceHandle;
  hid_t fMemSpaceHandle;
  hid_t fDataTypeHandle;

  hid_t fDSetCreateProps;
  hid_t fDSetXferProps;
  hid_t fDSetAccessProps;
};

void* RunThreadFunction( void* BlockPtr )
{
    //1. cast the incoming void to a shared block pointer

    shared_block* Block = (shared_block*) (BlockPtr);

    //2. sleep for the duration of the run
    for(std::size_t runT = 0; runT < Block->fRunDuration; runT++) {
      if(Block->fRunCondition == eError) {
	mantis_logger::Error("Error condition encountered.  Exiting...");
	break;
      }
      // Otherwise we just sleep for one second.
      sleep(1);
    }

    //3. lock the mutex protecting the run status and set it to halt

    pthread_mutex_lock( &Block->fRunMutex );
    Block->fRunStatus = eHalt;
    pthread_mutex_unlock( &Block->fRunMutex );

    //4. bail

    return NULL;
}

void* ReadThreadFunction( void* BlockPtr )
{
    //1. cast the incoming void to a shared block pointer

    shared_block* Block = (shared_block*) (BlockPtr);

    //2. allocate and initialize a record counter and a status buffer,
    //   as well as a status flag
    int PX4Result;
    bool canTakeData = true;
    data_id_t DataId = 0;
    run_status_t RunStatus = eRun;

    //3. arm recording

    mantis_logger::Info("beginning buffered PCI acquisition...");

    PX4Result = BeginBufferedPciAcquisitionPX4( Block->fDigitizerHandle, 
						PX4_FREE_RUN );
    if( PX4Result != SIG_SUCCESS ) {
      mantis_logger::Error("Couldn't start PCI acquisition!");
      canTakeData = false;
      Block->fRunCondition = eError;
    }

    //4. go go go go
    while( true && canTakeData )
    {

        //a. check the run status

        pthread_mutex_lock( &Block->fRunMutex );
        RunStatus = Block->fRunStatus;
        pthread_mutex_unlock( &Block->fRunMutex );
        if( RunStatus == eHalt )
        {
            break;
        }

        //b. pop the run counter and dump into data buffer A

        DataId++;

        pthread_mutex_lock( &Block->fDataMutexA );
        GetPciAcquisitionDataFastPX4( Block->fDigitizerHandle, JOELLE_SAMPLE_SIZE, Block->fDataA, 0 );
        Block->fDataIdA = DataId;
        Block->fDataStatusA = eComplete;
        pthread_mutex_unlock( &Block->fDataMutexA );

        //c. check the run status

        pthread_mutex_lock( &Block->fRunMutex );
        RunStatus = Block->fRunStatus;
        pthread_mutex_unlock( &Block->fRunMutex );
        if( RunStatus == eHalt )
        {
            break;
        }

        //d. pop the run counter and dump into data buffer B

        DataId++;

        pthread_mutex_lock( &Block->fDataMutexB );
        GetPciAcquisitionDataFastPX4( Block->fDigitizerHandle, JOELLE_SAMPLE_SIZE, Block->fDataB, 0 );
        Block->fDataIdB = DataId;
        Block->fDataStatusB = eComplete;
        pthread_mutex_unlock( &Block->fDataMutexB );

        //e. say something cute
        if( DataId % 1000 == 0 )
        {
            cout << DataId << " records acquired..." << endl;
        }

    }

    //5. clean up board

    EndBufferedPciAcquisitionPX4( Block->fDigitizerHandle );

    cout << DataId << " total records acquired" << endl;

    //6. bail

    return NULL;
}

void* WriteThreadFunction( void* BlockPtr )
{

    //1. cast the incoming void to a shared block pointer

    shared_block* Block = (shared_block*) (BlockPtr);

    //2. allocate a dataset id, a dataset name, and a spot for the run status

    hid_t DataSetHandle;
    stringstream DataSetName;
    run_status_t RunStatus = eRun;

    //3. go go go go

    while( true )
    {
        //a. check the run status

        pthread_mutex_lock( &Block->fRunMutex );
        RunStatus = Block->fRunStatus;
        pthread_mutex_unlock( &Block->fRunMutex );
        if( RunStatus == eHalt )
        {
            break;
        }

        //b. make a dataset, write the dataset using buffer A, free the dataset

        pthread_mutex_lock( &Block->fDataMutexA );
        if( Block->fDataStatusA == eComplete )
        {
            DataSetName.clear();
            DataSetName.str( "" );
            DataSetName << "Sample" << Block->fDataIdA;

            DataSetHandle = H5Dcreate2( Block->fFileHandle, DataSetName.str().c_str(), Block->fDataTypeHandle, Block->fDataSpaceHandle, H5P_DEFAULT, Block->fDSetCreateProps, Block->fDSetAccessProps );
            H5Dwrite( DataSetHandle, H5T_NATIVE_UCHAR, Block->fMemSpaceHandle, Block->fDataSpaceHandle, Block->fDSetXferProps, Block->fDataA );
            H5Dclose( DataSetHandle );
            
            if( Block-> fDataIdA % 1000 == 0 )
            {
	      // H5Fflush( Block->fFileHandle, H5F_SCOPE_LOCAL );
            }
            Block->fDataStatusA = eAvailable;
        }
        pthread_mutex_unlock( &Block->fDataMutexA );

        //c. check the run status

        pthread_mutex_lock( &Block->fRunMutex );
        RunStatus = Block->fRunStatus;
        pthread_mutex_unlock( &Block->fRunMutex );
        if( RunStatus == eHalt )
        {
            break;
        }

        //b. make a dataset, write the dataset using buffer B, free the dataset

        pthread_mutex_lock( &Block->fDataMutexB );
        if( Block->fDataStatusB == eComplete )
        {
            DataSetName.clear();
            DataSetName.str( "" );
            DataSetName << "Sample" << Block->fDataIdB;

            DataSetHandle = H5Dcreate2( Block->fFileHandle, 
					DataSetName.str().c_str(), 
					Block->fDataTypeHandle, 
					Block->fDataSpaceHandle, 
					H5P_DEFAULT, 
					Block->fDSetCreateProps, 
					Block->fDSetAccessProps );
            H5Dwrite( DataSetHandle, 
		      H5T_NATIVE_UCHAR, 
		      Block->fMemSpaceHandle, 
		      Block->fDataSpaceHandle, 
		      Block->fDSetXferProps, 
		      Block->fDataB );
            H5Dclose( DataSetHandle );
            
            if( Block-> fDataIdB % 1000 == 0 )
            {
	      //	      H5Fflush( Block->fFileHandle, H5F_SCOPE_LOCAL );
            }
            Block->fDataStatusB = eAvailable;
        }
        pthread_mutex_unlock( &Block->fDataMutexB );

    }
    
    //4. flush any and all remaining buffers to disk
    
    H5Fflush( Block->fFileHandle, H5F_SCOPE_GLOBAL );

    //5. bail

    return NULL;
}

int main(int argc, char** argv)
{

  /*
   * set up signal handling such that on ctrl-c, we clean up file handlers and
   * mutexes sensibly.
   */
  void (*sig_hand)(int) = signal(SIGINT, abort_run);
  if(sig_hand == SIG_IGN) signal(SIGINT, SIG_IGN);

  /*
   * parse the argc/argv into an environment.  if we catch an exception,
   * die with some meaningful error here.
   */
  static safeEnvPtr runEnvironment;
  try {
    runEnvironment = mantis_env::parseArgs(argc, argv);
  }
  catch(argument_exception* e) {
    mantis_logger::Error(e->what());
    exit(env_arg_error);
  }

  mantis_logger::Info("mantis started.  requested run environment:");
  cout << runEnvironment << endl;

    //****************************
    //shared memory initialization
    //****************************

    shared_block Buffer;
    
    pthread_mutex_init( &Buffer.fDataMutexA, NULL );
    Buffer.fDataA = NULL;
    Buffer.fDataIdA = 0;
    Buffer.fDataStatusA = eAvailable;

    pthread_mutex_init( &Buffer.fDataMutexB, NULL );
    Buffer.fDataB = NULL;
    Buffer.fDataIdB = 0;
    Buffer.fDataStatusB = eAvailable;

    pthread_mutex_init( &Buffer.fRunMutex, NULL );
    Buffer.fRunDuration = runEnvironment->getRunLength();
    Buffer.fRunStatus = eRun;

    //************************
    //digitizer initialization
    //************************

    int PX4Result; //this reused variable holds the results of px4 library function calls

    //1. try to connect to digitizer
    mantis_logger::Info("connecting to digitizer...");

    PX4Result = ConnectToDevicePX4( &Buffer.fDigitizerHandle, 1 );
    if( PX4Result != SIG_SUCCESS )
    {
        DumpLibErrorPX4( PX4Result, "failed to connect to digitizer card: " );
        return -1;
    }

    //2. set the parameters to the default state

    mantis_logger::Info("setting digizer defaults...");

    PX4Result = SetPowerupDefaultsPX4( Buffer.fDigitizerHandle );
    if( PX4Result != SIG_SUCCESS )
    {
        DumpLibErrorPX4( PX4Result, "failed to enter default state: " );
        return -1;
    }

    //3. set channel 1 for acquisition

    mantis_logger::Info("enabling channel one...");

    PX4Result = SetActiveChannelsPX4( Buffer.fDigitizerHandle, PX4CHANSEL_SINGLE_CH1 );
    if( PX4Result != SIG_SUCCESS )
    {
        DumpLibErrorPX4( PX4Result, "failed to activate channel 1: " );
        return -1;
    }

    //4. set sampling rate
    mantis_logger::Info("setting sample rate...");


    PX4Result = SetInternalAdcClockRatePX4( Buffer.fDigitizerHandle, runEnvironment->getClockRate() );
    if( PX4Result != SIG_SUCCESS )
    {
        DumpLibErrorPX4( PX4Result, "failed to set sampling rate: " );
        return -1;
    }

    //*********************
    //dma buffer allocation
    //*********************

    //1. allocate first DMA buffer

    mantis_logger::Info( "allocating first DMA buffer...");

    PX4Result = AllocateDmaBufferPX4( Buffer.fDigitizerHandle, JOELLE_SAMPLE_SIZE, &Buffer.fDataA );
    if( PX4Result != SIG_SUCCESS )
    {
        DumpLibErrorPX4( PX4Result, "failed to allocate first DMA buffer: " );
        return -1;
    }

    //2. allocate second DMA buffer

    mantis_logger::Info("allocating second DMA buffer...");

    PX4Result = AllocateDmaBufferPX4( Buffer.fDigitizerHandle, JOELLE_SAMPLE_SIZE, &Buffer.fDataB );
    if( PX4Result != SIG_SUCCESS )
    {
        DumpLibErrorPX4( PX4Result, "failed to allocate second DMA buffer: " );
        return -1;
    }

    //*******************
    //hdf5 initialization
    //*******************

    mantis_logger::Info("setting up new hdf5 file..");

    hsize_t H5DataSpaceDimensions;

    //1. make a new hdf5 type file

    Buffer.fFileHandle = H5Fcreate( runEnvironment->getOutName().c_str(), 
				    H5F_ACC_TRUNC, 
				    H5P_DEFAULT, 
				    H5P_DEFAULT );

    //2. make a new hdf5 data space of rank 1 with a length as long as our dimensions

    H5DataSpaceDimensions = JOELLE_SAMPLE_SIZE;
    Buffer.fDataSpaceHandle = H5Screate_simple( 1, &H5DataSpaceDimensions, NULL );
    Buffer.fMemSpaceHandle = H5Screate_simple( 1, &H5DataSpaceDimensions, NULL );

    //3. make a new datatype that just copies the native unsigned character type

    Buffer.fDataTypeHandle = H5Tcopy( H5T_NATIVE_UCHAR );

    //4. set the dataset poperties 
    Buffer.fDSetCreateProps = H5Pcreate(H5P_DATASET_CREATE);
    Buffer.fDSetXferProps   = H5Pcreate(H5P_DATASET_XFER);
    Buffer.fDSetAccessProps = H5Pcreate(H5P_DATASET_ACCESS);
    
    size_t xfer_buf_size = 2*1024*1024;

    H5Pset_buffer(Buffer.fDSetXferProps,xfer_buf_size,NULL,NULL);

    //***************
    //go threads go!!
    //***************

    pthread_t WriteThread;
    pthread_t ReadThread;
    pthread_t RunThread;

    pthread_create( &WriteThread, NULL, &WriteThreadFunction, &Buffer );
    pthread_create( &ReadThread, NULL, &ReadThreadFunction, &Buffer );
    pthread_create( &RunThread, NULL, &RunThreadFunction, &Buffer );

    pthread_join( WriteThread, NULL );
    pthread_join( ReadThread, NULL );
    pthread_join( RunThread, NULL );

    //*************
    //finalize hdf5
    //*************

    mantis_logger::Info("closing hdf5 file...");

    //1. clean up datatype

    H5Tclose( Buffer.fDataTypeHandle );

    //2. clean up dataspace

    H5Sclose( Buffer.fDataSpaceHandle );

    //3. clean up file

    H5Fclose( Buffer.fFileHandle );

    //**********************
    //deallocate dma buffers
    //**********************

    //1. deallocate first DMA buffer

    mantis_logger::Info("deallocating first DMA buffer...");

    PX4Result = FreeDmaBufferPX4( Buffer.fDigitizerHandle, Buffer.fDataA );
    if( PX4Result != SIG_SUCCESS )
    {
        DumpLibErrorPX4( PX4Result, "failed to deallocate first DMA buffer: " );
        return -1;
    }

    //2. deallocate second DMA buffer

    mantis_logger::Info("deallocating second DMA buffer...");

    PX4Result = FreeDmaBufferPX4( Buffer.fDigitizerHandle, Buffer.fDataB );
    if( PX4Result != SIG_SUCCESS )
    {
        DumpLibErrorPX4( PX4Result, "failed to deallocate second DMA buffer: " );
        return -1;
    }

    //******************
    //finalize digitizer
    //******************

    //1. disconnect from digitizer

    mantis_logger::Info("disconnecting from digitizer...");

    PX4Result = DisconnectFromDevicePX4( Buffer.fDigitizerHandle );
    if( PX4Result != SIG_SUCCESS )
    {
        DumpLibErrorPX4( PX4Result, "failed to disconnect from digitizer card: " );
        return -1;
    }

    //**********************
    //finalize shared memory
    //**********************

    pthread_mutex_destroy( &Buffer.fDataMutexA );
    pthread_mutex_destroy( &Buffer.fDataMutexB );
    pthread_mutex_destroy( &Buffer.fRunMutex );

    return 0;
}
