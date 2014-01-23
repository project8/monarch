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
            const MonarchRecord* GetRecordInterleaved() const;

            //get the pointer to the current separate channel one record.
            const MonarchRecord* GetRecordSeparateOne() const;

            //get the pointer to the current separate channel two record.
            const MonarchRecord* GetRecordSeparateTwo() const;

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
            MonarchRecord* GetRecordInterleaved();

            //get the pointer to the current separate channel one record.
            MonarchRecord* GetRecordSeparateOne();

            //get the pointer to the current separate channel two record.
            MonarchRecord* GetRecordSeparateTwo();

            //close the file pointer
            void Close();

        private:
            //the MonarchIO class wraps a bare C file pointer.
            MonarchIO* fIO;

            //the header
            mutable MonarchHeader* fHeader;

            //the records
            mutable long int fDataSize;

            mutable size_t fInterleavedRecordSize;

            mutable MonarchRecord* fRecordInterleaved;
            mutable char* fRecordInterleavedBytes;

            mutable size_t fSeparateRecordSize;

            mutable MonarchRecord* fRecordSeparateOne;
            mutable char* fRecordSeparateOneBytes;

            mutable MonarchRecord* fRecordSeparateTwo;
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
            static void Zip( const size_t aSize, const DataType* __restrict__ aRecordOne, const DataType* __restrict__ aRecordTwo, DataType* __restrict__ anInterleavedRecord );
#else
            static void Zip( const size_t aSize, const DataType*  aRecordOne, const DataType*  aRecordTwo, DataType*  anInterleavedRecord );
#endif

#ifdef __GNUG__
            static void Unzip( const size_t aSize, DataType* __restrict__ aRecordOne, DataType* __restrict__ aRecordTwo, const DataType* __restrict__ anInterleavedRecord );
#else
            static void Unzip( const size_t aSize, DataType*  aRecordOne, DataType*  aRecordTwo, const DataType*  anInterleavedRecord );
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

    inline const MonarchRecord* Monarch::GetRecordSeparateOne() const
    {
        return fRecordSeparateOne;
    }
    inline MonarchRecord* Monarch::GetRecordSeparateOne()
    {
        return fRecordSeparateOne;
    }

    inline const MonarchRecord* Monarch::GetRecordSeparateTwo() const
    {
        return fRecordSeparateTwo;
    }
    inline MonarchRecord* Monarch::GetRecordSeparateTwo()
    {
        return fRecordSeparateTwo;
    }

    inline const MonarchRecord* Monarch::GetRecordInterleaved() const
    {
        return fRecordInterleaved;
    }
    inline MonarchRecord* Monarch::GetRecordInterleaved()
    {
        return fRecordInterleaved;
    }

#ifdef __GNUG__
    inline void Monarch::Zip( const size_t aSize, const DataType* __restrict__ aRecordOne, const DataType* __restrict__ aRecordTwo, DataType* __restrict__ anInterleavedRecord )
#else
    inline void Monarch::Zip( const size_t aSize, const DataType* aRecordOne, const DataType* aRecordTwo, DataType* anInterleavedRecord )
#endif
    {
        for( size_t anIndex = 0; anIndex < aSize; anIndex++ )
        {
            *anInterleavedRecord = *aRecordOne;
            anInterleavedRecord++;
            aRecordOne++;

            *anInterleavedRecord = *aRecordTwo;
            anInterleavedRecord++;
            aRecordTwo++;
        }
    }

#ifdef __GNUG__
    inline void Monarch::Unzip( const size_t aSize, DataType* __restrict__ aRecordOne, DataType* __restrict__ aRecordTwo, const DataType* __restrict__ anInterleavedRecord )
#else
    inline void Monarch::Unzip( const size_t aSize, DataType* aRecordOne, DataType* aRecordTwo, const DataType* anInterleavedRecord )
#endif
    {
        for( size_t anIndex = 0; anIndex < aSize; anIndex++ )
        {
            *aRecordOne = *anInterleavedRecord;
            anInterleavedRecord++;
            aRecordOne++;

            *aRecordTwo = *anInterleavedRecord;
            anInterleavedRecord++;
            aRecordTwo++;
        }
    }

}

#endif
