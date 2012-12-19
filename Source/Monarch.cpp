#include "Monarch.hpp"

#include <iostream>
using std::cout;
using std::endl;

Monarch::Monarch() :
    fState( eClosed ),
    fIO( NULL ),
    fHeader( NULL ),
    fInterleavedRecordSize( 0 ),
    fRecordInterleaved( NULL ),
    fRecordInterleavedBytes( NULL ),
    fSplitRecordSize( 0 ),
    fRecordOne( NULL ),
    fRecordOneBytes( NULL ),
    fRecordTwo( NULL ),
    fRecordTwoBytes( NULL ),
    fReadFunction( &Monarch::ReadRecordOne )
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

    if( fRecordInterleavedBytes != NULL )
    {
        fRecordInterleaved->~MonarchRecord();
        delete[] fRecordInterleavedBytes;
        fRecordInterleavedBytes = NULL;
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

    if( fHeader->GetAcqMode() == sOneChannel )
    {
        fInterleavedRecordSize = sizeof(AcqIdType) + sizeof(RecIdType) + sizeof(ClockType) + fHeader->GetRecordSize();
        fSplitRecordSize = sizeof(AcqIdType) + sizeof(RecIdType) + sizeof(ClockType) + fHeader->GetRecordSize();

        fRecordInterleavedBytes = new char[fInterleavedRecordSize];
        fRecordInterleaved = new( fRecordInterleavedBytes ) MonarchRecord();

        fRecordOneBytes = new char[fSplitRecordSize];
        fRecordOne = new( fRecordOneBytes ) MonarchRecord();

        fRecordTwoBytes = NULL;
        fRecordTwo = NULL;

        fReadFunction = &Monarch::ReadRecordOne;
    }
    if( fHeader->GetAcqMode() == sTwoChannel )
    {
        fInterleavedRecordSize = sizeof(AcqIdType) + sizeof(RecIdType) + sizeof(ClockType) + 2 * fHeader->GetRecordSize();
        fSplitRecordSize = sizeof(AcqIdType) + sizeof(RecIdType) + sizeof(ClockType) + fHeader->GetRecordSize();
        fReadFunction = &Monarch::ReadRecordTwo;

        fRecordInterleavedBytes = new char[fInterleavedRecordSize];
        fRecordInterleaved = new( fRecordInterleavedBytes ) MonarchRecord();

        fRecordOneBytes = new char[fSplitRecordSize];
        fRecordOne = new( fRecordOneBytes ) MonarchRecord();

        fRecordTwoBytes = new char[fSplitRecordSize];
        fRecordTwo = new( fRecordTwoBytes ) MonarchRecord();
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

    if( fHeader->GetAcqMode() == sOneChannel )
    {
        fInterleavedRecordSize = sizeof(AcqIdType) + sizeof(RecIdType) + sizeof(ClockType) + fHeader->GetRecordSize();
        fSplitRecordSize = sizeof(AcqIdType) + sizeof(RecIdType) + sizeof(ClockType) + fHeader->GetRecordSize();

        fRecordInterleavedBytes = new char[fInterleavedRecordSize];
        fRecordInterleaved = new( fRecordInterleavedBytes ) MonarchRecord();

        fRecordOneBytes = new char[fSplitRecordSize];
        fRecordOne = new( fRecordOneBytes ) MonarchRecord();

        fRecordTwoBytes = NULL;
        fRecordTwo = NULL;

        fReadFunction = &Monarch::ReadRecordOne;
    }
    if( fHeader->GetAcqMode() == sTwoChannel )
    {
        fInterleavedRecordSize = sizeof(AcqIdType) + sizeof(RecIdType) + sizeof(ClockType) + 2 * fHeader->GetRecordSize();
        fSplitRecordSize = sizeof(AcqIdType) + sizeof(RecIdType) + sizeof(ClockType) + fHeader->GetRecordSize();
        fReadFunction = &Monarch::ReadRecordTwo;

        fRecordInterleavedBytes = new char[fInterleavedRecordSize];
        fRecordInterleaved = new( fRecordInterleavedBytes ) MonarchRecord();

        fRecordOneBytes = new char[fSplitRecordSize];
        fRecordOne = new( fRecordOneBytes ) MonarchRecord();

        fRecordTwoBytes = new char[fSplitRecordSize];
        fRecordTwo = new( fRecordTwoBytes ) MonarchRecord();
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
    if( fIO->Read( fRecordInterleavedBytes, fInterleavedRecordSize ) == false )
    {
        if( fIO->Done() != true )
        {
            cout << "could not read next interleaved record" << endl;
        }
        return false;
    }

    fRecordOne->fAId = fRecordInterleaved->fAId;

    fRecordOne->fRId = fRecordInterleaved->fRId;

    fRecordOne->fTick = fRecordInterleaved->fTick;

    DataType* tRecordInterleavedPtr = fRecordInterleaved->fDataPtr;
    DataType* tRecordOnePtr = fRecordOne->fDataPtr;

    for( unsigned int tIndex = 0; tIndex < fSplitRecordSize; tIndex++ )
    {
        *tRecordInterleavedPtr = *tRecordOnePtr;
        tRecordOnePtr++;
        tRecordInterleavedPtr++;
    }

    return true;
}
bool Monarch::ReadRecordTwo() const
{
    if( fIO->Read( fRecordInterleavedBytes, fInterleavedRecordSize ) == false )
    {
        if( fIO->Done() != true )
        {
            cout << "could not read next interleaved record" << endl;
        }
        return false;
    }

    fRecordOne->fAId = fRecordInterleaved->fAId;
    fRecordTwo->fAId = fRecordInterleaved->fAId;

    fRecordOne->fRId = fRecordInterleaved->fRId;
    fRecordTwo->fRId = fRecordInterleaved->fRId;

    fRecordOne->fTick = fRecordInterleaved->fTick;
    fRecordTwo->fTick = fRecordInterleaved->fTick;

    DataType* tRecordInterleavedPtr = fRecordInterleaved->fDataPtr;
    DataType* tRecordOnePtr = fRecordOne->fDataPtr;
    DataType* tRecordTwoPtr = fRecordTwo->fDataPtr;

    for( unsigned int tIndex = 0; tIndex < fSplitRecordSize; tIndex++ )
    {
        *tRecordInterleavedPtr = *tRecordOnePtr;
        tRecordOnePtr++;
        tRecordInterleavedPtr++;

        *tRecordInterleavedPtr = *tRecordTwoPtr;
        tRecordTwoPtr++;
        tRecordInterleavedPtr++;
    }

    return true;
}

bool Monarch::WriteRecord()
{
    if( fIO->Write( fRecordInterleavedBytes, fInterleavedRecordSize ) == false )
    {
        cout << "could not write interleaved record" << endl;
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