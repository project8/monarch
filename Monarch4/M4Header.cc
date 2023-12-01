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

#include "xtensor/xarray.hpp"
#include "xtensor/xio.hpp"

#include <cstdlib> // for atol in parsing timestamp
#include <cstring> // for strcpy
#include <sstream>  // for parsing timestamp

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
    * @brief Construct a new multi-channel M4StreamHeader object from specifications
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
std::cout << "M4StreamHeader::M4StreamHeader() for " << aNChannels << " channels: stream " << aNumber << std::endl;
        SetNumber( aNumber );       // set stream number

        for( unsigned iChannel = 0; iChannel < fNChannels; ++iChannel )
        {
            fChannels[ iChannel ] = aFirstChannel + iChannel;
        }
    }

    /*************************************************************************
    * @brief Construct a new single-channel M4StreamHeader object from 
    *        M4StreamHeader template
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
std::cout << "M4StreamHeader::M4StreamHeader() from template for single-channel: stream " << orig.fNumber << std::endl;
        SetNumber( orig.fNumber );      // set stream number
    }

    /*************************************************************************
    * @brief Destroy the M4StreamHeader::M4StreamHeader object
    * @note Default DTOR
    * @return none
    *************************************************************************/
    M4StreamHeader::~M4StreamHeader()
    {
//std::cout << "M4StreamHeader::~M4StreamHeader() DTOR: stream " << fNumber << std::endl;;
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
    /*************************************************************************
    * @brief Write the stream/channel metadata configuration
    * 
    * @param [in] aGroup  <root>/"streams"
    *************************************************************************/
    // void M4StreamHeader::WriteToHDF5( z5GroupHandle* aParent )
    void M4StreamHeader::WriteToFile( z5GroupHandle aGroup )
    {
std::cout << "M4StreamHeader::WriteToFile(): " << fLabel << std::endl;

        LDEBUG( mlog, "Writing stream header <" << fLabel << ">" );
#if 0 // old HDF5 code reference
        H5::Group tThisStreamGroup = aParent->createGroup( fLabel );
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
#endif

        z5::createGroup( aGroup, fLabel );
        z5GroupHandle grpHandle = z5GroupHandle( aGroup, fLabel );  // <root>/"streams"/"streamsN"
        nlohmann::json streamAttr;

//std::cout << "  number: " << fNumber << std::endl;
//std::cout << "  source: " << fSource << std::endl;
//std::cout << "  n_channels: " << fNChannels << std::endl;
//std::cout << "  channel_format: " << fChannelFormat << std::endl;
//std::cout << "  acquisition_rate: " << fAcquisitionRate << std::endl;
//std::cout << "  record_size: " << fRecordSize << std::endl;
//std::cout << "  sample_size: " << fSampleSize << std::endl;
//std::cout << "  data_type_size: " << fDataTypeSize << std::endl;
//std::cout << "  data_format: " << fDataFormat << std::endl;
//std::cout << "  bit_depth: " << fBitDepth << std::endl;
//std::cout << "  bit_alignment: " << fBitAlignment << std::endl;
//std::cout << "  n_acquisitions: " << fNAcquisitions << std::endl;
//std::cout << "  n_records: " << fNRecords << std::endl;

        streamAttr["number"] = fNumber;
        streamAttr["source"] = fSource;
        streamAttr["n_channels"] = fNChannels;
        streamAttr["channel_format"] = fChannelFormat;
        streamAttr["acquisition_rate"] = fAcquisitionRate;
        streamAttr["record_size"] = fRecordSize;
        streamAttr["sample_size"] = fSampleSize;
        streamAttr["data_type_size"] = fDataTypeSize;
        streamAttr["data_format"] = fDataFormat;
        streamAttr["bit_depth"] = fBitDepth;
        streamAttr["bit_alignment"] = fBitAlignment;
        streamAttr["n_acquisitions"] = fNAcquisitions;
        streamAttr["n_records"] = fNRecords;
        z5::writeAttributes(grpHandle, streamAttr);

std::cout << "M4StreamHeader::WriteToFile(): void " << fLabel << std::endl;
    }

    /*************************************************************************
    * @brief 
    * 
    * @param [in] aGroup  <root>/"streams"
    * @param [in] aLabel 
    *************************************************************************/
    // void M4StreamHeader::ReadFromHDF5( const z5GroupHandle* aParent, const std::string& aLabel ) const
    void M4StreamHeader::ReadFromFile( const z5GroupHandle aGroup, const std::string& aLabel ) const
    {
//std::cout <<  "M4StreamHeader::ReadFromFile()\n";

        LDEBUG( mlog, "Reading stream header <" << aLabel << ">" );
#if 0 // old HDF5 code reference
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
#endif

        nlohmann::json jstreamAttr;
        z5::readAttributes( aGroup, jstreamAttr );

//std::cout << "number: " << jstreamAttr.at("number") << std::endl;
//std::cout << "source: " << jstreamAttr.at("source") << std::endl;
//std::cout << "n_channels: " << jstreamAttr.at("n_channels") << std::endl;
//std::cout << "channel_format: " << jstreamAttr.at("channel_format") << std::endl;
//std::cout << "acquisition_rate: " << jstreamAttr.at("acquisition_rate") << std::endl;
//std::cout << "record_size: " << jstreamAttr.at("record_size") << std::endl;
//std::cout << "sample_size: " << jstreamAttr.at("sample_size") << std::endl;
//std::cout << "data_type_size: " << jstreamAttr.at("data_type_size") << std::endl;
//std::cout << "data_format: " << jstreamAttr.at("data_format") << std::endl;
//std::cout << "bit_depth: " << jstreamAttr.at("bit_depth") << std::endl;
//std::cout << "bit_alignment: " << jstreamAttr.at("bit_alignment") << std::endl;
//std::cout << "n_acquisitions: " << jstreamAttr.at("n_acquisitions") << std::endl;
//std::cout << "n_records: " << jstreamAttr.at("n_records") << std::endl << std::endl;


        try
        {
            // Programmer's Note: using casts to ensure correct conversion function gets selected
            // Programmer's Note: using .at() function to ensure proper exception operation
            fNumber = (uint32_t)jstreamAttr.at("number");
            fSource = (std::string)jstreamAttr.at("source");
            fNChannels = (uint32_t)jstreamAttr.at("n_channels");
            fChannelFormat = (uint32_t)jstreamAttr.at("channel_format");
            fAcquisitionRate = (uint32_t)jstreamAttr.at("acquisition_rate");
            fRecordSize = (uint32_t)jstreamAttr.at("record_size");
            fSampleSize = (uint32_t)jstreamAttr.at("sample_size");
            fDataTypeSize = (uint32_t)jstreamAttr.at("data_type_size");
            fDataFormat = (uint32_t)jstreamAttr.at("data_format");
            fBitDepth = (uint32_t)jstreamAttr.at("bit_depth");
            fBitAlignment = jstreamAttr.at("bit_alignment");
            fNAcquisitions = (uint32_t)jstreamAttr.at("n_acquisitions");
            fNRecords = (uint32_t)jstreamAttr.at("n_records");            
        }
        catch (const nlohmann::json::exception& e)
        { // JSON error
            throw M4Exception() << "JSON error M4StreamHeader::ReadFromFile(): " << e.what();
        }

//std::cout <<  "M4StreamHeader::ReadFromFile(): void\n";
    }

    /*************************************************************************
    * @brief Write data channels
    * 
    * @param [in] aGroup <root>/"channels"
    *************************************************************************/
    // void M4StreamHeader::WriteChannels( HAS_ATTR_IFC* aLoc )
    void M4StreamHeader::WriteChannels( z5GroupHandle aGroup )
    {
std::cout << "M4StreamHeader::WriteChannels() for " << fLabel << std::endl;
#if 0 // old HDF5 code reference
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
#endif
        // create a new Dataset for the data of this channel
        xt::xarray<uint32_t>::shape_type tCSShape = { 1,fNChannels };   // <row>,<col>
        xt::xarray<uint32_t> tCSBuffer(tCSShape);

        // Populate write buffer from the fChannelStreams
        for( unsigned i = 0; i < fNChannels; ++i )
        {
            tCSBuffer[ i ] = fChannels[ i ];;
        }

        // Create the dataset for "channels"
        const std::string dsName = "channels";
        std::vector<size_t> shape = { 1,fNChannels };   // <row>,<col>
        std::vector<size_t> chunks = { 1,fNChannels };  // <row>,<col>
        auto ds = z5::createDataset( aGroup, dsName, "uint32", shape, chunks );
        
std::cout << "Write the streams to the file\n";
        z5::types::ShapeType writeOffset = { 0,0 };
        z5::multiarray::writeSubarray<uint32_t>(ds, tCSBuffer, writeOffset.begin());

std::cout << "M4StreamHeader::WriteChannels() void " << fLabel << std::endl;
    }

    /*************************************************************************
    * @brief Read data channels
    * 
    * @param [in] aGroup <root>/"channels"
    *************************************************************************/
    void M4StreamHeader::ReadChannels( const z5GroupHandle aGroup ) const
    {
std::cout << "M4StreamHeader::ReadChannels() " << fLabel << std::endl;
#if 0 // old HDF5 code reference
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
#endif

        // Open the "channels" dataset from file
        auto ds = z5::openDataset(aGroup, "channels");

        // create a new Dataset for the data of this channel
        xt::xarray<uint32_t>::shape_type tCSShape = { 1,fNChannels };       // <row>,<col>
        xt::xarray<uint32_t> tCSBuffer(tCSShape);
        
        z5::types::ShapeType readOffset = { 0,0 };
        z5::multiarray::readSubarray<uint32_t>(ds, tCSBuffer, readOffset.begin());

        // Read data channels
        fChannels.clear();
        fChannels.resize( fNChannels );
        for( unsigned i = 0; i < fNChannels; ++i )
        {
            fChannels[ i ] = tCSBuffer[ i ];
        }

std::cout << "M4StreamHeader::ReadChannels(): void " << fLabel << std::endl;;        
    }

    /*************************************************************************
    * @brief Construct a new M4ChannelHeader::M4ChannelHeader object
    * @return none
    * @note Default M4ChannelHeader CTOR, initializes member vars
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
//std::cout << "M4ChannelHeader::M4ChannelHeader() default CTOR\n";
    }

    /*************************************************************************
    * @brief Construct a new M4ChannelHeader::M4ChannelHeader object from 
    * parameter specifications and initializes member vars with specified values.
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
//std::cout << "M4ChannelHeader::M4ChannelHeader() params CTOR: chan " << aNumber << std::endl;
        SetNumber( aNumber );
    }

    /*************************************************************************
    * @brief Construct a new M4ChannelHeader::M4ChannelHeader object from 
    * template specifications and initializes member vars with specified values.
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
//std::cout << "M4ChannelHeader::M4ChannelHeader() template CTOR: chan " << orig.fNumber << std::endl;
        SetNumber( orig.fNumber );
    }

    /*************************************************************************
    * @brief Destroy the M4ChannelHeader::M4ChannelHeader object
    * @return none
    * @note Default M4ChannelHeader DTOR
    *************************************************************************/
    M4ChannelHeader::~M4ChannelHeader()
    {
//std::cout << "M4ChannelHeader::~M4ChannelHeader() DTOR: chan " << fNumber << std::endl;
        if (fLabel != nullptr)
        {
            delete [] fLabel;
        }
    }

    /*************************************************************************
    * @brief Set M4ChannelHeader identifier label number
    * 
    * @param aNumber Channel number
    *************************************************************************/
    void M4ChannelHeader::SetNumber( uint32_t aNumber ) const
    {
///@todo fLabel as std::string 
 
        fNumber = aNumber;
        static const size_t prefixSize = 7; // # of characters in "channel"
        if(fLabel != nullptr)
        {
            delete[] fLabel;
        }
        fLabel = new char[ prefixSize + 10 ]; // 10 = max digits in 32-bit integer
        strcpy( fLabel, "channel" );
        u32toa( aNumber, fLabel + prefixSize );
    }

    /*************************************************************************
    * @brief Write the channel header attributes to file
    * 
    * @param [in] aGroup <root>/"channels" group
    *************************************************************************/
    // void M4ChannelHeader::WriteToHDF5( HAS_GRP_IFC* aParent )
    void M4ChannelHeader::WriteToFile( z5GroupHandle aGroup )
    {
//std::cout << "M4ChannelHeader::WriteToFile(): " << fLabel << std::endl;
        LDEBUG( mlog, "Writing channel header <" << fLabel << ">" );
        
#if 0 // old HDF5 code reference
        // Create a group to hold channels, handle to access it
        H5::Group tThisChannelGroup = aParent->createGroup( fLabel );

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
#endif        
        // Create the attributes component and write to file
        z5::createGroup(aGroup, fLabel);
        auto channelsHandle = z5GroupHandle(aGroup, fLabel);
        nlohmann::json jchanAttr;

//std::cout << "  number: " << fNumber << std::endl;
//std::cout << "  source: " << fSource << std::endl;
//std::cout << "  acquisition_rate: " << fAcquisitionRate << std::endl;
//std::cout << "  record_size: " << fRecordSize << std::endl;
//std::cout << "  sample_size: " << fSampleSize << std::endl;
//std::cout << "  data_type_size: " << fDataTypeSize << std::endl;
//std::cout << "  data_format: " << fDataFormat << std::endl;
//std::cout << "  bit_depth: " << fBitDepth << std::endl;
//std::cout << "  bit_alignment: " << fBitAlignment << std::endl;
//std::cout << "  voltage_offset: " << fVoltageOffset << std::endl;
//std::cout << "  voltage_range: " << fVoltageRange << std::endl;
//std::cout << "  dac_gain: " << fDACGain << std::endl;
//std::cout << "  frequency_min: " << fFrequencyMin << std::endl;
//std::cout << "  frequency_range: " << fFrequencyRange << std::endl;

        jchanAttr["number"] = fNumber;
        jchanAttr["source"] = fSource;
        jchanAttr["acquisition_rate"] = fAcquisitionRate;
        jchanAttr["record_size"] = fRecordSize;
        jchanAttr["sample_size"] = fSampleSize;
        jchanAttr["data_type_size"] = fDataTypeSize;
        jchanAttr["data_format"] = fDataFormat;
        jchanAttr["bit_depth"] = fBitDepth;
        jchanAttr["bit_alignment"] = fBitAlignment;
        jchanAttr["voltage_offset"] = fVoltageOffset;
        jchanAttr["voltage_range"] = fVoltageRange;
        jchanAttr["dac_gain"] = fDACGain;
        jchanAttr["frequency_min"] = fFrequencyMin;
        jchanAttr["frequency_range"] = fFrequencyRange;

        z5::writeAttributes(channelsHandle, jchanAttr);

//std::cout << "M4ChannelHeader::WriteToFile(): void " << fLabel << std::endl;
    }

    /*************************************************************************
    * @brief Read channel header attributes from file
    * 
    * @param [in] aGroup <root>/"channels" group
    * @param [in] aLabel 
    *************************************************************************/
    void M4ChannelHeader::ReadFromFile( const z5GroupHandle aGroup, const std::string& aLabel ) const
    {
//std::cout << "M4ChannelHeader::ReadFromFile(): " << aLabel << std::endl;
///@todo ?? don't need aLabel parameter anymore ?? 
/// 
        LDEBUG( mlog, "Reading channel header <" << aLabel << ">" );
#if 0 // old HDF5 code reference
        H5::Group tThisChannelGroup = aParent->openGroup( aLabel.c_str() );

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
#endif        
        nlohmann::json jchanAttr;
        z5::readAttributes( aGroup, jchanAttr );

//std::cout << "  number: " << chanAttr.at("number") << std::endl;
//std::cout << "  source: " << chanAttr.at("source") << std::endl;
//std::cout << "  acquisition_rate: " << chanAttr.at("acquisition_rate") << std::endl;
//std::cout << "  record_size: " << chanAttr.at("record_size") << std::endl;
//std::cout << "  sample_size: " << chanAttr.at("sample_size") << std::endl;
//std::cout << "  data_type_size: " << chanAttr.at("data_type_size") << std::endl;
//std::cout << "  data_format: " << chanAttr.at("data_format") << std::endl;
//std::cout << "  bit_depth: " << chanAttr.at("bit_depth") << std::endl;
//std::cout << "  bit_alignment: " << chanAttr.at("bit_alignment") << std::endl;
//std::cout << "  voltage_offset: " << chanAttr.at("voltage_offset") << std::endl;
//std::cout << "  voltage_range: " << chanAttr.at("voltage_range") << std::endl;
//std::cout << "  dac_gain: " << chanAttr.at("dac_gain") << std::endl;
//std::cout << "  frequency_min: " << chanAttr.at("frequency_min") << std::endl;
//std::cout << "  frequency_range: " << chanAttr.at("frequency_range") << std::endl;

        try
        {
            // Programmer's Note: using casts to ensure correct conversion function gets selected
            // Programmer's Note: using .at() function to ensure proper exception operation
            fNumber = (uint32_t)jchanAttr.at("number");
            fSource = (std::string)jchanAttr.at("source");
            fAcquisitionRate = (uint32_t)jchanAttr.at("acquisition_rate");
            fRecordSize = (uint32_t)jchanAttr.at("record_size");
            fSampleSize = (uint32_t)jchanAttr.at("sample_size");
            fDataTypeSize = (uint32_t)jchanAttr.at("data_type_size");
            fDataFormat = (uint32_t)jchanAttr.at("data_format");
            fBitDepth = (uint32_t)jchanAttr.at("bit_depth");
            fBitAlignment = (uint32_t)jchanAttr.at("bit_alignment");
            fVoltageOffset = (double)jchanAttr.at("voltage_offset");
            fVoltageRange = (double)jchanAttr.at("voltage_range");
            fDACGain = (double)jchanAttr.at("dac_gain");
            fFrequencyMin = (double)jchanAttr.at("frequency_min");
            fFrequencyRange = (double)jchanAttr.at("frequency_range");
        }
        catch (const nlohmann::json::exception& e)
        { // JSON error
            throw M4Exception() << "JSON error M4ChannelHeader::ReadFromFile(): " << e.what();
        }

//std::cout << "M4ChannelHeader::ReadFromFile(): void\n";
    }

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
            fFile(nullptr),
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
std::cout << "M4Header::~M4Header()\n";
        if (fStreamsGroup != nullptr)  
        {
            delete fStreamsGroup;
        }
        
        if (fChannelsGroup != nullptr)  
        {
            delete fChannelsGroup;
        }
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
//std::cout << "M4Header::SetCoherence()\n";

        if( aChanA >= fNChannels || aChanB >= fNChannels )
        {
            throw M4Exception() << "Channel out of bounds: " << aChanA << " or " << aChanB << " >= " << fNChannels;
        }
        fChannelCoherence[ aChanA ][ aChanB ] = aCoherence;
        fChannelCoherence[ aChanB ][ aChanA ] = aCoherence;

//std::cout << "M4Header::SetCoherence(): void\n";
    }

    /*************************************************************************
    * @brief Add a new single-channel stream 
    * 
    * @param [in] aSource Device used to produce the stream
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
    * @param [out] aChanVec Vector returned with the channel numbers of this stream
    * 
    * @return uint32_t Returns the stream number (used to address the stream later)
    *************************************************************************/
    uint32_t M4Header::AddStream( const std::string& aSource,
                                 uint32_t anAcqRate, uint32_t aRecSize, uint32_t aSampleSize,
                                 uint32_t aDataTypeSize, uint32_t aDataFormat,
                                 uint32_t aBitDepth, uint32_t aBitAlignment,
                                 std::vector< unsigned >* aChanVec )
    {
std::cout << "M4Header::AddStream() single-channel: " << aSource << std::endl;

        LDEBUG( mlog, "Adding stream " << fNStreams << " for channel " << fNChannels << " with record size " << aRecSize );
        
        if( aChanVec != nullptr ) 
        { 
            aChanVec->push_back( fNChannels );      // this channel number allocated for this stream
        }

        fChannelStreams.push_back( fNStreams );     // adding another stream to M4Header

        // Create a new channel header with specified metadata, add to M4Header
        fChannelHeaders.push_back( M4ChannelHeader( aSource, fNChannels, anAcqRate, aRecSize, aSampleSize, 
                                                    aDataTypeSize, aDataFormat, aBitDepth, aBitAlignment ) );
        
        // Create a new (single) stream header with specified metadata, add to M4Header
        fStreamHeaders.push_back( M4StreamHeader( aSource, fNStreams, 1, fNChannels, sSeparate, anAcqRate, 
                                                  aRecSize, aSampleSize, aDataTypeSize, aDataFormat, aBitDepth, aBitAlignment ) );

        ++fNChannels;   // next channel number used

        // Setup channel coherence mapping
//std::cout << "resizing coherence to " << fNChannels << " channels" << std::endl;
        fChannelCoherence.resize( fNChannels ); // resize number of columns
        for( unsigned i = 0; i < fNChannels; ++i ) // resize each row
        {
            fChannelCoherence[ i ].resize( fNChannels, false );
        }
        fChannelCoherence.back().back() = true; // each channel is coherent with itself

std::cout << "M4Header::AddStream() single-channel: EXIT\n";
        return fNStreams++;
    }

    /*************************************************************************
    * @brief Add a new multi-channel stream 
    * 
    * @param [in] aSource Device used to produce the stream
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
    * @param [out] aChanVec Vector returned with the channel numbers used in this stream
    * 
    * @return uint32_t Returns the stream number (used to address the stream later)
    *************************************************************************/
    uint32_t M4Header::AddStream( const std::string& aSource, uint32_t aNChannels, uint32_t aFormat,
                                 uint32_t anAcqRate, uint32_t aRecSize, uint32_t aSampleSize,
                                 uint32_t aDataTypeSize, uint32_t aDataFormat,
                                 uint32_t aBitDepth, uint32_t aBitAlignment,
                                 std::vector< unsigned >* aChanVec )
    {
std::cout << "M4Header::AddStream() multi-channel: " << aSource << std::endl;

        LDEBUG( mlog, "Adding stream " << fNStreams << " for multiple channels with record size " << aRecSize );

        // First new channel for this stream picks up where previously allocated channels left off
        unsigned tFirstNewChannel = fNChannels;
        
        // Add multiple channels to this stream
        for( uint32_t iNewChannel = 0; iNewChannel < aNChannels; ++iNewChannel )
        {
            LDEBUG( mlog, "Adding channel " << fNChannels );
            if( aChanVec != nullptr ) 
            {
                aChanVec->push_back( fNChannels );  // report the channel number used in this stream
            }
            
            fChannelStreams.push_back( fNStreams ); // adding another stream to M4Header

            // Create a new channel header with specified metadata, add to M4Header
            fChannelHeaders.push_back( M4ChannelHeader( aSource, fNChannels, anAcqRate, aRecSize, 
                aSampleSize, aDataTypeSize, aDataFormat, aBitDepth, aBitAlignment ) );

            ++fNChannels;   // next channel number
            
            fChannelCoherence.resize( fNChannels );         // resize number of columns
            for( unsigned i = 0; i < fNChannels; ++i )      // resize all rows
            {
                fChannelCoherence[ i ].resize( fNChannels, false );
            }
            
            // Setup channel coherence mapping
            fChannelCoherence.back().back() = true;     // each channel is coherent with itself

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

        // Create the new (multi) stream header
        fStreamHeaders.push_back( M4StreamHeader( aSource, fNStreams, aNChannels, tFirstNewChannel, 
            aFormat, anAcqRate, aRecSize, aSampleSize, aDataTypeSize, aDataFormat, aBitDepth, aBitAlignment ) );
        
std::cout << "M4Header::AddStream() multi-channel EXIT\n";
        return fNStreams++;
    }

    /*************************************************************************
    * @brief Write file attributes, stream headers, channel headers to file
    * 
    * @param aFile path/filename
    * @return none, throw M4Exception on failure
    *************************************************************************/
    // void M4Header::WriteToHDF5( H5::H5File* aFile )
    void M4Header::WriteToFile( z5FileHandle* aFile )
    {
std::cout << "M4Header::WriteToFile()\n"; 
#if 0 // old HDF5 code reference
        try
        {
            LDEBUG( mlog, "Writing run header" );
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

            LDEBUG( mlog, "Writing stream headers" );
            fStreamsGroup = new H5::Group( fFile->createGroup( "streams" ) );
            for( uint32_t iStream = 0; iStream < fNStreams; ++iStream )
            {
                fStreamHeaders[ iStream ].WriteToHDF5( fStreamsGroup );
            }

            LDEBUG( mlog, "Writing channel headers" );
            fChannelsGroup = new H5::Group( fFile->createGroup( "channels" ) );
            for( uint32_t iChan = 0; iChan < fNChannels; ++iChan )
            {
                fChannelHeaders[ iChan ].WriteToHDF5( fChannelsGroup );
            }
        }
        catch( H5::Exception& e )
        {
            throw M4Exception() << "HDF5 error while writing header: " << e.getCDetailMsg();
        }
        catch( M4Exception& e )
        {
            LDEBUG( mlog, "M4Exception: " << e.what() );
            throw;
        }
#endif
        LDEBUG( mlog, "Writing run header" );
        fFile = aFile;

        nlohmann::json jheaderAttr;

        jheaderAttr["egg_version"] = fEggVersion;
        jheaderAttr["filename"] = fFilename;
        jheaderAttr["n_channels"] = fNChannels;
        jheaderAttr["n_streams"] = fNStreams;
        jheaderAttr["run_duration"] = fRunDuration;
        jheaderAttr["timestamp"] = fTimestamp;
        jheaderAttr["description"] = fDescription;
        z5::writeAttributes(*aFile, jheaderAttr);

        WriteChannelStreams( aFile );
        WriteChannelCoherence( aFile );

        // Write all the stream headers to file
        z5::createGroup(*aFile, "streams");    
        fStreamsGroup = new z5GroupHandle(*aFile, "streams");   // <root>/"streams" group

        // create attr: NStreams to record how many streams were in run
        nlohmann::json jstreamAttr;
        jstreamAttr["NStreams"] = fNStreams;
        z5::writeAttributes(*fStreamsGroup, jstreamAttr);

        LDEBUG( mlog, "Writing all stream headers" );
        for( uint32_t iStream = 0; iStream < fNStreams; ++iStream )
        {
            fStreamHeaders[ iStream ].WriteToFile( *fStreamsGroup );
        }

        // Write all the channel headers
        LDEBUG( mlog, "Writing all channel headers" );

        z5::createGroup( *aFile, "channels" );   
        fChannelsGroup = new z5GroupHandle( *aFile, "channels" );   // <root>/"channels" group

        // create attr: NChannels to record how many channels were in run
        nlohmann::json jchanAttr;

        jchanAttr["NChannels"] = fNChannels;
        z5::writeAttributes(*fChannelsGroup, jchanAttr);

        for( uint32_t iChan = 0; iChan < fNChannels; ++iChan )
        {
            // Write each of the channel headers
            fChannelHeaders[ iChan ].WriteToFile( *fChannelsGroup );
        }

std::cout << "M4Header::WriteToFile(): void\n";        
    }

    /*************************************************************************
    * @brief Read file attributes, stream headers, channel headers from file
    * 
    * @param aFile path/filename
    * @return none, throw M4Exception on failure
    *************************************************************************/
    void M4Header::ReadFromFile(const z5FileHandle* aFile ) const
    {
std::cout << "M4Header::ReadFromFile()\n"; 
       
#if 0 // old HDF5 code reference
        try
        {
            LDEBUG( mlog, "Reading run header" );
            fFile = const_cast< H5::H5File* >( aFile );
            EggVersion() = ReadScalarFromHDF5< string >( fFile, string("egg_version") );
            Filename() = ReadScalarFromHDF5< string >( fFile, "filename" );
            SetNChannels( ReadScalarFromHDF5< uint32_t >( fFile, "n_channels" ) );
            SetNStreams( ReadScalarFromHDF5< uint32_t >( fFile, "n_streams" ) );
            SetRunDuration( ReadScalarFromHDF5< uint32_t >( fFile, "run_duration" ) );
            Timestamp() = ReadScalarFromHDF5< string >( fFile, "timestamp" );
            Description() = ReadScalarFromHDF5< string >( fFile, "description" );

            //fChannelStreams.clear();
            //Read1DFromHDF5< std::vector< uint32_t > >( fFile, "channel_streams", fChannelStreams );
            ReadChannelStreams( aFile );
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
            fStreamsGroup = new H5::Group( fFile->openGroup( "streams" ) );
            hsize_t nStreams = fStreamsGroup->getNumObjs();
            if( nStreams != fNStreams )
            {
                throw M4Exception() << "Number of streams <" << fNStreams << "> disagrees with the number of objects in the stream group <" << nStreams << ">";
            }
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
            fChannelsGroup = new H5::Group( fFile->openGroup( "channels" ) );
            hsize_t nChannels = fChannelsGroup->getNumObjs();
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
        LDEBUG( mlog, "Reading run header" );
        fFile = (z5FileHandle*)aFile;

        // // Read the File Atrributes
        nlohmann::json headerAttr;
        z5::readAttributes( *aFile, headerAttr );

        try
        {
            // Programmer's Note: using casts to ensure correct conversion function gets selected
            // Programmer's Note: using .at() function to ensure proper exception operation
            fEggVersion = (std::string)headerAttr.at("egg_version");
            fFilename = (std::string)headerAttr.at("filename");
            fNChannels = (uint32_t)headerAttr.at("n_channels");
            fNStreams = (uint32_t)headerAttr.at("n_streams");
            fRunDuration = (uint32_t)headerAttr.at("run_duration");
            fTimestamp = (std::string)headerAttr.at("timestamp");
            fDescription = (std::string)headerAttr.at("description");

//std::cout << "fEggVersion: " << (std::string)fEggVersion << std::endl;
//std::cout << "fFilename: " << (std::string)fFilename << std::endl;
//std::cout << "fNChannels: " << (uint32_t)fNChannels << std::endl;
//std::cout << "fNStreams: " << (uint32_t)fNStreams << std::endl;
//std::cout << "fRunDuration: " << (uint32_t)fRunDuration << std::endl;
//std::cout << "fTimestamp: " << (std::string)fTimestamp << std::endl;
//std::cout << "fDescription: " << (std::string)fDescription << std::endl;

            fChannelStreams.clear();
            ReadChannelStreams( aFile );            // Read Stream Attributes
            ReadChannelCoherence( aFile );

///@todo adapt for Windows
            // // Windows' HDF5 really doesn't like using std::strings
            // const unsigned tBuffSize = 256;
            // char tBuffer[ tBuffSize ];

            // Create streams group correspoding to streams group
            fStreamsGroup = new z5GroupHandle(*aFile, "streams");   // <root>/"streams" group

            // See how many streams are used
            nlohmann::json jstreamAttr;
            z5::readAttributes( *fStreamsGroup, jstreamAttr );

            size_t nStreams = jstreamAttr.at("NStreams");

            LDEBUG( mlog, "Reading stream headers" );
            fStreamHeaders.clear();

            // Read each of the stream-header objects and store
            for( size_t iStream = 0; iStream < nStreams; ++iStream )
            {
                // Construct the stream label
                std::string streamLabel = "streams/stream" + std::to_string(iStream);

                // Create a new M4StreamHeader in the collection, at the back
                fStreamHeaders.push_back( M4StreamHeader() );

                // Populate the new header (within the collection) from the file
                // Stream group at this label-path
                auto strmHandle = z5GroupHandle( *aFile, streamLabel );
                fStreamHeaders.back().ReadFromFile( strmHandle, streamLabel );
            }

            // Create channels group correspoding to file group
            fChannelsGroup = new z5GroupHandle( *aFile, "channels");    // <root>/"channels"

            nlohmann::json jchanAttr;
            z5::readAttributes( *fChannelsGroup, jchanAttr );

            // Get how many channels are in group
            size_t nChannels = jchanAttr.at("NChannels");
            std::cout << "nChannels: " << nChannels << std::endl;

            LDEBUG( mlog, "Reading channel headers" );
            fChannelHeaders.clear();

            // Read each of the channel-header objects and store
            for( size_t iChan = 0; iChan < nChannels; ++iChan )
            {
                // Channel label
                std::string chanLabel = "channels/channel" + std::to_string(iChan);

                // Create a new M4ChannelHeader in the collection, at the back
                fChannelHeaders.push_back( M4ChannelHeader() );

                // Populate the new header (within the collection) from the file
                // Channel group at this label-path
                auto chanHandle = z5GroupHandle( *aFile, chanLabel );
                fChannelHeaders.back().ReadFromFile( chanHandle, chanLabel );
            }
        }
        catch (const nlohmann::json::exception& e)
        { // JSON error
            throw M4Exception() << "JSON error M4Header::ReadFromFile(): " << e.what();
        }

std::cout << "M4Header::ReadFromFile(): void\n";
    }

    /*************************************************************************
    * @brief Write channel-streams to file
    * 
    * @param aFile File handle
    * @return none, no exceptions thrown 
    *************************************************************************/
    void M4Header::WriteChannelStreams( z5FileHandle* aFile)
    {
//std::cout << "M4Header::WriteChannelStreams()\n";
#if 0 // old HDF5 code reference
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
#endif

        // create a new Dataset for the data of this stream
        xt::xarray<uint32_t>::shape_type tCSShape = { 1,fNChannels };       // <row>,<col>
        xt::xarray<uint32_t> tCSBuffer(tCSShape,0);

        // Populate write buffer from the fChannelStreams: channel->stream mapping
        for( unsigned i = 0; i < fNChannels; ++i )
        {
            tCSBuffer[ i ] = fChannelStreams[ i ];
//std::cout << "fChannelStreams[" << i << "]: " << fChannelStreams[ i ] << " / " << tCSBuffer[ i ] << std::endl;
        }

        // Create the Dataspace for "channel_streams"
        const std::string dsName = "channel_streams";
        std::vector<size_t> shape = { 1,fNChannels };       // <row>,<col>
        std::vector<size_t> chunks = { 1,fNChannels };      // <row>,<col>
        auto ds = z5::createDataset( *aFile, dsName, "uint32", shape, chunks );
        
        z5::types::ShapeType writeOffset = { 0,0 };
        z5::multiarray::writeSubarray<uint32_t>(ds, tCSBuffer, writeOffset.begin());

//std::cout << "M4Header::WriteChannelStreams(): void\n";
    }

    /*************************************************************************
    * @brief Read the channel-streams from file
    * 
    * @param aLoc Stream location attribute
    * @return none, no exceptions thrown 
    *************************************************************************/
    void M4Header::ReadChannelStreams( const z5FileHandle* aFile ) const
    {
//std::cout << "M4Header::ReadChannelStreams(): " << fNChannels << " channels\n";
 #if 0 // old HDF5 code reference
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
#endif    

        // Open the "channel_streams" dataset from file
        const auto dsHandle = z5DatasetHandle(*aFile, "channel_streams");
        auto ds = z5::openDataset(*aFile, "channel_streams");

        // Programmer's Note: M4Header::ReadFromFile() initializes fNChannels from M4Header metadata
        xt::xarray<uint32_t>::shape_type tCSShape = { 1,fNChannels };   // <row>,<col>
        xt::xarray<uint32_t> tCSBuffer(tCSShape,0);   // buffer-array for dataset
        
        z5::types::ShapeType readOffset = { 0,0 };
        z5::multiarray::readSubarray<uint32_t>(ds, tCSBuffer, readOffset.begin());

        // Populate the M4Header::fChannelStreams from read buffer
        fChannelStreams.clear();
        fChannelStreams.resize( fNChannels );
        for( unsigned i = 0; i < fNChannels; ++i )
        {
            fChannelStreams[ i ] = tCSBuffer[ i ];
//std::cout << "fChannelStreams[" << i << "]: " << fChannelStreams[ i ] << " / " << tCSBuffer[ i ] << std::endl;
        }

//std::cout << "M4Header::ReadChannelStreams(): void\n";
    }

    /*************************************************************************
    * @brief Write channel-coherence to file
    * 
    * @param aLoc 
    *************************************************************************/
    void M4Header::WriteChannelCoherence( z5FileHandle* aFile )
    {
//std::cout << "M4Header::WriteChannelCoherence()\n";
#if 0 // old HDF5 code reference
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
#endif

        // create a new Dataset for the data of this channel
        xt::xarray<uint8_t>::shape_type tCCShape = { 1,fNChannels * fNChannels };   // reversed: <row>,<col>
        xt::xarray<uint8_t> tCCBuffer(tCCShape);

        // Populate write buffer from the fChannelStreams
        for( unsigned i = 0; i < fNChannels; ++i )
        {
            for( unsigned j = 0; j < fNChannels; ++j )
            {
                tCCBuffer[ i * fNChannels + j ] = (uint8_t)fChannelCoherence[ i ][ j ];
            }
        }

        // Create the dataset for "channel_coherence"
        const std::string dsName = "channel_coherence";
        std::vector<size_t> shape = { 1,fNChannels * fNChannels };      // <row>,<col>
        std::vector<size_t> chunks = { 1,fNChannels * fNChannels };     // <row>,<col>
        auto ds = z5::createDataset( *aFile, dsName, "uint8", shape, chunks );
        
        z5::types::ShapeType writeOffset = { 0,0 };
        z5::multiarray::writeSubarray<uint8_t>(ds, tCCBuffer, writeOffset.begin());

//std::cout << "M4Header::WriteChannelCoherence(): void\n";
    }

    /*************************************************************************
    * @brief Read channel-coherence from file
    * 
    * @param aLoc  
    *************************************************************************/
    void M4Header::ReadChannelCoherence( const z5FileHandle* aFile ) const
    {
//std::cout << "M4Header::ReadChannelCoherence()\n";
#if 0 // old HDF5 code reference
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
#endif
        // Open the "channel_coherence" dataset from file
        const auto dsHandle = z5DatasetHandle(*aFile, "channel_coherence");
        auto ds = z5::openDataset(*aFile, "channel_coherence");

        // create a new Dataset for the data of this stream
        xt::xarray<uint8_t>::shape_type tCCShape = { 1,fNChannels * fNChannels };   // <row>,<col>
        xt::xarray<uint8_t> tCCBuffer(tCCShape);
        
        z5::types::ShapeType readOffset = { 0,0 };
        z5::multiarray::readSubarray<uint8_t>(ds, tCCBuffer, readOffset.begin());

        // Copy over channel-coherence from temporary buffer into component
        fChannelCoherence.clear();
        fChannelCoherence.resize( fNChannels );
        for( unsigned i = 0; i < fNChannels; ++i )
        {
            fChannelCoherence[ i ].resize( fNChannels );
            for( unsigned j = 0; j < fNChannels; ++j )
            {
                fChannelCoherence[ i ][ j ] = (bool)tCCBuffer[ i * fNChannels + j ];
//std::cout << "fChannelCoherence[" << i << "][" << j << "]: " << fChannelCoherence[ i ][ j ] << std::endl;
            }
        }

//std::cout << "M4Header::ReadChannelCoherence(): void\n";
    }
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
    out << "\nStream Header Content:\n";
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
    out << "\nChannel Header Content:\n";
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
