/*
 * M3Monarch.hh
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 */

#ifndef M3MONARCH_HH_
#define M3MONARCH_HH_

#include "M3Exception.hh"
#include "M3Header.hh"
#include "M3Logger.hh"
#include "M3Stream.hh"

#include "H5Cpp.h"

#include <string>

namespace monarch3
{
    M3LOGGER( mlog_mmonarch, "M3Monarch3.h" );

    /*!
     @class Monarch3
     @author N. S. Oblath (v3), D. Furse (original)

     @brief Egg file read/write access

     @details
     Monarch3 objects are created with one of the two static Open functions:
      - OpenForReading() will create a read-only Monarch3 object;
      - OpenForWriting() will create a writeable Monarch3 object.

     The data structure and general header information is setup/read via the header (accessed with GetHeader()).

     Stream data is written or read via the stream objects (accessed with GetStream( unsigned )).
    */
    class M3_API Monarch3
    {
            //***********************
            // constructors and state
            //***********************

        private:
            //private to force use of static constructor methods
            Monarch3();

            //current state of monarch
            typedef enum
            {
                eOpenToRead, // state when monarch has a file open but hasn't read the header
                eOpenToWrite, // state when monarch has a file open but hasn't written the header
                eReadyToRead, // state when monarch has dealt with the header and is reading records
                eReadyToWrite, // state when monarch has dealt with the header and is writing records
                eClosed // state when monarch has no file
            } State;
            mutable State fState;

        public:
            ~Monarch3();

            //********************************
            // methods for reading (all const)
            //********************************

        public:

            /// This static method opens the file for reading.
            /// If the file exists and can be read, this returns a prepared monarch pointer, and memory is allocated for the header.
            /// Upon successful return monarch is in the eOpen state.
            static const Monarch3* OpenForReading( const std::string& filename );

            /// This method extracts the header information from the file.
            /// If the header read correctly, this returns and the header may be examined, and memory is allocated for the record.
            /// Upon successful return monarch is in the eReady state.
            /// An exception is thrown if the header is not read.
            void ReadHeader() const;

            /// Get the pointer to the header.
            const M3Header* GetHeader() const;

            /// Get the pointer to a particular stream
            const M3Stream* GetStream( unsigned stream ) const;

            /// Close the file.
            void FinishReading() const;

            //*********************************
            // methods for writing (none const)
            //*********************************

        public:

            /// This static method opens the file for writing.
            /// If the file exists and can be written, this returns a prepared monarch pointer, and memory is allocated for the header.
            /// Upon successful return monarch is in the eOpen state.
            static Monarch3* OpenForWriting( const std::string& filename );

            /// This method marshals the current header to the file.
            /// If the header marshalled correctly, this returns true, memory is allocated for the record(s).
            /// Upon successful return monarch is in the eReady state.
            void WriteHeader();

            /// Get the pointer to the header.
            M3Header* GetHeader();

            /// Get the pointer to a particular stream
            M3Stream* GetStream( unsigned stream );

            /// Close the file.
            void FinishWriting();

        private:
            // the HDF5 file
            mutable H5::H5File* fFile;

            // the header
            mutable M3Header* fHeader;

            // the streams
            mutable std::vector< M3Stream* > fStreams;

    };

    inline const M3Header* Monarch3::GetHeader() const
    {
        return fHeader;
    }
    inline M3Header* Monarch3::GetHeader()
    {
        return fHeader;
    }

    inline const M3Stream* Monarch3::GetStream( unsigned iStream ) const
    {
        return fStreams.at( iStream );
    }
    inline M3Stream* Monarch3::GetStream( unsigned iStream )
    {
        return fStreams.at( iStream );
    }

}

#endif /* M3MONARCH_HH_ */
