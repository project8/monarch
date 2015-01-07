#ifndef MONARCHHEADER_HH_
#define MONARCHHEADER_HH_

#include "M2Types.hh"

#include <string>
using std::string;

namespace Protobuf
{
    class MonarchHeader;
}

namespace monarch2
{

    class M2Header
    {
        private:
            mutable Protobuf::MonarchHeader* fProtobufHeader;

        public:
            M2Header();
            ~M2Header();

            //size of monarch header in bytes (fixed by .proto definition)
            int ByteSize() const;

            //marshal a MonarchHeader to an array of data
            bool MarshalToArray( void* anArray, int aSize ) const;
            bool MarshalToStream( std::ostream* aStream ) const;

            //demarshal a MonarchHeader object from an array of data
            bool DemarshalFromArray( void* anArray, int aSize ) const;
            bool DemarshalFromStream( std::istream* aStream ) const;

            //access methods

            // Required in protobuf header

            void SetFilename( const string& aFilename );
            const string& GetFilename() const;

            // Acquisition mode: how many channels?
            void SetAcquisitionMode( AcquisitionModeType aMode );
            AcquisitionModeType GetAcquisitionMode() const;

            // Acquisition rate in MHz
            void SetAcquisitionRate( double aRate );
            double GetAcquisitionRate() const;

            // Run duration in ms
            void SetRunDuration( unsigned int aDuration );
            unsigned int GetRunDuration() const;

            // Record size in samples
            void SetRecordSize( unsigned int aSize );
            unsigned int GetRecordSize() const;


            // Optional in protobuf header

            void SetTimestamp( const string& aTimestamp );
            const string& GetTimestamp() const;

            void SetDescription( const string& aDescription );
            const string& GetDescription() const;

            void SetRunType( RunType aRunType );
            RunType GetRunType() const;

            void SetRunSource( RunSourceType aRunSource );
            RunSourceType GetRunSource() const;

            void SetFormatMode( FormatModeType aFormatMode );
            FormatModeType GetFormatMode() const;

            void SetDataTypeSize( unsigned aSize );
            unsigned GetDataTypeSize() const;

            void SetBitDepth( unsigned aBitDepth );
            unsigned GetBitDepth() const;

            void SetVoltageMin( double aVoltage );
            double GetVoltageMin() const;

            void SetVoltageRange( double aVoltage );
            double GetVoltageRange() const;

    };

}

// Pretty printing method
std::ostream& operator<<( std::ostream& out, const monarch2::M2Header& hdr );

#endif
