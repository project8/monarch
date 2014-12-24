/*
 * MHeader.hh
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 */

#ifndef MHEADER_HH_
#define MHEADER_HH_

#include "MMemberVariable.hh"
#include "MTypes.hh"

#include <string>
#include <vector>

namespace monarch
{
    /*!
     @class MChannelHeader
     @author N. S. Oblath

     @brief Single-channel header information

     @details
    */
    class MChannelHeader
    {
        public:
            MChannelHeader();
            ~MChannelHeader();

            MEMBERVARIABLE( unsigned, Number );

            MEMBERVARIABLE( std::string, Source );

            MEMBERVARIABLE( unsigned, AcquisitionRate );

            MEMBERVARIABLE( unsigned, DataTypeSize );

            MEMBERVARIABLE( DataFormatType, DataFormat );

            MEMBERVARIABLE( unsigned, BitDepth );

            MEMBERVARIABLE( double, VoltageMin );

            MEMBERVARIABLE( double, VoltageRange );

            MEMBERVARIABLE( double, FrequencyMin );

            MEMBERVARIABLE( double, FrequencyRange );

    };


    /*!
     @class MHeader
     @author N. S. Oblath

     @brief Egg file header information

     @details
    */
    class MHeader
    {
            friend class Monarch;

        public:
            MHeader();
            ~MHeader();

            MEMBERVARIABLEREF( std::string, SchemaVersion );

            MEMBERVARIABLEREF( std::string, Filename );

            MEMBERVARIABLE( unsigned, NChannels );

            MEMBERVARIABLE( unsigned, NStreams );

            // channel configuration and coherence

            MEMBERVARIABLE( unsigned, RunDuration );

            MEMBERVARIABLEREF( std::string, Timestamp );

            MEMBERVARIABLEREF( std::string, Description );

        public:
            /// Add a stream with one channel with aRecSize samples per record
            /// Returns the stream number (used to address the stream later)
            unsigned AddStream( unsigned aRecSize );
            /// Add a stream with multiple channels with aRecSize samples per record
            /// Returns the stream number (used to address the stream later)
            unsigned AddStream( unsigned aNChannels, unsigned aRecSize );

            // channel headers

            MEMBERVARIABLEREF_NOSET( std::vector< unsigned >, ChannelStreams );

    };

}

// Pretty printing method
std::ostream& operator<<( std::ostream& out, const monarch::MHeader& hdr );

#endif
