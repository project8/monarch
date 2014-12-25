/*
 * MHeader.cc
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 */

#include "MHeader.hh"

#include "MLogger.hh"

#include <cstdlib> // for atol in parsing timestamp

// for parsing timestamp
#include <sstream>
using std::stringstream;
using std::string;

namespace monarch
{
    MLOGGER( mlog, "MHeader" );

    //*********************
    // MStreamHeader
    //*********************

    MStreamHeader::MStreamHeader() :
            fNumber( 0 ),
            fSource(),
            fNChannels( 0 ),
            fAcquisitionRate( 0 ),
            fRecordSize( 0 ),
            fDataTypeSize( 0 ),
            fDataFormat( sDigitized ),
            fBitDepth( 0 )
    {
    }

    MStreamHeader::MStreamHeader( const std::string& aSource, unsigned aNChannels,
                    unsigned anAcqRate, unsigned aRecSize,
                    unsigned aDataTypeSize, DataFormatType aDataFormat,
                    unsigned aBitDepth ) :
            fNumber( 0 ),
            fSource( aSource ),
            fNChannels( aNChannels ),
            fAcquisitionRate( anAcqRate ),
            fRecordSize( aRecSize ),
            fDataTypeSize( aDataTypeSize ),
            fDataFormat( aDataFormat ),
            fBitDepth( aBitDepth )
    {
    }

    MStreamHeader::MStreamHeader( const MStreamHeader& orig ) :
            fNumber( orig.fNumber ),
            fSource( orig.fSource ),
            fNChannels( orig.fNChannels ),
            fAcquisitionRate( orig.fAcquisitionRate ),
            fRecordSize( orig.fRecordSize ),
            fDataTypeSize( orig.fDataTypeSize ),
            fDataFormat( orig.fDataFormat ),
            fBitDepth( orig.fBitDepth )
    {
    }

    MStreamHeader::~MStreamHeader()
    {
    }


    //*********************
    // MChannelHeader
    //*********************

    MChannelHeader::MChannelHeader() :
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

    MChannelHeader::MChannelHeader( const std::string& aSource, unsigned anAcqRate, unsigned aRecSize,
                    unsigned aDataTypeSize, DataFormatType aDataFormat,
                    unsigned aBitDepth ) :
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
    }

    MChannelHeader::MChannelHeader( const MChannelHeader& orig ) :
            fNumber( orig.fNumber ),
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
    }

    MChannelHeader::~MChannelHeader()
    {
    }


    //*********************
    // MHeader
    //*********************

    MHeader::MHeader() :
            fSchemaVersion(),
            fFilename(),
            fNChannels( 0 ),
            fNStreams( 0 ),
            fRunDuration( 0 ),
            fTimestamp(),
            fDescription(),
            fChannelStreams()
    {
    }

    MHeader::~MHeader()
    {
    }

    unsigned MHeader::AddStream( const std::string& aSource,
                                 unsigned anAcqRate, unsigned aRecSize,
                                 unsigned aDataTypeSize, DataFormatType aDataFormat,
                                 unsigned aBitDepth )
    {
        MDEBUG( mlog, "Adding stream " << fNStreams << " for channel " << fNChannels << " with record size " << aRecSize );
        fChannelStreams.push_back( fNStreams );
        fChannelHeaders.push_back( MChannelHeader( aSource, anAcqRate, aRecSize, aDataTypeSize, aDataFormat, aBitDepth ) );
        fStreamHeaders.push_back( MStreamHeader( aSource, 1, anAcqRate, aRecSize, aDataTypeSize, aDataFormat, aBitDepth ) );
        ++fNChannels;
        return ++fNStreams;
    }

    unsigned MHeader::AddStream( const std::string& aSource, unsigned aNChannels,
                                 unsigned anAcqRate, unsigned aRecSize,
                                 unsigned aDataTypeSize, DataFormatType aDataFormat,
                                 unsigned aBitDepth )
    {
        MDEBUG( mlog, "Adding stream " << fNStreams << " for multiple channels with record size " << aRecSize );
        for( unsigned iNewChannel = 0; iNewChannel < aNChannels; ++iNewChannel )
        {
            MDEBUG( mlog, "Adding channel " << fNChannels );
            fChannelStreams.push_back( fNStreams );
            fChannelHeaders.push_back( MChannelHeader( aSource, anAcqRate, aRecSize, aDataTypeSize, aDataFormat, aBitDepth ) );
            ++fNChannels;
        }
        fStreamHeaders.push_back( MStreamHeader( aSource, aNChannels, anAcqRate, aRecSize, aDataTypeSize, aDataFormat, aBitDepth ) );
        return ++fNStreams;
    }

    void MHeader::WriteToHDF5( H5::Group* aGroup ) const
    {
        try
        {
            // now populate the group
            WriteScalarToHDF5( aGroup, "schema_version",   GetSchemaVersion() );
            WriteScalarToHDF5( aGroup, "filename",         GetFilename() );
            WriteScalarToHDF5( aGroup, "n_channels",       GetNChannels() );
            WriteScalarToHDF5( aGroup, "n_streams",        GetNStreams() );
            WriteScalarToHDF5( aGroup, "run_duration",     GetRunDuration() );
            WriteScalarToHDF5( aGroup, "timestamp",        GetTimestamp() );
            WriteScalarToHDF5( aGroup, "description",      GetDescription() );

            Write1DToHDF5( aGroup, "channel_streams",  GetChannelStreams() );
        }
        catch( H5::Exception& e )
        {
            throw MException() << "HDF5 error while writing header: \n\t" << e.getDetailMsg();
        }
        catch( MException& e )
        {
            throw( e );
        }

        return;
    }

    void MHeader::ReadFromHDF5( const H5::Group* aGroup )
    {
        try
        {
            SetSchemaVersion( ReadScalarFromHDF5< string >( aGroup, string("schema_version") ) );
            SetFilename( ReadScalarFromHDF5< string >( aGroup, "filename" ) );
            SetNChannels( ReadScalarFromHDF5< unsigned >( aGroup, "n_channels" ) );
            SetNStreams( ReadScalarFromHDF5< unsigned >( aGroup, "n_streams" ) );
            SetRunDuration( ReadScalarFromHDF5< unsigned >( aGroup, "run_duration" ) );
            SetTimestamp( ReadScalarFromHDF5< string >( aGroup, "timestamp" ) );
            SetDescription( ReadScalarFromHDF5< string >( aGroup, "description" ) );

            fChannelStreams.clear();
            Read1DFromHDF5< unsigned >( aGroup, "channel_streams", fChannelStreams );
        }
        catch( H5::Exception& e )
        {
            MERROR( mlog, "Unable to open header group or find header data:\n\t" << e.getCDetailMsg() );
        }
    }

}

std::ostream& operator<<( std::ostream& out, const monarch::MStreamHeader& hdr )
{
    out << "Stream Header Content:\n";
    out << "\tStream Number: " << hdr.GetNumber() << '\n';
    out << "\tSource: " << hdr.GetSource() << '\n';
    out << "\tNumber of Channels: " << hdr.GetNChannels() << '\n';
    out << "\tAcquisition Rate: " << hdr.GetAcquisitionRate() << " MHz\n";
    out << "\tRecord Size: " << hdr.GetRecordSize() << " samples\n";
    out << "\tData Type Size: " << hdr.GetDataTypeSize() << " bytes\n";
    out << "\tData Format: " << hdr.GetDataFormat() << '\n';
    out << "\tBit Depth: " << hdr.GetBitDepth() << " bits\n";
    return out;
}

std::ostream& operator<<( std::ostream& out, const monarch::MChannelHeader& hdr )
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

std::ostream& operator<<( std::ostream& out, const monarch::MHeader& hdr )
{
    out << "Monarch Header Content:\n";
    out << "\tSchema Version: " << hdr.GetSchemaVersion() << "\n";
    out << "\tFilename: " << hdr.GetFilename() << "\n";
    out << "\tRun Duration: " << hdr.GetRunDuration() << " ms\n";
    out << "\tTimestamp: " << hdr.GetTimestamp() << "\n";
    out << "\tDescription: " << hdr.GetDescription() << "\n";
    out << "\tNumber of Channels: " << hdr.GetNChannels() << "\n";
    out << "\tNumber of Streams: " << hdr.GetNStreams() << "\n";
    out << "\tChannel-to-stream mapping:\n";
    for( unsigned iChan = 0; iChan < hdr.GetNChannels(); ++iChan )
    {
        out << "\t\tChannel " << iChan << " --> Stream " << hdr.GetChannelStreams()[ iChan ] << "\n";
    }
    out << "\tStream headers:\n";
    for( unsigned iStream = 0; iStream < hdr.GetNStreams(); ++iStream )
    {
        out << hdr.GetStreamHeaders()[ iStream ];
    }
    out << "\tChannel headers:\n";
    for( unsigned iChan = 0; iChan < hdr.GetNChannels(); ++iChan )
    {
        out << hdr.GetChannelHeaders()[ iChan ];
    }
    return out;
}
