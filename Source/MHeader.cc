/*
 * MHeader.cc
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 */

#include "MHeader.hh"

#include "MLogger.hh"

#include <cstdlib> // for atol in parsing timestamp

#include <map>
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
            fRunDuration( 0 ),
            fTimestamp(),
            fDescription(),
            fStreamCount( 0 ),
            fChannelStreams()
    {
        SetNChannels( 0 );
    }

    MHeader::~MHeader()
    {
    }

    void MHeader::SetNChannels( unsigned aNChannels )
    {
        fNChannels = aNChannels;
        if( aNChannels == 0 )
        {
            fChannelStreams.clear();
            return;
        }

        fChannelStreams.resize( aNChannels );
        for( unsigned iChannel = 0; iChannel < fNChannels; ++iChannel )
        {
            fChannelStreams[ iChannel ] = iChannel;
        }

        fStreamCount = aNChannels;
        return;
    }

    void MHeader::AssignToStream( unsigned* aChannelArray, unsigned aNChannels )
    {
        // Assign the channels specified in aChannelArray to stream number fStreamCount+1
        for( unsigned iChanIndex = 0; iChanIndex < aNChannels; ++iChanIndex )
        {
            if( aChannelArray[iChanIndex] > fNChannels )
            {
                throw MException() << "Channel specified for stream (" << aChannelArray[iChanIndex] <<
                        " ) is greater than current number of channels";
            }
            fChannelStreams[ aChannelArray[iChanIndex] ] = fStreamCount + 1;
        }

        // Determine the number of streams currently in use, and renumber streams accordingly
        std::map< unsigned, unsigned > tStreamNoConv;
        for( unsigned iChannel = 0; iChannel < fNChannels; ++iChannel )
        {
            tStreamNoConv.insert( std::pair< unsigned, unsigned >( fChannelStreams[ iChannel ], 0 ) );
        }

        unsigned tRenumberedStreamNo = 0;
        for( std::map< unsigned, unsigned >::iterator tStreamNoIt = tStreamNoConv.begin(); tStreamNoIt != tStreamNoConv.end(); ++tStreamNoIt )
        {
            tStreamNoIt->second = tRenumberedStreamNo++;
        }

        for( unsigned iChannel = 0; iChannel < fNChannels; ++iChannel )
        {
            MDEBUG( mlog, "Channel " << iChannel << "; before - " << fChannelStreams[ iChannel ] << "; after - " << tStreamNoConv[ fChannelStreams[ iChannel ] ] );
            fChannelStreams[ iChannel ] = tStreamNoConv[ fChannelStreams[ iChannel ] ];
        }

        fStreamCount = tStreamNoConv.size();
        MDEBUG( mlog, "Stream count is now " << fStreamCount );

        return;
    }


}

std::ostream& operator<<( std::ostream& out, const monarch::MHeader& hdr )
{
    out << "Monarch Header Content: " << "\n";
    out << "\tSchema Version: " << hdr.GetSchemaVersion() << "\n";
    out << "\tFilename: " << hdr.GetFilename() << "\n";
    out << "\tNumber of Channels: " << hdr.GetNChannels() << "\n";
    out << "\tRun Duration: " << hdr.GetRunDuration() << " ms\n";
    out << "\tTimestamp: " << hdr.GetTimestamp() << "\n";
    out << "\tDescription: " << hdr.GetDescription() << "\n";
    return out;
}
