/*
 * M4Stream.cc
 *
 *  Created on: Apr 12, 2023
 *      Author: N.S. Oblath
 */

#define M4_API_EXPORTS

#include "M4Stream.hh"

#include "M4IToA.hh"
#include "logger.hh"
#include "z5includes.hh"

#include <cstdlib> // for abs

/* Notes on reading:
 *
 * During reading, both fNRecordsInAcq and fRecordsCount are used, as well as both fAcquisitionId and fNAcquisitions;
 * All four variables will always be valid.
 *
 */

/* Notes on writing:
 *
 * As writing progresses, fRecordCountInAcq is incremented, and fNRecordsInAcq is ignored.
 * When the acquisition is finalized with FinalizeCurrentAcq(), fNRecordsInAcq is updated to be fRecordCountInAcq + 1.
 * Therefore fRecordCountInAcq is only valid for the last completed acquisition.
 *
 * As writing progresses, fAcquisitionId is incremented and fNAcquisitions is ignored.
 * When the stream is finalized with FinalizeStream(), fNAcquisitions is updated to be fAcquisitionId + 1.
 * Therefore fNAcquisitions is only valid after the stream is finalized.
 *
 */

namespace monarch4
{
    LOGGER( mlog, "M4Stream" );

    /*************************************************************************
    * @brief Construct a new M4Stream::M4Stream object from specified context
    * 
    * @param[in] aHeader reference to M4StreamHeader representing the stream
    * @param[in] aStreamsLoc hierarchy position: <file>/"streams"/"streamsN"
    * @param aAccessFormat Specifies whether the data channels are interleaved 
    *        or separate in a stream (sSeparate, sInterleaved), defaults to
    *        sSeparate
    *************************************************************************/
    M4Stream::M4Stream( const M4StreamHeader& aHeader, z5GroupHandle* aStreamsLoc, uint32_t aAccessFormat ) :
            fMode( kRead ),
            fDoReadRecord( nullptr ),
            fDoWriteRecord( nullptr ),
            fIsInitialized( false ),
            fRecordsAccessed( false ),
            fDataTypeSize( aHeader.GetDataTypeSize() ),
            fSampleSize( aHeader.GetSampleSize() ),
            fStrRecNBytes( aHeader.GetNChannels() * aHeader.GetRecordSize() * aHeader.GetSampleSize() * aHeader.GetDataTypeSize() ),
            fStrRecSize( aHeader.GetNChannels() * aHeader.GetRecordSize() ),
            fChanRecNBytes( aHeader.GetRecordSize() * aHeader.GetSampleSize() * aHeader.GetDataTypeSize() ),
            fChanRecSize( aHeader.GetRecordSize() ),
            fChanRecLength( (double)aHeader.GetRecordSize() / ((double)aHeader.GetAcquisitionRate() * 1.e-3) ),
            fStreamRecord(),
            fNChannels( aHeader.GetNChannels() ),
            fChannelRecords( new M4Record[ aHeader.GetNChannels() ] ),
            fNAcquisitions( 0 ),
            fAcquisitionId( 0 ),
            fRecordCountInAcq( 0 ),
            fNRecordsInAcq( 0 ),
            fAcqFirstRecTime( 0 ),
            fAcqFirstRecId( 0 ),
            fAcqFirstRecTimes( nullptr ),
            fAcqFirstRecIds( nullptr ),
            fDataInterleaved( aHeader.GetChannelFormat() == sInterleaved ),
            fAccessFormat( aAccessFormat ),
            fRecordIndex(),
            fRecordCountInFile( 0 ),
            fNRecordsInFile( 0 ),
            fFirstRecordInFile( 0 ),
            // fStreamParentLoc( new H5::Group( aStreamsLoc->openGroup( aHeader.GetLabel() ) ) ),
            fStreamParentLoc(new z5GroupHandle(*aStreamsLoc, aHeader.GetLabel())),
            fAcqLoc( nullptr ),
            fCurrentAcqDataSet( nullptr ),
            fDataSpaceUser( nullptr ),
            fMutexPtr( new std::mutex() )
    {
std::cout << "\nM4Stream::M4Stream() : "  << aHeader.GetLabel() << std::endl;

        LDEBUG( mlog, "Creating stream for <" << aHeader.GetLabel() << ">" );

        if( aHeader.GetDataFormat() == sDigitizedUS )
        {
std::cout << "sDigitizedUS stream data of: ";
            switch( fDataTypeSize )
            {
                case 1:
                    fDataTypeInFile = z5::types::uint8;     //H5::PredType::STD_U8LE;
                    fDataTypeUser = z5::types::uint8;       //H5::PredType::NATIVE_UINT8;
std::cout << "uint8\n";
                   break;
                case 2:
                    fDataTypeInFile = z5::types::uint16;    //H5::PredType::STD_U16LE;
                    fDataTypeUser = z5::types::uint16;      //H5::PredType::NATIVE_UINT16;
std::cout << "uint16\n";
                    break;
                case 4:
                    fDataTypeInFile = z5::types::uint32;    //H5::PredType::STD_U32LE;
                    fDataTypeUser = z5::types::uint32;      //H5::PredType::NATIVE_UINT32;
std::cout << "uint32\n";
                    break;
                case 8:
                    fDataTypeInFile = z5::types::uint64;    //H5::PredType::STD_U64LE;
                    fDataTypeUser = z5::types::uint64;      //H5::PredType::NATIVE_UINT64;
std::cout << "uint64\n";
                    break;
                default:
                    throw M4Exception() << "M4Stream::M4Stream() Unknown unsigned integer data type size: " << fDataTypeSize;
            }
        }
        else if( aHeader.GetDataFormat() == sDigitizedS )
        {
std::cout << "sDigitizedS stream data of: ";
            switch( fDataTypeSize )
            {
                case 1:
                    fDataTypeInFile = z5::types::int8;      //H5::PredType::STD_I8LE;
                    fDataTypeUser = z5::types::int8;        //H5::PredType::NATIVE_INT8;
std::cout << "int8\n";
                   break;
                case 2:
                    fDataTypeInFile = z5::types::int16;      //H5::PredType::STD_I16LE;
                    fDataTypeUser = z5::types::int16;        //H5::PredType::NATIVE_INT16;
std::cout << "int16\n";
                    break;
                case 4:
                    fDataTypeInFile = z5::types::int32;     //H5::PredType::STD_I32LE;
                    fDataTypeUser = z5::types::int32;       //H5::PredType::NATIVE_INT32;
std::cout << "int32\n";
                    break;
                case 8:
                    fDataTypeInFile = z5::types::int64;     //H5::PredType::STD_I64LE;
                    fDataTypeUser = z5::types::int64;       //H5::PredType::NATIVE_INT64;
std::cout << "int64\n";
                    break;
                default:
                    throw M4Exception() << "M4Stream::M4Stream() Unknown signed integer data type size: " << fDataTypeSize;
            }
        }
        else // aHeader.GetDataFormat() == sAnalog
        {
std::cout << "Analog/floating-point stream data of: ";
            switch( fDataTypeSize )
            {
                case 4:
                    fDataTypeInFile = z5::types::float32;   //H5::PredType::IEEE_F32LE;
                    fDataTypeUser = z5::types::float32;     //H5::PredType::NATIVE_FLOAT;
std::cout << "float32\n";
                    break;
                case 8:
                    fDataTypeInFile = z5::types::float64;   //H5::PredType::IEEE_F64LE ;
                    fDataTypeUser = z5::types::float64;     //H5::PredType::NATIVE_DOUBLE;
std::cout << "float64\n";
                    break;
                default:
                    throw M4Exception() << "M4Stream::M4Stream() Unknown floating-point data type size: " << fDataTypeSize;
            }
        }

#if 0 // original HDF5 code
        // variables to store the HDF5 error printing state
        H5E_auto2_t tAutoPrintFunc;
        void* tClientData;

        // Determine if we're in read or write mode
        // and get/create the acquisitions group
        // Nested exceptions are used so that the outer try block can be used to determine whether we're reading or writing
        try
        {
            // turn off HDF5 error printing because the throwing of an exception here means we're writing instead of reading
            H5::Exception::getAutoPrint( tAutoPrintFunc, &tClientData );
            H5::Exception::dontPrint();

            fH5AcqLoc = new H5::Group( fH5StreamParentLoc->openGroup( "acquisitions" ) );
            LDEBUG( mlog, "Opened acquisition group in <read> mode" );

            // turn HDF5 error printing back on
            H5::Exception::setAutoPrint( tAutoPrintFunc, tClientData );

            try
            {
                H5::Attribute tAttrNAcq( fH5StreamParentLoc->openAttribute( "n_acquisitions" ) );
                tAttrNAcq.read( tAttrNAcq.getDataType(), &fNAcquisitions );
                H5::Attribute tAttrNRec( fH5StreamParentLoc->openAttribute( "n_records" ) );
                tAttrNRec.read( tAttrNRec.getDataType(), &fNRecordsInFile );
                BuildIndex();
            }
            catch( H5::Exception& )
            {
                throw M4Exception() << "Acquisitions group is not properly setup for reading\n";
            }

            LDEBUG( mlog, "Number of acquisitions found: " << fNAcquisitions << "; Number of records found: " << fNRecordsInFile );
            fMode = kRead;
        }
        catch( H5::Exception& )
        {
            // if we ended up here, the acquisitions group doesn't exist, so we must be in write mode

            // turn HDF5 error printing back on
            H5::Exception::setAutoPrint( tAutoPrintFunc, tClientData );

            try
            {
                fH5AcqLoc = new H5::Group( fH5StreamParentLoc->createGroup( "acquisitions" ) );
                LDEBUG( mlog, "Opened acquisition group in <write> mode" );
                fMode = kWrite;
            }
            catch( H5::Exception& )
            {
                throw M4Exception() << "Unable to open new acquisitions group for writing\n";
            }
        }

        Initialize();
#endif

        // Determine if we're in read or write mode and get/create the acquisitions group
        // See if there are any acquisition records currently in file
std::cout << "See if there are any acquisition records for: " << fStreamParentLoc->path() << std::endl;

          // handle to stream's "acquisitions" group/dataset
          fAcqLoc = new z5GroupHandle(*fStreamParentLoc, "acquisitions");     // <file>/streams/streamsN/acquisitions

          // Programmer's Note: readAttributes() doesn't fail if there are no attributes.
          // The nlohmann::json component is empty in that case.
          nlohmann::json acqGroupAttr;
          z5::readAttributes( *fAcqLoc, acqGroupAttr );        // read the existing attributes

        if( (acqGroupAttr.contains("n_acquisitions") == true) &&
            (acqGroupAttr.contains("n_records") == true) )
        { // "acquisitions" attributes exist == read mode
std::cout << "existing \"acquisitions\" records in file\n";

            LDEBUG( mlog, "Opened acquisition group in <read> mode" );

            try
            { // Store existing attributes into class

                fNAcquisitions = (unsigned)acqGroupAttr["n_acquisitions"];
                fNRecordsInFile = (unsigned)acqGroupAttr["n_records"];

std::cout << "fNAcquisitions: " << fNAcquisitions << std::endl;
std::cout << "fNRecordsInFile: " << fNAcquisitions << std::endl;
                BuildIndex();

                LDEBUG( mlog, "Number of acquisitions found: " << fNAcquisitions << "; Number of records found: " << fNRecordsInFile );
                fMode = kRead;      // read mode
            }
            catch (const nlohmann::json::exception& e)
            { // JSON error
                throw M4Exception() << "M4Stream::M4Stream(): JSON error: " << e.what();
            }
        }
        else
        { // no "acquisitions" attributes == write mode, create new
std::cout << "no \"acquisitions\" in file\n";

            LDEBUG( mlog, "Opened acquisition group in <write> mode" );

            // Programmer's Note: This is a handle to dataset to be created in 
            // M4Stream::Initialize() by z5::openDataset()
            fAcqLoc = new z5GroupHandle(*fStreamParentLoc, "acquisitions");     // <file>/streams/streamsN/acquisitions
            fMode = kWrite;  // write mode
        }

        // Initialize the stream
        Initialize();

std::cout << "M4Stream::M4Stream() : void "  << aHeader.GetLabel() << std::endl;
    }

    /*************************************************************************
    * @brief Destroy the M4Stream::M4Stream object
    * 
    *************************************************************************/
    M4Stream::~M4Stream()
    {
std::cout << "M4Stream::~M4Stream() DTOR\n";

#if 0 // original HDF5 code
        delete fH5DataSpaceUser; fH5DataSpaceUser = NULL;
        delete fZ5CurrentAcqDataSet; fH5CurrentAcqDataSet = nullptr;
        delete fZ5AcqLoc; fH5AcqLoc = nullptr;
        delete fZ5StreamParentLoc; fH5StreamParentLoc = nullptr;

        delete [] fChannelRecords;
#endif
        // avoid releasing unallocated memory: segfault
        if(fDataSpaceUser != nullptr)
        {
//          delete fDataSpaceUser;
            fDataSpaceUser.reset();   // std::unique_ptr<z5::Dataset>
        }
//      fDataSpaceUser = nullptr;

        // avoid releasing unallocated memory: segfault
        if(fCurrentAcqDataSet != nullptr)
        {
            delete fCurrentAcqDataSet;
        }
        fCurrentAcqDataSet = nullptr;

        // avoid releasing unallocated memory: segfault
        if(fAcqLoc != nullptr)
        {
            delete fAcqLoc;
        }
        fAcqLoc = nullptr;

        // avoid releasing unallocated memory: segfault
        // <file>/streams/streamN
        if(fStreamParentLoc != nullptr)
        {
            delete fStreamParentLoc;
        }
        fStreamParentLoc = nullptr;

        // avoid releasing unallocated memory: segfault
        if(fChannelRecords != nullptr)
        {
            delete[] fChannelRecords;
        }

        //TODO: release fMutexPtr created in CTOR M4Stream()
std::cout << "M4Stream::~M4Stream() DTOR: void\n";
    }

    /*************************************************************************
    * @brief Initialize M4Stream, opens dataset
    * @return none
    *************************************************************************/
    void M4Stream::Initialize() const
    {
std::cout << "M4Stream::Initialize()\n";  
      
#if 0 // original HDF5 code
        LDEBUG( mlog, "Initializing stream" );
        fIsInitialized = false;

        // The case where the access format is separate, but the data in the file is interleaved is special.
        // In this case, the stream record memory is not used.
        // Reading and writing is done directly from the channel records using HDF5's interleaving capabilities.
        if( fAccessFormat == sSeparate && fDataInterleaved && fNChannels != 1 )
        {
            // no memory is allocated for the stream record
            fStreamRecord.Initialize();

            // allocate memory for each channel record
            for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
            {
                fChannelRecords[ iChan ].Initialize( fChanRecNBytes );
            }

            // set the read/write functions to the special versions
            fDoReadRecord = &M4Stream::ReadRecordInterleavedToSeparate;
            fDoWriteRecord = &M4Stream::WriteRecordSeparateToInterleaved;

            // Arrays for HDF5 file reading/writing
            fStrDataDims[ 0 ] = 1;                 fStrDataDims[ 1 ] = fStrRecSize * fSampleSize;
            fStrMaxDataDims[ 0 ] = H5S_UNLIMITED;  fStrMaxDataDims[ 1 ] = fStrRecSize * fSampleSize;
            fStrDataChunkDims[ 0 ] = 1;            fStrDataChunkDims[ 1 ] = fStrRecSize * fSampleSize;
            fDataDims1Rec[ 0 ] = 1;             fDataDims1Rec[ 1 ] = fChanRecSize * fSampleSize;
            fDataOffset[ 0 ] = 0;               fDataOffset[ 1 ] = 0;
            fDataStride[ 0 ] = 1;               fDataStride[ 1 ] = fNChannels;
            fDataBlock[ 0 ] = 1;                fDataBlock[ 1 ] = fSampleSize;
            /*
            std::cout << "str data dims: " << fStrDataDims[0] << " " << fStrDataDims[1] << std::endl;
            std::cout << "str max data dims: " << fStrMaxDataDims[0] << " " << fStrMaxDataDims[1] << std::endl;
            std::cout << "str data chunk dims: " << fStrDataChunkDims[0] << " " << fStrDataChunkDims[1] << std::endl;
            std::cout << "str data dims 1 rec: " << fDataDims1Rec[0] << " " << fDataDims1Rec[1] << std::endl;
            std::cout << "str data offset: " << fDataOffset[0] << " " << fDataOffset[1] << std::endl;
            std::cout << "str data stride: " << fDataStride[0] << " " << fDataStride[1] << std::endl;
            std::cout << "str data block: " << fDataBlock[0] << " " << fDataBlock[1] << std::endl;
            */

            // HDF5 object initialization
            delete fH5DataSpaceUser;
            fH5DataSpaceUser = new H5::DataSpace( N_DATA_DIMS, fDataDims1Rec, NULL );

            fIsInitialized = true;
            return;
#endif

        LDEBUG( mlog, "Initializing stream" );
        fIsInitialized = false;

        // The case where the access format is separate, but the data in the file is interleaved is special.
        // In this case, the stream record memory is not used.
        // Reading and writing is done directly from the channel records using HDF5's interleaving capabilities.
        if( fAccessFormat == sSeparate && 
            fDataInterleaved && 
            fNChannels != 1 )
        { // Interleaved, multichannel records
std::cout << "Interleaved, multichannel records\n";

            // no memory is allocated for the stream record, only clears any existing
            fStreamRecord.Initialize();

            // allocate memory for each channel record
            for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
            {
                fChannelRecords[ iChan ].Initialize( fChanRecNBytes );
            }

            // set the read/write functions to the special versions
            fDoReadRecord = &M4Stream::ReadRecordInterleavedToSeparate;
            fDoWriteRecord = &M4Stream::WriteRecordSeparateToInterleaved;

            // Arrays for data file reading/writing
            // fStrDataDims[]  This keeps track of how much data has been recorded.  
            // [0] is the number of records recorded, and 
            // [1] is the size of a stream record in bytes
            fStrDataDims[ 0 ] = 1;                              // record is one array row
            fStrDataDims[ 1 ] = fStrRecSize * fSampleSize;      // number of columns to bytes
            
            // fStrMaxDataDims[]  This is used to create the HDF5 dataset object.  
            // It has unlimited rows, and its width is the size of a stream record in bytes
            fStrMaxDataDims[ 0 ] = -1;                            //H5S_UNLIMITED;  
            fStrMaxDataDims[ 1 ] = fStrRecSize * fSampleSize;     // number of columns to bytes
            
            // fStrDataChunkDims[]  In HDF5 a chunk is the size of data that is contiguous, 
            // and helps inform how the file is structured.  In this case the dimensions are 
            // always 1 record (i.e. 1 row) by a stream record in bytes wide.
            fStrDataChunkDims[ 0 ] = 1;                           // access sections by row
            fStrDataChunkDims[ 1 ] = fStrRecSize * fSampleSize;   // number of columns to bytes
            
            // fDataDims1Rec[]  This describes the size of the data object that a user interacts with.  
            // So when the data is accessed as separate channels, its one channel record wide in bytes.  
            // When data is not accessed separately, its one stream record wide in bytes.
            fDataDims1Rec[ 0 ] = 1;             
            fDataDims1Rec[ 1 ] = fStrRecSize * fSampleSize;
            
            // fDataOffset[]  This says where within the data space the data reading/writing starts.  
            // So how many records (i.e. rows) down, and how many bytes over (only non-zero if 
            // using separate-channel access)
            fDataOffset[ 0 ] = 0;               
            fDataOffset[ 1 ] = 0;
            
            // fDataStride[]  (Noah) I believe this specifies how far to move in memory between reads/writes
            fDataStride[ 0 ] = 1;                   // stride by array row
            fDataStride[ 1 ] = fNChannels;          // stride by number of channels per record???
            
            // fDataBlock[]  (Noah) I dont actually remember the significance of this or why its fSampleSize 
            // for separate access, but the stream record size in the other case
            fDataBlock[ 0 ] = 1;                    // data block is one array row
            fDataBlock[ 1 ] = fSampleSize;

//std::cout << "str data dims: " << fStrDataDims[0] << " " << fStrDataDims[1] << std::endl;
//std::cout << "str max data dims: " << fStrMaxDataDims[0] << " " << fStrMaxDataDims[1] << std::endl;
//std::cout << "str data chunk dims: " << fStrDataChunkDims[0] << " " << fStrDataChunkDims[1] << std::endl;
//std::cout << "str data dims 1 rec: " << fDataDims1Rec[0] << " " << fDataDims1Rec[1] << std::endl;
//std::cout << "str data offset: " << fDataOffset[0] << " " << fDataOffset[1] << std::endl;
//std::cout << "str data stride: " << fDataStride[0] << " " << fDataStride[1] << std::endl;
//std::cout << "str data block: " << fDataBlock[0] << " " << fDataBlock[1] << std::endl;

            // Data space object initialization
            if(fDataSpaceUser != nullptr)
            { // release any existing data space
//              delete fDataSpaceUser;
              fDataSpaceUser.reset(); // std::unique_ptr<z5::Dataset>
            }

            // A dataspace defines the size and shape of the dataset or attribute raw data, 
            // and must be defined when the dataset or attribute is created.
            // A Dataset contains the data values themselves.
            
            // A dataset is an object composed of a collection of data elements, or raw data, 
            // and metadata that stores a description of the data elements, data layout, and all other 
            // information necessary to write, read, and interpret the stored data.

///@todo verify fDataDims1Rec[1]
            std::vector<size_t> shape = { N_DATA_DIMS,fDataDims1Rec[1] };       // <row>,<col>
            std::vector<size_t> chunks = { N_DATA_DIMS,fDataDims1Rec[1] };      // <row>,<col>

            // map lookup/convert type to string
            z5::types::Datatypes::InverseDtypeMap& N5type = z5::types::Datatypes::dtypeToN5();
            std::string strDataType = N5type[fDataTypeInFile];     
std::cout << "stream datatype: " << strDataType << std::endl;

            // Programmer's Note: std::unique_ptr<z5::Dataset>
            fDataSpaceUser = z5::openDataset( *fStreamParentLoc, "acquisitions");

            fIsInitialized = true;
            return;
        }

#if 0 // original HDF5 code
        // allocate stream record memory
        fStreamRecord.Initialize( fStrRecNBytes );

        // channel records point to portions of the stream record and do not own their own data
        byte_type* tChanDataPtr = fStreamRecord.GetData();
        for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
        {
            fChannelRecords[ iChan ].Initialize( fStreamRecord.GetRecordIdPtr(), fStreamRecord.GetTimePtr(), tChanDataPtr + fChanRecNBytes*iChan );
        }

        // set the read/write functions to the general versions
        fDoReadRecord = &M4Stream::ReadRecordAsIs;
        fDoWriteRecord = &M4Stream::WriteRecordAsIs;

        // Arrays for HDF5 file reading/writing
        fStrDataDims[ 0 ] = 1;                 fStrDataDims[ 1 ] = fStrRecSize * fSampleSize;
        fStrMaxDataDims[ 0 ] = H5S_UNLIMITED;  fStrMaxDataDims[ 1 ] = fStrRecSize * fSampleSize;
        fStrDataChunkDims[ 0 ] = 1;            fStrDataChunkDims[ 1 ] = fStrRecSize * fSampleSize;
        fDataDims1Rec[ 0 ] = 1;             fDataDims1Rec[ 1 ] = fStrRecSize * fSampleSize;
        fDataOffset[ 0 ] = 0;               fDataOffset[ 1 ] = 0;
        fDataStride[ 0 ] = 1;               fDataStride[ 1 ] = fSampleSize;
        fDataBlock[ 0 ] = 1;                fDataBlock[ 1 ] = fStrRecSize * fSampleSize;
        /*
        std::cout << "str data dims: " << fStrDataDims[0] << " " << fStrDataDims[1] << std::endl;
        std::cout << "str max data dims: " << fStrMaxDataDims[0] << " " << fStrMaxDataDims[1] << std::endl;
        std::cout << "str data chunk dims: " << fStrDataChunkDims[0] << " " << fStrDataChunkDims[1] << std::endl;
        std::cout << "str data dims 1 rec: " << fDataDims1Rec[0] << " " << fDataDims1Rec[1] << std::endl;
        std::cout << "str data offset: " << fDataOffset[0] << " " << fDataOffset[1] << std::endl;
        std::cout << "str data stride: " << fDataStride[0] << " " << fDataStride[1] << std::endl;
        std::cout << "str data block: " << fDataBlock[0] << " " << fDataBlock[1] << std::endl;
        */

        // HDF5 object initialization
        delete fH5DataSpaceUser;
        fH5DataSpaceUser = new H5::DataSpace( N_DATA_DIMS, fDataDims1Rec, NULL );

        fIsInitialized = true;
        return;
#endif

std::cout << "non-interleaved records\n";
        // allocate stream record memory
        fStreamRecord.Initialize( fStrRecNBytes );

        // channel records point to portions of the stream record and do not own their own data
        byte_type* tChanDataPtr = fStreamRecord.GetData();
        for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
        {
std::cout << "Record: " << iChan << " initialize\n";
            fChannelRecords[ iChan ].Initialize( fStreamRecord.GetRecordIdPtr(), fStreamRecord.GetTimePtr(), tChanDataPtr + fChanRecNBytes*iChan );
        }

        // set the read/write functions to the general versions
        fDoReadRecord = &M4Stream::ReadRecordAsIs;
        fDoWriteRecord = &M4Stream::WriteRecordAsIs;

        // Arrays for HDF5 file reading/writing

        // fStrDataDims  This keeps track of how much data has been recorded.  
        // [0] is the number of records recorded, and 
        // [1] is the size of a stream record in bytes
        fStrDataDims[ 0 ] = 1;                 
        fStrDataDims[ 1 ] = fStrRecSize * fSampleSize;

        // fStrMaxDataDims  This is used to create the HDF5 dataset object.  
        // It has unlimited rows, and its width is the size of a stream record in bytes
        fStrMaxDataDims[ 0 ] = -1; //H5S_UNLIMITED;  
        fStrMaxDataDims[ 1 ] = fStrRecSize * fSampleSize;

        // fStrDataChunkDims  In HDF5 a chunk is the size of data that is contiguous, 
        // and helps inform how the file is structured.  In this case the dimensions are 
        // always 1 record (i.e. 1 row) by a stream record in bytes wide.
        fStrDataChunkDims[ 0 ] = 1;            
        fStrDataChunkDims[ 1 ] = fStrRecSize * fSampleSize;

        // fDataDims1Rec  This describes the size of the data object that a user interacts with.  
        // So when the data is accessed as separate channels, its one channel record wide in bytes.  
        // When data is not accessed separately, its one stream record wide in bytes.
        fDataDims1Rec[ 0 ] = 1;             
        fDataDims1Rec[ 1 ] = fStrRecSize * fSampleSize;

        // fDataOffset  This says where within the data space the data reading/writing starts.  
        // So how many records (i.e. rows) down, and how many bytes over (only non-zero if 
        // using separate-channel access)
        fDataOffset[ 0 ] = 0;               
        fDataOffset[ 1 ] = 0;

        // fDataStride  I believe this specifies how far to move in memory between reads/writes
        fDataStride[ 0 ] = 1;               
        fDataStride[ 1 ] = fSampleSize;

        // fDataBlock  I dont actually remember the significance of this or why its fSampleSize 
        // for separate access, but the stream record size in the other case
        fDataBlock[ 0 ] = 1;                
        fDataBlock[ 1 ] = fStrRecSize * fSampleSize;

//std::cout << "str data dims: " << fStrDataDims[0] << " " << fStrDataDims[1] << std::endl;
//std::cout << "str max data dims: " << fStrMaxDataDims[0] << " " << fStrMaxDataDims[1] << std::endl;
//std::cout << "str data chunk dims: " << fStrDataChunkDims[0] << " " << fStrDataChunkDims[1] << std::endl;
//std::cout << "str data dims 1 rec: " << fDataDims1Rec[0] << " " << fDataDims1Rec[1] << std::endl;
//std::cout << "str data offset: " << fDataOffset[0] << " " << fDataOffset[1] << std::endl;
//std::cout << "str data stride: " << fDataStride[0] << " " << fDataStride[1] << std::endl;
//std::cout << "str data block: " << fDataBlock[0] << " " << fDataBlock[1] << std::endl;

        // map lookup to convert type to string for dataset type
        z5::types::Datatypes::InverseDtypeMap& N5type = z5::types::Datatypes::dtypeToN5();
        std::string strDataType = N5type[fDataTypeInFile];     
std::cout << "stream datatype: " << strDataType << std::endl;

        std::vector<size_t> shape = { 1,fStrRecSize };       // <row>,<col>
        std::vector<size_t> chunks = { 1,fStrRecSize };      // <row>,<col>

        // Programmer's Note: std::unique_ptr<z5::Dataset>
        fDataSpaceUser = z5::openDataset( *fStreamParentLoc, "acquisitions");

        fIsInitialized = true;
std::cout << "M4Stream::Initialize(): void\n";        
    }

    /*************************************************************************
    * @brief Get the pointer to the stream record
    * 
    * @return const M4Record* 
    *************************************************************************/
    const M4Record* M4Stream::GetStreamRecord() const
    {
        return &fStreamRecord;
    }

    /*************************************************************************
    * @brief Get the pointer to a particular channel record
    * 
    * @param aChannel Selected stream channel
    * @return const M4Record* 
    *************************************************************************/
    const M4Record* M4Stream::GetChannelRecord( unsigned aChannel ) const
    {
        if( aChannel < fNChannels )
        {
            return &(fChannelRecords[ aChannel ]);
        }
        throw M4Exception() << "Channel <" << aChannel << "> requested; only " << fNChannels << " in this stream.";
    }

    /*************************************************************************
    * @brief 
    * 
    * @param anOffset 
    * @param aIfNewAcqStartAtFirstRec 
    * @return bool
    *   - true Sucessful read
    *   - false Read failure
    *************************************************************************/
    bool M4Stream::ReadRecord( int anOffset, bool aIfNewAcqStartAtFirstRec ) const
    {
std::cout << "M4Stream::ReadRecord()\n";
#if 0 // Original HDF5 code
        if( ! fIsInitialized ) Initialize();

        std::unique_lock< std::mutex >( *fMutexPtr.get() );

        // anOffset should not move us forward if this is the very first record read 
        // in the file (fRecordsAccessed == false).  Otherwise anOffset should be incremented 
        // to 1 to move us forward appropriately (fRecordsAccessed == true)
        anOffset += (int)fRecordsAccessed;

        LDEBUG( mlog, "Before moving: Record count in file = " << fRecordCountInFile << "; Record ID (in acquisition) = " << fRecordCountInAcq );

        if( ( anOffset < 0 && (unsigned)abs( anOffset ) > fRecordCountInFile ) ||
            ( anOffset > 0 && fRecordCountInFile + anOffset >= fNRecordsInFile ) ||
            ( anOffset == 0 && fNRecordsInFile == 0 ))
        {
            // either requested to go back before the beginning of the file, or past the end
            LDEBUG( mlog, "Requested offset would move is out of range for the file" );
            return false;
        }

        fRecordCountInFile = fRecordCountInFile + anOffset;
        unsigned nextAcq = fRecordIndex.at( fRecordCountInFile ).first;
        fRecordCountInAcq = fRecordIndex.at( fRecordCountInFile ).second;
        LDEBUG( mlog, "After offset calculation: Record count in file = " << fRecordCountInFile << "; Record ID (in acquisition) = " << fRecordCountInAcq );

        try
        {
            bool tIsNewAcq = false;
            if( nextAcq != fAcquisitionId || ! fRecordsAccessed )
            { // we are going to a new acquisition
                
                // check if we need to correct our position in the new acquisition back to the 
                // beginning of the acquisition
                if( aIfNewAcqStartAtFirstRec && fRecordCountInAcq != 0 )
                {
                    fRecordCountInFile -= fRecordCountInAcq;
                    
                    // make sure the record correction ended up in the same new acquisition
                    if( fRecordIndex.at( fRecordCountInFile ).first != nextAcq )
                    {
                        throw M4Exception() << "Tried to start at the beginning of the new acquisition, but ended up in a different acquisition: " << fRecordIndex.at( fRecordCountInFile ).first << " != " << nextAcq;
                    }
                    fRecordCountInAcq = 0;
                    LDEBUG( mlog, "After offset calc + new acq correction: Record count in file = " << fRecordCountInFile << "; Record ID (in acquisition) = " << fRecordCountInAcq );
                }

                tIsNewAcq = true;
                fAcquisitionId = nextAcq;
                delete fCurrentAcqDataSet;
                u32toa( fAcquisitionId, fAcqNameBuffer );
                fH5CurrentAcqDataSet = new H5::DataSet( fH5AcqLoc->openDataSet( fAcqNameBuffer ) );
                H5::Attribute tAttrNRIA( fCurrentAcqDataSet->openAttribute( "n_records" ) );
                tAttrNRIA.read( tAttrNRIA.getDataType(), &fNRecordsInAcq );
            }

            LDEBUG( mlog, "Going to record: record count in file: " << fRecordCountInFile << " -- acquisition: " << nextAcq << " -- record in acquisition: " << fRecordCountInAcq );

            fDataOffset[ 0 ] = fRecordCountInAcq;

            (this->*fDoReadRecord)( tIsNewAcq );

            // can now update the first record in the file
            if( ! fRecordsAccessed )
            {
                fRecordsAccessed = true;
                fFirstRecordInFile = fAcqFirstRecId;
                LDEBUG( mlog, "First record in file: " << fFirstRecordInFile );
            }
        }
        catch( H5::Exception& e )
        {
            throw M4Exception() << "HDF5 error while reading a record:\n\t" << e.getCDetailMsg() << " (function: " << e.getFuncName() << ")";
        }

#endif

std::cout << "M4Stream::ReadRecord(): void\n";
        return true;
    }

    /*************************************************************************
    * @brief Close the M4Stream, release handles
    * 
    *************************************************************************/
    void M4Stream::Close() const
    {
std::cout << "M4Stream::Close()\n";
        LDEBUG( mlog, "const M4Stream::Close()" );

//      delete fDataSpaceUser;
//      fDataSpaceUser = nullptr;
        fDataSpaceUser.reset();   // std::unique_ptr<z5::Dataset>

        delete fCurrentAcqDataSet; 
        fCurrentAcqDataSet = nullptr;

        delete fAcqLoc; 
        fAcqLoc = nullptr;

        delete fStreamParentLoc; 
        fStreamParentLoc = NULL;
std::cout << "M4Stream::Close(): void\n";
    }

    /*************************************************************************
    * @brief Return read/write access to stream channel record
    * 
    * @param aChannel Selected channel
    * @return M4Record* 
    *************************************************************************/
    M4Record* M4Stream::GetChannelRecord( unsigned aChannel )
    {
        if( aChannel < fNChannels )
        {
            return &(fChannelRecords[ aChannel ]);
        }
        throw M4Exception() << "Channel <" << aChannel << "> requested; only " << fNChannels << " in this stream.";
    }

    /*************************************************************************
    * @brief Write stream record
    * 
    * @param aIsNewAcquisition 
    * @return bool
    *   - true Sucessful write
    *   - false Write failure
    *************************************************************************/
    bool M4Stream::WriteRecord( bool aIsNewAcquisition )
    {
std::cout << "M4Stream::WriteRecord()\n";
        // note: fRecordCountInAcq is used to keep track of the number of records written in each acquisition;
        //       fNRecordsInAcq is only valid for the last completed acquisition.

#if 0 // Original HDF5 code
        if( ! fIsInitialized ) Initialize();

        if( ! fRecordsAccessed ) aIsNewAcquisition = true;

        try
        {
            std::unique_lock< std::mutex >( *fMutexPtr.get() );

            if( aIsNewAcquisition )
            { // New acquisition, create new dataset

                FinalizeCurrentAcq();

                if( fRecordsAccessed ) 
                {
                    ++fAcquisitionId;
                }
                else 
                {
                    fRecordsAccessed = true;
                }

                // Setup the new dataset
                fStrDataDims[ 0 ] = 1;
                H5::DSetCreatPropList tPropList;
                tPropList.setChunk( N_DATA_DIMS, fStrDataChunkDims );

                u32toa( fAcquisitionId, fAcqNameBuffer );
                fCurrentAcqDataSet = new H5::DataSet( fH5AcqLoc->createDataSet( fAcqNameBuffer, fDataTypeInFile, 
                H5::DataSpace( N_DATA_DIMS, fStrDataDims, fStrMaxDataDims ), tPropList ) );
            }
            else
            { // Extend the current dataset
                
                fStrDataDims[ 0 ] = fStrDataDims[ 0 ] + 1;
                fCurrentAcqDataSet->extend( fStrDataDims );
            }

            LTRACE( mlog, "Writing acq. " << fAcquisitionId << ", record " << fRecordCountInAcq );

            fDataOffset[ 0 ] = fRecordCountInAcq;

            (this->*fDoWriteRecord)( aIsNewAcquisition );

            ++fRecordCountInAcq;
            ++fRecordCountInFile;

            return true;
        }
        catch( H5::Exception& e )
        {
            LWARN( mlog, "DIAGNOSTIC: id of fCurrentAcqDataSet: " << fCurrentAcqDataSet->getId() );
            LWARN( mlog, "DIAGNOSTIC: class name: " << fCurrentAcqDataSet->fromClass() );
            H5D_space_status_t t_status;
            fCurrentAcqDataSet->getSpaceStatus( t_status );
            LWARN( mlog, "DIAGNOSTIC: offset: " << fCurrentAcqDataSet->getOffset() << "  space status: " << t_status << "  storage size: " << fH5CurrentAcqDataSet->getStorageSize() << "  in mem data size: " << fH5CurrentAcqDataSet->getInMemDataSize() );
            throw M4Exception() << "HDF5 error while writing a record:\n\t" << e.getCDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        catch( std::exception& e )
        {
            throw M4Exception() << e.what();
        }

#endif
std::cout << "M4Stream::WriteRecord(): void\n";
        return false;
    }

    /*************************************************************************
    * @brief Close stream, release handles
    * 
    *************************************************************************/
    void M4Stream::Close()
    {
std::cout << "M4Stream::Close()\n";        

        LDEBUG( mlog, "non-const M4Stream::Close()" );
        FinalizeStream();

        if(fDataSpaceUser != nullptr)
        {
//        delete fDataSpaceUser;
          fDataSpaceUser.reset();   // std::unique_ptr<z5::Dataset>
        }
//      fDataSpaceUser = nullptr;

        if(fCurrentAcqDataSet != nullptr)
        {
          delete fCurrentAcqDataSet;
        }
        fCurrentAcqDataSet = nullptr;

        if(fAcqLoc != nullptr)
        {
          delete fAcqLoc;
        }
        fAcqLoc = nullptr;

        if(fStreamParentLoc != nullptr)
        {
          delete fStreamParentLoc;
        }
        fStreamParentLoc = nullptr;
std::cout << "M4Stream::Close(): void\n";        
    }

    /*************************************************************************
    * @brief Configure access to M4Stream
    * 
    * @param aFormat 
    *************************************************************************/
    void M4Stream::SetAccessFormat( uint32_t aFormat ) const
    {
        fAccessFormat = aFormat;
        fIsInitialized = false;
    }

    /*************************************************************************
    * @brief 
    * 
    * @param aIsNewAcquisition 
    *************************************************************************/
    void M4Stream::ReadRecordInterleavedToSeparate( bool aIsNewAcquisition ) const
    {
std::cout << "M4Stream::ReadRecordInterleavedToSeparate()\n";

#if 0 // original HDF5 code
        if( aIsNewAcquisition )
        {
            // try
            // {
                delete [] fAcqFirstRecTimes;
                fAcqFirstRecTimes = new TimeType[ fNChannels ];
                H5::Attribute tAttrAFRT( fCurrentAcqDataSet->openAttribute( "first_record_time" ) );
                tAttrAFRT.read( tAttrAFRT.getDataType(), fAcqFirstRecTimes );

                delete [] fAcqFirstRecIds;
                fAcqFirstRecIds = new RecordIdType[ fNChannels ];
                H5::Attribute tAttrAFRI( fCurrentAcqDataSet->openAttribute( "first_record_id" ) );
                tAttrAFRI.read( tAttrAFRI.getDataType(), fAcqFirstRecIds );
            // }
            // catch( H5::Exception& )
            // {
            //     // Backwards compatibility with older files that don't have first_record_time and first_record_id
            //     // Times increment by the record length starting at 0, but ID increments starting at 0
            //     for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
            //     {
            //         fAcqFirstRecTimes[ iChan ] = 0;
            //         fAcqFirstRecIds[ iChan ] = 0;
            //     }
            // }
            fAcqFirstRecTime = fAcqFirstRecTimes[0];
            fAcqFirstRecId = fAcqFirstRecIds[0];
        }

        H5::DataSpace tDataSpaceInFile = fH5CurrentAcqDataSet->getSpace();
        for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
        {
            fDataOffset[ 1 ] = iChan;

            tDataSpaceInFile.selectHyperslab( H5S_SELECT_SET, fDataDims1Rec, fDataOffset, fDataStride, fDataBlock );
            fCurrentAcqDataSet->read( fChannelRecords[ iChan ].GetData(), fDataTypeUser, *fDataSpaceUser, tDataSpaceInFile );
            fChannelRecords[ iChan ].SetTime( fAcqFirstRecTimes[ iChan ] + fRecordCountInAcq * fChanRecLength );
            fChannelRecords[ iChan ].SetRecordId( fAcqFirstRecIds[ iChan ] + fRecordCountInAcq );
        }
#endif

std::cout << "M4Stream::ReadRecordInterleavedToSeparate(): void\n";
    }

    /*************************************************************************
    * @brief 
    * 
    * @param aIsNewAcquisition 
    *************************************************************************/
    void M4Stream::ReadRecordAsIs( bool aIsNewAcquisition ) const
    {
std::cout << "M4Stream::ReadRecordAsIs()\n";

#if 0 // original HDF5 code
        if( aIsNewAcquisition )
        {
            // try
            // {
                H5::Attribute tAttrAFRT( fCurrentAcqDataSet->openAttribute( "first_record_time" ) );
                tAttrAFRT.read( tAttrAFRT.getDataType(), &fAcqFirstRecTime );

                H5::Attribute tAttrAFRI( fCurrentAcqDataSet->openAttribute( "first_record_id" ) );
                tAttrAFRI.read( tAttrAFRI.getDataType(), &fAcqFirstRecId );
            // }
            // catch( H5::Exception& )
            // {
            //     // Backwards compatibility with older files that don't have first_record_time and first_record_id
            //     // Times increment by the record length starting at 0, but ID increments starting at 0
            //     fAcqFirstRecTime = 0;
            //     fAcqFirstRecId = 0;
            //     for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
            //     {
            //         fAcqFirstRecTimes[ iChan ] = 0;
            //         fAcqFirstRecIds[ iChan ] = 0;
            //     }
            // }
        }

        H5::DataSpace tDataSpaceInFile = fCurrentAcqDataSet->getSpace();
        tDataSpaceInFile.selectHyperslab( H5S_SELECT_SET, fDataDims1Rec, fDataOffset );
        
        fCurrentAcqDataSet->read( fStreamRecord.GetData(), fDataTypeUser, *fDataSpaceUser, tDataSpaceInFile );
        fStreamRecord.SetTime( fAcqFirstRecTime + fRecordCountInAcq * fChanRecLength );
        fStreamRecord.SetRecordId( fAcqFirstRecId + fRecordCountInAcq );
        for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
        {
            fChannelRecords[ iChan ].SetTime( fStreamRecord.GetTime() );
            fChannelRecords[ iChan ].SetRecordId( fStreamRecord.GetRecordId() );
        }
#endif

std::cout << "M4Stream::ReadRecordAsIs(): void\n";
    }

    /*************************************************************************
    * @brief 
    * 
    * @param aIsNewAcquisition 
    *************************************************************************/
    void M4Stream::WriteRecordSeparateToInterleaved( bool aIsNewAcquisition )
    {
std::cout << "M4Stream::WriteRecordSeparateToInterleaved()\n";

#if 0 // original HDF5 code
        H5::DataSpace tDataSpaceInFile = fCurrentAcqDataSet->getSpace();
        for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
        {
            fDataOffset[ 1 ] = iChan;
            tDataSpaceInFile.selectHyperslab( H5S_SELECT_SET, fDataDims1Rec, fDataOffset, fDataStride, fDataBlock );
            //std::cout << "about to write separate to interleaved  " << fDataTypeUser.fromClass() << std::endl;
            fCurrentAcqDataSet->write( fChannelRecords[ iChan ].GetData(), fDataTypeUser, *fDataSpaceUser, tDataSpaceInFile );
        }

        if( aIsNewAcquisition )
        {
            TimeType* tTimes = new TimeType[ fNChannels ];
            RecordIdType* tIds = new RecordIdType[ fNChannels ];

            for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
            {
                tTimes[ iChan ] = fChannelRecords[ iChan ].GetTime();
                tIds[ iChan ] = fChannelRecords[ iChan ].GetRecordId();
            }
            fCurrentAcqDataSet->createAttribute( "first_record_time", MH5Type< TimeType >::H5(), H5::DataSpace( H5S_SCALAR ) ).write( MH5Type< TimeType >::Native(), tTimes );
            fCurrentAcqDataSet->createAttribute( "first_record_id", MH5Type< RecordIdType >::H5(), H5::DataSpace( H5S_SCALAR ) ).write( MH5Type< RecordIdType >::Native(), tIds );
            delete [] tTimes;
            delete [] tIds;
        }
#endif

std::cout << "M4Stream::WriteRecordSeparateToInterleaved(): void\n";
    }

    /*************************************************************************
    * @brief 
    * 
    * @param aIsNewAcquisition 
    *************************************************************************/
    void M4Stream::WriteRecordAsIs( bool aIsNewAcquisition )
    {
std::cout << "M4Stream::WriteRecordAsIs()\n";

#if 0 // original HDF5 code
        H5::DataSpace tDataSpaceInFile = fCurrentAcqDataSet->getSpace();

        tDataSpaceInFile.selectHyperslab( H5S_SELECT_SET, fDataDims1Rec, fDataOffset );
        fCurrentAcqDataSet->write( fStreamRecord.GetData(), fDataTypeUser, *fDataSpaceUser, tDataSpaceInFile );
        
        if( aIsNewAcquisition )
        {
            TimeType tTime = fStreamRecord.GetTime();
            RecordIdType tId = fStreamRecord.GetRecordId();
            fCurrentAcqDataSet->createAttribute( "first_record_time", MH5Type< TimeType >::H5(), H5::DataSpace( H5S_SCALAR ) ).write( MH5Type< TimeType >::Native(), &tTime );
            fCurrentAcqDataSet->createAttribute( "first_record_id", MH5Type< RecordIdType >::H5(), H5::DataSpace( H5S_SCALAR ) ).write( MH5Type< RecordIdType >::Native(), &tId );
        }
#endif

std::cout << "M4Stream::WriteRecordAsIs(): void\n";
    }

    /*************************************************************************
    * @brief Read data file index
    * 
    *************************************************************************/
    void M4Stream::BuildIndex() const
    {
std::cout << "M4Stream::BuildIndex()\n";  
      
#if 0 // original HDF5 code
        fRecordIndex.resize( fNRecordsInFile );
        unsigned tNRecInAcq;
        unsigned iRecInFile = 0;
        for( unsigned iAcq = 0; iAcq < fNAcquisitions; ++iAcq )
        {
            u32toa( iAcq, fAcqNameBuffer );
            H5::Attribute tAttr( fH5AcqLoc->openDataSet( fAcqNameBuffer ).openAttribute( "n_records" ) );
            tAttr.read( tAttr.getDataType(), &tNRecInAcq );
            LDEBUG( mlog, "Acquisition <" << fAcqNameBuffer << "> has " << tNRecInAcq << " records" );
            for( unsigned iRecInAcq = 0; iRecInAcq < tNRecInAcq; ++iRecInAcq )
            {
                fRecordIndex.at( iRecInFile ).first = iAcq;
                fRecordIndex.at( iRecInFile ).second = iRecInAcq;
                LTRACE( mlog, "Record index: " << iRecInFile << " -- " << iAcq << " -- " << iRecInAcq );
                ++iRecInFile;
            }
        }
        return;
#endif    
    
std::cout << "M4Stream::BuildIndex(): void\n";        
    }

    /*************************************************************************
    * @brief Write the final stream-acquisition attributes at closing
    *   <file>/streams/streamN/acquisitions/<acqIdr>/n_records
    *************************************************************************/
    void M4Stream::FinalizeCurrentAcq()
    {
std::cout << "M4Stream::FinalizeCurrentAcq()\n";

#if 0  // original HDF5 code
        if( fCurrentAcqDataSet == nullptr ) return;
        fNRecordsInAcq = fRecordCountInAcq;

        fCurrentAcqDataSet->createAttribute( "n_records", MH5Type< unsigned >::H5(), H5::DataSpace( H5S_SCALAR ) ).write( MH5Type< unsigned >::Native(), &fNRecordsInAcq );
        LDEBUG( mlog, "Finalizing acq. " << fAcquisitionId << " with " << fNRecordsInAcq << " records" );

        fRecordCountInAcq = 0;

        delete fCurrentAcqDataSet;
#endif      
        if( fCurrentAcqDataSet == nullptr ) 
        {
///@todo one entry, one exit!
std::cout << "M4Stream::FinalizeCurrentAcq(): empty fCurrentAcqDataSet: " << fStreamParentLoc->path() << std::endl;
          return;
        }

        fNRecordsInAcq = fRecordCountInAcq;
  
std::cout << "Finalize acquistion for: " << fCurrentAcqDataSet->path() << std::endl;
        // Programmer's Note: fStreamParentLoc: <file>/streams/streamN/<acqId>
        // create attr: for acquisitions this stream 
        nlohmann::json jacqAttr;
        jacqAttr["n_records"] = fNRecordsInAcq;

        // Programmer's Note: This will update exiting atribute "n_records" with new value
        // <file>/streams/streamN/acquisitions/<acqId>/<attrib>
std::cout << "Write <file>/streams/streamN/<acqId>/<attrib>: " << fStreamParentLoc->path() << std::endl;
        z5::writeAttributes(*fCurrentAcqDataSet, jacqAttr);

        LDEBUG( mlog, "Finalizing acq. " << fAcquisitionId << " with " << fNRecordsInAcq << " records" );
        fRecordCountInAcq = 0;
        fCurrentAcqDataSet = nullptr;
    }

    /*************************************************************************
    * @brief Write the final stream attributes at closing
    *   <file>/streams/streamN/n_acquisitions
    *   <file>/streams/streamN/n_records
    *************************************************************************/
    void M4Stream::FinalizeStream()
    {
std::cout << "M4Stream::FinalizeStream()\n";   
     
#if 0   // original HDF5 code
        FinalizeCurrentAcq();

        if( fH5AcqLoc == NULL ) return;

        fNAcquisitions = ( fAcquisitionId + 1 ) * (unsigned)fRecordsAccessed;
        fH5StreamParentLoc->openAttribute( "n_acquisitions" ).write( MH5Type< unsigned >::Native(), &fNAcquisitions );
        fH5StreamParentLoc->openAttribute( "n_records" ).write( MH5Type< unsigned >::Native(), &fRecordCountInFile );
        LDEBUG( mlog, "Finalizing stream with " << fNAcquisitions << " acquisitions and " << fRecordCountInFile << " records" );

        return;
#endif

        FinalizeCurrentAcq();

///@todo one entry, one exit
        if( fAcqLoc == nullptr ) 
        {
std::cout << "M4Stream::FinalizeStream(): empty fAcqLoc: " << fStreamParentLoc->path() << std::endl;
          return;
        }

        fNAcquisitions = ( fAcquisitionId + 1 ) * (unsigned)fRecordsAccessed;

        // Programmer's Note: fStreamParentLoc: <file>/streams/streamN
        // create attr: for acquisitions this stream: 
        nlohmann::json jacqAttr;

        jacqAttr["n_acquisitions"] = fNAcquisitions;
        jacqAttr["n_records"] = fRecordCountInFile;

        // Programmer's Note: This will update atributes "n_acquisitions" and "n_records" with new values
        // <file>/streams/streamN/<attrib>
std::cout << "Write <file>/streams/streamN/<attrib>: " << fStreamParentLoc->path() << std::endl;
std::cout << "n_acquisitions: " << fNAcquisitions << " n_records: " << fRecordCountInFile << std::endl;
        z5::writeAttributes(*fStreamParentLoc, jacqAttr);

        LDEBUG( mlog, "Finalizing stream with " << fNAcquisitions << " acquisitions and " << fRecordCountInFile << " records" );

std::cout << "M4Stream::FinalizeStream(): void\n";        
    }
} /* namespace monarch */
