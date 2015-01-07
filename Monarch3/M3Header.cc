/*
 * M3Header.cc
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 */

#include "M3Header.hh"

#include "M3IToA.hh"
#include "M3Logger.hh"
#include "M3Version.hh"

#include <cstdlib> // for atol in parsing timestamp

// for parsing timestamp
#include <sstream>
using std::stringstream;
using std::string;

namespace monarch3
{
    M3LOGGER( mlog, "M3Header" );

    //*********************
    // M3StreamHeader
    //*********************

    M3StreamHeader::M3StreamHeader() :
            fLabel( NULL ),
            fNumber( 0 ),
            fSource(),
            fNChannels( 0 ),
            fChannelFormat( sSeparate ),
            fAcquisitionRate( 0 ),
            fRecordSize( 0 ),
            fDataTypeSize( 0 ),
            fDataFormat( sDigitized ),
            fBitDepth( 0 ),
            fNAcquisitions( 0 ),
            fNRecords( 0 )
    {
    }

    M3StreamHeader::M3StreamHeader( const std::string& aSource, uint32_t aNumber, uint32_t aNChannels, MultiChannelFormatType aFormat,
                    uint32_t anAcqRate, uint32_t aRecSize,
                    uint32_t aDataTypeSize, DataFormatType aDataFormat,
                    uint32_t aBitDepth ) :
            fLabel( NULL ),
            fNumber( 0 ),
            fSource( aSource ),
            fNChannels( aNChannels ),
            fChannelFormat( aFormat ),
            fAcquisitionRate( anAcqRate ),
            fRecordSize( aRecSize ),
            fDataTypeSize( aDataTypeSize ),
            fDataFormat( aDataFormat ),
            fBitDepth( aBitDepth ),
            fNAcquisitions( 0 ),
            fNRecords( 0 )
    {
        SetNumber( aNumber );
    }

    M3StreamHeader::M3StreamHeader( const M3StreamHeader& orig ) :
            fLabel( NULL ),
            fNumber( 0 ),
            fSource( orig.fSource ),
            fNChannels( orig.fNChannels ),
            fChannelFormat( orig.fChannelFormat ),
            fAcquisitionRate( orig.fAcquisitionRate ),
            fRecordSize( orig.fRecordSize ),
            fDataTypeSize( orig.fDataTypeSize ),
            fDataFormat( orig.fDataFormat ),
            fBitDepth( orig.fBitDepth ),
            fNAcquisitions( orig.fNAcquisitions ),
            fNRecords( orig.fNRecords )
    {
        SetNumber( orig.fNumber );
    }

    M3StreamHeader::~M3StreamHeader()
    {
        delete [] fLabel;
    }

    void M3StreamHeader::SetNumber( uint32_t aNumber ) const
    {
        fNumber = aNumber;

        static const size_t prefixSize = 6; // # of characters in "stream"
        delete [] fLabel;
        fLabel = new char[ prefixSize + 10 ]; // 10 = max digits in 32-bit integer
        strcpy( fLabel, "stream" );
        u32toa( aNumber, fLabel + prefixSize );

        return;
    }

    void M3StreamHeader::WriteToHDF5( H5::CommonFG* aParent )
    {
        M3DEBUG( mlog, "Writing stream <" << fLabel << ">" );
        H5::Group tThisStreamGroup = aParent->createGroup( fLabel );

        M3Header::WriteScalarToHDF5( &tThisStreamGroup, "number", fNumber );
        M3Header::WriteScalarToHDF5( &tThisStreamGroup, "source", fSource );
        M3Header::WriteScalarToHDF5( &tThisStreamGroup, "n_channels", fNChannels );
        M3Header::WriteScalarToHDF5( &tThisStreamGroup, "channel_format", fChannelFormat );
        M3Header::WriteScalarToHDF5( &tThisStreamGroup, "acquisition_rate", fAcquisitionRate );
        M3Header::WriteScalarToHDF5( &tThisStreamGroup, "record_size", fRecordSize );
        M3Header::WriteScalarToHDF5( &tThisStreamGroup, "data_type_size", fDataTypeSize );
        M3Header::WriteScalarToHDF5( &tThisStreamGroup, "data_format", fDataFormat );
        M3Header::WriteScalarToHDF5( &tThisStreamGroup, "bit_depth", fBitDepth );
        M3Header::WriteScalarToHDF5( &tThisStreamGroup, "n_acquisitions", fNAcquisitions );
        M3Header::WriteScalarToHDF5( &tThisStreamGroup, "n_records", fNRecords );

        return;
    }

    void M3StreamHeader::ReadFromHDF5( const H5::CommonFG* aParent, const std::string& aLabel ) const
    {
        M3DEBUG( mlog, "Reading stream <" << aLabel << ">" );
        H5::Group tThisStreamGroup = aParent->openGroup( aLabel );

        SetNumber( M3Header::ReadScalarFromHDF5< uint32_t >( &tThisStreamGroup, "number" ) );
        SetSource( M3Header::ReadScalarFromHDF5< string >( &tThisStreamGroup, "source" ) );
        SetNChannels( M3Header::ReadScalarFromHDF5< uint32_t >( &tThisStreamGroup, "n_channels" ) );
        SetChannelFormat( M3Header::ReadScalarFromHDF5< MultiChannelFormatType >( &tThisStreamGroup, "channel_format" ) );
        SetAcquisitionRate( M3Header::ReadScalarFromHDF5< uint32_t >( &tThisStreamGroup, "acquisition_rate" ) );
        SetRecordSize( M3Header::ReadScalarFromHDF5< uint32_t >( &tThisStreamGroup, "record_size" ) );
        SetDataTypeSize( M3Header::ReadScalarFromHDF5< uint32_t >( &tThisStreamGroup, "data_type_size" ) );
        SetDataFormat( M3Header::ReadScalarFromHDF5< DataFormatType >( &tThisStreamGroup, "data_format" ) );
        SetBitDepth( M3Header::ReadScalarFromHDF5< uint32_t >( &tThisStreamGroup, "bit_depth" ) );
        SetNAcquisitions( M3Header::ReadScalarFromHDF5< uint32_t >( &tThisStreamGroup, "n_acquisitions" ) );
        SetNRecords( M3Header::ReadScalarFromHDF5< uint32_t >( &tThisStreamGroup, "n_records" ) );

        return;
    }

    //*********************
    // M3ChannelHeader
    //*********************

    M3ChannelHeader::M3ChannelHeader() :
            fLabel( NULL ),
            fNumber( 0 ),
            fSource(),
            fAcquisitionRate( 0 ),
            fRecordSize( 0 ),
            fDataTypeSize( 0 ),
            fDataFormat( sDigitized ),
            fBitDepth( 0 ),
            fVoltageMin( 0. ),
            fVoltageRange( 0. ),
            fFrequencyMin( 0. ),
            fFrequencyRange( 0. )
    {
    }

    M3ChannelHeader::M3ChannelHeader( const std::string& aSource, uint32_t aNumber,
                    uint32_t anAcqRate, uint32_t aRecSize,
                    uint32_t aDataTypeSize, DataFormatType aDataFormat,
                    uint32_t aBitDepth ) :
            fLabel( NULL ),
            fNumber( 0 ),
            fSource( aSource ),
            fAcquisitionRate( anAcqRate ),
            fRecordSize( aRecSize ),
            fDataTypeSize( aDataTypeSize ),
            fDataFormat( aDataFormat ),
            fBitDepth( aBitDepth ),
            fVoltageMin( 0. ),
            fVoltageRange( 0. ),
            fFrequencyMin( 0. ),
            fFrequencyRange( 0. )
    {
        SetNumber( aNumber );
    }

    M3ChannelHeader::M3ChannelHeader( const M3ChannelHeader& orig ) :
            fLabel( NULL ),
            fNumber( 0 ),
            fSource( orig.fSource ),
            fAcquisitionRate( orig.fAcquisitionRate ),
            fRecordSize( orig.fRecordSize ),
            fDataTypeSize( orig.fDataTypeSize ),
            fDataFormat( orig.fDataFormat ),
            fBitDepth( orig.fBitDepth ),
            fVoltageMin( orig.fVoltageMin ),
            fVoltageRange( orig.fVoltageRange ),
            fFrequencyMin( orig.fFrequencyMin ),
            fFrequencyRange( orig.fFrequencyRange )
    {
        SetNumber( orig.fNumber );
    }

    M3ChannelHeader::~M3ChannelHeader()
    {
        delete [] fLabel;
    }

    void M3ChannelHeader::SetNumber( uint32_t aNumber ) const
    {
        fNumber = aNumber;

        static const size_t prefixSize = 7; // # of characters in "channel"
        delete [] fLabel;
        fLabel = new char[ prefixSize + 10 ]; // 10 = max digits in 32-bit integer
        strcpy( fLabel, "channel" );
        u32toa( aNumber, fLabel + prefixSize );

        return;
    }

    void M3ChannelHeader::WriteToHDF5( H5::CommonFG* aParent )
    {
        M3DEBUG( mlog, "Writing channel <" << fLabel << ">" );
        H5::Group tThisChannelGroup = aParent->createGroup( fLabel );

        M3Header::WriteScalarToHDF5( &tThisChannelGroup, "number", fNumber );
        M3Header::WriteScalarToHDF5( &tThisChannelGroup, "source", fSource );
        M3Header::WriteScalarToHDF5( &tThisChannelGroup, "acquisition_rate", fAcquisitionRate );
        M3Header::WriteScalarToHDF5( &tThisChannelGroup, "record_size", fRecordSize );
        M3Header::WriteScalarToHDF5( &tThisChannelGroup, "data_type_size", fDataTypeSize );
        M3Header::WriteScalarToHDF5( &tThisChannelGroup, "data_format", fDataFormat );
        M3Header::WriteScalarToHDF5( &tThisChannelGroup, "bit_depth", fBitDepth );
        M3Header::WriteScalarToHDF5( &tThisChannelGroup, "voltage_min", fVoltageMin );
        M3Header::WriteScalarToHDF5( &tThisChannelGroup, "voltage_range", fVoltageRange );
        M3Header::WriteScalarToHDF5( &tThisChannelGroup, "frequency_min", fFrequencyMin );
        M3Header::WriteScalarToHDF5( &tThisChannelGroup, "frequency_range", fFrequencyRange );

        return;
    }

    void M3ChannelHeader::ReadFromHDF5( const H5::CommonFG* aParent, const std::string& aLabel ) const
    {
        M3DEBUG( mlog, "Reading channel <" << aLabel << ">" );
        H5::Group tThisChannelGroup = aParent->openGroup( aLabel );

        SetNumber( M3Header::ReadScalarFromHDF5< uint32_t >( &tThisChannelGroup, "number" ) );
        SetSource( M3Header::ReadScalarFromHDF5< string >( &tThisChannelGroup, "source" ) );
        SetAcquisitionRate( M3Header::ReadScalarFromHDF5< uint32_t >( &tThisChannelGroup, "acquisition_rate" ) );
        SetRecordSize( M3Header::ReadScalarFromHDF5< uint32_t >( &tThisChannelGroup, "record_size" ) );
        SetDataTypeSize( M3Header::ReadScalarFromHDF5< uint32_t >( &tThisChannelGroup, "data_type_size" ) );
        SetDataFormat( M3Header::ReadScalarFromHDF5< DataFormatType >( &tThisChannelGroup, "data_format" ) );
        SetBitDepth( M3Header::ReadScalarFromHDF5< uint32_t >( &tThisChannelGroup, "bit_depth" ) );
        SetVoltageMin( M3Header::ReadScalarFromHDF5< double >( &tThisChannelGroup, "voltage_min" ) );
        SetVoltageRange( M3Header::ReadScalarFromHDF5< double >( &tThisChannelGroup, "voltage_range" ) );
        SetFrequencyMin( M3Header::ReadScalarFromHDF5< double >( &tThisChannelGroup, "frequency_min" ) );
        SetFrequencyRange( M3Header::ReadScalarFromHDF5< double >( &tThisChannelGroup, "frequency_range" ) );

        return;
    }


    //*********************
    // M3Header
    //*********************

    M3Header::M3Header() :
            fEggVersion( TOSTRING(Egg_VERSION) ),
            fFilename(),
            fNChannels( 0 ),
            fNStreams( 0 ),
            fRunDuration( 0 ),
            fTimestamp(),
            fDescription(),
            fChannelStreams(),
            fFile( NULL ),
            fStreamsGroup( NULL ),
            fChannelsGroup( NULL )
    {
    }

    M3Header::~M3Header()
    {
    }

    uint32_t M3Header::AddStream( const std::string& aSource,
                                 uint32_t anAcqRate, uint32_t aRecSize,
                                 uint32_t aDataTypeSize, DataFormatType aDataFormat,
                                 uint32_t aBitDepth )
    {
        M3DEBUG( mlog, "Adding stream " << fNStreams << " for channel " << fNChannels << " with record size " << aRecSize );
        fChannelStreams.push_back( fNStreams );
        fChannelHeaders.push_back( M3ChannelHeader( aSource, fNChannels, anAcqRate, aRecSize, aDataTypeSize, aDataFormat, aBitDepth ) );
        fStreamHeaders.push_back( M3StreamHeader( aSource, fNStreams, 1, sSeparate, anAcqRate, aRecSize, aDataTypeSize, aDataFormat, aBitDepth ) );
        ++fNChannels;
        return fNStreams++;
    }

    uint32_t M3Header::AddStream( const std::string& aSource, uint32_t aNChannels, MultiChannelFormatType aFormat,
                                 uint32_t anAcqRate, uint32_t aRecSize,
                                 uint32_t aDataTypeSize, DataFormatType aDataFormat,
                                 uint32_t aBitDepth )
    {
        M3DEBUG( mlog, "Adding stream " << fNStreams << " for multiple channels with record size " << aRecSize );
        for( uint32_t iNewChannel = 0; iNewChannel < aNChannels; ++iNewChannel )
        {
            M3DEBUG( mlog, "Adding channel " << fNChannels );
            fChannelStreams.push_back( fNStreams );
            fChannelHeaders.push_back( M3ChannelHeader( aSource, fNChannels, anAcqRate, aRecSize, aDataTypeSize, aDataFormat, aBitDepth ) );
            ++fNChannels;
        }
        fStreamHeaders.push_back( M3StreamHeader( aSource, fNStreams, aNChannels, aFormat, anAcqRate, aRecSize, aDataTypeSize, aDataFormat, aBitDepth ) );
        return fNStreams++;
    }

    void M3Header::WriteToHDF5( H5::H5File* aFile )
    {
        try
        {
            M3DEBUG( mlog, "Writing run header" );
            fFile = aFile;
            WriteScalarToHDF5( fFile, "egg_version",   GetEggVersion() );
            WriteScalarToHDF5( fFile, "filename",         GetFilename() );
            WriteScalarToHDF5( fFile, "n_channels",       GetNChannels() );
            WriteScalarToHDF5( fFile, "n_streams",        GetNStreams() );
            WriteScalarToHDF5( fFile, "run_duration",     GetRunDuration() );
            WriteScalarToHDF5( fFile, "timestamp",        GetTimestamp() );
            WriteScalarToHDF5( fFile, "description",      GetDescription() );
            Write1DToHDF5( fFile, "channel_streams",  GetChannelStreams() );

            M3DEBUG( mlog, "Writing stream headers" );
            fStreamsGroup = new H5::Group( fFile->createGroup( "streams" ) );
            for( uint32_t iStream = 0; iStream < fNStreams; ++iStream )
            {
                fStreamHeaders[ iStream ].WriteToHDF5( fStreamsGroup );
            }

            M3DEBUG( mlog, "Writing channel headers" );
            fChannelsGroup = new H5::Group( fFile->createGroup( "channels" ) );
            for( uint32_t iChan = 0; iChan < fNChannels; ++iChan )
            {
                fChannelHeaders[ iChan ].WriteToHDF5( fChannelsGroup );
            }
        }
        catch( H5::Exception& e )
        {
            throw M3Exception() << "HDF5 error while writing header: \n\t" << e.getDetailMsg();
        }
        catch( M3Exception& e )
        {
            throw( e );
        }

        return;
    }

    void M3Header::ReadFromHDF5( const H5::H5File* aFile ) const
    {
        try
        {
            M3DEBUG( mlog, "Reading run header" );
            fFile = const_cast< H5::H5File* >( aFile );
            SetEggVersion( ReadScalarFromHDF5< string >( fFile, string("egg_version") ) );
            SetFilename( ReadScalarFromHDF5< string >( fFile, "filename" ) );
            SetNChannels( ReadScalarFromHDF5< uint32_t >( fFile, "n_channels" ) );
            SetNStreams( ReadScalarFromHDF5< uint32_t >( fFile, "n_streams" ) );
            SetRunDuration( ReadScalarFromHDF5< uint32_t >( fFile, "run_duration" ) );
            SetTimestamp( ReadScalarFromHDF5< string >( fFile, "timestamp" ) );
            SetDescription( ReadScalarFromHDF5< string >( fFile, "description" ) );

            fChannelStreams.clear();
            //Read1DFromHDF5< std::vector< uint32_t > >( fFile, "channel_streams", fChannelStreams );

            M3DEBUG( mlog, "Reading stream headers" );
            fStreamHeaders.clear();
            fStreamsGroup = new H5::Group( fFile->openGroup( "streams" ) );
            hsize_t nStreams = fStreamsGroup->getNumObjs();
            if( nStreams != fNStreams )
            {
                throw M3Exception() << "Number of streams <" << fNStreams << "> disagrees with the number of objects in the stream group <" << nStreams << ">";
            }
            for( hsize_t iStream = 0; iStream < nStreams; ++iStream )
            {
                string tStreamLabel = fStreamsGroup->getObjnameByIdx( iStream );
                fStreamHeaders.push_back( M3StreamHeader() );
                fStreamHeaders.back().ReadFromHDF5( fStreamsGroup, tStreamLabel );
            }

            M3DEBUG( mlog, "Reading channel headers" );
            fChannelHeaders.clear();
            fChannelsGroup = new H5::Group( fFile->openGroup( "channels" ) );
            hsize_t nChannels = fChannelsGroup->getNumObjs();
            for( hsize_t iChan = 0; iChan < nChannels; ++iChan )
            {
                string tChannelLabel = fChannelsGroup->getObjnameByIdx( iChan );
                fChannelHeaders.push_back( M3ChannelHeader() );
                fChannelHeaders.back().ReadFromHDF5( fChannelsGroup, tChannelLabel );
            }
        }
        catch( H5::Exception& e )
        {
            M3ERROR( mlog, "Unable to open header group or find header data:\n\t" << e.getCDetailMsg() );
        }
    }

}

std::ostream& operator<<( std::ostream& out, const monarch3::M3StreamHeader& hdr )
{
    out << "Stream Header Content:\n";
    out << "\tStream Number: " << hdr.GetNumber() << '\n';
    out << "\tSource: " << hdr.GetSource() << '\n';
    out << "\tNumber of Channels: " << hdr.GetNChannels() << '\n';
    out << "\tChannel Format: " << hdr.GetChannelFormat() << '\n';
    out << "\tAcquisition Rate: " << hdr.GetAcquisitionRate() << " MHz\n";
    out << "\tRecord Size: " << hdr.GetRecordSize() << " samples\n";
    out << "\tData Type Size: " << hdr.GetDataTypeSize() << " bytes\n";
    out << "\tData Format: " << hdr.GetDataFormat() << '\n';
    out << "\tBit Depth: " << hdr.GetBitDepth() << " bits\n";
    out << "\tNumber of Acquisitions: " << hdr.GetNAcquisitions() << '\n';
    out << "\tNumber of Records: " << hdr.GetNRecords() << '\n';
    return out;
}

std::ostream& operator<<( std::ostream& out, const monarch3::M3ChannelHeader& hdr )
{
    out << "Channel Header Content:\n";
    out << "\tChannel Number: " << hdr.GetNumber() << '\n';
    out << "\tSource: " << hdr.GetSource() << '\n';
    out << "\tAcquisition Rate: " << hdr.GetAcquisitionRate() << " MHz\n";
    out << "\tRecord Size: " << hdr.GetRecordSize() << " samples\n";
    out << "\tData Type Size: " << hdr.GetDataTypeSize() << " bytes\n";
    out << "\tData Format: " << hdr.GetDataFormat() << '\n';
    out << "\tBit Depth: " << hdr.GetBitDepth() << " bits\n";
    out << "\tVoltage Min: " << hdr.GetVoltageMin() << " V\n";
    out << "\tVoltage Range: " << hdr.GetVoltageRange() << " V\n";
    out << "\tFrequency Min: " << hdr.GetFrequencyMin() << " Hz\n";
    out << "\tFrequency Range: " << hdr.GetFrequencyRange() << " Hz\n";
    return out;
}

std::ostream& operator<<( std::ostream& out, const monarch3::M3Header& hdr )
{
    out << "Monarch Header Content:\n";
    out << "\tEgg Version: " << hdr.GetEggVersion() << "\n";
    out << "\tFilename: " << hdr.GetFilename() << "\n";
    out << "\tRun Duration: " << hdr.GetRunDuration() << " ms\n";
    out << "\tTimestamp: " << hdr.GetTimestamp() << "\n";
    out << "\tDescription: " << hdr.GetDescription() << "\n";
    out << "\tNumber of Channels: " << hdr.GetNChannels() << "\n";
    out << "\tNumber of Streams: " << hdr.GetNStreams() << "\n";
    out << "\tChannel-to-stream mapping:\n";
    for( uint32_t iChan = 0; iChan < hdr.GetChannelStreams().size(); ++iChan )
    {
        out << "\t\tChannel " << iChan << " --> Stream " << hdr.GetChannelStreams()[ iChan ] << "\n";
    }
    out << "\tStream headers:\n";
    for( uint32_t iStream = 0; iStream < hdr.GetStreamHeaders().size(); ++iStream )
    {
        out << hdr.GetStreamHeaders()[ iStream ];
    }
    out << "\tChannel headers:\n";
    for( uint32_t iChan = 0; iChan < hdr.GetChannelHeaders().size(); ++iChan )
    {
        out << hdr.GetChannelHeaders()[ iChan ];
    }
    return out;
}
