/*
 * MStream.cc
 *
 *  Created on: Dec 26, 2014
 *      Author: nsoblath
 */

#include "MStream.hh"

#include "MIToA.hh"
#include "MLogger.hh"

/* Notes on reading:
 *
 * During reading, both fNRecordsInAcq and fRecordsCount are used, as well as both fAcquisitionId and fNAcquisitions;
 * All four variables will always be valid.
 *
 */

/* Notes on writing:
 *
 * As writing progresses, fRecordCount is incremented, and fNRecordsInAcq is ignored.
 * When the acquisition is finalized with FinalizeCurrentAcq(), fNRecordsInAcq is updated to be fRecordCount + 1.
 * Therefore fRecordCount is only valid for the last completed acquisition.
 *
 * As writing progresses, fAcquisitionId is incremented and fNAcquisitions is ignored.
 * When the stream is finalized with FinalizeStream(), fNAcquisitions is updated to be fAcquisitionId + 1.
 * Therefore fNAcquisitions is only valid after the stream is finalized.
 *
 */

namespace monarch
{
    MLOGGER( mlog, "MStream" );

    MStream::MStream( const MStreamHeader& aHeader, H5::CommonFG* aH5StreamsLoc, MultiChannelFormatType aAccessFormat ) :
            fMode( kRead ),
            fAcquisitionId( 0 ),
            fNAcquisitions( 0 ),
            fIsInitialized( false ),
            fRecordsAccessed( false ),
            fDataTypeSize( aHeader.GetDataTypeSize() ),
            fStrRecNBytes( aHeader.GetNChannels() * aHeader.GetRecordSize() * aHeader.GetDataTypeSize() ),
            fStrRecSize( aHeader.GetNChannels() * aHeader.GetRecordSize() ),
            fChanRecNBytes( aHeader.GetRecordSize() * aHeader.GetDataTypeSize() ),
            fChanRecSize( aHeader.GetRecordSize() ),
            fStreamRecord(),
            fNChannels( aHeader.GetNChannels() ),
            fChannelRecords( new MRecord[ aHeader.GetNChannels() ] ),
            fRecordCount( 0 ),
            fNRecordsInAcq( 0 ),
            fDataInterleaved( aHeader.GetChannelFormat() == sInterleaved ),
            fAccessFormat( aAccessFormat ),
            fDoReadRecord( NULL ),
            fDoWriteRecord( NULL ),
            fH5StreamParentLoc( new H5::Group( aH5StreamsLoc->openGroup( aHeader.GetLabel() ) ) ),
            fH5AcqLoc( NULL ),
            fH5CurrentAcqDataSet( NULL ),
            fH5DataSpaceUser( NULL )
    {
        MDEBUG( mlog, "Creating stream for <" << aHeader.GetLabel() << ">" );
        std::cout << "stream record at: " << &fStreamRecord << std::endl;

        for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
        {
            std::cout << "channel " << iChan << " record at: " << &(fChannelRecords[iChan]) << std::endl;
        }
        if( aHeader.GetDataFormat() == sDigitized )
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
                    throw MException() << "Unknown integer data type size: " << fDataTypeSize;
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
                    throw MException() << "Unknown floating-point data type size: " << fDataTypeSize;
            }
        }

        // Determine if we're in read or write mode
        // and get/create the acquisitions group
        // Nested exceptions are used so that the outer try block can be used to determine whether we're reading or writing
        try
        {
            fH5AcqLoc = new H5::Group( fH5StreamParentLoc->openGroup( "acquisitions" ) );
            MDEBUG( mlog, "Opened acquisition group in <read> mode" );
            try
            {
                fH5AcqLoc->openAttribute( "n_acquisitions" ).read( MH5TypeAccess< unsigned >::GetType(), &fNAcquisitions );
            }
            catch( H5::Exception& e2 )
            {
                throw;
            }
            MDEBUG( mlog, "\tNumber of acquisitions found: " << fNAcquisitions );
            fMode = kRead;
        }
        catch( H5::Exception& e1 )
        {
            // if we ended up here, the acquisitions group doesn't exist, so we must be in write mode
            try
            {
                fH5AcqLoc = new H5::Group( fH5StreamParentLoc->createGroup( "acquisitions" ) );
                MDEBUG( mlog, "Opened acquisition group in <write> mode" );
                fMode = kWrite;
            }
            catch( H5::Exception& e2 )
            {
                throw;
            }
        }

        Initialize();
    }

    MStream::~MStream()
    {
        delete fH5DataSpaceUser; fH5DataSpaceUser = NULL;
        delete fH5CurrentAcqDataSet; fH5CurrentAcqDataSet = NULL;
        delete fH5AcqLoc; fH5AcqLoc = NULL;
        delete fH5StreamParentLoc; fH5StreamParentLoc = NULL;

        delete [] fChannelRecords;
    }

    void MStream::Initialize() const
    {
        MDEBUG( mlog, "Initializing stream" );
        fIsInitialized = false;

        // The case where the access format is separate, but the data in the file is interleaved is special.
        // In this case, the stream record memory is not used.
        // Reading and writing is done directly from the channel records using HDF5's interleaving capabilities.
        if( fAccessFormat == sSeparate && fDataInterleaved && fNChannels != 1 )
        {
            // no memory is allocated for the stream record
            fStreamRecord.SetData();

            // allocate memory for each channel record
            for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
            {
                fChannelRecords[ iChan ].SetData( fChanRecNBytes );
            }

            // set the read/write functions to the special versions
            fDoReadRecord = &MStream::ReadRecordInterleavedToSeparate;
            fDoWriteRecord = &MStream::WriteRecordSeparateToInterleaved;

            // Arrays for HDF5 file reading/writing
            fStrDataDims[ 0 ] = 1;                 fStrDataDims[ 1 ] = fStrRecSize;
            fStrMaxDataDims[ 0 ] = H5S_UNLIMITED;  fStrMaxDataDims[ 1 ] = fStrRecSize;
            fStrDataChunkDims[ 0 ] = 1;            fStrDataChunkDims[ 1 ] = fStrRecSize;
            fDataDims1Rec[ 0 ] = 1;             fDataDims1Rec[ 1 ] = fChanRecSize;
            fDataOffset[ 0 ] = 0;               fDataOffset[ 1 ] = 0;
            fDataStride[ 0 ] = 1;               fDataStride[ 1 ] = 2;
            fDataBlock[ 0 ] = 1;                fDataBlock[ 1 ] = 1;

            // HDF5 object initialization
            delete fH5DataSpaceUser;
            fH5DataSpaceUser = new H5::DataSpace( N_DATA_DIMS, fDataDims1Rec, NULL );

            fIsInitialized = true;
            return;
        }

        // allocate stream record memory
        fStreamRecord.SetData( fStrRecNBytes );

        // channel records point to portions of the stream record and do not own their own data
        byte_type* tChanDataPtr = fStreamRecord.GetData();
        for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
        {
            fChannelRecords[ iChan ].SetData( tChanDataPtr + fChanRecNBytes*iChan );
        }

        // set the read/write functions to the general versions
        fDoReadRecord = &MStream::ReadRecordAsIs;
        fDoWriteRecord = &MStream::WriteRecordAsIs;

        // Arrays for HDF5 file reading/writing
        fStrDataDims[ 0 ] = 1;                 fStrDataDims[ 1 ] = fStrRecSize;
        fStrMaxDataDims[ 0 ] = H5S_UNLIMITED;  fStrMaxDataDims[ 1 ] = fStrRecSize;
        fStrDataChunkDims[ 0 ] = 1;            fStrDataChunkDims[ 1 ] = fStrRecSize;
        fDataDims1Rec[ 0 ] = 1;             fDataDims1Rec[ 1 ] = fStrRecSize;
        fDataOffset[ 0 ] = 0;               fDataOffset[ 1 ] = 0;
        fDataStride[ 0 ] = 0;               fDataStride[ 1 ] = 0;
        fDataBlock[ 0 ] = 1;                fDataBlock[ 1 ] = fStrRecSize;

        // HDF5 object initialization
        delete fH5DataSpaceUser;
        fH5DataSpaceUser = new H5::DataSpace( N_DATA_DIMS, fDataDims1Rec, NULL );


        fIsInitialized = true;
        return;
    }

    const MRecord* MStream::GetStreamRecord() const
    {
        return &fStreamRecord;
    }

    const MRecord* MStream::GetChannelRecord( unsigned aChannel ) const
    {
        if( aChannel < fNChannels )
        {
            return &(fChannelRecords[ aChannel ]);
        }
        throw MException() << "Channel <" << aChannel << "> requested; only " << fNChannels << " in this stream.";
    }

    bool MStream::ReadRecord() const
    {
        if( ! fIsInitialized ) Initialize();

        ++fRecordCount;
        if( fRecordCount == fNRecordsInAcq || fH5CurrentAcqDataSet == NULL )
        {
            // Move to the next acquisition
            if( fRecordsAccessed ) ++fAcquisitionId;
            else fRecordsAccessed = true;
            fRecordCount = 0;

            delete fH5CurrentAcqDataSet;
            fH5CurrentAcqDataSet = NULL;

            if( fAcquisitionId == fNAcquisitions )
            {
                // end of file reached
                return false;
            }

            u32toa( fAcquisitionId, fAcqNameBuffer );
            fH5CurrentAcqDataSet = new H5::DataSet( fH5AcqLoc->openDataSet( fAcqNameBuffer ) );
            fH5CurrentAcqDataSet->openAttribute( "n_records" ).read( MH5TypeAccess< unsigned >::GetType(), &fNRecordsInAcq );
        }

        fDataOffset[ 0 ] = fRecordCount;

        (this->*fDoReadRecord)();
        //H5::DataSpace readSpace( N_DATA_DIMS, fDataDims1Rec, NULL );
        //H5::DataSpace fileSpace = fH5CurrentAcqDataSet->getSpace();
        //fileSpace.selectHyperslab( H5S_SELECT_SET, fDataDims1Rec, fDataOffset );
        //fH5CurrentAcqDataSet->read( fStreamRecord.GetData(), fDataTypeUser, readSpace, fileSpace );

        return true;
    }

    void MStream::Close() const
    {
        MDEBUG( mlog, "const MStream::Close()" );

        delete fH5DataSpaceUser; fH5DataSpaceUser = NULL;
        delete fH5CurrentAcqDataSet; fH5CurrentAcqDataSet = NULL;
        delete fH5AcqLoc; fH5AcqLoc = NULL;
        delete fH5StreamParentLoc; fH5StreamParentLoc = NULL;

        return;
    }


    MRecord* MStream::GetStreamRecord()
    {
        return &fStreamRecord;
    }

    MRecord* MStream::GetChannelRecord( unsigned aChannel )
    {
        if( aChannel < fNChannels )
        {
            return &(fChannelRecords[ aChannel ]);
        }
        throw MException() << "Channel <" << aChannel << "> requested; only " << fNChannels << " in this stream.";
    }

    bool MStream::WriteRecord( bool aIsNewAcquisition )
    {
        // note: fRecordCount is used to keep track of the number of records written in each acquisition;
        //       fNRecordsInAcq is only valid for the last completed acquisition.

        if( ! fIsInitialized ) Initialize();

        try
        {
            if( aIsNewAcquisition )
            {
                FinalizeCurrentAcq();

                if( fRecordsAccessed ) ++fAcquisitionId;
                else fRecordsAccessed = true;

                // Setup the new dataset
                fStrDataDims[ 0 ] = 1;
                //H5::DataSpace* tExtDataspace = new H5::DataSpace( N_DATA_DIMS, fDataDims, fMaxDataDims );
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

            MDEBUG( mlog, "Writing acq. " << fAcquisitionId << ", record " << fRecordCount );

            fDataOffset[ 0 ] = fRecordCount;

            (this->*fDoWriteRecord)();
            //H5::DataSpace writeSpace( N_DATA_DIMS, fDataDims1Rec, NULL );
            //H5::DataSpace fileSpace = fH5CurrentAcqDataSet->getSpace();
            //fileSpace.selectHyperslab( H5S_SELECT_SET, fDataDims1Rec, fDataOffset );
            //fH5CurrentAcqDataSet->write( fStreamRecord.GetData(), fDataTypeUser, writeSpace, fileSpace );

            ++fRecordCount;
            return true;
        }
        catch( H5::Exception& e )
        {
            MERROR( mlog, "HDF5 error while writing a record:\n\t" << e.getCDetailMsg() << " (function: " << e.getFuncName() << ")" );
        }
        catch( MException& e )
        {
            MERROR( mlog, e.what() );
        }

        return false;
    }

    void MStream::Close()
    {
        MDEBUG( mlog, "non-const MStream::Close()" );
        FinalizeStream();

        delete fH5DataSpaceUser; fH5DataSpaceUser = NULL;
        delete fH5CurrentAcqDataSet; fH5CurrentAcqDataSet = NULL;
        delete fH5AcqLoc; fH5AcqLoc = NULL;
        delete fH5StreamParentLoc; fH5StreamParentLoc = NULL;

        return;
    }

    void MStream::SetAccessFormat( MultiChannelFormatType aFormat ) const
    {
        fAccessFormat = aFormat;
        fIsInitialized = false;
        return;
    }

    void MStream::ReadRecordInterleavedToSeparate() const
    {
        H5::DataSpace tDataSpaceInFile = fH5CurrentAcqDataSet->getSpace();
        for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
        {
            fDataOffset[ 1 ] = iChan;
            tDataSpaceInFile.selectHyperslab( H5S_SELECT_SET, fDataDims1Rec, fDataOffset, fDataStride, fDataBlock );
            fH5CurrentAcqDataSet->read( fChannelRecords[ iChan ].GetData(), fDataTypeUser, *fH5DataSpaceUser, tDataSpaceInFile );
        }
        return;
    }

    void MStream::ReadRecordAsIs() const
    {
        H5::DataSpace tDataSpaceInFile = fH5CurrentAcqDataSet->getSpace();
        tDataSpaceInFile.selectHyperslab( H5S_SELECT_SET, fDataDims1Rec, fDataOffset );
        fH5CurrentAcqDataSet->read( fStreamRecord.GetData(), fDataTypeUser, *fH5DataSpaceUser, tDataSpaceInFile );
        return;
    }

    void MStream::WriteRecordSeparateToInterleaved()
    {
        H5::DataSpace tDataSpaceInFile = fH5CurrentAcqDataSet->getSpace();
        for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
        {
            fDataOffset[ 1 ] = iChan;
            tDataSpaceInFile.selectHyperslab( H5S_SELECT_SET, fDataDims1Rec, fDataOffset, fDataStride, fDataBlock );
            std::cout << "attempting to write from: " << fChannelRecords[ iChan ].GetData() << "  dataspaceuser: " << fH5DataSpaceUser << std::endl;
            fH5CurrentAcqDataSet->write( fChannelRecords[ iChan ].GetData(), fDataTypeUser, *fH5DataSpaceUser, tDataSpaceInFile );
        }
        return;
    }

    void MStream::WriteRecordAsIs()
    {
        //H5::DataSpace writeSpace( N_DATA_DIMS, fDataDims1Rec, NULL );
        H5::DataSpace tDataSpaceInFile = fH5CurrentAcqDataSet->getSpace();
        tDataSpaceInFile.selectHyperslab( H5S_SELECT_SET, fDataDims1Rec, fDataOffset );
        fH5CurrentAcqDataSet->write( fStreamRecord.GetData(), fDataTypeUser, *fH5DataSpaceUser, tDataSpaceInFile );
        return;
    }

    void MStream::FinalizeCurrentAcq()
    {
        if( fH5CurrentAcqDataSet == NULL ) return;

        fNRecordsInAcq = fRecordCount;

        H5::DataType tType = MH5TypeAccess< unsigned >::GetType();
        fH5CurrentAcqDataSet->createAttribute( "n_records", tType, H5::DataSpace( H5S_SCALAR ) ).write( tType, &fNRecordsInAcq );
        MDEBUG( mlog, "Finalizing acq. " << fAcquisitionId << " with " << fNRecordsInAcq << " records" );

        fRecordCount = 0;
        delete fH5CurrentAcqDataSet;
        fH5CurrentAcqDataSet = NULL;

        return;
    }

    void MStream::FinalizeStream()
    {
        FinalizeCurrentAcq();

        if( fH5AcqLoc == NULL ) return;

        fNAcquisitions = ( fAcquisitionId + 1 ) * (unsigned)fRecordsAccessed;
        H5::DataType tType = MH5TypeAccess< unsigned >::GetType();
        fH5AcqLoc->createAttribute( "n_acquisitions", tType, H5::DataSpace( H5S_SCALAR ) ).write( tType, &fNAcquisitions );
        MDEBUG( mlog, "Finalizing stream with " << fNAcquisitions << " acquisitions" );

        return;
    }

} /* namespace monarch */
