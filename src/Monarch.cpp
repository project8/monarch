#include "Monarch.hpp"

#include <iostream>
using std::cout;
using std::endl;

Monarch::Monarch() :
    fState( eClosed ),
    fIO( NULL ),
    fHeader( NULL ),
    fRecord( NULL ),
    fRecordBytes( NULL ),
    fRecordSize( 0 )
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

    if( fRecordBytes != NULL )
    {
        fRecord->~MonarchRecord();
        delete[] fRecordBytes;
        fRecordBytes = NULL;
        fRecordSize = 0;
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
    tMonarch->fHeader->SetFilename( aFilename );

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
    tMonarch->fHeader->SetFilename( aFilename );

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

    fRecordSize = sizeof(ChIdType) + sizeof(AcqIdType) + sizeof(RecIdType) + sizeof(ClockType) + fHeader->GetRecordSize();
    fRecordBytes = new char[fRecordSize];
    fRecord = new( fRecordBytes ) MonarchRecord();

    fState = eReady;
    return true;
}
bool Monarch::WriteHeader()
{
    PreludeType tPrelude = fHeader->ByteSize();
    if( fIO->Write( &tPrelude ) == false )
    {
        cout << "prelude was not written properly" << endl;
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

    fRecordSize = sizeof(ChIdType) + sizeof(AcqIdType) + sizeof(RecIdType) + sizeof(ClockType) + fHeader->GetRecordSize();
    fRecordBytes = new char[fRecordSize];
    fRecord = new( fRecordBytes ) MonarchRecord();

    fState = eReady;
    return true;
}

bool Monarch::ReadRecord() const
{
    if( fIO->Read( fRecordBytes, fRecordSize ) == false )
    {
        cout << "could not read next record" << endl;
        return false;
    }
    return true;
}
bool Monarch::WriteRecord()
{
    if( fIO->Write( fRecordBytes, fRecordSize ) == false )
    {
        cout << "could not write record" << endl;
        return false;
    }
    return true;
}

bool Monarch::Close() const
{
    if( fIO->Close() == false )
    {
        cout << "could not close file" << endl;
        return false;
    }
    return true;
}
bool Monarch::Close()
{
    if( fIO->Close() == false )
    {
        cout << "could not close file" << endl;
        return false;
    }
    return true;
}
