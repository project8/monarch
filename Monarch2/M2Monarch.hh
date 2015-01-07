#ifndef MONARCH2_HH_
#define MONARCH2_HH_

#include "M2IO.hh"
#include "M2Header.hh"
#include "M2Record.hh"

#include <string>
using std::string;

namespace monarch2
{

    class Monarch2
    {
            //***********************
            // constructors and state
            //***********************

        private:
            //private to force use of static constructor methods
            Monarch2();

            //current state of monarch
            typedef enum
            {
                eOpen, // state when monarch has a file open but hasn't written/read the header
                eReady, // state when monarch has dealt with the header and is writing/reading records
                eClosed // state when monarch has no file
            } State;
            mutable State fState;

        public:
            ~Monarch2();

            //********************************
            // methods for reading (all const)
            //********************************

        public:

            //this static method opens the file for reading.
            //if the file exists and can be read, this returns a prepared monarch pointer, and memory is allocated for the header.
            //upon successful return monarch is in the eOpen state.
            static const Monarch2* OpenForReading( const string& filename );

            //this method parses the file for the header contents.
            //if the header demarshalled correctly, this returns and the header may be examined, and memory is allocated for the record.
            //upon successful return monarch is in the eReady state.
            //an exception is thrown if the header is not read.
            void ReadHeader() const;

            //get the pointer to the header.
            const M2Header* GetHeader() const;

            //set the interface type to use
            void SetInterface( InterfaceModeType aMode ) const;

            //this method parses the file for a next record.
            //if the record demarshalled correctly, this returns true and the record is refreshed with content.
            //when the end of the file is reached, this will return false.
            bool ReadRecord( int anOffset = 0 ) const;

            //get the pointer to the current interleaved record.
            const M2RecordBytes* GetRecordInterleaved() const;

            //get the pointer to the current separate channel one record.
            const M2RecordBytes* GetRecordSeparateOne() const;

            //get the pointer to the current separate channel two record.
            const M2RecordBytes* GetRecordSeparateTwo() const;

            //close the file pointer.
            void Close() const;

            //*********************************
            // methods for writing (none const)
            //*********************************

        public:

            //this static method opens the file for writing.
            //if the file exists and can be written, this returns a prepared monarch pointer, and memory is allocated for the header.
            //upon successful return monarch is in the eOpen state.
            static Monarch2* OpenForWriting( const string& filename );

            //this method marshals the current header to the file.
            //if the header marshalled correctly, this returns true, memory is allocated for the record(s).
            //upon successful return monarch is in the eReady state.
            void WriteHeader();

            //get the pointer to the header.
            M2Header* GetHeader();

            //set the interface type to use.
            void SetInterface( InterfaceModeType aMode );

            //this method marshals the current record into the file.
            //if the record marshalled correctly, this returns true.
            bool WriteRecord();

            //get the pointer to the current interleaved record.
            M2RecordBytes* GetRecordInterleaved();

            //get the pointer to the current separate channel one record.
            M2RecordBytes* GetRecordSeparateOne();

            //get the pointer to the current separate channel two record.
            M2RecordBytes* GetRecordSeparateTwo();

            //close the file pointer
            void Close();

        private:
            //the M2IO class wraps a bare C file pointer.
            M2IO* fIO;

            //the header
            mutable M2Header* fHeader;

            //size of the native type of the records in bytes
            mutable size_t fDataTypeSize;

            //number of bytes in the data array in a record
            mutable size_t fDataNBytes;
            //number of samples in the data array in a record
            mutable size_t fDataSize;

            //number of bytes in an interleaved record
            mutable size_t fInterleavedRecordNBytes;

            //pointer to a M2RecordBytes occupying the interleaved record
            mutable M2RecordBytes* fRecordInterleaved;
            //pointer to the bytes that hold the interleaved record
            mutable byte_type* fRecordInterleavedBytes;

            //number of bytes in a separate record
            mutable size_t fSeparateRecordNBytes;

            //pointer to a M2RecordBytes occupying the first separate record
            mutable M2RecordBytes* fRecordSeparateOne;
            //pointer to the bytes that hold the first separate record
            mutable byte_type* fRecordSeparateOneBytes;

            //pointer to a M2RecordBytes occupying the second separate record
            mutable M2RecordBytes* fRecordSeparateTwo;
            //pointer to the bytes that hold the second separate record
            mutable byte_type* fRecordSeparateTwoBytes;

            //the private read functions
            mutable bool (Monarch2::*fReadFunction)( int anOffset ) const;
            bool InterleavedFromSingle( int anOffset ) const;
            bool InterleavedFromSeparate( int anOffset ) const;
            bool InterleavedFromInterleaved( int anOffset ) const;
            bool SeparateFromSingle( int anOffset ) const;
            bool SeparateFromSeparate( int anOffset ) const;
            bool SeparateFromInterleaved( int anOffset ) const;

            //the private write functions
            mutable bool (Monarch2::*fWriteFunction)();
            bool InterleavedToSingle();
            bool InterleavedToSeparate();
            bool InterleavedToInterleaved();
            bool SeparateToSingle();
            bool SeparateToSeparate();
            bool SeparateToInterleaved();

        private:
#ifdef __GNUG__
            static void Zip( const size_t aSize, const size_t aDataTypeSize, const byte_type* __restrict__ aRecordOne, const byte_type* __restrict__ aRecordTwo, byte_type* __restrict__ anInterleavedRecord );
#else
            static void Zip( const size_t aSize, const size_t aDataTypeSize, const byte_type*  aRecordOne, const byte_type*  aRecordTwo, byte_type*  anInterleavedRecord );
#endif

#ifdef __GNUG__
            static void Unzip( const size_t aSize, const size_t aDataTypeSize, byte_type* __restrict__ aRecordOne, byte_type* __restrict__ aRecordTwo, const byte_type* __restrict__ anInterleavedRecord );
#else
            static void Unzip( const size_t aSize, const size_t aDataTypeSize, byte_type*  aRecordOne, byte_type*  aRecordTwo, const byte_type*  anInterleavedRecord );
#endif

    };

    inline const M2Header* Monarch2::GetHeader() const
    {
        return fHeader;
    }
    inline M2Header* Monarch2::GetHeader()
    {
        return fHeader;
    }

    inline const M2RecordBytes* Monarch2::GetRecordSeparateOne() const
    {
        return fRecordSeparateOne;
    }
    inline M2RecordBytes* Monarch2::GetRecordSeparateOne()
    {
        return fRecordSeparateOne;
    }

    inline const M2RecordBytes* Monarch2::GetRecordSeparateTwo() const
    {
        return fRecordSeparateTwo;
    }
    inline M2RecordBytes* Monarch2::GetRecordSeparateTwo()
    {
        return fRecordSeparateTwo;
    }

    inline const M2RecordBytes* Monarch2::GetRecordInterleaved() const
    {
        return fRecordInterleaved;
    }
    inline M2RecordBytes* Monarch2::GetRecordInterleaved()
    {
        return fRecordInterleaved;
    }

#ifdef __GNUG__
    inline void Monarch2::Zip( const size_t aSize, const size_t aDataTypeSize, const byte_type* __restrict__ aRecordOne, const byte_type* __restrict__ aRecordTwo, byte_type* __restrict__ anInterleavedRecord )
#else
    inline void Monarch2::Zip( const size_t aSize, const size_t aDataTypeSize, const byte_type* aRecordOne, const byte_type* aRecordTwo, byte_type* anInterleavedRecord )
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
    inline void Monarch2::Unzip( const size_t aSize, const size_t aDataTypeSize, byte_type* __restrict__ aRecordOne, byte_type* __restrict__ aRecordTwo, const byte_type* __restrict__ anInterleavedRecord )
#else
    inline void Monarch2::Unzip( const size_t aSize, const size_t aDataTypeSize, byte_type* aRecordOne, byte_type* aRecordTwo, const byte_type* anInterleavedRecord )
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
