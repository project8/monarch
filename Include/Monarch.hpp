#ifndef __MONARCH_HPP
#define __MONARCH_HPP

#include "MonarchIO.hpp"
#include "MonarchHeader.hpp"
#include "MonarchRecord.hpp"

#include <string>
using std::string;

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
        //if the header demarshalled correctly, this returns true and the header may be examined, and memory is allocated for the record.
        //upon successful return monarch is in the eReady state.
        bool ReadHeader() const;

        //get the pointer to the header.
        const MonarchHeader* GetHeader() const;

        //this method parses the file for a next record.
        //if the record demarshalled correctly, this returns true and the record is refreshed with content.
        //when the end of the file is reached, this will return false.
        bool ReadRecord( int anOffset = 0 ) const;

        //get the pointer to the current channel one record.
        const MonarchRecord* GetRecordOne() const;

        //get the pointer to the current channel two record.
        const MonarchRecord* GetRecordTwo() const;

        //close the file pointer.
        bool Close() const;

        //********************
        // methods for writing
        //********************

    public:

        //this static method opens the file for writing.
        //if the file exists and can be written, this returns a prepared monarch pointer, and memory is allocated for the header.
        //upon successful return monarch is in the eOpen state.
        static Monarch* OpenForWriting( const string& filename );

        //this method marshals the current header to the file.
        //if the header marshalled correctly, this returns true, memory is allocated for the record.
        //upon successful return monarch is in the eReady state.
        bool WriteHeader();

        //get the pointer to the header.
        MonarchHeader* GetHeader();

        //this method marshals the current record into the file.
        //if the record marshalled correctly, this returns true.
        bool WriteRecord();

        //get the pointer to the current record.
        MonarchRecord* GetRecordInterleaved();

        //close the file pointer
        bool Close();

    private:
        //the MonarchIO class wraps a bare C file pointer.
        MonarchIO* fIO;

        //the header
        mutable MonarchHeader* fHeader;

        //the records
        mutable size_t fInterleavedRecordSize;

        mutable MonarchRecord* fRecordInterleaved;
        mutable char* fRecordInterleavedBytes;

        mutable size_t fSplitRecordSize;

        mutable MonarchRecord* fRecordOne;
        mutable char* fRecordOneBytes;

        mutable MonarchRecord* fRecordTwo;
        mutable char* fRecordTwoBytes;

        //the private read functions
        mutable bool (Monarch::*fReadFunction)() const;
        bool ReadRecordOne() const;
        bool ReadRecordTwo() const;
};

inline MonarchHeader* Monarch::GetHeader()
{
    return fHeader;
}
inline const MonarchHeader* Monarch::GetHeader() const
{
    return fHeader;
}

inline MonarchRecord* Monarch::GetRecordInterleaved()
{
    return fRecordInterleaved;
}

inline const MonarchRecord* Monarch::GetRecordOne() const
{
    return fRecordOne;
}
inline const MonarchRecord* Monarch::GetRecordTwo() const
{
    return fRecordTwo;
}

#endif
