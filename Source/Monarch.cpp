#include "Monarch.hpp"
#include "MonarchException.hpp"

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
        fSeparateRecordSize( 0 ),
        fRecordSeparateOne( NULL ),
        fRecordSeparateOneBytes( NULL ),
        fRecordSeparateTwo( NULL ),
        fRecordSeparateTwoBytes( NULL ),
        fReadFunction( &Monarch::InterleavedFromInterleaved ),
        fWriteFunction( &Monarch::InterleavedToInterleaved )
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

    if( fRecordSeparateOneBytes != NULL )
    {
        fRecordSeparateOne->~MonarchRecord();
        delete[] fRecordSeparateOneBytes;
        fRecordSeparateOneBytes = NULL;
    }

    if( fRecordSeparateTwoBytes != NULL )
    {
        fRecordSeparateTwo->~MonarchRecord();
        delete[] fRecordSeparateTwoBytes;
        fRecordSeparateTwoBytes = NULL;
    }
}

const Monarch* Monarch::OpenForReading( const string& aFilename )
{
    Monarch* tMonarch = new Monarch();

    tMonarch->fIO = new MonarchIO( sAccessRead );
    if( tMonarch->fIO->Open( aFilename ) == false )
    {
        delete tMonarch;
        throw MonarchException() << "could not open <" << aFilename << "> for reading";
        return NULL;
    }

    tMonarch->fHeader = new MonarchHeader();
    tMonarch->fHeader->SetFilename( aFilename );

    tMonarch->fState = eOpen;
    return tMonarch;
}

Monarch* Monarch::OpenForWriting( const string& aFilename )
{
    Monarch* tMonarch = new Monarch();

    tMonarch->fIO = new MonarchIO( sAccessWrite );
    if( tMonarch->fIO->Open( aFilename ) == false )
    {
        delete tMonarch;
        throw MonarchException() << "could not open <" << aFilename << "> for reading";
        return NULL;
    }

    tMonarch->fHeader = new MonarchHeader();
    tMonarch->fHeader->SetFilename( aFilename );

    tMonarch->fState = eOpen;

    return tMonarch;
}

void Monarch::ReadHeader() const
{
    PreludeType tPrelude = 0;
    if( fIO->Read( &tPrelude ) == false )
    {
        throw MonarchException() << "prelude was not read properly";
        return;
    }

    char* tHeaderBuffer = new char[ tPrelude ];
    if( fIO->Read( tHeaderBuffer, tPrelude ) == false )
    {
        delete[] tHeaderBuffer;
        throw MonarchException() << "header was not read properly";
        return;
    }
    if( fHeader->DemarshalFromArray( tHeaderBuffer, tPrelude ) == false )
    {
        delete[] tHeaderBuffer;
        throw MonarchException() << "header was not demarshalled properly";
        return;
    }
    delete[] tHeaderBuffer;

    if( fHeader->GetFormatMode() == sFormatSingle )
    {
        fDataSize = fHeader->GetLength() * sizeof(DataType);

        fInterleavedRecordSize = sizeof(TimeType) + sizeof(RecordIdType) + sizeof(TimeType) + fDataSize;
        fRecordInterleavedBytes = new char[ fInterleavedRecordSize ];
        fRecordInterleaved = new ( fRecordInterleavedBytes ) MonarchRecord();

        fSeparateRecordSize = sizeof(TimeType) + sizeof(RecordIdType) + sizeof(TimeType) + fDataSize;
        fRecordSeparateOneBytes = new char[ fSeparateRecordSize ];
        fRecordSeparateOne = new ( fRecordSeparateOneBytes ) MonarchRecord();

        //cout << "  *format is <" << sFormatSingle << ">" << endl;
        //cout << "  *data size is <" << fDataSize << ">" << endl;
        //cout << "  *interleaved size is <" << fInterleavedRecordSize << ">" << endl;
        //cout << "  *separate size is <" << fSeparateRecordSize << ">" << endl;

        fReadFunction = &Monarch::SeparateFromSingle;
    }
    if( fHeader->GetFormatMode() == sFormatSeparateDual )
    {
        fDataSize = fHeader->GetLength() * sizeof(DataType);

        fInterleavedRecordSize = sizeof(TimeType) + sizeof(RecordIdType) + sizeof(TimeType) + 2 * fDataSize;
        fRecordInterleavedBytes = new char[ fInterleavedRecordSize ];
        fRecordInterleaved = new ( fRecordInterleavedBytes ) MonarchRecord();

        fSeparateRecordSize = sizeof(TimeType) + sizeof(RecordIdType) + sizeof(TimeType) + fDataSize;
        fRecordSeparateOneBytes = new char[ fSeparateRecordSize ];
        fRecordSeparateOne = new ( fRecordSeparateOneBytes ) MonarchRecord();
        fRecordSeparateTwoBytes = new char[ fSeparateRecordSize ];
        fRecordSeparateTwo = new ( fRecordSeparateTwoBytes ) MonarchRecord();

        //cout << "  *format is <" << sFormatSeparateDual << ">" << endl;
        //cout << "  *data size is <" << fDataSize << ">" << endl;
        //cout << "  *interleaved size is <" << fInterleavedRecordSize << ">" << endl;
        //cout << "  *separate size is <" << fSeparateRecordSize << ">" << endl;

        fReadFunction = &Monarch::SeparateFromSeparate;
    }
    if( fHeader->GetFormatMode() == sFormatInterleavedDual )
    {
        fDataSize = fHeader->GetLength() * sizeof(DataType);

        fInterleavedRecordSize = sizeof(TimeType) + sizeof(RecordIdType) + sizeof(TimeType) + 2 * fDataSize;
        fRecordInterleavedBytes = new char[ fInterleavedRecordSize ];
        fRecordInterleaved = new ( fRecordInterleavedBytes ) MonarchRecord();

        fSeparateRecordSize = sizeof(TimeType) + sizeof(RecordIdType) + sizeof(TimeType) + fDataSize;
        fRecordSeparateOneBytes = new char[ fSeparateRecordSize ];
        fRecordSeparateOne = new ( fRecordSeparateOneBytes ) MonarchRecord();
        fRecordSeparateTwoBytes = new char[ fSeparateRecordSize ];
        fRecordSeparateTwo = new ( fRecordSeparateTwoBytes ) MonarchRecord();

        //cout << "  *format is <" << sFormatInterleavedDual << ">" << endl;
        //cout << "  *data size is <" << fDataSize << ">" << endl;
        //cout << "  *interleaved size is <" << fInterleavedRecordSize << ">" << endl;
        //cout << "  *separate size is <" << fSeparateRecordSize << ">" << endl;

        fReadFunction = &Monarch::InterleavedFromInterleaved;
    }

    fState = eReady;
    return;
}
void Monarch::WriteHeader()
{
    PreludeType tPrelude = fHeader->ByteSize();
    if( fIO->Write( &tPrelude ) == false )
    {
        throw MonarchException() << "prelude was not written properly";
        return;
    }

    char* tHeaderBuffer = new char[ tPrelude ];
    if( fHeader->MarshalToArray( tHeaderBuffer, tPrelude ) == false )
    {
        delete[] tHeaderBuffer;
        throw MonarchException() << "header was not marshalled properly";
        return;
    }
    if( fIO->Write( tHeaderBuffer, tPrelude ) == false )
    {
        delete[] tHeaderBuffer;
        throw MonarchException() << "header was not written properly";
        return;
    }
    delete[] tHeaderBuffer;

    if( fHeader->GetFormatMode() == sFormatSingle )
    {
        fDataSize = fHeader->GetLength() * sizeof(DataType);

        fInterleavedRecordSize = sizeof(TimeType) + sizeof(RecordIdType) + sizeof(TimeType) + fDataSize;
        fRecordInterleavedBytes = new char[ fInterleavedRecordSize ];
        fRecordInterleaved = new ( fRecordInterleavedBytes ) MonarchRecord();

        fSeparateRecordSize = sizeof(TimeType) + sizeof(RecordIdType) + sizeof(TimeType) + fDataSize;
        fRecordSeparateOneBytes = new char[ fSeparateRecordSize ];
        fRecordSeparateOne = new ( fRecordSeparateOneBytes ) MonarchRecord();

        //cout << "  *format is <" << sFormatSingle << ">" << endl;
        //cout << "  *data size is <" << fDataSize << ">" << endl;
        //cout << "  *interleaved size is <" << fInterleavedRecordSize << ">" << endl;
        //cout << "  *separate size is <" << fSeparateRecordSize << ">" << endl;

        fWriteFunction = &Monarch::SeparateToSingle;
    }
    if( fHeader->GetFormatMode() == sFormatSeparateDual )
    {
        fDataSize = fHeader->GetLength() * sizeof(DataType);

        fInterleavedRecordSize = sizeof(TimeType) + sizeof(RecordIdType) + sizeof(TimeType) + 2 * fDataSize;
        fRecordInterleavedBytes = new char[ fInterleavedRecordSize ];
        fRecordInterleaved = new ( fRecordInterleavedBytes ) MonarchRecord();

        fSeparateRecordSize = sizeof(TimeType) + sizeof(RecordIdType) + sizeof(TimeType) + fDataSize;
        fRecordSeparateOneBytes = new char[ fSeparateRecordSize ];
        fRecordSeparateOne = new ( fRecordSeparateOneBytes ) MonarchRecord();
        fRecordSeparateTwoBytes = new char[ fSeparateRecordSize ];
        fRecordSeparateTwo = new ( fRecordSeparateTwoBytes ) MonarchRecord();

        //cout << "  *format is <" << sFormatSeparateDual << ">" << endl;
        //cout << "  *data size is <" << fDataSize << ">" << endl;
        //cout << "  *interleaved size is <" << fInterleavedRecordSize << ">" << endl;
        //cout << "  *separate size is <" << fSeparateRecordSize << ">" << endl;

        fWriteFunction = &Monarch::SeparateToSeparate;
    }
    if( fHeader->GetFormatMode() == sFormatInterleavedDual )
    {
        fDataSize = fHeader->GetLength() * sizeof(DataType);

        fInterleavedRecordSize = sizeof(TimeType) + sizeof(RecordIdType) + sizeof(TimeType) + 2 * fDataSize;
        fRecordInterleavedBytes = new char[ fInterleavedRecordSize ];
        fRecordInterleaved = new ( fRecordInterleavedBytes ) MonarchRecord();

        fSeparateRecordSize = sizeof(TimeType) + sizeof(RecordIdType) + sizeof(TimeType) + fDataSize;
        fRecordSeparateOneBytes = new char[ fSeparateRecordSize ];
        fRecordSeparateOne = new ( fRecordSeparateOneBytes ) MonarchRecord();
        fRecordSeparateTwoBytes = new char[ fSeparateRecordSize ];
        fRecordSeparateTwo = new ( fRecordSeparateTwoBytes ) MonarchRecord();

        //cout << "  *format is <" << sFormatInterleavedDual << ">" << endl;
        //cout << "  *data size is <" << fDataSize << ">" << endl;
        //cout << "  *interleaved size is <" << fInterleavedRecordSize << ">" << endl;
        //cout << "  *separate size is <" << fSeparateRecordSize << ">" << endl;

        fWriteFunction = &Monarch::InterleavedToInterleaved;
    }

    fState = eReady;
    return;
}

void Monarch::SetInterface( InterfaceModeType aMode ) const
{
    if( aMode == sInterfaceInterleaved )
    {
        if( fHeader->GetFormatMode() == sFormatSingle )
        {
            fReadFunction = &Monarch::InterleavedFromSingle;
        }
        if( fHeader->GetFormatMode() == sFormatInterleavedDual )
        {
            fReadFunction = &Monarch::InterleavedFromInterleaved;
        }
        if( fHeader->GetFormatMode() == sFormatSeparateDual )
        {
            fReadFunction = &Monarch::InterleavedFromSeparate;
        }
    }
    if( aMode == sInterfaceSeparate )
    {
        if( fHeader->GetFormatMode() == sFormatSingle )
        {
            fReadFunction = &Monarch::SeparateFromSingle;
        }
        if( fHeader->GetFormatMode() == sFormatInterleavedDual )
        {
            fReadFunction = &Monarch::SeparateFromInterleaved;
        }
        if( fHeader->GetFormatMode() == sFormatSeparateDual )
        {
            fReadFunction = &Monarch::SeparateFromSeparate;
        }
    }
    return;
}
void Monarch::SetInterface( InterfaceModeType aMode )
{
    if( aMode == sInterfaceInterleaved )
    {
        if( fHeader->GetFormatMode() == sFormatSingle )
        {
            fWriteFunction = &Monarch::InterleavedToSingle;
        }
        if( fHeader->GetFormatMode() == sFormatInterleavedDual )
        {
            fWriteFunction = &Monarch::InterleavedToInterleaved;
        }
        if( fHeader->GetFormatMode() == sFormatSeparateDual )
        {
            fWriteFunction = &Monarch::InterleavedToSeparate;
        }
    }
    if( aMode == sInterfaceSeparate )
    {
        if( fHeader->GetFormatMode() == sFormatSingle )
        {
            fWriteFunction = &Monarch::SeparateToSingle;
        }
        if( fHeader->GetFormatMode() == sFormatInterleavedDual )
        {
            fWriteFunction = &Monarch::SeparateToInterleaved;
        }
        if( fHeader->GetFormatMode() == sFormatSeparateDual )
        {
            fWriteFunction = &Monarch::SeparateToSeparate;
        }
    }
    return;
}

bool Monarch::ReadRecord() const
{
    return (this->*fReadFunction)();
}
bool Monarch::InterleavedFromSingle() const
{
    if( fIO->Read( fRecordInterleavedBytes, fInterleavedRecordSize ) == false )
    {
        if( fIO->Done() != true )
        {
            throw MonarchException() << "could not read next single record";
        }
        return false;
    }

    return true;
}
bool Monarch::InterleavedFromSeparate() const
{
    if( fIO->Read( fRecordSeparateOneBytes, fSeparateRecordSize ) == false )
    {
        if( fIO->Done() != true )
        {
            throw MonarchException() << "could not read next channel one record";
        }
        return false;
    }

    if( fIO->Read( fRecordSeparateTwoBytes, fSeparateRecordSize ) == false )
    {
        if( fIO->Done() != true )
        {
            throw MonarchException() << "could not read next channel two record";
        }
        return false;
    }

    fRecordInterleaved->fTime = fRecordSeparateOne->fTime;
    fRecordInterleaved->fRecordId = fRecordSeparateOne->fRecordId;
    fRecordInterleaved->fTime = fRecordSeparateOne->fTime;
    Zip( fDataSize, fRecordSeparateOne->fData, fRecordSeparateTwo->fData, fRecordInterleaved->fData );

    return true;
}
bool Monarch::InterleavedFromInterleaved() const
{
    if( fIO->Read( fRecordInterleavedBytes, fInterleavedRecordSize ) == false )
    {
        if( fIO->Done() != true )
        {
            throw MonarchException() << "could not read next interleaved record";
        }
        return false;
    }

    return true;
}
bool Monarch::SeparateFromSingle() const
{
    if( fIO->Read( fRecordSeparateOneBytes, fSeparateRecordSize ) == false )
    {
        if( fIO->Done() != true )
        {
            throw MonarchException() << "could not read next channel one record";
        }
        return false;
    }

    return true;
}
bool Monarch::SeparateFromSeparate() const
{
    if( fIO->Read( fRecordSeparateOneBytes, fSeparateRecordSize ) == false )
    {
        if( fIO->Done() != true )
        {
            throw MonarchException() << "could not read next channel one record";
        }
        return false;
    }

    if( fIO->Read( fRecordSeparateTwoBytes, fSeparateRecordSize ) == false )
    {
        if( fIO->Done() != true )
        {
            throw MonarchException() << "could not read next channel two record";
        }
        return false;
    }

    return true;
}
bool Monarch::SeparateFromInterleaved() const
{
    if( fIO->Read( fRecordInterleavedBytes, fInterleavedRecordSize ) == false )
    {
        if( fIO->Done() != true )
        {
            throw MonarchException() << "could not read next interleaved record";
        }
        return false;
    }

    fRecordSeparateOne->fTime = fRecordInterleaved->fTime;
    fRecordSeparateTwo->fTime = fRecordInterleaved->fTime;
    fRecordSeparateOne->fRecordId = fRecordInterleaved->fRecordId;
    fRecordSeparateTwo->fRecordId = fRecordInterleaved->fRecordId;
    fRecordSeparateOne->fTime = fRecordInterleaved->fTime;
    fRecordSeparateTwo->fTime = fRecordInterleaved->fTime;
    Unzip( fDataSize, fRecordSeparateOne->fData, fRecordSeparateTwo->fData, fRecordInterleaved->fData );

    return true;
}

bool Monarch::WriteRecord()
{
    return (this->*fWriteFunction)();
}
bool Monarch::InterleavedToSingle()
{
    if( fIO->Write( fRecordInterleavedBytes, fInterleavedRecordSize ) == false )
    {
        throw MonarchException() << "could not write single record";
        return false;
    }

    return true;
}
bool Monarch::InterleavedToSeparate()
{
    fRecordSeparateOne->fTime = fRecordInterleaved->fTime;
    fRecordSeparateTwo->fTime = fRecordInterleaved->fTime;
    fRecordSeparateOne->fRecordId = fRecordInterleaved->fRecordId;
    fRecordSeparateTwo->fRecordId = fRecordInterleaved->fRecordId;
    fRecordSeparateOne->fTime = fRecordInterleaved->fTime;
    fRecordSeparateTwo->fTime = fRecordInterleaved->fTime;
    Unzip( fDataSize, fRecordSeparateOne->fData, fRecordSeparateTwo->fData, fRecordInterleaved->fData );

    if( fIO->Write( fRecordSeparateOneBytes, fSeparateRecordSize ) == false )
    {
        throw MonarchException() << "could not write next channel one record";
        return false;
    }

    if( fIO->Write( fRecordSeparateTwoBytes, fSeparateRecordSize ) == false )
    {
        throw MonarchException() << "could not write next channel two record";
        return false;
    }

    return true;
}
bool Monarch::InterleavedToInterleaved()
{
    if( fIO->Write( fRecordInterleavedBytes, fInterleavedRecordSize ) == false )
    {
        throw MonarchException() << "could not write interleaved record";
        return false;
    }

    return true;
}
bool Monarch::SeparateToSingle()
{
    if( fIO->Write( fRecordSeparateOneBytes, fSeparateRecordSize ) == false )
    {
        throw MonarchException() << "could not write single record";
        return false;
    }

    return true;
}
bool Monarch::SeparateToSeparate()
{
    if( fIO->Write( fRecordSeparateOneBytes, fSeparateRecordSize ) == false )
    {
        throw MonarchException() << "could not write next channel one record";
        return false;
    }

    if( fIO->Write( fRecordSeparateTwoBytes, fSeparateRecordSize ) == false )
    {
        throw MonarchException() << "could not write next channel two record";
        return false;
    }

    return true;
}
bool Monarch::SeparateToInterleaved()
{
    fRecordInterleaved->fTime = fRecordSeparateOne->fTime;
    fRecordInterleaved->fRecordId = fRecordSeparateOne->fRecordId;
    fRecordInterleaved->fTime = fRecordSeparateOne->fTime;
    Zip( fDataSize, fRecordSeparateOne->fData, fRecordSeparateTwo->fData, fRecordInterleaved->fData );

    if( fIO->Write( fRecordInterleavedBytes, fInterleavedRecordSize ) == false )
    {
        throw MonarchException() << "could not write interleaved record";
        return false;
    }

    return true;
}

void Monarch::Close() const
{
    if( fIO->Close() == false )
    {
        throw MonarchException() << "could not close file";
    }
    return;
}
void Monarch::Close()
{
    if( fIO->Close() == false )
    {
        throw MonarchException() << "could not close file";
    }
    return;
}
