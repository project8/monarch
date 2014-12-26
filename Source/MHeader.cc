/*
 * MHeader.cc
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 */

#include "MHeader.hh"

#include "MIToA.hh"
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
            fLabel( NULL ),
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

    MStreamHeader::MStreamHeader( const std::string& aSource, unsigned aNumber, unsigned aNChannels,
                    unsigned anAcqRate, unsigned aRecSize,
                    unsigned aDataTypeSize, DataFormatType aDataFormat,
                    unsigned aBitDepth ) :
            fLabel( NULL ),
            fNumber( 0 ),
            fSource( aSource ),
            fNChannels( aNChannels ),
            fAcquisitionRate( anAcqRate ),
            fRecordSize( aRecSize ),
            fDataTypeSize( aDataTypeSize ),
            fDataFormat( aDataFormat ),
            fBitDepth( aBitDepth )
    {
        SetNumber( aNumber );
    }

    MStreamHeader::MStreamHeader( const MStreamHeader& orig ) :
            fLabel( NULL ),
            fNumber( 0 ),
            fSource( orig.fSource ),
            fNChannels( orig.fNChannels ),
            fAcquisitionRate( orig.fAcquisitionRate ),
            fRecordSize( orig.fRecordSize ),
            fDataTypeSize( orig.fDataTypeSize ),
            fDataFormat( orig.fDataFormat ),
            fBitDepth( orig.fBitDepth )
    {
        SetNumber( orig.fNumber );
    }

    MStreamHeader::~MStreamHeader()
    {
        delete [] fLabel;
    }

    void MStreamHeader::SetNumber( unsigned aNumber )
    {
        fNumber = aNumber;

        static const size_t prefixSize = 6; // # of characters in "stream"
        delete [] fLabel;
        fLabel = new char[ prefixSize + 10 ]; // 10 = max digits in 32-bit integer
        strcpy( fLabel, "stream" );
        u32toa( aNumber, fLabel + prefixSize );

        return;
    }

    void MStreamHeader::WriteToHDF5( H5::CommonFG* aParent ) const
    {
        MDEBUG( mlog, "Writing stream <" << fLabel << ">" );
        H5::Group tThisStreamGroup = aParent->createGroup( fLabel );

        MHeader::WriteScalarToHDF5( &tThisStreamGroup, "number", fNumber );
        MHeader::WriteScalarToHDF5( &tThisStreamGroup, "source", fSource );
        MHeader::WriteScalarToHDF5( &tThisStreamGroup, "n_channels", fNChannels );
        MHeader::WriteScalarToHDF5( &tThisStreamGroup, "acquisition_rate", fAcquisitionRate );
        MHeader::WriteScalarToHDF5( &tThisStreamGroup, "record_size", fRecordSize );
        MHeader::WriteScalarToHDF5( &tThisStreamGroup, "data_type_size", fDataTypeSize );
        MHeader::WriteScalarToHDF5( &tThisStreamGroup, "data_format", fDataFormat );
        MHeader::WriteScalarToHDF5( &tThisStreamGroup, "bit_depth", fBitDepth );

        return;
    }

    void MStreamHeader::ReadFromHDF5( const H5::CommonFG* aParent, const std::string& aLabel )
    {
        MDEBUG( mlog, "Reading stream <" << aLabel << ">" );
        H5::Group tThisStreamGroup = aParent->openGroup( aLabel );

        SetNumber( MHeader::ReadScalarFromHDF5< unsigned >( &tThisStreamGroup, "number" ) );
        SetSource( MHeader::ReadScalarFromHDF5< string >( &tThisStreamGroup, "source" ) );
        SetAcquisitionRate( MHeader::ReadScalarFromHDF5< unsigned >( &tThisStreamGroup, "acquisition_rate" ) );
        SetRecordSize( MHeader::ReadScalarFromHDF5< unsigned >( &tThisStreamGroup, "record_size" ) );
        SetDataTypeSize( MHeader::ReadScalarFromHDF5< unsigned >( &tThisStreamGroup, "data_type_size" ) );
        SetDataFormat( MHeader::ReadScalarFromHDF5< DataFormatType >( &tThisStreamGroup, "data_format" ) );
        SetBitDepth( MHeader::ReadScalarFromHDF5< unsigned >( &tThisStreamGroup, "bit_depth" ) );

        return;
    }

    //*********************
    // MChannelHeader
    //*********************

    MChannelHeader::MChannelHeader() :
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

    MChannelHeader::MChannelHeader( const std::string& aSource, unsigned aNumber,
                    unsigned anAcqRate, unsigned aRecSize,
                    unsigned aDataTypeSize, DataFormatType aDataFormat,
                    unsigned aBitDepth ) :
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

    MChannelHeader::MChannelHeader( const MChannelHeader& orig ) :
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

    MChannelHeader::~MChannelHeader()
    {
        delete [] fLabel;
    }

    void MChannelHeader::SetNumber( unsigned aNumber )
    {
        fNumber = aNumber;

        static const size_t prefixSize = 7; // # of characters in "channel"
        delete [] fLabel;
        fLabel = new char[ prefixSize + 10 ]; // 10 = max digits in 32-bit integer
        strcpy( fLabel, "channel" );
        u32toa( aNumber, fLabel + prefixSize );

        return;
    }

    void MChannelHeader::WriteToHDF5( H5::CommonFG* aParent ) const
    {
        MDEBUG( mlog, "Writing channel <" << fLabel << ">" );
        H5::Group tThisChannelGroup = aParent->createGroup( fLabel );

        MHeader::WriteScalarToHDF5( &tThisChannelGroup, "number", fNumber );
        MHeader::WriteScalarToHDF5( &tThisChannelGroup, "source", fSource );
        MHeader::WriteScalarToHDF5( &tThisChannelGroup, "acquisition_rate", fAcquisitionRate );
        MHeader::WriteScalarToHDF5( &tThisChannelGroup, "record_size", fRecordSize );
        MHeader::WriteScalarToHDF5( &tThisChannelGroup, "data_type_size", fDataTypeSize );
        MHeader::WriteScalarToHDF5( &tThisChannelGroup, "data_format", fDataFormat );
        MHeader::WriteScalarToHDF5( &tThisChannelGroup, "bit_depth", fBitDepth );
        MHeader::WriteScalarToHDF5( &tThisChannelGroup, "voltage_min", fVoltageMin );
        MHeader::WriteScalarToHDF5( &tThisChannelGroup, "voltage_range", fVoltageRange );
        MHeader::WriteScalarToHDF5( &tThisChannelGroup, "frequency_min", fFrequencyMin );
        MHeader::WriteScalarToHDF5( &tThisChannelGroup, "frequency_range", fFrequencyRange );

        return;
    }

    void MChannelHeader::ReadFromHDF5( const H5::CommonFG* aParent, const std::string& aLabel )
    {
        MDEBUG( mlog, "Reading channel <" << aLabel << ">" );
        H5::Group tThisChannelGroup = aParent->openGroup( aLabel );

        SetNumber( MHeader::ReadScalarFromHDF5< unsigned >( &tThisChannelGroup, "number" ) );
        SetSource( MHeader::ReadScalarFromHDF5< string >( &tThisChannelGroup, "source" ) );
        SetAcquisitionRate( MHeader::ReadScalarFromHDF5< unsigned >( &tThisChannelGroup, "acquisition_rate" ) );
        SetRecordSize( MHeader::ReadScalarFromHDF5< unsigned >( &tThisChannelGroup, "record_size" ) );
        SetDataTypeSize( MHeader::ReadScalarFromHDF5< unsigned >( &tThisChannelGroup, "data_type_size" ) );
        SetDataFormat( MHeader::ReadScalarFromHDF5< DataFormatType >( &tThisChannelGroup, "data_format" ) );
        SetBitDepth( MHeader::ReadScalarFromHDF5< unsigned >( &tThisChannelGroup, "bit_depth" ) );
        SetVoltageMin( MHeader::ReadScalarFromHDF5< double >( &tThisChannelGroup, "voltage_min" ) );
        SetVoltageRange( MHeader::ReadScalarFromHDF5< double >( &tThisChannelGroup, "voltage_range" ) );
        SetFrequencyMin( MHeader::ReadScalarFromHDF5< double >( &tThisChannelGroup, "frequency_min" ) );
        SetFrequencyRange( MHeader::ReadScalarFromHDF5< double >( &tThisChannelGroup, "frequency_range" ) );

        return;
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
        fChannelHeaders.push_back( MChannelHeader( aSource, fNChannels, anAcqRate, aRecSize, aDataTypeSize, aDataFormat, aBitDepth ) );
        fStreamHeaders.push_back( MStreamHeader( aSource, fNStreams, 1, anAcqRate, aRecSize, aDataTypeSize, aDataFormat, aBitDepth ) );
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
            fChannelHeaders.push_back( MChannelHeader( aSource, fNChannels, anAcqRate, aRecSize, aDataTypeSize, aDataFormat, aBitDepth ) );
            ++fNChannels;
        }
        fStreamHeaders.push_back( MStreamHeader( aSource, fNStreams, aNChannels, anAcqRate, aRecSize, aDataTypeSize, aDataFormat, aBitDepth ) );
        return ++fNStreams;
    }

    void MHeader::WriteToHDF5( H5::CommonFG* aParent ) const
    {
        try
        {
            MDEBUG( mlog, "Writing run header" );
            H5::Group* runGroup = new H5::Group( aParent->createGroup( "/run" ) );
            WriteScalarToHDF5( runGroup, "schema_version",   GetSchemaVersion() );
            WriteScalarToHDF5( runGroup, "filename",         GetFilename() );
            WriteScalarToHDF5( runGroup, "n_channels",       GetNChannels() );
            WriteScalarToHDF5( runGroup, "n_streams",        GetNStreams() );
            WriteScalarToHDF5( runGroup, "run_duration",     GetRunDuration() );
            WriteScalarToHDF5( runGroup, "timestamp",        GetTimestamp() );
            WriteScalarToHDF5( runGroup, "description",      GetDescription() );
            Write1DToHDF5( runGroup, "channel_streams",  GetChannelStreams() );

            MDEBUG( mlog, "Writing stream headers" );
            H5::Group tStreamGroup = aParent->createGroup( "streams" );
            for( unsigned iStream = 0; iStream < fNStreams; ++iStream )
            {
                fStreamHeaders[ iStream ].WriteToHDF5( &tStreamGroup );
            }

            MDEBUG( mlog, "Writing channel headers" );
            H5::Group tChannelGroup = aParent->createGroup( "channels" );
            for( unsigned iChan = 0; iChan < fNChannels; ++iChan )
            {
                fChannelHeaders[ iChan ].WriteToHDF5( &tChannelGroup );
            }
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

    void MHeader::ReadFromHDF5( const H5::CommonFG* aParent )
    {
        try
        {
            MDEBUG( mlog, "Reading run header" );
            H5::Group tRunGroup = aParent->openGroup( "run" );
            SetSchemaVersion( ReadScalarFromHDF5< string >( &tRunGroup, string("schema_version") ) );
            SetFilename( ReadScalarFromHDF5< string >( &tRunGroup, "filename" ) );
            SetNChannels( ReadScalarFromHDF5< unsigned >( &tRunGroup, "n_channels" ) );
            SetNStreams( ReadScalarFromHDF5< unsigned >( &tRunGroup, "n_streams" ) );
            SetRunDuration( ReadScalarFromHDF5< unsigned >( &tRunGroup, "run_duration" ) );
            SetTimestamp( ReadScalarFromHDF5< string >( &tRunGroup, "timestamp" ) );
            SetDescription( ReadScalarFromHDF5< string >( &tRunGroup, "description" ) );

            fChannelStreams.clear();
            Read1DFromHDF5< unsigned >( &tRunGroup, "channel_streams", fChannelStreams );

            MDEBUG( mlog, "Reading stream headers" );
            fStreamHeaders.clear();
            H5::Group tStreamGroup = aParent->openGroup( "streams" );
            hsize_t nStreams = tStreamGroup.getNumObjs();
            for( hsize_t iStream = 0; iStream < nStreams; ++iStream )
            {
                string tStreamLabel = tStreamGroup.getObjnameByIdx( iStream );
                fStreamHeaders.push_back( MStreamHeader() );
                fStreamHeaders.back().ReadFromHDF5( &tStreamGroup, tStreamLabel );
            }

            MDEBUG( mlog, "Reading channel headers" );
            fChannelHeaders.clear();
            H5::Group tChannelGroup = aParent->openGroup( "channels" );
            hsize_t nChannels = tChannelGroup.getNumObjs();
            for( hsize_t iChan = 0; iChan < nChannels; ++iChan )
            {
                string tChannelLabel = tChannelGroup.getObjnameByIdx( iChan );
                fChannelHeaders.push_back( MChannelHeader() );
                fChannelHeaders.back().ReadFromHDF5( &tChannelGroup, tChannelLabel );
            }
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
