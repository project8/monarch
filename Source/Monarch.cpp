#include "Monarch.hpp"

#include <iostream>
using std::cout;
using std::endl;

Monarch::Monarch() :
    fState( eClosed ),
    fIO( NULL ),
    fHeader( NULL ),
    fRecordSize( 0 ),
    fRecordOne( NULL ),
    fRecordOneBytes( NULL ),
    fRecordTwo( NULL ),
    fRecordTwoBytes( NULL ),
    fReadFunction( &Monarch::ReadRecordOne ),
    fWriteFunction( &Monarch::WriteRecordOne )
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

    if( fRecordOneBytes != NULL )
    {
        fRecordOne->~MonarchRecord();
        delete[] fRecordOneBytes;
        fRecordOneBytes = NULL;
    }

    if( fRecordTwoBytes != NULL )
    {
        fRecordTwo->~MonarchRecord();
        delete[] fRecordTwoBytes;
        fRecordTwoBytes = NULL;
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

    if( fHeader->GetAcqMode() == sOneChannel )
    {
        fRecordOneBytes = new char[fRecordSize];
        fRecordOne = new( fRecordOneBytes ) MonarchRecord();

        fRecordTwoBytes = NULL;
        fRecordTwo = NULL;

        fWriteFunction = &Monarch::WriteRecordOne;
        fReadFunction = &Monarch::ReadRecordOne;
    }
    if( fHeader->GetAcqMode() == sTwoChannel )
    {
        fRecordOneBytes = new char[fRecordSize];
        fRecordOne = new( fRecordOneBytes ) MonarchRecord();

        fRecordTwoBytes = new char[fRecordSize];
        fRecordTwo = new( fRecordTwoBytes ) MonarchRecord();

        fWriteFunction = &Monarch::WriteRecordTwo;
        fReadFunction = &Monarch::ReadRecordTwo;
    }

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

    if( fHeader->GetAcqMode() == sOneChannel )
    {
        fRecordOneBytes = new char[fRecordSize];
        fRecordOne = new( fRecordOneBytes ) MonarchRecord();

        fRecordTwoBytes = NULL;
        fRecordTwo = NULL;

        fWriteFunction = &Monarch::WriteRecordOne;
        fReadFunction = &Monarch::ReadRecordOne;
    }
    if( fHeader->GetAcqMode() == sTwoChannel )
    {
        fRecordOneBytes = new char[fRecordSize];
        fRecordOne = new( fRecordOneBytes ) MonarchRecord();

        fRecordTwoBytes = new char[fRecordSize];
        fRecordTwo = new( fRecordTwoBytes ) MonarchRecord();

        fWriteFunction = &Monarch::WriteRecordTwo;
        fReadFunction = &Monarch::ReadRecordTwo;
    }

    fState = eReady;
    return true;
}

bool Monarch::ReadRecord() const
{
    return (this->*fReadFunction)();
}
bool Monarch::ReadRecordOne() const
{
    if( fIO->Read( fRecordOneBytes, fRecordSize ) == false )
    {
        if( fIO->Done() != true )
        {
            cout << "could not read next channel one record" << endl;
        }
        return false;
    }
    return true;
}
bool Monarch::ReadRecordTwo() const
{
    if( fIO->Read( fRecordOneBytes, fRecordSize ) == false )
    {
        if( fIO->Done() != true )
        {
            cout << "could not read next channel one record" << endl;
        }
        return false;
    }
    if( fIO->Read( fRecordTwoBytes, fRecordSize ) == false )
    {
        if( fIO->Done() != true )
        {
            cout << "could not read next channel two record" << endl;
        }
        return false;
    }
    return true;
}

bool Monarch::WriteRecord()
{
    return (this->*fWriteFunction)();
}
bool Monarch::WriteRecordOne()
{
    if( fIO->Write( fRecordOneBytes, fRecordSize ) == false )
    {
        cout << "could not write channel one record" << endl;
        return false;
    }
    return true;
}
bool Monarch::WriteRecordTwo()
{
    if( fIO->Write( fRecordOneBytes, fRecordSize ) == false )
    {
        cout << "could not write channel one record" << endl;
        return false;
    }
    if( fIO->Write( fRecordTwoBytes, fRecordSize ) == false )
    {
        cout << "could not write channel two record" << endl;
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
