/*
 * M3Stream.cc
 *
 *  Created on: Dec 26, 2014
 *      Author: nsoblath
 */

#define M3_API_EXPORTS

#include "M3Stream.hh"

#include "M3IToA.hh"
#include "logger.hh"

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

namespace monarch3
{
    LOGGER( mlog, "M3Stream" );

    M3Stream::M3Stream( const M3StreamHeader& aHeader, H5::CommonFG* aH5StreamsLoc, uint32_t aAccessFormat ) :
            fMode( kRead ),
            fDoReadRecord( NULL ),
            fDoWriteRecord( NULL ),
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
            fChannelRecords( new M3Record[ aHeader.GetNChannels() ] ),
            fNAcquisitions( 0 ),
            fAcquisitionId( 0 ),
            fRecordCountInAcq( 0 ),
            fNRecordsInAcq( 0 ),
            fAcqFirstRecTime( 0 ),
            fAcqFirstRecId( 0 ),
            fAcqFirstRecTimes( NULL ),
            fAcqFirstRecIds( NULL ),
            fDataInterleaved( aHeader.GetChannelFormat() == sInterleaved ),
            fAccessFormat( aAccessFormat ),
            fRecordIndex(),
            fRecordCountInFile( 0 ),
            fNRecordsInFile( 0 ),
            fH5StreamParentLoc( new H5::Group( aH5StreamsLoc->openGroup( aHeader.GetLabel() ) ) ),
            fH5AcqLoc( NULL ),
            fH5CurrentAcqDataSet( NULL ),
            fH5DataSpaceUser( NULL ),
            fMutexPtr( new std::mutex() )
    {
        LDEBUG( mlog, "Creating stream for <" << aHeader.GetLabel() << ">" );

        if( aHeader.GetDataFormat() == sDigitizedUS )
        {
            switch( fDataTypeSize )
            {
                case 1:
                    fDataTypeInFile = H5::PredType::STD_U8LE;
                    fDataTypeUser = H5::PredType::NATIVE_UINT8;
                   break;
                case 2:
                    fDataTypeInFile = H5::PredType::STD_U16LE;
                    fDataTypeUser = H5::PredType::NATIVE_UINT16;
                    break;
                case 4:
                    fDataTypeInFile = H5::PredType::STD_U32LE;
                    fDataTypeUser = H5::PredType::NATIVE_UINT32;
                    break;
                case 8:
                    fDataTypeInFile = H5::PredType::STD_U64LE;
                    fDataTypeUser = H5::PredType::NATIVE_UINT64;
                    break;
                default:
                    throw M3Exception() << "Unknown unsigned integer data type size: " << fDataTypeSize;
            }
        }
        else if( aHeader.GetDataFormat() == sDigitizedS )
        {
            switch( fDataTypeSize )
            {
                case 1:
                    fDataTypeInFile = H5::PredType::STD_I8LE;
                    fDataTypeUser = H5::PredType::NATIVE_INT8;
                   break;
                case 2:
                    fDataTypeInFile = H5::PredType::STD_I16LE;
                    fDataTypeUser = H5::PredType::NATIVE_INT16;
                    break;
                case 4:
                    fDataTypeInFile = H5::PredType::STD_I32LE;
                    fDataTypeUser = H5::PredType::NATIVE_INT32;
                    break;
                case 8:
                    fDataTypeInFile = H5::PredType::STD_I64LE;
                    fDataTypeUser = H5::PredType::NATIVE_INT64;
                    break;
                default:
                    throw M3Exception() << "Unknown signed integer data type size: " << fDataTypeSize;
            }
        }
        else // aHeader.GetDataFormat() == sAnalog
        {
            switch( fDataTypeSize )
            {
                case 4:
                    fDataTypeInFile = H5::PredType::IEEE_F32LE;
                    fDataTypeUser = H5::PredType::NATIVE_FLOAT;
                    break;
                case 8:
                    fDataTypeInFile = H5::PredType::IEEE_F64LE ;
                    fDataTypeUser = H5::PredType::NATIVE_DOUBLE;
                    break;
                default:
                    throw M3Exception() << "Unknown floating-point data type size: " << fDataTypeSize;
            }
        }

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
                throw M3Exception() << "Acquisitions group is not properly setup for reading\n";
            }

            LDEBUG( mlog, "\tNumber of acquisitions found: " << fNAcquisitions << "; Number of records found: " << fNRecordsInFile );
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
                throw M3Exception() << "Unable to open new acquisitions group for writing\n";
            }
        }

        Initialize();
    }

    M3Stream::~M3Stream()
    {
        delete fH5DataSpaceUser; fH5DataSpaceUser = NULL;
        delete fH5CurrentAcqDataSet; fH5CurrentAcqDataSet = NULL;
        delete fH5AcqLoc; fH5AcqLoc = NULL;
        delete fH5StreamParentLoc; fH5StreamParentLoc = NULL;

        delete [] fChannelRecords;
    }

    void M3Stream::Initialize() const
    {
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
            fDoReadRecord = &M3Stream::ReadRecordInterleavedToSeparate;
            fDoWriteRecord = &M3Stream::WriteRecordSeparateToInterleaved;

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
        }

        // allocate stream record memory
        fStreamRecord.Initialize( fStrRecNBytes );

        // channel records point to portions of the stream record and do not own their own data
        byte_type* tChanDataPtr = fStreamRecord.GetData();
        for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
        {
            fChannelRecords[ iChan ].Initialize( fStreamRecord.GetRecordIdPtr(), fStreamRecord.GetTimePtr(), tChanDataPtr + fChanRecNBytes*iChan );
        }

        // set the read/write functions to the general versions
        fDoReadRecord = &M3Stream::ReadRecordAsIs;
        fDoWriteRecord = &M3Stream::WriteRecordAsIs;

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
    }

    const M3Record* M3Stream::GetStreamRecord() const
    {
        return &fStreamRecord;
    }

    const M3Record* M3Stream::GetChannelRecord( unsigned aChannel ) const
    {
        if( aChannel < fNChannels )
        {
            return &(fChannelRecords[ aChannel ]);
        }
        throw M3Exception() << "Channel <" << aChannel << "> requested; only " << fNChannels << " in this stream.";
    }

    bool M3Stream::ReadRecord( int anOffset, bool aIfNewAcqStartAtFirstRec ) const
    {
        if( ! fIsInitialized ) Initialize();

        std::unique_lock< std::mutex >( *fMutexPtr.get() );

        // anOffset should not move us forward if this is the very first record read in the file (fRecordsAccessed == false)
        // Otherwise anOffset should be incremented to 1 to move us forward appropriately (fRecordsAccessed == true)
        anOffset += (int)fRecordsAccessed;

        if( ( anOffset < 0 && (unsigned)abs( anOffset ) > fRecordCountInFile ) ||
            ( anOffset > 0 && fRecordCountInFile + anOffset >= fNRecordsInFile ) ||
            ( anOffset == 0 && fNRecordsInFile == 0 ))
        {
            // either requested to go back before the beginning of the file, or past the end
            return false;
        }

        fRecordCountInFile = fRecordCountInFile + anOffset;
        unsigned nextAcq = fRecordIndex.at( fRecordCountInFile ).first;
        fRecordCountInAcq = fRecordIndex.at( fRecordCountInFile ).second;

        try
        {
            bool tIsNewAcq = false;
            if( nextAcq != fAcquisitionId || ! fRecordsAccessed )
            {
                // we are going to a new acquisition

                // check if we need to correct our position in the new acquisition back to the beginning of the acquisition
                if( aIfNewAcqStartAtFirstRec && fRecordCountInAcq != 0 )
                {
                    fRecordCountInFile -= fRecordCountInAcq;
                    // make sure the record correction ended up in the same new acquisition
                    if( fRecordIndex.at( fRecordCountInFile ).first != nextAcq )
                    {
                        throw M3Exception() << "Tried to start at the beginning of the new acquisition, but ended up in a different acquisition: " << fRecordIndex.at( fRecordCountInFile ).first << " != " << nextAcq;
                    }
                    fRecordCountInAcq = 0;
                }

                tIsNewAcq = true;
                fRecordsAccessed = true;
                fAcquisitionId = nextAcq;
                delete fH5CurrentAcqDataSet;
                u32toa( fAcquisitionId, fAcqNameBuffer );
                fH5CurrentAcqDataSet = new H5::DataSet( fH5AcqLoc->openDataSet( fAcqNameBuffer ) );
                H5::Attribute tAttrNRIA( fH5CurrentAcqDataSet->openAttribute( "n_records" ) );
                tAttrNRIA.read( tAttrNRIA.getDataType(), &fNRecordsInAcq );
            }

            LDEBUG( mlog, "Going to record: record in file: " << fRecordCountInFile << " -- acquisition: " << nextAcq << " -- record in acquisition: " << fRecordCountInAcq );

            fDataOffset[ 0 ] = fRecordCountInAcq;

            (this->*fDoReadRecord)( tIsNewAcq );

            // fix fRecordCountInFile; e.g. if a file doesn't start at record 0, we need to fix the record count value after reading the record
            if( tIsNewAcq )
            {
                fRecordCountInFile = fAcqFirstRecId;
                LDEBUG( mlog, "Updated record in file: " << fRecordCountInFile );
            }
        }
        catch( H5::Exception& e )
        {
            throw M3Exception() << "HDF5 error while reading a record:\n\t" << e.getCDetailMsg() << " (function: " << e.getFuncName() << ")";
        }

        return true;
    }

    void M3Stream::Close() const
    {
        //LDEBUG( mlog, "const M3Stream::Close()" );

        delete fH5DataSpaceUser; fH5DataSpaceUser = NULL;
        delete fH5CurrentAcqDataSet; fH5CurrentAcqDataSet = NULL;
        delete fH5AcqLoc; fH5AcqLoc = NULL;
        delete fH5StreamParentLoc; fH5StreamParentLoc = NULL;

        return;
    }


    M3Record* M3Stream::GetChannelRecord( unsigned aChannel )
    {
        if( aChannel < fNChannels )
        {
            return &(fChannelRecords[ aChannel ]);
        }
        throw M3Exception() << "Channel <" << aChannel << "> requested; only " << fNChannels << " in this stream.";
    }

    bool M3Stream::WriteRecord( bool aIsNewAcquisition )
    {
        // note: fRecordCountInAcq is used to keep track of the number of records written in each acquisition;
        //       fNRecordsInAcq is only valid for the last completed acquisition.

        if( ! fIsInitialized ) Initialize();
        if( ! fRecordsAccessed ) aIsNewAcquisition = true;

        try
        {
            std::unique_lock< std::mutex >( *fMutexPtr.get() );

            if( aIsNewAcquisition )
            {
                FinalizeCurrentAcq();

                if( fRecordsAccessed ) ++fAcquisitionId;
                else fRecordsAccessed = true;

                // Setup the new dataset
                fStrDataDims[ 0 ] = 1;
                H5::DSetCreatPropList tPropList;
                tPropList.setChunk( N_DATA_DIMS, fStrDataChunkDims );

                u32toa( fAcquisitionId, fAcqNameBuffer );
                fH5CurrentAcqDataSet = new H5::DataSet( fH5AcqLoc->createDataSet( fAcqNameBuffer, fDataTypeInFile, H5::DataSpace( N_DATA_DIMS, fStrDataDims, fStrMaxDataDims ), tPropList ) );
            }
            else
            {
                // Extend the current dataset
                fStrDataDims[ 0 ] = fStrDataDims[ 0 ] + 1;
                fH5CurrentAcqDataSet->extend( fStrDataDims );
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
            throw M3Exception() << "HDF5 error while writing a record:\n\t" << e.getCDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        catch( std::exception& e )
        {
            throw M3Exception() << e.what();
        }

        return false;
    }

    void M3Stream::Close()
    {
        //LDEBUG( mlog, "non-const M3Stream::Close()" );
        FinalizeStream();

        delete fH5DataSpaceUser; fH5DataSpaceUser = NULL;
        delete fH5CurrentAcqDataSet; fH5CurrentAcqDataSet = NULL;
        delete fH5AcqLoc; fH5AcqLoc = NULL;
        delete fH5StreamParentLoc; fH5StreamParentLoc = NULL;

        return;
    }

    void M3Stream::SetAccessFormat( uint32_t aFormat ) const
    {
        fAccessFormat = aFormat;
        fIsInitialized = false;
        return;
    }

    void M3Stream::ReadRecordInterleavedToSeparate( bool aIsNewAcquisition ) const
    {
        if( aIsNewAcquisition )
        {
            try
            {
                delete [] fAcqFirstRecTimes;
                fAcqFirstRecTimes = new TimeType[ fNChannels ];
                H5::Attribute tAttrAFRT( fH5CurrentAcqDataSet->openAttribute( "first_record_time" ) );
                tAttrAFRT.read( tAttrAFRT.getDataType(), fAcqFirstRecTimes );

                delete [] fAcqFirstRecIds;
                fAcqFirstRecIds = new RecordIdType[ fNChannels ];
                H5::Attribute tAttrAFRI( fH5CurrentAcqDataSet->openAttribute( "first_record_id" ) );
                tAttrAFRI.read( tAttrAFRI.getDataType(), fAcqFirstRecIds );
            }
            catch( H5::Exception& )
            {
                // Backwards compatibility with older files that don't have first_record_time and first_record_id
                // Times increment by the record length starting at 0, but ID increments starting at 0
                for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
                {
                    fAcqFirstRecTimes[ iChan ] = 0;
                    fAcqFirstRecIds[ iChan ] = 0;
                }
            }
            fAcqFirstRecTime = fAcqFirstRecTimes[0];
            fAcqFirstRecId = fAcqFirstRecIds[0];
        }

        H5::DataSpace tDataSpaceInFile = fH5CurrentAcqDataSet->getSpace();
        for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
        {
            fDataOffset[ 1 ] = iChan;
            tDataSpaceInFile.selectHyperslab( H5S_SELECT_SET, fDataDims1Rec, fDataOffset, fDataStride, fDataBlock );
            fH5CurrentAcqDataSet->read( fChannelRecords[ iChan ].GetData(), fDataTypeUser, *fH5DataSpaceUser, tDataSpaceInFile );
            fChannelRecords[ iChan ].SetTime( fAcqFirstRecTimes[ iChan ] + fRecordCountInAcq * fChanRecLength );
            fChannelRecords[ iChan ].SetRecordId( fAcqFirstRecIds[ iChan ] + fRecordCountInAcq );
        }
        return;
    }

    void M3Stream::ReadRecordAsIs( bool aIsNewAcquisition ) const
    {
        if( aIsNewAcquisition )
        {
            try
            {
                H5::Attribute tAttrAFRT( fH5CurrentAcqDataSet->openAttribute( "first_record_time" ) );
                tAttrAFRT.read( tAttrAFRT.getDataType(), &fAcqFirstRecTime );
                H5::Attribute tAttrAFRI( fH5CurrentAcqDataSet->openAttribute( "first_record_id" ) );
                tAttrAFRI.read( tAttrAFRI.getDataType(), &fAcqFirstRecId );
            }
            catch( H5::Exception& )
            {
                // Backwards compatibility with older files that don't have first_record_time and first_record_id
                // Times increment by the record length starting at 0, but ID increments starting at 0
                fAcqFirstRecTime = 0;
                fAcqFirstRecId = 0;
                for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
                {
                    fAcqFirstRecTimes[ iChan ] = 0;
                    fAcqFirstRecIds[ iChan ] = 0;
                }
            }
        }

        H5::DataSpace tDataSpaceInFile = fH5CurrentAcqDataSet->getSpace();
        tDataSpaceInFile.selectHyperslab( H5S_SELECT_SET, fDataDims1Rec, fDataOffset );
        fH5CurrentAcqDataSet->read( fStreamRecord.GetData(), fDataTypeUser, *fH5DataSpaceUser, tDataSpaceInFile );
        fStreamRecord.SetTime( fAcqFirstRecTime + fRecordCountInAcq * fChanRecLength );
        fStreamRecord.SetRecordId( fAcqFirstRecId + fRecordCountInAcq );
        for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
        {
            fChannelRecords[ iChan ].SetTime( fStreamRecord.GetTime() );
            fChannelRecords[ iChan ].SetRecordId( fStreamRecord.GetRecordId() );
        }
        return;
    }

    void M3Stream::WriteRecordSeparateToInterleaved( bool aIsNewAcquisition )
    {
        H5::DataSpace tDataSpaceInFile = fH5CurrentAcqDataSet->getSpace();
        for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
        {
            fDataOffset[ 1 ] = iChan;
            tDataSpaceInFile.selectHyperslab( H5S_SELECT_SET, fDataDims1Rec, fDataOffset, fDataStride, fDataBlock );
            //std::cout << "about to write separate to interleaved  " << fDataTypeUser.fromClass() << std::endl;
            fH5CurrentAcqDataSet->write( fChannelRecords[ iChan ].GetData(), fDataTypeUser, *fH5DataSpaceUser, tDataSpaceInFile );
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
            fH5CurrentAcqDataSet->createAttribute( "first_record_time", MH5Type< TimeType >::H5(), H5::DataSpace( H5S_SCALAR ) ).write( MH5Type< TimeType >::Native(), tTimes );
            fH5CurrentAcqDataSet->createAttribute( "first_record_id", MH5Type< RecordIdType >::H5(), H5::DataSpace( H5S_SCALAR ) ).write( MH5Type< RecordIdType >::Native(), tIds );
            delete [] tTimes;
            delete [] tIds;
        }
        return;
    }

    void M3Stream::WriteRecordAsIs( bool aIsNewAcquisition )
    {
        H5::DataSpace tDataSpaceInFile = fH5CurrentAcqDataSet->getSpace();
        tDataSpaceInFile.selectHyperslab( H5S_SELECT_SET, fDataDims1Rec, fDataOffset );
        fH5CurrentAcqDataSet->write( fStreamRecord.GetData(), fDataTypeUser, *fH5DataSpaceUser, tDataSpaceInFile );
        if( aIsNewAcquisition )
        {
            TimeType tTime = fStreamRecord.GetTime();
            RecordIdType tId = fStreamRecord.GetRecordId();
            fH5CurrentAcqDataSet->createAttribute( "first_record_time", MH5Type< TimeType >::H5(), H5::DataSpace( H5S_SCALAR ) ).write( MH5Type< TimeType >::Native(), &tTime );
            fH5CurrentAcqDataSet->createAttribute( "first_record_id", MH5Type< RecordIdType >::H5(), H5::DataSpace( H5S_SCALAR ) ).write( MH5Type< RecordIdType >::Native(), &tId );
        }
        return;
    }

    void M3Stream::BuildIndex() const
    {
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
    }

    void M3Stream::FinalizeCurrentAcq()
    {
        if( fH5CurrentAcqDataSet == NULL ) return;

        fNRecordsInAcq = fRecordCountInAcq;

        fH5CurrentAcqDataSet->createAttribute( "n_records", MH5Type< unsigned >::H5(), H5::DataSpace( H5S_SCALAR ) ).write( MH5Type< unsigned >::Native(), &fNRecordsInAcq );
        LDEBUG( mlog, "Finalizing acq. " << fAcquisitionId << " with " << fNRecordsInAcq << " records" );

        fRecordCountInAcq = 0;
        delete fH5CurrentAcqDataSet;
        fH5CurrentAcqDataSet = NULL;

        return;
    }

    void M3Stream::FinalizeStream()
    {
        FinalizeCurrentAcq();

        if( fH5AcqLoc == NULL ) return;

        fNAcquisitions = ( fAcquisitionId + 1 ) * (unsigned)fRecordsAccessed;
        fH5StreamParentLoc->openAttribute( "n_acquisitions" ).write( MH5Type< unsigned >::Native(), &fNAcquisitions );
        fH5StreamParentLoc->openAttribute( "n_records" ).write( MH5Type< unsigned >::Native(), &fRecordCountInFile );
        LDEBUG( mlog, "Finalizing stream with " << fNAcquisitions << " acquisitions and " << fRecordCountInFile << " records" );

        return;
    }

} /* namespace monarch */
