#include "MonarchHeader.hpp"
#include "MonarchHeader.pb.h"

#include "MonarchException.hpp"

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
    fProtobufHeader->set_timestamp( aTimestamp );
    return;
}
const string& MonarchHeader::GetTimestamp() const
{
    return fProtobufHeader->timestamp();
}

void MonarchHeader::SetDescription( const string& aDescription )
{
    fProtobufHeader->set_description( aDescription );
    return;
}
const string& MonarchHeader::GetDescription() const
{
    return fProtobufHeader->description();
}


void MonarchHeader::SetContentMode( ContentModeType aContentMode )
{
    switch( aContentMode )
    {
        case sContentSignal:
            fProtobufHeader->set_contentmode( Protobuf::MonarchHeader_ContentModes_Signal );
            return;
        case sContentBackground:
            fProtobufHeader->set_contentmode( Protobuf::MonarchHeader_ContentModes_Background );
            return;
        case sContentOther:
            fProtobufHeader->set_contentmode( Protobuf::MonarchHeader_ContentModes_Other );
            return;
        default:
            throw MonarchException() << "got unknown content mode";
            return;
    }
}
ContentModeType MonarchHeader::GetContentMode() const
{
    switch( fProtobufHeader->contentmode() )
    {
        case Protobuf::MonarchHeader_ContentModes_Signal:
            return sContentSignal;
        case Protobuf::MonarchHeader_ContentModes_Background:
            return sContentBackground;
        case Protobuf::MonarchHeader_ContentModes_Other:
            return sContentOther;
        default:
            throw MonarchException() << "has unknown content mode";
            return -1;
    }
}

void MonarchHeader::SetSourceMode( SourceModeType aSourceMode )
{
    switch( aSourceMode )
    {
        case sSourceMantis:
            fProtobufHeader->set_sourcemode( Protobuf::MonarchHeader_SourceModes_Mantis );
            return;
        case sSourceSimulation:
            fProtobufHeader->set_sourcemode( Protobuf::MonarchHeader_SourceModes_Simulation );
            return;
        default:
            throw MonarchException() << "got unknown source mode";
            return;
    }
}
SourceModeType MonarchHeader::GetSourceMode() const
{
    switch( fProtobufHeader->sourcemode() )
    {
        case Protobuf::MonarchHeader_SourceModes_Mantis:
            return sSourceMantis;
        case Protobuf::MonarchHeader_SourceModes_Simulation:
            return sSourceSimulation;
        default:
            throw MonarchException() << "has unknown source mode";
            return -1;
    }
}

void MonarchHeader::SetFormatMode( FormatModeType aFormatMode )
{
    switch( aFormatMode )
    {
        case sFormatSingle:
            fProtobufHeader->set_formatmode( Protobuf::MonarchHeader_FormatModes_Single );
            return;
        case sFormatSeparateDual:
            fProtobufHeader->set_formatmode( Protobuf::MonarchHeader_FormatModes_SeparateDual );
            return;
        case sFormatInterleavedDual:
            fProtobufHeader->set_formatmode( Protobuf::MonarchHeader_FormatModes_InterleavedDual );
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
        case Protobuf::MonarchHeader_FormatModes_Single:
            return sFormatSingle;
        case Protobuf::MonarchHeader_FormatModes_SeparateDual:
            return sFormatSeparateDual;
        case Protobuf::MonarchHeader_FormatModes_InterleavedDual:
            return sFormatInterleavedDual;
        default:
            throw MonarchException() << "has unknown format mode";
            return -1;
    }
}

void MonarchHeader::SetRate( double aRate )
{
    fProtobufHeader->set_rate( aRate );
    return;
}
double MonarchHeader::GetRate() const
{
    return fProtobufHeader->rate();
}

void MonarchHeader::SetDuration( unsigned int aDuration )
{
    fProtobufHeader->set_duration( aDuration );
    return;
}
unsigned int MonarchHeader::GetDuration() const
{
    return fProtobufHeader->duration();
}

void MonarchHeader::SetLength( unsigned int aLength )
{
    fProtobufHeader->set_length( aLength );
    return;
}
unsigned int MonarchHeader::GetLength() const
{
    return fProtobufHeader->length();
}

std::ostream& operator<<( std::ostream& out, const MonarchHeader& hdr )
{
    out << "Monarch Header Content: " << "\n";
    out << "\tFilename: " << hdr.GetFilename() << "\n";
    out << "\tTimestamp: " << hdr.GetTimestamp() << "\n";
    out << "\tDescription: " << hdr.GetDescription() << "\n";
    out << "\tContent Mode: " << hdr.GetContentMode() << "\n";
    out << "\tSource Mode: " << hdr.GetSourceMode() << "\n";
    out << "\tFormat Mode: " << hdr.GetFormatMode() << "\n";
    out << "\tRate: " << hdr.GetRate() << "\n";
    out << "\tDuration: " << hdr.GetDuration() << "\n";
    out << "\tLength: " << hdr.GetLength();
    return out;
}
