#include "MonarchHeader.hpp"
#include "MonarchHeader.pb.h"

#include "MonarchException.hpp"

#include <cstdlib> // for atol in parsing timestamp

// for parsing timestamp
#include <sstream>
using std::stringstream;

MonarchHeader::MonarchHeader() :
    fProtobufHeader( new Protobuf::MonarchHeader() )
{
}
MonarchHeader::~MonarchHeader()
{
    delete fProtobufHeader;
}

int MonarchHeader::ByteSize() const
{
    return fProtobufHeader->ByteSize();
}
bool MonarchHeader::MarshalToArray( void* data, int size ) const
{
    return fProtobufHeader->SerializeToArray( data, size );
}
bool MonarchHeader::MarshalToStream( std::ostream* aStream ) const
{
    return fProtobufHeader->SerializeToOstream( aStream );
}
bool MonarchHeader::DemarshalFromArray( void* anArray, int aSize ) const
{
    return fProtobufHeader->ParseFromArray( anArray, aSize );
}
bool MonarchHeader::DemarshalFromStream( std::istream* aStream ) const
{
    return fProtobufHeader->ParseFromIstream( aStream );
}

void MonarchHeader::SetFilename( const string& aFilename )
{
    fProtobufHeader->set_filename( aFilename );
    return;
}
const string& MonarchHeader::GetFilename() const
{
    return fProtobufHeader->filename();
}

void MonarchHeader::SetTimestamp( const string& aTimestamp )
{
    fProtobufHeader->set_rundate( aTimestamp );
    return;
}
const string& MonarchHeader::GetTimestamp() const
{
    return fProtobufHeader->rundate();
}
/*
const string MonarchHeader::GetDateTime() const
{
    // This shouldn't be called very often, so parsing in place shouldn't be much of a problem
    // If this needs to be called a lot, this strategy should be rethought
    stringstream ss(GetTimestamp());
    string item;
    // first token in the timestamp is the date/time string
    std::getline(ss, item, sRecordTimeCalSep);
    return item;
}

TimeType MonarchHeader::GetRecordTime0() const
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
void MonarchHeader::SetDescription( const string& aDescription )
{
    fProtobufHeader->set_runinfo( aDescription );
    return;
}
const string& MonarchHeader::GetDescription() const
{
    return fProtobufHeader->runinfo();
}


void MonarchHeader::SetRunType( RunType aRunType )
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
            throw MonarchException() << "got unknown content mode";
            return;
    }
}
RunType MonarchHeader::GetRunType() const
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
            throw MonarchException() << "has unknown run type";
            return 1000;
    }
}

void MonarchHeader::SetRunSource( RunSourceType aRunSource )
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
            throw MonarchException() << "got unknown source mode";
            return;
    }
}
RunSourceType MonarchHeader::GetRunSource() const
{
    switch( fProtobufHeader->runsource() )
    {
        case Protobuf::MonarchHeader_RunSource_Mantis:
            return sSourceMantis;
        case Protobuf::MonarchHeader_RunSource_Simulation:
            return sSourceSimulation;
        default:
            throw MonarchException() << "has unknown source mode";
            return 1000;
    }
}

void MonarchHeader::SetFormatMode( FormatModeType aFormatMode )
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
            throw MonarchException() << "got unknown format mode";
            return;
    }
}
FormatModeType MonarchHeader::GetFormatMode() const
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
            throw MonarchException() << "has unknown format mode";
            return -1;
    }
}

void MonarchHeader::SetAcquisitionMode( unsigned int aMode )
{
    fProtobufHeader->set_acqmode( aMode );
    return;
}

unsigned int MonarchHeader::GetAcquisitionMode() const
{
    return fProtobufHeader->acqmode();
}

void MonarchHeader::SetAcquisitionRate( double aRate )
{
    fProtobufHeader->set_acqrate( aRate );
    return;
}
double MonarchHeader::GetAcquisitionRate() const
{
    return fProtobufHeader->acqrate();
}

void MonarchHeader::SetRunDuration( unsigned int aDuration )
{
    fProtobufHeader->set_acqtime( aDuration );
    return;
}
unsigned int MonarchHeader::GetRunDuration() const
{
    return fProtobufHeader->acqtime();
}

void MonarchHeader::SetRecordSize( unsigned int aSize )
{
    fProtobufHeader->set_recsize( aSize );
    return;
}
unsigned int MonarchHeader::GetRecordSize() const
{
    return fProtobufHeader->recsize();
}

std::ostream& operator<<( std::ostream& out, const MonarchHeader& hdr )
{
    out << "Monarch Header Content: " << "\n";
    out << "\tFilename: " << hdr.GetFilename() << "\n";
    out << "\tTimestamp: " << hdr.GetTimestamp() << "\n";
    out << "\tDescription: " << hdr.GetDescription() << "\n";
    out << "\tRun Type: " << hdr.GetRunType() << "\n";
    out << "\tRun Source: " << hdr.GetRunSource() << "\n";
    out << "\tFormat Mode: " << hdr.GetFormatMode() << "\n";
    out << "\tRate: " << hdr.GetAcquisitionRate() << "\n";
    out << "\tDuration: " << hdr.GetRunDuration() << "\n";
    out << "\tLength: " << hdr.GetRecordSize();
    return out;
}
