/*
 * M4Monarch.hh
 *
 *  Created on: Apr 12, 2023
 *      Author: N.S. Oblath
 */

#ifndef M4MONARCH_HH_
#define M4MONARCH_HH_

#include "M4Exception.hh"
#include "M4Header.hh"
#include "logger.hh"
#include "M4Stream.hh"

// factory functions to create files, groups and datasets
#include "z5/factory.hxx"
// handles for z5 filesystem objects
#include "z5/filesystem/handle.hxx"
// io for xtensor multi-arrays
#include "z5/multiarray/xtensor_access.hxx"
// attribute functionality
#include "z5/attributes.hxx"



#include <string>
#include <stdexcept>

namespace monarch4
{
    LOGGER( mlog_mmonarch, "M4Monarch4.h" );

    /*!
     @class Monarch4
     @author N. S. Oblath (v3), D. Furse (original)

     @brief Egg file read/write access

     @details
     Monarch4 objects are created with one of the two static Open functions:
      - OpenForReading() will create a read-only Monarch4 object;
      - OpenForWriting() will create a writeable Monarch4 object.

     The data structure and general header information is setup/read via the header (accessed with GetHeader()).

     Stream data is written or read via the stream objects (accessed with GetStream( unsigned )).
    */
    class M4_API Monarch4
    {
            //***********************
            // constructors and state
            //***********************

        private:
            //private to force use of static constructor methods
            Monarch4();
            Monarch4( const Monarch4& ) = delete;
            Monarch4( Monarch4&& ) = delete;

            //current state of monarch
        public:
            typedef enum
            {
                eOpenToRead, // state when monarch has a file open but hasn't read the header
                eOpenToWrite, // state when monarch has a file open but hasn't written the header
                eReadyToRead, // state when monarch has dealt with the header and is reading records
                eReadyToWrite, // state when monarch has dealt with the header and is writing records
                eClosed // state when monarch has no file
            } State;
            State GetState() const;
            
        private:
            mutable State fState;

        public:
            ~Monarch4();

            //********************************
            // methods for reading (all const)
            //********************************

        public:

            /// This static method opens the file for reading.
            /// If the file exists and can be read, this returns a prepared monarch pointer, and memory is allocated for the header.
            /// Upon successful return monarch is in the eOpen state.
            static const Monarch4* OpenForReading( const std::string& filename );

            /// This method extracts the header information from the file.
            /// If the header read correctly, this returns and the header may be examined, and memory is allocated for the record.
            /// Upon successful return monarch is in the eReady state.
            /// An exception is thrown if the header is not read.
            void ReadHeader() const;

            /// Get the pointer to the header.
            const M4Header* GetHeader() const;

            /// Get the pointer to a particular stream
            const M4Stream* GetStream( unsigned stream ) const;

            /// Close the file.
            void FinishReading() const;

            //*********************************
            // methods for writing (none const)
            //*********************************

        public:

            /// This static method opens the file for writing.
            /// If the file exists and can be written, this returns a prepared monarch pointer, and memory is allocated for the header.
            /// Upon successful return monarch is in the eOpen state.
            static Monarch4* OpenForWriting( const std::string& filename );

            /// This method marshals the current header to the file.
            /// If the header marshalled correctly, this returns true, memory is allocated for the record(s).
            /// Upon successful return monarch is in the eReady state.
            void WriteHeader();

            /// Get the pointer to the header.
            M4Header* GetHeader();

            /// Get the pointer to a particular stream
            M4Stream* GetStream( unsigned stream );

            /// Close the file.
            void FinishWriting();

        private:
            // the Zarr/Egg file
            // mutable std::unique_ptr< z5::filesystem::handle::File > fFile;
            mutable z5FileHandle* hFile;

            // the header
            // mutable std::unique_ptr< M4Header > fHeader;
            mutable std::shared_ptr< M4Header > fHeader;

            // the streams
            mutable std::vector< M4Stream* > fStreams;

            // stream read/write mutex
            mutable mutex_ptr fMutexPtr;

    };

    inline const M4Header* Monarch4::GetHeader() const
    {
        return fHeader.get();       // access to raw pointer not a good idea
    }
    inline M4Header* Monarch4::GetHeader()
    {
        return fHeader.get();       // access to raw poiner not a good idea
    }

    inline const M4Stream* Monarch4::GetStream( unsigned iStream ) const
    {
        try
        {
            return fStreams.at( iStream );
        }
        catch( std::out_of_range& e )
        {
            return nullptr;
        }
    }
    inline M4Stream* Monarch4::GetStream( unsigned iStream )
    {
        try
        {
            return fStreams.at( iStream );
        }
        catch( std::out_of_range& e )
        {
            return nullptr;
        }
    }

}

#endif /* M4MONARCH_HH_ */
