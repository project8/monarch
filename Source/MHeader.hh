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
        public:
            MHeader();
            ~MHeader();

            MEMBERVARIABLEREF( std::string, SchemaVersion );

            MEMBERVARIABLEREF( std::string, Filename );

            MEMBERVARIABLE_NOSET( unsigned, NChannels );

            // channel configuration and coherence

            MEMBERVARIABLE( unsigned, RunDuration );

            MEMBERVARIABLEREF( std::string, Timestamp );

            MEMBERVARIABLEREF( std::string, Description );

        public:
            /// Sets the number of channels and resets the stream assignments
            void SetNChannels( unsigned aNChannels );
            void AssignToStream( unsigned* aChannelArray, unsigned aNChannels );

            // channel headers

        private:
            unsigned fStreamCount;
            std::vector< unsigned > fChannelStreams;

    };

}

// Pretty printing method
std::ostream& operator<<( std::ostream& out, const monarch::MHeader& hdr );

#endif
