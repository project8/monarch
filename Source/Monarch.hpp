#ifndef MONARCH_HPP_
#define MONARCH_HPP_

#include "MonarchIO.hpp"
#include "MonarchHeader.hpp"
#include "MonarchRecord.hpp"

#include <string>
using std::string;

namespace monarch
{

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

            //this static method opens the file for reading.
            //if the file exists and can be read, this returns a prepared monarch pointer, and memory is allocated for the header.
            //upon successful return monarch is in the eOpen state.
            static const Monarch* OpenForReading( const string& filename );

            //this method parses the file for the header contents.
            //if the header demarshalled correctly, this returns and the header may be examined, and memory is allocated for the record.
            //upon successful return monarch is in the eReady state.
            //an exception is thrown if the header is not read.
            void ReadHeader() const;

            //get the pointer to the header.
            const MonarchHeader* GetHeader() const;

            //set the interface type to use
            void SetInterface( InterfaceModeType aMode ) const;

            //this method parses the file for a next record.
            //if the record demarshalled correctly, this returns true and the record is refreshed with content.
            //when the end of the file is reached, this will return false.
            bool ReadRecord( int anOffset = 0 ) const;

            //get the pointer to the current interleaved record.
            const MonarchRecordBytes* GetRecordInterleaved() const;

            //get the pointer to the current separate channel one record.
            const MonarchRecordBytes* GetRecordSeparateOne() const;

            //get the pointer to the current separate channel two record.
            const MonarchRecordBytes* GetRecordSeparateTwo() const;

            //close the file pointer.
            void Close() const;

            //*********************************
            // methods for writing (none const)
            //*********************************

        public:

            //this static method opens the file for writing.
            //if the file exists and can be written, this returns a prepared monarch pointer, and memory is allocated for the header.
            //upon successful return monarch is in the eOpen state.
            static Monarch* OpenForWriting( const string& filename );

            //this method marshals the current header to the file.
            //if the header marshalled correctly, this returns true, memory is allocated for the record(s).
            //upon successful return monarch is in the eReady state.
            void WriteHeader();

            //get the pointer to the header.
            MonarchHeader* GetHeader();

            //set the interface type to use.
            void SetInterface( InterfaceModeType aMode );

            //this method marshals the current record into the file.
            //if the record marshalled correctly, this returns true.
            bool WriteRecord();

            //get the pointer to the current interleaved record.
            MonarchRecordBytes* GetRecordInterleaved();

            //get the pointer to the current separate channel one record.
            MonarchRecordBytes* GetRecordSeparateOne();

            //get the pointer to the current separate channel two record.
            MonarchRecordBytes* GetRecordSeparateTwo();

            //close the file pointer
            void Close();

        private:
            //the MonarchIO class wraps a bare C file pointer.
            MonarchIO* fIO;

            //the header
            mutable MonarchHeader* fHeader;

            //size of the native type of the records in bytes
            mutable size_t fDataTypeSize;

            //number of bytes in the data array in a record
            mutable size_t fDataNBytes;
            //number of samples in the data array in a record
            mutable size_t fDataSize;

            //number of bytes in an interleaved record
            mutable size_t fInterleavedRecordNBytes;

            //pointer to a MonarchRecordBytes occupying the interleaved record
            mutable MonarchRecordBytes* fRecordInterleaved;
            //pointer to the bytes that hold the interleaved record
            mutable char* fRecordInterleavedBytes;

            //number of bytes in a separate record
            mutable size_t fSeparateRecordNBytes;

            //pointer to a MonarchRecordBytes occupying the first separate record
            mutable MonarchRecordBytes* fRecordSeparateOne;
            //pointer to the bytes that hold the first separate record
            mutable char* fRecordSeparateOneBytes;

            //pointer to a MonarchRecordBytes occupying the second separate record
            mutable MonarchRecordBytes* fRecordSeparateTwo;
            //pointer to the bytes that hold the second separate record
            mutable char* fRecordSeparateTwoBytes;

            //the private read functions
            mutable bool (Monarch::*fReadFunction)( int anOffset ) const;
            bool InterleavedFromSingle( int anOffset ) const;
            bool InterleavedFromSeparate( int anOffset ) const;
            bool InterleavedFromInterleaved( int anOffset ) const;
            bool SeparateFromSingle( int anOffset ) const;
            bool SeparateFromSeparate( int anOffset ) const;
            bool SeparateFromInterleaved( int anOffset ) const;

            //the private write functions
            mutable bool (Monarch::*fWriteFunction)();
            bool InterleavedToSingle();
            bool InterleavedToSeparate();
            bool InterleavedToInterleaved();
            bool SeparateToSingle();
            bool SeparateToSeparate();
            bool SeparateToInterleaved();

        private:
#ifdef __GNUG__
            static void Zip( const size_t aSize, const size_t aDataTypeSize, const char* __restrict__ aRecordOne, const char* __restrict__ aRecordTwo, char* __restrict__ anInterleavedRecord );
#else
            static void Zip( const size_t aSize, const size_t aDataTypeSize, const char*  aRecordOne, const char*  aRecordTwo, char*  anInterleavedRecord );
#endif

#ifdef __GNUG__
            static void Unzip( const size_t aSize, const size_t aDataTypeSize, char* __restrict__ aRecordOne, char* __restrict__ aRecordTwo, const char* __restrict__ anInterleavedRecord );
#else
            static void Unzip( const size_t aSize, const size_t aDataTypeSize, char*  aRecordOne, char*  aRecordTwo, const char*  anInterleavedRecord );
#endif

    };

    inline const MonarchHeader* Monarch::GetHeader() const
    {
        return fHeader;
    }
    inline MonarchHeader* Monarch::GetHeader()
    {
        return fHeader;
    }

    inline const MonarchRecordBytes* Monarch::GetRecordSeparateOne() const
    {
        return fRecordSeparateOne;
    }
    inline MonarchRecordBytes* Monarch::GetRecordSeparateOne()
    {
        return fRecordSeparateOne;
    }

    inline const MonarchRecordBytes* Monarch::GetRecordSeparateTwo() const
    {
        return fRecordSeparateTwo;
    }
    inline MonarchRecordBytes* Monarch::GetRecordSeparateTwo()
    {
        return fRecordSeparateTwo;
    }

    inline const MonarchRecordBytes* Monarch::GetRecordInterleaved() const
    {
        return fRecordInterleaved;
    }
    inline MonarchRecordBytes* Monarch::GetRecordInterleaved()
    {
        return fRecordInterleaved;
    }

#ifdef __GNUG__
    inline void Monarch::Zip( const size_t aSize, const size_t aDataTypeSize, const char* __restrict__ aRecordOne, const char* __restrict__ aRecordTwo, char* __restrict__ anInterleavedRecord )
#else
    inline void Monarch::Zip( const size_t aSize, const size_t aDataTypeSize, const char* aRecordOne, const char* aRecordTwo, char* anInterleavedRecord )
#endif
    {
        for( size_t anIndex = 0; anIndex < aSize; anIndex++ )
        {
            *anInterleavedRecord = *aRecordOne;
            anInterleavedRecord += aDataTypeSize;
            aRecordOne += aDataTypeSize;

            *anInterleavedRecord = *aRecordTwo;
            anInterleavedRecord += aDataTypeSize;
            aRecordTwo += aDataTypeSize;
        }
    }

#ifdef __GNUG__
    inline void Monarch::Unzip( const size_t aSize, const size_t aDataTypeSize, char* __restrict__ aRecordOne, char* __restrict__ aRecordTwo, const char* __restrict__ anInterleavedRecord )
#else
    inline void Monarch::Unzip( const size_t aSize, const size_t aDataTypeSize, char* aRecordOne, char* aRecordTwo, const char* anInterleavedRecord )
#endif
    {
        for( size_t anIndex = 0; anIndex < aSize; anIndex++ )
        {
            *aRecordOne = *anInterleavedRecord;
            anInterleavedRecord += aDataTypeSize;
            aRecordOne += aDataTypeSize;

            *aRecordTwo = *anInterleavedRecord;
            anInterleavedRecord += aDataTypeSize;
            aRecordTwo += aDataTypeSize;
        }
    }

}

#endif
