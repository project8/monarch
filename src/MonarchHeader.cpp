#include "MonarchHeader.hpp"
#include "MonarchHeader.pb.h"

MonarchHeader::MonarchHeader() :
    fPBHeader( new MonarchPB::MonarchHeader )
{
}
MonarchHeader::~MonarchHeader()
{
    delete fPBHeader;
}

int MonarchHeader::ByteSize()
{
    return fPBHeader->ByteSize();
}
bool MonarchHeader::MarshalToArray( void* data, int size )
{
    return fPBHeader->SerializeToArray( data, size );
}
bool MonarchHeader::DemarshalFromArray( void* anArray, int aSize ) const
{
    return fPBHeader->ParseFromArray( anArray, aSize );
}

void MonarchHeader::SetFilename( const string& aFilename )
{
    fPBHeader->set_filename( aFilename );
    return;
}
const string& MonarchHeader::GetFilename() const
{
    return fPBHeader->filename();
}

void MonarchHeader::SetAcqRate( double aRate )
{
    fPBHeader->set_acqrate( aRate );
    return;
}
double MonarchHeader::GetAcqRate() const
{
    return fPBHeader->acqrate();
}

void MonarchHeader::SetAcqMode( unsigned int aMode )
{
    fPBHeader->set_acqmode( aMode );
    return;
}
unsigned int MonarchHeader::GetAcqMode() const
{
    return fPBHeader->acqmode();
}

void MonarchHeader::SetAcqTime( unsigned int aTime )
{
    fPBHeader->set_acqtime( aTime );
    return;
}
unsigned int MonarchHeader::GetAcqTime() const
{
    return fPBHeader->acqtime();
}

void MonarchHeader::SetRecordSize( size_t aSize )
{
    fPBHeader->set_recsize( aSize );
    return;
}
size_t MonarchHeader::GetRecordSize() const
{
    return fPBHeader->recsize();
}

std::ostream& operator<<( std::ostream& out, MonarchHeader& hdr )
{
    out << "Here's the header: " << std::endl << std::endl;
    out << "\tFilename: " << hdr.GetFilename() << std::endl;
    out << "\tAcquisition Mode: " << hdr.GetAcqMode() << std::endl;
    out << "\tRecord Size: " << hdr.GetRecordSize() << std::endl;
    out << "\tAcquisition Time: " << hdr.GetAcqTime() << std::endl;
    out << "\tDigitization Rate: " << hdr.GetAcqRate() << std::endl;
    return out;
}
