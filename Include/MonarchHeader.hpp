#ifndef MONARCHHEADER_HPP_
#define MONARCHHEADER_HPP_

#include "MonarchTypes.hpp"

#include <string>
using std::string;

namespace Protobuf
{
    class MonarchHeader;
}

class MonarchHeader
{
    private:
        mutable Protobuf::MonarchHeader* fProtobufHeader;

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

        void SetTimestamp( const string& aTimestamp );
        const string& GetTimestamp() const;

        void SetDescription( const string& aDescription );
        const string& GetDescription() const;


        void SetContentMode( ContentModeType aContentMode );
        ContentModeType GetContentMode() const;

        void SetSourceMode( SourceModeType aSourceMode );
        SourceModeType GetSourceMode() const;

        void SetFormatMode( FormatModeType aFormatMode );
        FormatModeType GetFormatMode() const;


        void SetRate( double aRate );
        double GetRate() const;

        void SetDuration( unsigned int aDuration );
        unsigned int GetDuration() const;

        void SetLength( unsigned int aLength );
        unsigned int GetLength() const;
};

// Pretty printing method
std::ostream& operator<<( std::ostream& out, const MonarchHeader& hdr );

#endif
