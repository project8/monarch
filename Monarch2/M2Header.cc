#include "M2Header.hh"
#include "MonarchHeader.pb.h"

#include "M2Exception.hh"

#include <cstdlib> // for atol in parsing timestamp

// for parsing timestamp
#include <sstream>
using std::stringstream;

namespace monarch2
{

    M2Header::M2Header() :
            fProtobufHeader( new Protobuf::MonarchHeader() )
    {
    }
    M2Header::~M2Header()
    {
        delete fProtobufHeader;
    }

    int M2Header::ByteSize() const
    {
        return fProtobufHeader->ByteSize();
    }
    bool M2Header::MarshalToArray( void* data, int size ) const
    {
        return fProtobufHeader->SerializeToArray( data, size );
    }
    bool M2Header::MarshalToStream( std::ostream* aStream ) const
    {
        return fProtobufHeader->SerializeToOstream( aStream );
    }
    bool M2Header::DemarshalFromArray( void* anArray, int aSize ) const
    {
        return fProtobufHeader->ParseFromArray( anArray, aSize );
    }
    bool M2Header::DemarshalFromStream( std::istream* aStream ) const
    {
        return fProtobufHeader->ParseFromIstream( aStream );
    }

    void M2Header::SetFilename( const string& aFilename )
    {
        fProtobufHeader->set_filename( aFilename );
        return;
    }
    const string& M2Header::GetFilename() const
    {
        return fProtobufHeader->filename();
    }

    void M2Header::SetTimestamp( const string& aTimestamp )
    {
        fProtobufHeader->set_rundate( aTimestamp );
        return;
    }
    const string& M2Header::GetTimestamp() const
    {
        return fProtobufHeader->rundate();
    }
    /*
const string M2Header::GetDateTime() const
{
    // This shouldn't be called very often, so parsing in place shouldn't be much of a problem
    // If this needs to be called a lot, this strategy should be rethought
    stringstream ss(GetTimestamp());
    string item;
    // first token in the timestamp is the date/time string
    std::getline(ss, item, sRecordTimeCalSep);
    return item;
}

TimeType M2Header::GetRecordTime0() const
{
    // This shouldn't be called very often, so parsing in place shouldn't be much of a problem
    // If this needs to be called a lot, this strategy should be rethought
    std::stringstream ss(GetTimestamp());
    string item;
    // second token in the timestamp is the record time_0
    std::getline(ss, item, sRecordTimeCalSep);
    std::getline(ss, item, sRecordTimeCalSep);
    return (long long int)atol(item.c_str());
}
     */
    void M2Header::SetDescription( const string& aDescription )
    {
        fProtobufHeader->set_runinfo( aDescription );
        return;
    }
    const string& M2Header::GetDescription() const
    {
        return fProtobufHeader->runinfo();
    }


    void M2Header::SetRunType( RunType aRunType )
    {
        switch( aRunType )
        {
            case sRunTypeSignal:
                fProtobufHeader->set_runtype( Protobuf::MonarchHeader_RunType_Signal );
                return;
            case sRunTypeBackground:
                fProtobufHeader->set_runtype( Protobuf::MonarchHeader_RunType_Background );
                return;
            case sRunTypeOther:
                fProtobufHeader->set_runtype( Protobuf::MonarchHeader_RunType_Other );
                return;
            default:
                throw M2Exception() << "got unknown content mode";
                return;
        }
    }
    RunType M2Header::GetRunType() const
    {
        switch( fProtobufHeader->runtype() )
        {
            case Protobuf::MonarchHeader_RunType_Signal:
                return sRunTypeSignal;
            case Protobuf::MonarchHeader_RunType_Background:
                return sRunTypeBackground;
            case Protobuf::MonarchHeader_RunType_Other:
                return sRunTypeOther;
            default:
                throw M2Exception() << "has unknown run type";
                return 1000;
        }
    }

    void M2Header::SetRunSource( RunSourceType aRunSource )
    {
        switch( aRunSource )
        {
            case sSourceMantis:
                fProtobufHeader->set_runsource( Protobuf::MonarchHeader_RunSource_Mantis );
                return;
            case sSourceSimulation:
                fProtobufHeader->set_runsource( Protobuf::MonarchHeader_RunSource_Simulation );
                return;
            default:
                throw M2Exception() << "got unknown source mode";
                return;
        }
    }
    RunSourceType M2Header::GetRunSource() const
    {
        switch( fProtobufHeader->runsource() )
        {
            case Protobuf::MonarchHeader_RunSource_Mantis:
                return sSourceMantis;
            case Protobuf::MonarchHeader_RunSource_Simulation:
                return sSourceSimulation;
            default:
                throw M2Exception() << "has unknown source mode";
                return 1000;
        }
    }

    void M2Header::SetFormatMode( FormatModeType aFormatMode )
    {
        switch( aFormatMode )
        {
            case sFormatSingle:
                fProtobufHeader->set_formatmode( Protobuf::MonarchHeader_FormatMode_Single );
                return;
            case sFormatMultiSeparate:
                fProtobufHeader->set_formatmode( Protobuf::MonarchHeader_FormatMode_MultiSeparate );
                return;
            case sFormatMultiInterleaved:
                fProtobufHeader->set_formatmode( Protobuf::MonarchHeader_FormatMode_MultiInterleaved );
                return;
            default:
                throw M2Exception() << "got unknown format mode";
                return;
        }
    }
    FormatModeType M2Header::GetFormatMode() const
    {
        switch( fProtobufHeader->formatmode() )
        {
            case Protobuf::MonarchHeader_FormatMode_Single:
                return sFormatSingle;
            case Protobuf::MonarchHeader_FormatMode_MultiSeparate:
                return sFormatMultiSeparate;
            case Protobuf::MonarchHeader_FormatMode_MultiInterleaved:
                return sFormatMultiInterleaved;
            default:
                throw M2Exception() << "has unknown format mode";
                return -1;
        }
    }

    void M2Header::SetAcquisitionMode( unsigned int aMode )
    {
        fProtobufHeader->set_acqmode( aMode );
        return;
    }

    unsigned int M2Header::GetAcquisitionMode() const
    {
        return fProtobufHeader->acqmode();
    }

    void M2Header::SetAcquisitionRate( double aRate )
    {
        fProtobufHeader->set_acqrate( aRate );
        return;
    }
    double M2Header::GetAcquisitionRate() const
    {
        return fProtobufHeader->acqrate();
    }

    void M2Header::SetRunDuration( unsigned int aDuration )
    {
        fProtobufHeader->set_acqtime( aDuration );
        return;
    }
    unsigned int M2Header::GetRunDuration() const
    {
        return fProtobufHeader->acqtime();
    }

    void M2Header::SetRecordSize( unsigned int aSize )
    {
        fProtobufHeader->set_recsize( aSize );
        return;
    }
    unsigned int M2Header::GetRecordSize() const
    {
        return fProtobufHeader->recsize();
    }

    void M2Header::SetDataTypeSize( unsigned aSize )
    {
        fProtobufHeader->set_datatypesize( aSize );
        return;
    }
    unsigned M2Header::GetDataTypeSize() const
    {
        return fProtobufHeader->datatypesize();
    }

    void M2Header::SetBitDepth( unsigned aBitDepth )
    {
        fProtobufHeader->set_bitdepth( aBitDepth );
        return;
    }
    unsigned M2Header::GetBitDepth() const
    {
        return fProtobufHeader->bitdepth();
    }

    void M2Header::SetVoltageMin( double aVoltage )
    {
        fProtobufHeader->set_voltagemin( aVoltage );
        return;
    }
    double M2Header::GetVoltageMin() const
    {
        return fProtobufHeader->voltagemin();
    }

    void M2Header::SetVoltageRange( double aVoltage )
    {
        fProtobufHeader->set_voltagerange( aVoltage );
        return;
    }
    double M2Header::GetVoltageRange() const
    {
        return fProtobufHeader->voltagerange();
    }
}

std::ostream& operator<<( std::ostream& out, const monarch2::M2Header& hdr )
{
    out << "Monarch Header Content: " << "\n";
    out << "\tFilename: " << hdr.GetFilename() << "\n";
    out << "\tAcquisition Mode (# channels): " << hdr.GetAcquisitionMode() << "\n";
    out << "\tAcquisition Rate: " << hdr.GetAcquisitionRate() << " MHz\n";
    out << "\tRun Duration: " << hdr.GetRunDuration() << " ms\n";
    out << "\tRecord Size: " << hdr.GetRecordSize() << "\n";
    out << "\tTimestamp: " << hdr.GetTimestamp() << "\n";
    out << "\tDescription: " << hdr.GetDescription() << "\n";
    out << "\tRun Type: " << hdr.GetRunType() << "\n";
    out << "\tRun Source: " << hdr.GetRunSource() << "\n";
    out << "\tFormat Mode: " << hdr.GetFormatMode() << "\n";
    out << "\tData Type Size: " << hdr.GetDataTypeSize() << " bytes\n";
    out << "\tBit Depth: " << hdr.GetBitDepth() << " bits\n";
    out << "\tVoltage Min: " << hdr.GetVoltageMin() << " V\n";
    out << "\tVoltage Range: " << hdr.GetVoltageRange() << " V\n";
    return out;
}
