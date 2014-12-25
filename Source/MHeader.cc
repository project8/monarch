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

    MChannelHeader::MChannelHeader() :
            fNumber( 0 ),
            fSource(),
            fAcquisitionRate( 0 ),
            fDataTypeSize( 0 ),
            fDataFormat( sDigitized ),
            fBitDepth( 0 ),
            fVoltageMin( 0. ),
            fVoltageRange( 0. ),
            fFrequencyMin( 0. ),
            fFrequencyRange( 0. )
    {
    }

    MChannelHeader::~MChannelHeader()
    {
    }

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

    unsigned MHeader::AddStream( unsigned aRecSize )
    {
        MDEBUG( mlog, "Adding stream " << fNStreams << " for channel " << fNChannels << " with record size " << aRecSize );
        fChannelStreams.push_back( fNStreams );
        ++fNChannels;
        return ++fNStreams;
    }

    unsigned MHeader::AddStream( unsigned aNChannels, unsigned aRecSize )
    {
        MDEBUG( mlog, "Adding stream " << fNStreams << " for multiple channels with record size " << aRecSize );
        for( unsigned iNewChannel = 0; iNewChannel < aNChannels; ++iNewChannel )
        {
            MDEBUG( mlog, "Adding channel " << fNChannels );
            fChannelStreams.push_back( fNStreams );
            ++fNChannels;
        }
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

std::ostream& operator<<( std::ostream& out, const monarch::MHeader& hdr )
{
    out << "Monarch Header Content: " << "\n";
    out << "\tSchema Version: " << hdr.GetSchemaVersion() << "\n";
    out << "\tFilename: " << hdr.GetFilename() << "\n";
    out << "\tNumber of Channels: " << hdr.GetNChannels() << "\n";
    out << "\tNumber of Streams: " << hdr.GetNStreams() << "\n";
    for( unsigned iChan = 0; iChan < hdr.GetNChannels(); ++iChan )
    {
        out << "\t\tChannel " << iChan << " --> Stream " << hdr.GetChannelStreams()[ iChan ] << "\n";
    }
    out << "\tRun Duration: " << hdr.GetRunDuration() << " ms\n";
    out << "\tTimestamp: " << hdr.GetTimestamp() << "\n";
    out << "\tDescription: " << hdr.GetDescription() << "\n";
    return out;
}
