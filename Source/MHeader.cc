/*
 * MHeader.cc
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 */

#include "MHeader.hh"

#include <cstdlib> // for atol in parsing timestamp

// for parsing timestamp
#include <sstream>
using std::stringstream;
using std::string;

namespace monarch
{
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
            fDescription()
    {
    }

    MHeader::~MHeader()
    {
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
