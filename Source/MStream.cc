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

    MStream::MStream( const MStreamHeader& aHeader, H5::CommonFG* aH5StreamsLoc ) :
            fMode( kRead ),
            fAcquisitionId( 0 ),
            fNAcquisitions( 0 ),
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
            fInterleaved( aHeader.GetChannelFormat() == sInterleaved ),
            fH5StreamParentLoc( new H5::Group( aH5StreamsLoc->openGroup( aHeader.GetLabel() ) ) ),
            fH5AcqLoc( NULL ),
            fH5CurrentAcqDataSet( NULL )
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

        // Arrays for HDF5 file reading/writing
        fDataDims[ 0 ] = 1; fDataDims[ 1 ] = fStrRecSize;
        fMaxDataDims[ 0 ] = H5S_UNLIMITED; fMaxDataDims[ 1 ] = fStrRecSize;
        fDataChunkDims[ 0 ] = 1; fDataChunkDims[ 1 ] = fStrRecSize;
        fDataDims1Rec[ 0 ] = 1; fDataDims1Rec[ 1 ] = fStrRecSize;
        fDataOffset[ 0 ] = 0; fDataOffset[ 1 ] = 0;

        // Determine if we're in read or write mode
        // and get/create the acquisitions group
        try
        {
            fH5AcqLoc = new H5::Group( fH5StreamParentLoc->openGroup( "acquisitions" ) );
            MDEBUG( mlog, "Opened acquisition group in <read> mode" );
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

        // Allocate the stream record
        fStreamRecord.SetData( fStrRecNBytes );

        // do channel records need their own memory allocation?
        // if single channel, or multiple channels in separate mode, then no; channel record data pointers point to the stream pointer
        // if multiple channels in interleaved mode, then yes; channel record data get their own memory
        if( fNChannels == 1 || ! fInterleaved )
        {
            byte_type* tChanDataPtr = fStreamRecord.GetData();
            for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
            {
                fChannelRecords[ iChan ].SetData( tChanDataPtr + fChanRecNBytes*iChan );
            }
        }
        else // multiple channels in interleaved mode
        {
            for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
            {
                fChannelRecords[ iChan ].SetData( fChanRecNBytes );
            }
        }

    }

    MStream::~MStream()
    {
        delete fH5CurrentAcqDataSet;
        fH5CurrentAcqDataSet = NULL;
        delete fH5AcqLoc;
        fH5AcqLoc = NULL;
        delete fH5StreamParentLoc;
        fH5StreamParentLoc = NULL;

        delete [] fChannelRecords;
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
        ++fRecordCount;
        if( fRecordCount == fNRecordsInAcq )
        {
            // Move to the next acquisition
            ++fAcquisitionId;
            fRecordCount = 0;

            delete fH5CurrentAcqDataSet;
            fH5CurrentAcqDataSet = NULL;

            if( fAcquisitionId == fNAcquisitions )
            {
                return false;
            }

            u32toa( fAcquisitionId, fAcqNameBuffer );
            fH5CurrentAcqDataSet = new H5::DataSet( fH5AcqLoc->openDataSet( fAcqNameBuffer ) );
            fH5CurrentAcqDataSet->openAttribute( "n_records" ).read( MH5TypeAccess< unsigned >::GetType(), &fNRecordsInAcq );

        }

        fDataOffset[ 0 ] = fRecordCount;

        H5::DataSpace readSpace( N_DATA_DIMS, fDataDims1Rec, NULL );
        H5::DataSpace fileSpace = fH5CurrentAcqDataSet->getSpace();
        fileSpace.selectHyperslab( H5S_SELECT_SET, fDataDims1Rec, fDataOffset );
        fH5CurrentAcqDataSet->read( fStreamRecord.GetData(), fDataTypeUser, readSpace, fileSpace );

        if( fInterleaved )
        {
            UnzipChannels();
        }

        return true;
    }

    void MStream::Close() const
    {
        MDEBUG( mlog, "const MStream::Close()" );

        delete fH5CurrentAcqDataSet;
        fH5CurrentAcqDataSet = NULL;
        delete fH5AcqLoc;
        fH5AcqLoc = NULL;
        delete fH5StreamParentLoc;
        fH5StreamParentLoc = NULL;

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

        try
        {
            if( aIsNewAcquisition )
            {
                FinalizeCurrentAcq();

                ++fAcquisitionId;

                // Setup the new dataset
                fDataDims[ 0 ] = 1;
                //H5::DataSpace* tExtDataspace = new H5::DataSpace( N_DATA_DIMS, fDataDims, fMaxDataDims );
                H5::DSetCreatPropList tPropList;
                tPropList.setChunk( N_DATA_DIMS, fDataChunkDims );

                u32toa( fAcquisitionId, fAcqNameBuffer );
                fH5CurrentAcqDataSet = new H5::DataSet( fH5AcqLoc->createDataSet( fAcqNameBuffer, fDataTypeInFile, H5::DataSpace( N_DATA_DIMS, fDataDims, fMaxDataDims ), tPropList ) );
            }
            else
            {
                // Extend the current dataset
                fDataDims[ 0 ] = fDataDims[ 0 ] + 1;
                fH5CurrentAcqDataSet->extend( fDataDims );
            }

            MDEBUG( mlog, "Writing acq. " << fAcquisitionId << ", record " << fRecordCount );

            // Write data in stream record to disk
            if( fInterleaved )
            {
                ZipChannels();
            }

            fDataOffset[ 0 ] = fRecordCount;

            H5::DataSpace writeSpace( N_DATA_DIMS, fDataDims1Rec, NULL );
            H5::DataSpace fileSpace = fH5CurrentAcqDataSet->getSpace();
            fileSpace.selectHyperslab( H5S_SELECT_SET, fDataDims1Rec, fDataOffset );
            fH5CurrentAcqDataSet->write( fStreamRecord.GetData(), fDataTypeUser, writeSpace, fileSpace );

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

        delete fH5CurrentAcqDataSet;
        fH5CurrentAcqDataSet = NULL;
        delete fH5AcqLoc;
        fH5AcqLoc = NULL;
        delete fH5StreamParentLoc;
        fH5StreamParentLoc = NULL;

        return;
    }

    void MStream::ZipChannels()
    {
        return;
    }

    void MStream::UnzipChannels() const
    {
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

        fNAcquisitions = fAcquisitionId;
        H5::DataType tType = MH5TypeAccess< unsigned >::GetType();
        fH5AcqLoc->createAttribute( "n_acquisitions", tType, H5::DataSpace( H5S_SCALAR ) ).write( tType, &fNAcquisitions );
        MDEBUG( mlog, "Finalizing stream with " << fNAcquisitions << " acquisitions" );

        return;
    }

} /* namespace monarch */
