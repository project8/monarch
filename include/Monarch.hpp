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
        bool ReadRecord() const;

        //get the pointer to the current record.
        const MonarchRecord* GetRecord() const;

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
        MonarchRecord* GetRecord();

        //close the file pointer
        bool Close();

    private:
        //the MonarchIO class wraps a bare C file pointer.
        MonarchIO* fIO;

        //the header
        mutable MonarchHeader* fHeader;

        //the record
        mutable MonarchRecord* fRecord;
        mutable char* fRecordBytes;
        mutable size_t fRecordSize;
};

inline MonarchHeader* Monarch::GetHeader()
{
    return fHeader;
}
inline const MonarchHeader* Monarch::GetHeader() const
{
    return fHeader;
}

inline MonarchRecord* Monarch::GetRecord()
{
    return fRecord;
}
inline const MonarchRecord* Monarch::GetRecord() const
{
    return fRecord;
}

#endif
