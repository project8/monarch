#include "Monarch.hpp"

#include <iostream>
using std::cout;
using std::endl;

Monarch::Monarch() :
    fIO( NULL ),
    fHeader( NULL ),
    fRecord( NULL ),
    fRaw( NULL )
{
}
Monarch::~Monarch()
{
    if( fIO != NULL )
    {
        delete fIO;
        fIO = NULL;
    }

    if( fHeader != NULL )
    {
        delete fHeader;
        fHeader = NULL;
    }

    if( fRecord != NULL )
    {
        delete fRecord;
        fRecord = NULL;
    }

    if( fRaw != NULL )
    {
        delete[] fRaw;
        fRaw = NULL;
    }
}

const Monarch* Monarch::OpenForReading( const string& aFilename )
{
    Monarch* tMonarch = new Monarch();
    tMonarch->fIO = new MonarchIO( sReadMode );

    if( tMonarch->fIO->Open( aFilename ) == false )
    {
        cout << "could not open <" << aFilename << "> for reading" << endl;
        delete tMonarch;
        return NULL;
    }

    tMonarch->fHeader = new MonarchHeader();

    tMonarch->fState = eOpen;
    return tMonarch;
};
Monarch* Monarch::OpenForWriting( const string& aFilename )
{
    Monarch* tMonarch = new Monarch();
    tMonarch->fIO = new MonarchIO( sWriteMode );

    if( tMonarch->fIO->Open( aFilename ) == false )
    {
        cout << "could not open <" << aFilename << "> for reading" << endl;
        delete tMonarch;
        return NULL;
    }

    tMonarch->fHeader = new MonarchHeader();

    tMonarch->fState = eOpen;
    return tMonarch;
};

bool Monarch::ReadHeader() const
{
    PreludeType tPrelude = 0;
    if( fIO->Read( &tPrelude ) == false )
    {
        cout << "prelude was not read properly" << endl;
        return false;
    }

    char* tHeaderBuffer = new char[ tPrelude ];
    if( fIO->Read( tHeaderBuffer, tPrelude ) == false )
    {
        cout << "header was not read properly" << endl;
        delete[] tHeaderBuffer;

        return false;
    }
    if( fHeader->DemarshalFromArray( tHeaderBuffer, tPrelude ) == false )
    {
        cout << "header was not demarshalled properly" << endl;
        delete[] tHeaderBuffer;

        return false;
    }
    delete[] tHeaderBuffer;

    fRaw = new char[sizeof(ChIdType) + sizeof(AcqIdType) + sizeof(RecIdType) + sizeof(ClockType) + fHeader->GetRecordSize()];
    fRecord = new( fRaw ) MonarchRecord();
    fState = eReady;

    return true;
}
bool Monarch::WriteHeader()
{
    PreludeType tPrelude = fHeader->ByteSize();
    if( fIO->Write( &tPrelude ) == false )
    {
        cout << "prelude was not read properly" << endl;
        return false;
    }

    char* tHeaderBuffer = new char[ tPrelude ];
    if( fHeader->MarshalToArray( tHeaderBuffer, tPrelude ) == false )
    {
        cout << "header was not marshalled properly" << endl;
        delete[] tHeaderBuffer;

        return false;
    }
    if( fIO->Write( tHeaderBuffer, tPrelude ) == false )
    {
        cout << "header was not written properly" << endl;
        delete[] tHeaderBuffer;

        return false;
    }
    delete[] tHeaderBuffer;

    fRaw = new char[sizeof(ChIdType) + sizeof(AcqIdType) + sizeof(RecIdType) + sizeof(ClockType) + fHeader->GetRecordSize()];
    fRecord = new( fRaw ) MonarchRecord();
    fState = eReady;

    return true;
}


//    if( ret )
//    {
//        // We need an array for the prelude and then another for the header to read in.
//        char prelude[8];
//        (ret->io)->Read( prelude, 8 );
//
//        unsigned int hdr_size;
//        sscanf( (char*) prelude, "%08x", &hdr_size );
//
//        // OK now we should know how big it is...
//        char hdr_buf[hdr_size];
//        (ret->io)->Read( hdr_buf, hdr_size );
//
//        // Now deserialize.
//        ret->hdr = MonarchHeader::FromArray( hdr_buf, hdr_size );
//        if( ret->hdr == NULL )
//        {
//            std::cout << "Header was not read from the file properly!" << std::endl;
//            delete ret;
//            return NULL;
//        }
//
//        // Set the recsize to be correct from the header.  This will be fixed
//        // once the constructor for reading is less goofy.
//        ret->recsize = sizeof(ChIdType) + sizeof(AcqIdType) + sizeof(RecIdType) + sizeof(ClockType) + ret->hdr->GetRecordSize();
//
//        // Allocate memory for the record we keep.
//        ret->AllocateRec( ret->recsize );
//    }
//
//    return ret;
//}

Monarch* Monarch::Open( MonarchHeader& hdr )
{
    Monarch* ret = new Monarch( hdr.GetFilename(), WriteMode );
    if( ret )
    {
        // Now just write the header & prelude on the file.  First we need to know
        // just how big the header is in bytes
        int hdr_size = hdr.ByteSize();

        // Now write the prelude.  Jesus christ this is ugly... not sure what I'm
        // doing wrong here but it seemed the only way to drop the null termination
        // reliably.
        char prelude_buf_tmp[9];
        char prelude_buf[8];
        snprintf( prelude_buf_tmp, 9, "%08x", hdr_size );
        memcpy( prelude_buf, prelude_buf_tmp, 8 );
        (ret->io)->Write( prelude_buf, 8 );

        // Now write the header on the stream.
        char hdr_buf[hdr_size];
        if( hdr.SerializeToArray( hdr_buf, hdr_size ) == false )
        {
            std::cout << "couldn't serialize header!!!" << std::endl;
        }
        (ret->io)->Write( hdr_buf, hdr_size );

        // Cache the size of records.
        ret->recsize = sizeof(ChIdType) + sizeof(AcqIdType) + sizeof(RecIdType) + sizeof(ClockType) + hdr.GetRecordSize();

        // OK, now we are ready for writing.

    }

    return ret;
}



Monarch* Monarch::OpenForWriting( std::string filename )
{
    std::size_t rsize = sizeof(ChIdType) + sizeof(AcqIdType) + sizeof(RecIdType) + sizeof(ClockType) + 1024;
    return new Monarch( filename, WriteMode );
}

bool Monarch::Close()
{
    return (this->io->Close());
}

MonarchRecord* Monarch::NewRecord( std::size_t dsize )
{
    std::size_t rsize = sizeof(ChIdType) + sizeof(AcqIdType) + sizeof(RecIdType) + sizeof(ClockType) + dsize;
    char* mem = new char[rsize]();
    return new ( mem ) MonarchRecord();
}

bool Monarch::WriteRecord( MonarchRecord* newrec )
{
    return (this->io)->Write( (char*) (newrec), this->recsize );
}

MonarchRecord* Monarch::GetNextEvent()
{
    std::size_t res = (this->io)->Read( (char*) (this->rec), this->recsize );
    if( res == this->recsize )
    {
        return (this->rec);
    }
    else
    {
        throw MonarchExceptions::EndOfFile( this->filename );
    }
}

MonarchHeader* Monarch::GetHeader()
{
    return this->hdr;
}
