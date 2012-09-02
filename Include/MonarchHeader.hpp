#ifndef MONARCHHEADER_HPP_
#define MONARCHHEADER_HPP_

#include "MonarchTypes.hpp"

#include <string>
using std::string;

namespace MonarchPB
{
    class MonarchHeader;
}

class MonarchHeader
{
    private:
        mutable MonarchPB::MonarchHeader* fPBHeader;

    public:
        MonarchHeader();
        ~MonarchHeader();

        //size of monarch header in bytes (fixed by .proto definition)
        int ByteSize() const;

        //marshal a MonarchHeader to an array of data
        bool MarshalToArray( void* anArray, int aSize ) const;
        bool MarshalToStream( std::ostream* aStream ) const;

        //demarshal a MonarchHeader object from an array of data
        bool DemarshalFromArray( void* anArray, int aSize ) const;
        bool DemarshalFromStream( std::istream* aStream ) const;

        //access methods

        void SetFilename( const string& aFilename );
        const string& GetFilename() const;

        void SetAcqRate( double acqr );
        double GetAcqRate() const;

        void SetAcqMode( unsigned int mode );
        unsigned int GetAcqMode() const;

        void SetAcqTime( unsigned int acqt );
        unsigned int GetAcqTime() const;

        void SetRecordSize( size_t recsize );
        size_t GetRecordSize() const;
};

// Pretty printing method
std::ostream& operator<<( std::ostream& out, const MonarchHeader& hdr );

#endif
