/*

 * M4Header.cc
 *
 *  Created on: Apr 12, 2023
 *      Author: N.S. Oblath
 */

#define M4_API_EXPORTS
#define hdf5_EXPORTS

#include "M4Header.hh"

#include "M4IToA.hh"
#include "logger.hh"
#include "M4Version.hh"
#include "M4Exception.hh"

#include <cstdlib> // for atol in parsing timestamp
#include <cstring> // for strcpy

// for parsing timestamp
#include <sstream>
using std::stringstream;
using std::string;

namespace monarch4
{
    LOGGER( mlog, "M4Header" );

    /*************************************************************************
    * @brief Construct a new M4StreamHeader::M4StreamHeader object
    * @note Default CTOR
    *************************************************************************/
    M4StreamHeader::M4StreamHeader() :
            fLabel( nullptr ),
            fNumber( 0 ),
            fSource(),
            fNChannels( 0 ),
            fChannels(),
            fChannelFormat( sSeparate ),
            fAcquisitionRate( 0 ),
            fRecordSize( 0 ),
            fSampleSize( 0 ),
            fDataTypeSize( 0 ),
            fDataFormat( sDigitizedUS ),
            fBitDepth( 0 ),
            fBitAlignment( sBitsAlignedLeft ),
            fNAcquisitions( 0 ),
            fNRecords( 0 )
    {
    }

   /*************************************************************************
    * @brief Construct a new M4StreamHeader::M4StreamHeader object from specifications
    * 
    * @param aSource Device used to produce the stream
    * @param aNumber Sequential integer used to uniquely identify each stream 
    *                in the file
    * @param aNChannels Number of channels in the stream
    * @param aFirstChannel 
    * @param aFormat Whether the samples from multiple channels are interleaved 
    *                or separate 
    *                0: interleaved
    *                1: separate
    * @param anAcqRate Acquisition rate in MHz
    * @param aRecSize Number of samples in each channel record 
    *                 (stream record size = # channels * record size)
    * @param aSampleSize Number of bytes used to represent each sample
    * @param aDataTypeSize Whether the data is analog or digitized
    *                    0: digitized
    *                    1: analog
    * @param aDataFormat Whether the data is analog or digitized
    * @param aBitDepth number of bits with which the data was digitized
    * @param aBitAlignment Whether the bits within each sample are left-aligned 
    *                      or right-aligned within the sample data word
    *                      0: left-aligned
    *                      1: right-aligned
    * @return none
    * @ notes The channel's group will be named "stream[Number]".
    *************************************************************************/
    M4StreamHeader::M4StreamHeader( const std::string& aSource, uint32_t aNumber, 
                    uint32_t aNChannels, uint32_t aFirstChannel, uint32_t aFormat,
                    uint32_t anAcqRate, uint32_t aRecSize, uint32_t aSampleSize,
                    uint32_t aDataTypeSize, uint32_t aDataFormat,
                    uint32_t aBitDepth, uint32_t aBitAlignment ) :
            fLabel( NULL ),
            fNumber( 0 ),
            fSource( aSource ),
            fNChannels( aNChannels ),
            fChannels( aNChannels ),
            fChannelFormat( aFormat ),
            fAcquisitionRate( anAcqRate ),
            fRecordSize( aRecSize ),
            fSampleSize( aSampleSize ),
            fDataTypeSize( aDataTypeSize ),
            fDataFormat( aDataFormat ),
            fBitDepth( aBitDepth ),
            fBitAlignment( aBitAlignment ),
            fNAcquisitions( 0 ),
            fNRecords( 0 )
    {
        SetNumber( aNumber );
        for( unsigned iChannel = 0; iChannel < fNChannels; ++iChannel )
        {
            fChannels[ iChannel ] = aFirstChannel + iChannel;
        }
    }

    /*************************************************************************
    * @brief Construct a new M4StreamHeader::M4StreamHeader object from 
    *        M4StreamHeader
    * @param orig M4StreamHeader reference
    * @return none
    *************************************************************************/
    M4StreamHeader::M4StreamHeader( const M4StreamHeader& orig ) :
            fLabel( NULL ),
            fNumber( 0 ),
            fSource( orig.fSource ),
            fNChannels( orig.fNChannels ),
            fChannels( orig.fChannels ),
            fChannelFormat( orig.fChannelFormat ),
            fAcquisitionRate( orig.fAcquisitionRate ),
            fRecordSize( orig.fRecordSize ),
            fSampleSize( orig.fSampleSize ),
            fDataTypeSize( orig.fDataTypeSize ),
            fDataFormat( orig.fDataFormat ),
            fBitDepth( orig.fBitDepth ),
            fBitAlignment( orig.fBitAlignment ),
            fNAcquisitions( orig.fNAcquisitions ),
            fNRecords( orig.fNRecords )
    {
        SetNumber( orig.fNumber );
    }

    /*************************************************************************
    * @brief Destroy the M4StreamHeader::M4StreamHeader object
    * @note Default DTOR
    * @return none
    *************************************************************************/
    M4StreamHeader::~M4StreamHeader()
    {
        if (fLabel != nullptr)
            delete [] fLabel;
    }

    /*************************************************************************
    * @brief Set stream identifier label number
    * 
    * @param aNumber stream number
    * @return none, M4StreamHeader::fLabel updated
    *************************************************************************/
    void M4StreamHeader::SetNumber( uint32_t aNumber ) const
    {
        fNumber = aNumber;
//TODO: stream fLabel as std::string
        static const size_t prefixSize = 6; // # of characters in "stream"
        delete [] fLabel;
        fLabel = new char[ prefixSize + 10 ]; // 10 = max digits in 32-bit integer
        strcpy( fLabel, "stream" );
        u32toa( aNumber, fLabel + prefixSize );
    }
#if 0
    /*************************************************************************
    * @brief 
    * 
    * @param aParent 
    *************************************************************************/
    // void M4StreamHeader::WriteToHDF5( z5GroupHandle* aParent )
    void M4StreamHeader::WriteToFile( z5GroupHandle* aParent )
    {

// z5::createGroup( channelsHandle, name );
        LDEBUG( mlog, "Writing stream <" << fLabel << ">" );
        H5::Group tThisStreamGroup = aParent->createGroup( fLabel );

// nlohmann::json chGroupAttr;
// chGroupAttr["nChannels"] = nChannels;
// z5::writeAttributes(channelsHandle, chGroupAttr);
        M4Header::WriteScalarToHDF5( &tThisStreamGroup, "number", fNumber );
        M4Header::WriteScalarToHDF5( &tThisStreamGroup, "source", fSource );
        M4Header::WriteScalarToHDF5( &tThisStreamGroup, "n_channels", fNChannels );
        M4Header::WriteScalarToHDF5( &tThisStreamGroup, "channel_format", fChannelFormat );
        M4Header::WriteScalarToHDF5( &tThisStreamGroup, "acquisition_rate", fAcquisitionRate );
        M4Header::WriteScalarToHDF5( &tThisStreamGroup, "record_size", fRecordSize );
        M4Header::WriteScalarToHDF5( &tThisStreamGroup, "sample_size", fSampleSize );
        M4Header::WriteScalarToHDF5( &tThisStreamGroup, "data_type_size", fDataTypeSize );
        M4Header::WriteScalarToHDF5( &tThisStreamGroup, "data_format", fDataFormat );
        M4Header::WriteScalarToHDF5( &tThisStreamGroup, "bit_depth", fBitDepth );
        M4Header::WriteScalarToHDF5( &tThisStreamGroup, "bit_alignment", fBitAlignment );
        M4Header::WriteScalarToHDF5( &tThisStreamGroup, "n_acquisitions", fNAcquisitions );
        M4Header::WriteScalarToHDF5( &tThisStreamGroup, "n_records", fNRecords );

        WriteChannels( &tThisStreamGroup );
    }

    /*************************************************************************
    * @brief 
    * 
    * @param[in] aParent 
    * @param[in] aLabel 
    *************************************************************************/
    // void M4StreamHeader::ReadFromHDF5( const z5GroupHandle* aParent, const std::string& aLabel ) const
    void M4StreamHeader::ReadFromFile( const z5GroupHandle* aParent, const std::string& aLabel ) const
    {

// nlohmann::json oneStrGroupAttr;
// z5::readAttributes( streamHandle, oneStrGroupAttr );

        LDEBUG( mlog, "Reading stream <" << aLabel << ">" );
        H5::Group tThisStreamGroup = aParent->openGroup( aLabel.c_str() );


        SetNumber( M4Header::ReadScalarFromHDF5< uint32_t >( &tThisStreamGroup, "number" ) );
        Source() = M4Header::ReadScalarFromHDF5< string >( &tThisStreamGroup, "source" );
        SetNChannels( M4Header::ReadScalarFromHDF5< uint32_t >( &tThisStreamGroup, "n_channels" ) );
        SetChannelFormat( M4Header::ReadScalarFromHDF5< uint32_t >( &tThisStreamGroup, "channel_format" ) );
        SetAcquisitionRate( M4Header::ReadScalarFromHDF5< uint32_t >( &tThisStreamGroup, "acquisition_rate" ) );
        SetRecordSize( M4Header::ReadScalarFromHDF5< uint32_t >( &tThisStreamGroup, "record_size" ) );
        SetSampleSize( M4Header::ReadScalarFromHDF5< uint32_t >( &tThisStreamGroup, "sample_size" ) );
        SetDataTypeSize( M4Header::ReadScalarFromHDF5< uint32_t >( &tThisStreamGroup, "data_type_size" ) );
        SetDataFormat( M4Header::ReadScalarFromHDF5< uint32_t >( &tThisStreamGroup, "data_format" ) );
        SetBitDepth( M4Header::ReadScalarFromHDF5< uint32_t >( &tThisStreamGroup, "bit_depth" ) );
        SetBitAlignment( M4Header::ReadScalarFromHDF5< uint32_t >( &tThisStreamGroup, "bit_alignment", fBitAlignment ) );
        SetNAcquisitions( M4Header::ReadScalarFromHDF5< uint32_t >( &tThisStreamGroup, "n_acquisitions" ) );
        SetNRecords( M4Header::ReadScalarFromHDF5< uint32_t >( &tThisStreamGroup, "n_records" ) );

        ReadChannels( &tThisStreamGroup );
    }

    /*************************************************************************
    * @brief Write data channels
    * 
    * @param aLoc 
    * @return none
    *************************************************************************/
    void M4StreamHeader::WriteChannels( HAS_ATTR_IFC* aLoc )
    {
        const unsigned tNDims = 1;
        hsize_t tDims[ tNDims ] = { fNChannels };

        H5::DataSpace tDataspace( tNDims, tDims );
        //H5::DataSet tDataset = aLoc->createDataSet( "channels", MH5Type< uint32_t >::H5(), tDataspace );
        H5::Attribute tAttr = aLoc->createAttribute( "channels", MH5Type< uint32_t >::H5(), tDataspace );

        // Allocate temporary data buffer for write()
        uint32_t* tCSBuffer = new uint32_t[ fNChannels ];
        for( unsigned i = 0; i < fNChannels; ++i )
        { // write channels to data buffer
            tCSBuffer[ i ] = fChannels[ i ];
        }

        // Write data buffer to file
        //tDataset.write( tCSBuffer, MH5Type< uint32_t >::Native(), tDataspace );
        tAttr.write( MH5Type< uint32_t >::Native(), tCSBuffer );

        // Release temporary buffer
        delete [] tCSBuffer;
    }

    /*************************************************************************
    * @brief Read data channels
    * 
    * @param aLoc 
    * @return none
    *************************************************************************/
    void M4StreamHeader::ReadChannels( const HAS_ATTR_IFC* aLoc ) const
    {
        const unsigned tNDims = 1;
        hsize_t tDims[ tNDims ];

        //H5::DataSet tDataset = aLoc->openDataSet( "channels" );
        H5::Attribute tAttr = aLoc->openAttribute( "channels" );
        //H5::DataSpace tDataspace( tDataset.getSpace() );
        H5::DataSpace tDataspace( tAttr.getSpace() );
        tDataspace.getSimpleExtentDims( tDims );

        if( tDims[ 0 ] != fNChannels )
        {
            LERROR( mlog, "Channels dataset dimensions (" << tDims[ 0 ] << ") do not match number of channels, " << fNChannels );
            return;
        }

        // Allocate temporary data buffer for read()
        uint32_t* tCSBuffer = new uint32_t[ fNChannels ];

        //tDataset.read( tCSBuffer, MH5Type< uint32_t >::Native(), tDataspace );
        tAttr.read( MH5Type< uint32_t >::Native(), tCSBuffer );

        // Read data channels
        fChannels.clear();
        fChannels.resize( fNChannels );
        for( unsigned i = 0; i < fNChannels; ++i )
        {
            fChannels[ i ] = tCSBuffer[ i ];
        }

        // Release temporary buffer
        delete [] tCSBuffer;
    }
#endif

    /*************************************************************************
    * @brief Construct a new M4ChannelHeader::M4ChannelHeader object
    * @return none
    * @note Default M4ChannelHeader CTOR
    *************************************************************************/
    M4ChannelHeader::M4ChannelHeader() :
            fLabel( NULL ),
            fNumber( 0 ),
            fSource(),
            fAcquisitionRate( 0 ),
            fRecordSize( 0 ),
            fSampleSize( 0 ),
            fDataTypeSize( 0 ),
            fDataFormat( sDigitizedUS ),
            fBitDepth( 0 ),
            fBitAlignment( sBitsAlignedLeft ),
            fVoltageOffset( 0. ),
            fVoltageRange( 0. ),
            fDACGain( 0. ),
            fFrequencyMin( 0. ),
            fFrequencyRange( 0. )
    {
    }

    /*************************************************************************
    * @brief Construct a new M4ChannelHeader::M4ChannelHeader object from specifications
    * 
    * @param aSource Device used to produce the channel
    * @param aNumber Sequential integer used to uniquely identify each channel in the file
    * @param anAcqRate Acquisition rate in MHz
    * @param aRecSize Number of samples in each channel record 
    *                 (stream record size = # channels * record size)
    * @param aSampleSize Number of bytes used to represent each sample 
    * @param aDataTypeSize Whether the data is analog or digitized
    *                    0: digitized
    *                    1: analog
    * @param aDataFormat Whether the data is analog or digitized
    * @param aBitDepth number of bits with which the data was digitized
    * @param aBitAlignment Whether the bits within each sample are left-aligned 
    *                      or right-aligned within the sample data word
    *                      0: left-aligned
    *                      1: right-aligned
    * 
    * @return none
    * @ notes The channel's group will be named "channel[Number]".
    *************************************************************************/
    M4ChannelHeader::M4ChannelHeader( const std::string& aSource, uint32_t aNumber,
                    uint32_t anAcqRate, uint32_t aRecSize, uint32_t aSampleSize,
                    uint32_t aDataTypeSize, uint32_t aDataFormat,
                    uint32_t aBitDepth, uint32_t aBitAlignment ) :
        fLabel( NULL ),
        fNumber( 0 ),
        fSource( aSource ),
        fAcquisitionRate( anAcqRate ),
        fRecordSize( aRecSize ),
        fSampleSize( aSampleSize ),
        fDataTypeSize( aDataTypeSize ),
        fDataFormat( aDataFormat ),
        fBitDepth( aBitDepth ),
        fBitAlignment( aBitAlignment ),
        fVoltageOffset( 0. ),
        fVoltageRange( 0. ),
        fDACGain( 0. ),
        fFrequencyMin( 0. ),
        fFrequencyRange( 0. )
    {
        SetNumber( aNumber );
    }

    /*************************************************************************
    * @brief Construct a new M4ChannelHeader::M4ChannelHeader object from template
    * 
    * @param orig M4ChannelHeader template
    *************************************************************************/
    M4ChannelHeader::M4ChannelHeader( const M4ChannelHeader& orig ) :
            fLabel( NULL ),
            fNumber( 0 ),
            fSource( orig.fSource ),
            fAcquisitionRate( orig.fAcquisitionRate ),
            fRecordSize( orig.fRecordSize ),
            fSampleSize( orig.fSampleSize ),
            fDataTypeSize( orig.fDataTypeSize ),
            fDataFormat( orig.fDataFormat ),
            fBitDepth( orig.fBitDepth ),
            fBitAlignment( orig.fBitAlignment ),
            fVoltageOffset( orig.fVoltageOffset ),
            fVoltageRange( orig.fVoltageRange ),
            fDACGain( orig.fDACGain ),
            fFrequencyMin( orig.fFrequencyMin ),
            fFrequencyRange( orig.fFrequencyRange )
    {
        SetNumber( orig.fNumber );
    }

    /*************************************************************************
    * @brief Destroy the M4ChannelHeader::M4ChannelHeader object
    * @return none
    * @note Default M4ChannelHeader DTOR
    *************************************************************************/
    M4ChannelHeader::~M4ChannelHeader()
    {
        if (fLabel != nullptr)
            delete [] fLabel;
    }

    /*************************************************************************
    * @brief Set M4ChannelHeader identifier label number
    * 
    * @param aNumber Channel number
    *************************************************************************/
    void M4ChannelHeader::SetNumber( uint32_t aNumber ) const
    {
        fNumber = aNumber;
//TODO: fLabel as std::string
        static const size_t prefixSize = 7; // # of characters in "channel"
        delete [] fLabel;
        fLabel = new char[ prefixSize + 10 ]; // 10 = max digits in 32-bit integer
        strcpy( fLabel, "channel" );
        u32toa( aNumber, fLabel + prefixSize );
    }
#if 0
    /*************************************************************************
    * @brief 
    * 
    * @param aParent 
    *************************************************************************/
    void M4ChannelHeader::WriteToHDF5( HAS_GRP_IFC* aParent )
    {
// z5::createGroup( channelsHandle, name );

        LDEBUG( mlog, "Writing channel <" << fLabel << ">" );
        H5::Group tThisChannelGroup = aParent->createGroup( fLabel );

// nlohmann::json oneChGroupAttr;
// oneChGroupAttr["name"] = name;
// z5::writeAttributes(channelHandle, oneChGroupAttr);

        M4Header::WriteScalarToHDF5( &tThisChannelGroup, "number", fNumber );
        M4Header::WriteScalarToHDF5( &tThisChannelGroup, "source", fSource );
        M4Header::WriteScalarToHDF5( &tThisChannelGroup, "acquisition_rate", fAcquisitionRate );
        M4Header::WriteScalarToHDF5( &tThisChannelGroup, "record_size", fRecordSize );
        M4Header::WriteScalarToHDF5( &tThisChannelGroup, "sample_size", fSampleSize );
        M4Header::WriteScalarToHDF5( &tThisChannelGroup, "data_type_size", fDataTypeSize );
        M4Header::WriteScalarToHDF5( &tThisChannelGroup, "data_format", fDataFormat );
        M4Header::WriteScalarToHDF5( &tThisChannelGroup, "bit_depth", fBitDepth );
        M4Header::WriteScalarToHDF5( &tThisChannelGroup, "bit_alignment", fBitAlignment );
        M4Header::WriteScalarToHDF5( &tThisChannelGroup, "voltage_offset", fVoltageOffset );
        M4Header::WriteScalarToHDF5( &tThisChannelGroup, "voltage_range", fVoltageRange );
        M4Header::WriteScalarToHDF5( &tThisChannelGroup, "dac_gain", fDACGain );
        M4Header::WriteScalarToHDF5( &tThisChannelGroup, "frequency_min", fFrequencyMin );
        M4Header::WriteScalarToHDF5( &tThisChannelGroup, "frequency_range", fFrequencyRange );
    }

    /*************************************************************************
    * @brief 
    * 
    * @param aParent 
    * @param aLabel 
    *************************************************************************/
    void M4ChannelHeader::ReadFromHDF5( const HAS_GRP_IFC* aParent, const std::string& aLabel ) const
    {
        LDEBUG( mlog, "Reading channel <" << aLabel << ">" );
        H5::Group tThisChannelGroup = aParent->openGroup( aLabel.c_str() );

// auto streamsHandle = z5::filesystem::handle::Group( f, "streams" );
// nlohmann::json strGroupAttr;

// // Extract the stream attribute: nStreams to indicate how many streams are stored
// z5::readAttributes( streamsHandle, strGroupAttr );

        SetNumber( M4Header::ReadScalarFromHDF5< uint32_t >( &tThisChannelGroup, "number" ) );
        Source() = M4Header::ReadScalarFromHDF5< string >( &tThisChannelGroup, "source" );
        SetAcquisitionRate( M4Header::ReadScalarFromHDF5< uint32_t >( &tThisChannelGroup, "acquisition_rate" ) );
        SetRecordSize( M4Header::ReadScalarFromHDF5< uint32_t >( &tThisChannelGroup, "record_size" ) );
        SetSampleSize( M4Header::ReadScalarFromHDF5< uint32_t >( &tThisChannelGroup, "sample_size" ) );
        SetDataTypeSize( M4Header::ReadScalarFromHDF5< uint32_t >( &tThisChannelGroup, "data_type_size" ) );
        SetDataFormat( M4Header::ReadScalarFromHDF5< uint32_t >( &tThisChannelGroup, "data_format" ) );
        SetBitDepth( M4Header::ReadScalarFromHDF5< uint32_t >( &tThisChannelGroup, "bit_depth" ) );
        SetBitAlignment( M4Header::ReadScalarFromHDF5< uint32_t >( &tThisChannelGroup, "bit_alignment", fBitAlignment ) );
        SetVoltageOffset( M4Header::ReadScalarFromHDF5< double >( &tThisChannelGroup, "voltage_offset" ) );
        SetVoltageRange( M4Header::ReadScalarFromHDF5< double >( &tThisChannelGroup, "voltage_range" ) );
        SetDACGain( M4Header::ReadScalarFromHDF5< double >( &tThisChannelGroup, "dac_gain" ) );
        SetFrequencyMin( M4Header::ReadScalarFromHDF5< double >( &tThisChannelGroup, "frequency_min" ) );
        SetFrequencyRange( M4Header::ReadScalarFromHDF5< double >( &tThisChannelGroup, "frequency_range" ) );
    }
#endif

    /*************************************************************************
    * @brief Construct a (default) new M4Header::M4Header object
    * 
    *************************************************************************/
    M4Header::M4Header() :
            fEggVersion( TOSTRING(Egg_VERSION) ),
            fFilename(),
            fRunDuration( 0 ),
            fTimestamp(),
            fDescription(),
            fNChannels( 0 ),
            fNStreams( 0 ),
            fChannelStreams(),
            fChannelCoherence(),
            // fFile( nullptr ),
            fStreamsGroup( nullptr ),
            fChannelsGroup( nullptr )
    {
std::cout << "M4Header::M4Header()\n";
    }

    /*************************************************************************
    * @brief Destroy the M4Header::M4Header object
    * 
    *************************************************************************/
    M4Header::~M4Header()
    {
        if (fStreamsGroup != nullptr)
            delete fStreamsGroup;
        
        if (fChannelsGroup != nullptr)
            delete fChannelsGroup;
    }

    /*************************************************************************
    * @brief Clone information from existing M4Header into this one
    * 
    * @param[in] aOrig Existing M4Header (data source)
    *************************************************************************/
    void M4Header::CopyBasicInfo( const M4Header& aOrig )
    {
        fEggVersion = aOrig.fEggVersion;
        fFilename = aOrig.fFilename;
        fRunDuration = aOrig.fRunDuration;
        fTimestamp = aOrig.fTimestamp;
        fDescription = aOrig.fDescription;
    }

    /*************************************************************************
    * @brief Setup time-coherence relationship between channels
    * 
    * @param aChanA index of first channel
    * @param aChanB index of second
    * @param aCoherence 
    *   - true: channels accquired synchronized together
    *   - false: channels not synchronized together
    *************************************************************************/
    void M4Header::SetCoherence( unsigned aChanA, unsigned aChanB, bool aCoherence )
    {
        if( aChanA >= fNChannels || aChanB >= fNChannels )
        {
            throw M4Exception() << "Channel out of bounds: " << aChanA << " or " << aChanB << " >= " << fNChannels;
        }
        fChannelCoherence[ aChanA ][ aChanB ] = aCoherence;
        fChannelCoherence[ aChanB ][ aChanA ] = aCoherence;
    }

    /*************************************************************************
    * @brief Add a stream with one channel with aRecSize samples per record
    * 
    * @param aSource Device used to produce the stream
    * @param anAcqRate Acquisition rate in MHz
    * @param aRecSize Number of samples in each channel record 
    *                 (stream record size = # channels * record size)
    * @param aSampleSize Number of bytes used to represent each sample
    * @param aDataTypeSize Whether the data is analog or digitized
    *                    0: digitized
    *                    1: analog
    * @param aDataFormat Whether the data is analog or digitized
    * @param aBitDepth number of bits with which the data was digitized
    * @param aBitAlignment Whether the bits within each sample are left-aligned 
    *                      or right-aligned within the sample data word
    *                      0: left-aligned
    *                      1: right-aligned
    * @param aChanVec 
    * 
    * @return uint32_t Returns the stream number (used to address the stream later)
    *************************************************************************/
    uint32_t M4Header::AddStream( const std::string& aSource,
                                 uint32_t anAcqRate, uint32_t aRecSize, uint32_t aSampleSize,
                                 uint32_t aDataTypeSize, uint32_t aDataFormat,
                                 uint32_t aBitDepth, uint32_t aBitAlignment,
                                 std::vector< unsigned >* aChanVec )
    {
        LDEBUG( mlog, "Adding stream " << fNStreams << " for channel " << fNChannels << " with record size " << aRecSize );
        
        if( aChanVec != nullptr ) 
        {
            aChanVec->push_back( fNChannels );
        }

        fChannelStreams.push_back( fNStreams );
        fChannelHeaders.push_back( M4ChannelHeader( aSource, fNChannels, anAcqRate, aRecSize, aSampleSize, aDataTypeSize, aDataFormat, aBitDepth, aBitAlignment ) );
        fStreamHeaders.push_back( M4StreamHeader( aSource, fNStreams, 1, fNChannels, sSeparate, anAcqRate, aRecSize, aSampleSize, aDataTypeSize, aDataFormat, aBitDepth, aBitAlignment ) );
        ++fNChannels;

        //std::cout << "resizing to " << fNChannels << std::endl;
        fChannelCoherence.resize( fNChannels ); // resize number of columns
        for( unsigned i = 0; i < fNChannels; ++i ) // resize each row
        {
            fChannelCoherence[ i ].resize( fNChannels, false );
        }
        fChannelCoherence.back().back() = true; // each channel is coherent with itself

        return fNStreams++;
    }

    /*************************************************************************
    * @brief Add a stream with multiple channels with aRecSize samples per record
    * 
    * @param aSource Device used to produce the stream
    * @param aNChannels How many channels to add
    * @param aFormat 
    * @param anAcqRate Acquisition rate in MHz
    * @param aRecSize Number of samples in each channel record 
    *                 (stream record size = # channels * record size)
    * @param aSampleSize Number of bytes used to represent each sample
    * @param aDataTypeSize Whether the data is analog or digitized
    *                    0: digitized
    *                    1: analog
    * @param aDataFormat Whether the data is analog or digitized
    * @param aBitDepth snumber of bits with which the data was digitized
    * @param aBitAlignment Whether the bits within each sample are left-aligned 
    *                      or right-aligned within the sample data word
    *                      0: left-aligned
    *                      1: right-aligned
    * @param aChanVec 
    * 
    * @return uint32_t Returns the stream number (used to address the stream later)
    *************************************************************************/
    uint32_t M4Header::AddStream( const std::string& aSource, uint32_t aNChannels, uint32_t aFormat,
                                 uint32_t anAcqRate, uint32_t aRecSize, uint32_t aSampleSize,
                                 uint32_t aDataTypeSize, uint32_t aDataFormat,
                                 uint32_t aBitDepth, uint32_t aBitAlignment,
                                 std::vector< unsigned >* aChanVec )
    {
        LDEBUG( mlog, "Adding stream " << fNStreams << " for multiple channels with record size " << aRecSize );
        unsigned tFirstNewChannel = fNChannels;
        
        // Add channels
        for( uint32_t iNewChannel = 0; iNewChannel < aNChannels; ++iNewChannel )
        {
            LDEBUG( mlog, "Adding channel " << fNChannels );
            if( aChanVec != nullptr ) 
            {
                aChanVec->push_back( fNChannels );  
            }
            
            fChannelStreams.push_back( fNStreams );
            fChannelHeaders.push_back( M4ChannelHeader( aSource, fNChannels, anAcqRate, aRecSize, aSampleSize, aDataTypeSize, aDataFormat, aBitDepth, aBitAlignment ) );
            ++fNChannels;
            
            fChannelCoherence.resize( fNChannels ); // resize number of columns
            for( unsigned i = 0; i < fNChannels; ++i ) // resize all rows
            {
                fChannelCoherence[ i ].resize( fNChannels, false );
            }
            
            fChannelCoherence.back().back() = true; // each channel is coherent with itself
            if( fNChannels > 0 ) 
            { // this condition is necessary because if it's not the case, fNChannels-1 will 
              // be some huge number, since we're dealing with unsigned ints, so the for-loop 
              // condition won't be sufficient

                for( unsigned i = tFirstNewChannel; i < fNChannels - 1; ++i )
                {
                    fChannelCoherence[ fNChannels - 1 ][ i ] = true;
                    fChannelCoherence[ i ][ fNChannels - 1 ] = true;
               }
            }
        }
        fStreamHeaders.push_back( M4StreamHeader( aSource, fNStreams, aNChannels, tFirstNewChannel, aFormat, anAcqRate, aRecSize, aSampleSize, aDataTypeSize, aDataFormat, aBitDepth, aBitAlignment ) );
        
        return fNStreams++;
    }

    /*************************************************************************
    * @brief Write file attributes, stream headers, channel headers to file
    * 
    * @param aFile path/filename
    * @return none, throw M4Exception on failure
    *************************************************************************/
    // void M4Header::WriteToHDF5( H5::H5File* aFile )
    void M4Header::WriteToFile( z5FileHandle aFile )
    {
std::cout << "M4Header::WriteToFile()\n";        
        // try
        // {
            LDEBUG( mlog, "Writing run header" );

            nlohmann::json headerAttr;

            headerAttr["egg_version"] = fEggVersion;
            headerAttr["filename"] = fFilename;
            headerAttr["n_channels"] = fNChannels;
            headerAttr["n_streams"] = fNStreams;
            headerAttr["run_duration"] = fRunDuration;
            headerAttr["timestamp"] = fTimestamp;
            headerAttr["description"] = fDescription;
            z5::writeAttributes(aFile, headerAttr);

#if 0            
            fFile = aFile;
            WriteScalarToHDF5( fFile, "egg_version",   fEggVersion );
            WriteScalarToHDF5( fFile, "filename",      fFilename );
            WriteScalarToHDF5( fFile, "n_channels",    fNChannels );
            WriteScalarToHDF5( fFile, "n_streams",     fNStreams );
            WriteScalarToHDF5( fFile, "run_duration",  fRunDuration );
            WriteScalarToHDF5( fFile, "timestamp",     fTimestamp );
            WriteScalarToHDF5( fFile, "description",   fDescription );

            //////////////Write1DToHDF5( fFile, "channel_streams",  fChannelStreams );
            WriteChannelStreams( fFile );
            WriteChannelCoherence( fFile );

            // Write all the stream headers
            LDEBUG( mlog, "Writing stream headers" );
            fStreamsGroup = new H5::Group( fFile->createGroup( "streams" ) );
            for( uint32_t iStream = 0; iStream < fNStreams; ++iStream )
            {
                fStreamHeaders[ iStream ].WriteToHDF5( fStreamsGroup );
            }

            // Write all the channel headers
            LDEBUG( mlog, "Writing channel headers" );
            fChannelsGroup = new H5::Group( fFile->createGroup( "channels" ) );
            for( uint32_t iChan = 0; iChan < fNChannels; ++iChan )
            {
                fChannelHeaders[ iChan ].WriteToHDF5( fChannelsGroup );
            }
        // }
        // catch( H5::Exception& e )
        // {
        //     throw M4Exception() << "HDF5 error while writing header: " << e.getCDetailMsg();
        // }
        // catch( M4Exception& e )
        // {
        //     LDEBUG( mlog, "M4Exception: " << e.what() );
        //     throw;
        // }
#endif        
std::cout << "M4Header::WriteToFile(): void\n";        

    }


    /*************************************************************************
    * @brief Read file attributes, stream headers, channel headers from file
    * 
    * @param aFile path/filename
    * @return none, throw M4Exception on failure
    *************************************************************************/
    // void M4Header::ReadFromHDF5( const H5::H5File* aFile ) const
    void M4Header::ReadFromFile( const z5FileHandle aFile ) const
    {
std::cout << "M4Header::ReadFromFile()\n";        
        // try
        // {
            // LDEBUG( mlog, "Reading run header" );
            // fFile = const_cast< H5::H5File* >( aFile );

            // // Read the File Atrributes
            // EggVersion() = ReadScalarFromHDF5< string >( fFile, string("egg_version") );
            // Filename() = ReadScalarFromHDF5< string >( fFile, "filename" );
            // SetNChannels( ReadScalarFromHDF5< uint32_t >( fFile, "n_channels" ) );
            // SetNStreams( ReadScalarFromHDF5< uint32_t >( fFile, "n_streams" ) );
            // SetRunDuration( ReadScalarFromHDF5< uint32_t >( fFile, "run_duration" ) );
            // Timestamp() = ReadScalarFromHDF5< string >( fFile, "timestamp" );
            // Description() = ReadScalarFromHDF5< string >( fFile, "description" );

            nlohmann::json headerAttr;
            z5::readAttributes( aFile, headerAttr );

//TODO: handle nlohmann exception
            fEggVersion = headerAttr.at("egg_version");
            fFilename = headerAttr.at("filename");
            fNChannels = headerAttr.at("n_channels");
            fNStreams = headerAttr.at("n_streams");
            fRunDuration = headerAttr.at("run_duration");
            fTimestamp = headerAttr.at("timestamp");
            fDescription = headerAttr.at("description");

            std::cout << "EggVersion: " << fEggVersion << std::endl;
            std::cout << "Filename: " << fFilename << std::endl;
            std::cout << "NChannels: " << fNChannels << std::endl;
            std::cout << "NStreams: " << fNStreams << std::endl;

            std::cout << "RunDuration: " << fRunDuration << std::endl;
            std::cout << "Timestamp: " << fTimestamp << std::endl;
            std::cout << "Description: " << fDescription << std::endl;

#if 0
            //fChannelStreams.clear();
            //Read1DFromHDF5< std::vector< uint32_t > >( fFile, "channel_streams", fChannelStreams );
            ReadChannelStreams( aFile );            // Read Stream Attributes
            ReadChannelCoherence( aFile );
            /*
            for( unsigned i = 0; i < fNChannels; ++i )
            {
                std::cout << "   ";
                for( unsigned j = 0; j < fNChannels; ++j )
                {
                    std::cout << fChannelCoherence[ i ][ j ] << " ";
                }
                std::cout << std::endl;
            }
            */

			// Windows' HDF5 really doesn't like using std::strings
			const unsigned tBuffSize = 256;
			char tBuffer[ tBuffSize ];

            LDEBUG( mlog, "Reading stream headers" );
            fStreamHeaders.clear();

            // Create streams group correspoding to file streams
            fStreamsGroup = new H5::Group( fFile->openGroup( "streams" ) );
            hsize_t nStreams = fStreamsGroup->getNumObjs();
            if( nStreams != fNStreams )
            {
                throw M4Exception() << "Number of streams <" << fNStreams << "> disagrees with the number of objects in the stream group <" << nStreams << ">";
            }

            // Read each of the stream-header objects and store
            for( hsize_t iStream = 0; iStream < nStreams; ++iStream )
            {
                //string tStreamLabel = fStreamsGroup->getObjnameByIdx( iStream );
                unsigned tLabelSize = fStreamsGroup->getObjnameByIdx( iStream, tBuffer, tBuffSize );
                std::string tStreamLabel( tBuffer, tLabelSize );
				LDEBUG( mlog, "Attempting to read stream header #" << iStream << "; label <" << tStreamLabel << ">" );
                
                fStreamHeaders.push_back( M4StreamHeader() );
                
                LDEBUG( mlog, "Testing if we can access the last header: " << fStreamHeaders.back().GetLabel() );
                fStreamHeaders.back().ReadFromHDF5( fStreamsGroup, tStreamLabel );
            }

            LDEBUG( mlog, "Reading channel headers" );
            fChannelHeaders.clear();

            // Create new channel group
            fChannelsGroup = new H5::Group( fFile->openGroup( "channels" ) );
            hsize_t nChannels = fChannelsGroup->getNumObjs();
            
            // Read each of the stream-channel objects and store
            for( hsize_t iChan = 0; iChan < nChannels; ++iChan )
            {
                //string tChannelLabel = fChannelsGroup->getObjnameByIdx( iChan );
                unsigned tLabelSize = fChannelsGroup->getObjnameByIdx( iChan, tBuffer, tBuffSize );
                std::string tChannelLabel( tBuffer, tLabelSize );
                
                fChannelHeaders.push_back( M4ChannelHeader() );
                fChannelHeaders.back().ReadFromHDF5( fChannelsGroup, tChannelLabel );
            }

        }
        catch( H5::Exception& e )
        {
            //H5::Exception::printErrorStack();
            throw M4Exception() << "Unable to open header group or find header data\n";
        }
#endif  
std::cout << "M4Header::ReadFromFile(): void\n";        
    }

#if 0

    /*************************************************************************
    * @brief Write channel-streams to file
    * 
    * @param aLoc Stream location attribute
    * @return none, no exceptions thrown
    *************************************************************************/
    void M4Header::WriteChannelStreams( HAS_ATTR_IFC* aLoc )
    {
        const unsigned tNDims = 1;
        hsize_t tDims[ tNDims ] = { fNChannels };

        // Create data space for channel-streams
        H5::DataSpace tDataspace( tNDims, tDims );
        //H5::DataSet tDataset = aLoc->createDataSet( "channel_streams", MH5Type< uint32_t >::H5(), tDataspace );
        H5::Attribute tAttr = aLoc->createAttribute( "channel_streams", MH5Type< uint32_t >::H5(), tDataspace );

        // Allocate temporary write buffer
        uint32_t* tCSBuffer = new uint32_t[ fNChannels ];

        // Populate write buffer from the fChannelStreams
        for( unsigned i = 0; i < fNChannels; ++i )
        {
            tCSBuffer[ i ] = fChannelStreams[ i ];
        }

        // Write the streams to the file
        //tDataset.write( tCSBuffer, MH5Type< uint32_t >::Native(), tDataspace );
        tAttr.write( MH5Type< uint32_t >::Native(), tCSBuffer );

        delete [] tCSBuffer;        // release temporary buffer
    }

    /*************************************************************************
    * @brief Read the channel-streams from file
    * 
    * @param aLoc Stream location attribute
    * @return none, no exceptions thrown
    *************************************************************************/
    void M4Header::ReadChannelStreams( const HAS_ATTR_IFC* aLoc ) const
    {
        const unsigned tNDims = 1;
        hsize_t tDims[ tNDims ];

        // Get access to the channel streams: tAttr
        //H5::DataSet tDataset = aLoc->openDataSet( "channel_streams" );
        H5::Attribute tAttr = aLoc->openAttribute( "channel_streams" );
        
        // Get access to the streams data space: tDataspace
        //H5::DataSpace tDataspace( tDataset.getSpace() );
        H5::DataSpace tDataspace( tAttr.getSpace() );

        // Verify data dimensions match that expected in the M4Header::fNChannels
        tDataspace.getSimpleExtentDims( tDims );
        if( tDims[ 0 ] != fNChannels )
        {
            LERROR( mlog, "Channel-streams dataset dimensions (" << tDims[ 0 ] << ") do not match number of channels, " << fNChannels );
            return;
        }

        // allocate temporary read buffer
        uint32_t* tCSBuffer = new uint32_t[ fNChannels ];
        //tDataset.read( tCSBuffer, MH5Type< uint32_t >::Native(), tDataspace );
        tAttr.read( MH5Type< uint32_t >::Native(), tCSBuffer );

        // Populate the M4Header::fChannelStreams from read buffer
        fChannelStreams.clear();
        fChannelStreams.resize( fNChannels );
        for( unsigned i = 0; i < fNChannels; ++i )
        {
            fChannelStreams[ i ] = tCSBuffer[ i ];
        }

        delete [] tCSBuffer;        // release temporary read buffer
    }

    /*************************************************************************
    * @brief Write channel-coherence to file
    * 
    * @param aLoc 
    *************************************************************************/
    void M4Header::WriteChannelCoherence( HAS_ATTR_IFC* aLoc )
    {
        const unsigned tNDims = 2;
        hsize_t tDims[ tNDims ] = { fNChannels, fNChannels };

        // Create channel-coherence attribute from data-space: tAttr
        H5::DataSpace tDataspace( tNDims, tDims );
        //H5::DataSet tDataset = aLoc->createDataSet( "channel_coherence", MH5Type< bool >::H5(), tDataspace );
        H5::Attribute tAttr = aLoc->createAttribute( "channel_coherence", MH5Type< bool >::H5(), tDataspace );

        // allocate temporary write buffer
        uint8_t* tCCBuffer = new uint8_t[ fNChannels * fNChannels ];

        // Copy channel-coherence to write buffer
        for( unsigned i = 0; i < fNChannels; ++i )
        {
            for( unsigned j = 0; j < fNChannels; ++j )
            {
                tCCBuffer[ i * fNChannels + j ] = (uint8_t)fChannelCoherence[ i ][ j ];
            }
        }

        // Write coherence buffer to file
        //tDataset.write( tCCBuffer, MH5Type< bool >::Native(), tDataspace );
        tAttr.write( MH5Type< bool >::Native(), tCCBuffer );

        delete [] tCCBuffer;
    }

    /*************************************************************************
    * @brief Read channel-coherence from file
    * 
    * @param aLoc 
    *************************************************************************/
    void M4Header::ReadChannelCoherence( const HAS_ATTR_IFC* aLoc ) const
    {
        const unsigned tNDims = 2;
        hsize_t tDims[ tNDims ];

        // Get access to channel-coherence: tAttr
        //H5::DataSet tDataset = aLoc->openDataSet( "channel_coherence" );
        H5::Attribute tAttr = aLoc->openAttribute( "channel_coherence" );

        // Get acces to the data space: tDataspace
        //H5::DataSpace tDataspace( tDataset.getSpace() );
        H5::DataSpace tDataspace( tAttr.getSpace() );

        // Verify data dimensions match that expected in the M4Header::fNChannels
        tDataspace.getSimpleExtentDims( tDims );
        if( tDims[ 0 ] != fNChannels || tDims[ 1 ] != fNChannels )
        {
            LERROR( mlog, "Channel coherence dataset dimensions (" << tDims[ 0 ] << ", " << tDims[ 1 ] << ") do not match number of channels, " << fNChannels );
            return;
        }

        // Allocate temporary buffer for read
        uint8_t* tCCBuffer = new uint8_t[ fNChannels * fNChannels ];
        //tDataset.read( tCCBuffer, MH5Type< bool >::Native(), tDataspace );
        tAttr.read( MH5Type< bool >::Native(), tCCBuffer );

        // Copy over channel-coherence from temporary buffer into component
        fChannelCoherence.clear();
        fChannelCoherence.resize( fNChannels );
        for( unsigned i = 0; i < fNChannels; ++i )
        {
            fChannelCoherence[ i ].resize( fNChannels );
            for( unsigned j = 0; j < fNChannels; ++j )
            {
                fChannelCoherence[ i ][ j ] = (bool)tCCBuffer[ i * fNChannels + j ];
            }
        }

        delete [] tCCBuffer;        // release temporary buffer
    }
#endif
}

/*************************************************************************
* @brief output stream operator for M4StreamHeader component
* 
* @param[in out] out Output stream
* @param[in] hdr header component
* @return M4_API& output tail-chain
*************************************************************************/
M4_API std::ostream& operator<<( std::ostream& out, const monarch4::M4StreamHeader& hdr )
{
    out << "Stream Header Content:\n";
    out << "\tStream Number: " << hdr.GetNumber() << '\n';
    out << "\tSource: " << hdr.Source() << '\n';
    out << "\tNumber of Channels: " << hdr.GetNChannels() << '\n';
    out << "\tChannel Format: " << hdr.GetChannelFormat() << '\n';
    out << "\tAcquisition Rate: " << hdr.GetAcquisitionRate() << " MHz\n";
    out << "\tRecord Size: " << hdr.GetRecordSize() << " samples\n";
    out << "\tSample Size: " << hdr.GetSampleSize() << " elements\n";
    out << "\tData Type Size: " << hdr.GetDataTypeSize() << " bytes\n";
    out << "\tData Format: " << hdr.GetDataFormat() << '\n';
    out << "\tBit Depth: " << hdr.GetBitDepth() << " bits\n";
    out << "\tBit Alignment: " << hdr.GetBitAlignment() << '\n';
    out << "\tNumber of Acquisitions: " << hdr.GetNAcquisitions() << '\n';
    out << "\tNumber of Records: " << hdr.GetNRecords() << '\n';

    return out;
}

/*************************************************************************
* @brief output stream operator for M4ChannelHeader component
* 
* @param[in out] out Output stream
* @param[in] hdr header component
* @return M4_API& output tail-chain
*************************************************************************/
M4_API std::ostream& operator<<( std::ostream& out, const monarch4::M4ChannelHeader& hdr )
{
    out << "Channel Header Content:\n";
    out << "\tChannel Number: " << hdr.GetNumber() << '\n';
    out << "\tSource: " << hdr.Source() << '\n';
    out << "\tAcquisition Rate: " << hdr.GetAcquisitionRate() << " MHz\n";
    out << "\tRecord Size: " << hdr.GetRecordSize() << " samples\n";
    out << "\tSample Size: " << hdr.GetSampleSize() << " elements\n";
    out << "\tData Type Size: " << hdr.GetDataTypeSize() << " bytes\n";
    out << "\tData Format: " << hdr.GetDataFormat() << '\n';
    out << "\tBit Depth: " << hdr.GetBitDepth() << " bits\n";
    out << "\tBit Alignment: " << hdr.GetBitAlignment() << '\n';
    out << "\tVoltage Offset: " << hdr.GetVoltageOffset() << " V\n";
    out << "\tVoltage Range: " << hdr.GetVoltageRange() << " V\n";
    out << "\tFrequency Min: " << hdr.GetFrequencyMin() << " Hz\n";
    out << "\tFrequency Range: " << hdr.GetFrequencyRange() << " Hz\n";

    return out;
}

/*************************************************************************
* @brief output stream operator for M4Header component
* 
* @param[in out] out Output stream
* @param[in] hdr header component
* @return M4_API& output tail-chain
*************************************************************************/
M4_API std::ostream& operator<<( std::ostream& out, const monarch4::M4Header& hdr )
{
    out << "Monarch Header Content:\n";
    out << "\tEgg Version: " << hdr.EggVersion() << "\n";
    out << "\tFilename: " << hdr.Filename() << "\n";
    out << "\tRun Duration: " << hdr.GetRunDuration() << " ms\n";
    out << "\tTimestamp: " << hdr.Timestamp() << "\n";
    out << "\tDescription: " << hdr.Description() << "\n";
    out << "\tNumber of Channels: " << hdr.GetNChannels() << "\n";
    out << "\tNumber of Streams: " << hdr.GetNStreams() << "\n";
    out << "\tChannel-to-stream mapping:\n";

    for( uint32_t iChan = 0; iChan < hdr.ChannelStreams().size(); ++iChan )
    {
        out << "\t\tChannel " << iChan << " --> Stream " << hdr.ChannelStreams()[ iChan ] << "\n";
    }
    out << "\tStream headers:\n";
    for( uint32_t iStream = 0; iStream < hdr.StreamHeaders().size(); ++iStream )
    {
        out << hdr.StreamHeaders()[ iStream ];
    }
    out << "\tChannel headers:\n";
    for( uint32_t iChan = 0; iChan < hdr.ChannelHeaders().size(); ++iChan )
    {
        out << hdr.ChannelHeaders()[ iChan ];
    }

    return out;
}
