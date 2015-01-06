/*
 * MMonarch.hh
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 */

#ifndef MMONARCH_HH_
#define MMONARCH_HH_

#include "MException.hh"
#include "MHeader.hh"
#include "MLogger.hh"
#include "MStream.hh"

#include "H5Cpp.h"

#include <string>

namespace monarch
{
    MLOGGER( mlog_mmonarch, "MMonarch.h" );

    /*!
     @class Monarch
     @author N. S. Oblath (v3), D. Furse (original)

     @brief Egg file read/write access

     @details
    */
    class Monarch
    {
            //***********************
            // constructors and state
            //***********************

        private:
            //private to force use of static constructor methods
            Monarch();

            //current state of monarch
            typedef enum
            {
                eOpen, // state when monarch has a file open but hasn't written/read the header
                eReady, // state when monarch has dealt with the header and is writing/reading records
                eClosed // state when monarch has no file
            } State;
            mutable State fState;

        public:
            ~Monarch();

            //********************************
            // methods for reading (all const)
            //********************************

        public:

            /// This static method opens the file for reading.
            /// If the file exists and can be read, this returns a prepared monarch pointer, and memory is allocated for the header.
            /// Upon successful return monarch is in the eOpen state.
            static const Monarch* OpenForReading( const std::string& filename );

            /// This method extracts the header information from the file.
            /// If the header read correctly, this returns and the header may be examined, and memory is allocated for the record.
            /// Upon successful return monarch is in the eReady state.
            /// An exception is thrown if the header is not read.
            void ReadHeader() const;

            /// Get the pointer to the header.
            const MHeader* GetHeader() const;

            /// Get the pointer to a particular stream
            const MStream* GetStream( unsigned stream ) const;

            /// Close the file pointer.
            void Close() const;

            //*********************************
            // methods for writing (none const)
            //*********************************

        public:

            /// This static method opens the file for writing.
            /// If the file exists and can be written, this returns a prepared monarch pointer, and memory is allocated for the header.
            /// Upon successful return monarch is in the eOpen state.
            static Monarch* OpenForWriting( const std::string& filename );

            /// This method marshals the current header to the file.
            /// If the header marshalled correctly, this returns true, memory is allocated for the record(s).
            /// Upon successful return monarch is in the eReady state.
            void WriteHeader();

            /// Get the pointer to the header.
            MHeader* GetHeader();

            /// Get the pointer to a particular stream
            MStream* GetStream( unsigned stream );

            /// Close the file pointer
            void Close();

        private:
            // the HDF5 file
            mutable H5::H5File* fFile;

            // the header
            mutable MHeader* fHeader;

            // the streams
            mutable std::vector< MStream* > fStreams;

    };

    inline const MHeader* Monarch::GetHeader() const
    {
        return fHeader;
    }
    inline MHeader* Monarch::GetHeader()
    {
        return fHeader;
    }

    inline const MStream* Monarch::GetStream( unsigned iStream ) const
    {
        return fStreams.at( iStream );
    }
    inline MStream* Monarch::GetStream( unsigned iStream )
    {
        return fStreams.at( iStream );
    }

}

#endif /* MMONARCH_HH_ */
