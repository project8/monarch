#include "M2Monarch.hh"
#include "M2Exception.hh"

namespace monarch2
{

    Monarch2::Monarch2() :
                fState( eClosed ),
                fIO( NULL ),
                fHeader( NULL ),
                fDataTypeSize( 1 ),
                fDataNBytes( 0 ),
                fDataSize( 0 ),
                fInterleavedRecordNBytes( 0 ),
                fRecordInterleaved( NULL ),
                fRecordInterleavedBytes( NULL ),
                fSeparateRecordNBytes( 0 ),
                fRecordSeparateOne( NULL ),
                fRecordSeparateOneBytes( NULL ),
                fRecordSeparateTwo( NULL ),
                fRecordSeparateTwoBytes( NULL ),
                fReadFunction( &Monarch2::InterleavedFromInterleaved ),
                fWriteFunction( &Monarch2::InterleavedToInterleaved )
    {
    }
    Monarch2::~Monarch2()
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
            fRecordInterleaved->~M2RecordBytes();
            delete[] fRecordInterleavedBytes;
            fRecordInterleavedBytes = NULL;
        }

        if( fRecordSeparateOneBytes != NULL )
        {
            fRecordSeparateOne->~M2RecordBytes();
            delete[] fRecordSeparateOneBytes;
            fRecordSeparateOneBytes = NULL;
        }

        if( fRecordSeparateTwoBytes != NULL )
        {
            fRecordSeparateTwo->~M2RecordBytes();
            delete[] fRecordSeparateTwoBytes;
            fRecordSeparateTwoBytes = NULL;
        }
    }

    const Monarch2* Monarch2::OpenForReading( const std::string& aFilename )
    {
        Monarch2* tMonarch = new Monarch2();

        tMonarch->fIO = new M2IO( sAccessRead );
        if( tMonarch->fIO->Open( aFilename ) == false )
        {
            delete tMonarch;
            throw M2Exception() << "could not open <" << aFilename << "> for reading";
            return NULL;
        }

        tMonarch->fHeader = new M2Header();
        tMonarch->fHeader->SetFilename( aFilename );

        tMonarch->fState = eOpen;

        return tMonarch;
    }

    Monarch2* Monarch2::OpenForWriting( const std::string& aFilename )
    {
        Monarch2* tMonarch = new Monarch2();

        tMonarch->fIO = new M2IO( sAccessWrite );
        if( tMonarch->fIO->Open( aFilename ) == false )
        {
            delete tMonarch;
            throw M2Exception() << "could not open <" << aFilename << "> for writing";
            return NULL;
        }

        tMonarch->fHeader = new M2Header();
        tMonarch->fHeader->SetFilename( aFilename );

        tMonarch->fState = eOpen;

        return tMonarch;
    }

    void Monarch2::ReadHeader() const
    {
        PreludeType tPrelude = 0;
        if( fIO->Read( &tPrelude ) == false )
        {
            throw M2Exception() << "prelude was not read properly";
            return;
        }

        char* tHeaderBuffer = new char[ tPrelude ];
        if( fIO->Read( tHeaderBuffer, tPrelude ) == false )
        {
            delete[] tHeaderBuffer;
            throw M2Exception() << "header was not read properly";
            return;
        }
        if( fHeader->DemarshalFromArray( tHeaderBuffer, tPrelude ) == false )
        {
            delete[] tHeaderBuffer;
            throw M2Exception() << "header was not demarshalled properly";
            return;
        }
        delete[] tHeaderBuffer;

        fDataTypeSize = fHeader->GetDataTypeSize();

        if( fHeader->GetAcquisitionMode() == 1 /* the FormatMode is ignored for single-channel data */ )
        {
            fDataSize = fHeader->GetRecordSize();
            fDataNBytes = fDataSize * fDataTypeSize;

            fInterleavedRecordNBytes = sizeof(AcquisitionIdType) + sizeof(RecordIdType) + sizeof(TimeType) + fDataNBytes;
            fRecordInterleavedBytes = new byte_type[ fInterleavedRecordNBytes ];
            fRecordInterleaved = new ( fRecordInterleavedBytes ) M2RecordBytes();

            fSeparateRecordNBytes = sizeof(AcquisitionIdType) + sizeof(RecordIdType) + sizeof(TimeType) + fDataNBytes;
            fRecordSeparateOneBytes = new byte_type[ fSeparateRecordNBytes ];
            fRecordSeparateOne = new ( fRecordSeparateOneBytes ) M2RecordBytes();

            //cout << "  *format is <" << sFormatSingle << ">" << endl;
            //cout << "  *data size is <" << fDataSize << ">" << endl;
            //cout << "  *interleaved size is <" << fInterleavedRecordSize << ">" << endl;
            //cout << "  *separate size is <" << fSeparateRecordSize << ">" << endl;

            fReadFunction = &Monarch2::SeparateFromSingle;
        }
        else if( fHeader->GetAcquisitionMode() == 2 && fHeader->GetFormatMode() == sFormatMultiSeparate )
        {
            fDataSize = fHeader->GetRecordSize();
            fDataNBytes = fDataSize * fDataTypeSize;

            fInterleavedRecordNBytes = sizeof(AcquisitionIdType) + sizeof(RecordIdType) + sizeof(TimeType) + 2 * fDataNBytes;
            fRecordInterleavedBytes = new byte_type[ fInterleavedRecordNBytes ];
            fRecordInterleaved = new ( fRecordInterleavedBytes ) M2RecordBytes();

            fSeparateRecordNBytes = sizeof(AcquisitionIdType) + sizeof(RecordIdType) + sizeof(TimeType) + (size_t)fDataNBytes;
            fRecordSeparateOneBytes = new byte_type[ fSeparateRecordNBytes ];
            fRecordSeparateOne = new ( fRecordSeparateOneBytes ) M2RecordBytes();
            fRecordSeparateTwoBytes = new byte_type[ fSeparateRecordNBytes ];
            fRecordSeparateTwo = new ( fRecordSeparateTwoBytes ) M2RecordBytes();

            //cout << "  *format is <" << sFormatSeparateDual << ">" << endl;
            //cout << "  *data size is <" << fDataSize << ">" << endl;
            //cout << "  *interleaved size is <" << fInterleavedRecordSize << ">" << endl;
            //cout << "  *separate size is <" << fSeparateRecordSize << ">" << endl;

            fReadFunction = &Monarch2::SeparateFromSeparate;
        }
        else if( fHeader->GetAcquisitionMode() == 2 && fHeader->GetFormatMode() == sFormatMultiInterleaved )
        {
            fDataSize = fHeader->GetRecordSize();
            fDataNBytes = fDataSize * fDataTypeSize;

            fInterleavedRecordNBytes = sizeof(AcquisitionIdType) + sizeof(RecordIdType) + sizeof(TimeType) + 2 * fDataNBytes;
            fRecordInterleavedBytes = new byte_type[ fInterleavedRecordNBytes ];
            fRecordInterleaved = new ( fRecordInterleavedBytes ) M2RecordBytes();

            fSeparateRecordNBytes = sizeof(AcquisitionIdType) + sizeof(RecordIdType) + sizeof(TimeType) + fDataNBytes;
            fRecordSeparateOneBytes = new byte_type[ fSeparateRecordNBytes ];
            fRecordSeparateOne = new ( fRecordSeparateOneBytes ) M2RecordBytes();
            fRecordSeparateTwoBytes = new byte_type[ fSeparateRecordNBytes ];
            fRecordSeparateTwo = new ( fRecordSeparateTwoBytes ) M2RecordBytes();

            //cout << "  *format is <" << sFormatInterleavedDual << ">" << endl;
            //cout << "  *data size is <" << fDataSize << ">" << endl;
            //cout << "  *interleaved size is <" << fInterleavedRecordSize << ">" << endl;
            //cout << "  *separate size is <" << fSeparateRecordSize << ">" << endl;

            fReadFunction = &Monarch2::SeparateFromInterleaved;
        }
        else
        {
            throw M2Exception() << "Unable to read a header with acquisition mode <" << fHeader->GetAcquisitionMode() << "> and format mode <" << fHeader->GetFormatMode() << ">";
            return;
        }

        fState = eReady;
        return;
    }

    void Monarch2::WriteHeader()
    {
        PreludeType tPrelude = fHeader->ByteSize();
        if( fIO->Write( &tPrelude ) == false )
        {
            throw M2Exception() << "prelude was not written properly";
            return;
        }

        char* tHeaderBuffer = new char[ tPrelude ];
        if( fHeader->MarshalToArray( tHeaderBuffer, tPrelude ) == false )
        {
            delete[] tHeaderBuffer;
            throw M2Exception() << "header was not marshalled properly";
            return;
        }
        if( fIO->Write( tHeaderBuffer, tPrelude ) == false )
        {
            delete[] tHeaderBuffer;
            throw M2Exception() << "header was not written properly";
            return;
        }
        delete[] tHeaderBuffer;

        fDataTypeSize = fHeader->GetDataTypeSize();

        if( fHeader->GetAcquisitionMode() == 1 /* the FormatMode is ignored for single-channel data */ )
        {
            fDataSize = fHeader->GetRecordSize();
            fDataNBytes = fDataSize * fDataTypeSize;

            fInterleavedRecordNBytes = sizeof(AcquisitionIdType) + sizeof(RecordIdType) + sizeof(TimeType) + fDataNBytes;
            fRecordInterleavedBytes = new byte_type[ fInterleavedRecordNBytes ];
            fRecordInterleaved = new ( fRecordInterleavedBytes ) M2RecordBytes();

            fSeparateRecordNBytes = sizeof(AcquisitionIdType) + sizeof(RecordIdType) + sizeof(TimeType) + fDataNBytes;
            fRecordSeparateOneBytes = new byte_type[ fSeparateRecordNBytes ];
            fRecordSeparateOne = new ( fRecordSeparateOneBytes ) M2RecordBytes();

            //cout << "  *format is <" << sFormatSingle << ">" << endl;
            //cout << "  *data type size is <" << fDataTypeSize << ">" << endl;
            //cout << "  *data size is <" << fDataSize << "> and # of data bytes is <" << fDataNBytes << ">" << endl;
            //cout << "  *interleaved # of bytes is <" << fInterleavedRecordNBytes << ">" << endl;
            //cout << "  *separate # of bytes is <" << fSeparateRecordNBytes << ">" << endl;

            fWriteFunction = &Monarch2::SeparateToSingle;
        }
        else if( fHeader->GetAcquisitionMode() == 2 && fHeader->GetFormatMode() == sFormatMultiSeparate )
        {
            fDataSize = fHeader->GetRecordSize();
            fDataNBytes = fDataSize * fDataTypeSize;

            fInterleavedRecordNBytes = sizeof(AcquisitionIdType) + sizeof(RecordIdType) + sizeof(TimeType) + 2 * fDataNBytes;
            fRecordInterleavedBytes = new byte_type[ fInterleavedRecordNBytes ];
            fRecordInterleaved = new ( fRecordInterleavedBytes ) M2RecordBytes();

            fSeparateRecordNBytes = sizeof(AcquisitionIdType) + sizeof(RecordIdType) + sizeof(TimeType) + fDataNBytes;
            fRecordSeparateOneBytes = new byte_type[ fSeparateRecordNBytes ];
            fRecordSeparateOne = new ( fRecordSeparateOneBytes ) M2RecordBytes();
            fRecordSeparateTwoBytes = new byte_type[ fSeparateRecordNBytes ];
            fRecordSeparateTwo = new ( fRecordSeparateTwoBytes ) M2RecordBytes();

            //cout << "  *format is <" << sFormatMultiSeparate << ">" << endl;
            //cout << "  *data size is <" << fDataSize << "> and # of data bytes is <" << fDataNBytes << ">" << endl;
            //cout << "  *interleaved # of bytes is <" << fInterleavedRecordNBytes << ">" << endl;
            //cout << "  *separate # of bytes is <" << fSeparateRecordNBytes << ">" << endl;

            fWriteFunction = &Monarch2::SeparateToSeparate;
        }
        else if( fHeader->GetAcquisitionMode() == 2 && fHeader->GetFormatMode() == sFormatMultiInterleaved )
        {
            fDataSize = fHeader->GetRecordSize();
            fDataNBytes = fDataSize * fDataTypeSize;

            fInterleavedRecordNBytes = sizeof(AcquisitionIdType) + sizeof(RecordIdType) + sizeof(TimeType) + 2 * fDataNBytes;
            fRecordInterleavedBytes = new byte_type[ fInterleavedRecordNBytes ];
            fRecordInterleaved = new ( fRecordInterleavedBytes ) M2RecordBytes();

            fSeparateRecordNBytes = sizeof(AcquisitionIdType) + sizeof(RecordIdType) + sizeof(TimeType) + fDataNBytes;
            fRecordSeparateOneBytes = new byte_type[ fSeparateRecordNBytes ];
            fRecordSeparateOne = new ( fRecordSeparateOneBytes ) M2RecordBytes();
            fRecordSeparateTwoBytes = new byte_type[ fSeparateRecordNBytes ];
            fRecordSeparateTwo = new ( fRecordSeparateTwoBytes ) M2RecordBytes();

            //cout << "  *format is <" << sFormatMultiInterleaved << ">" << endl;
            //cout << "  *data size is <" << fDataSize << "> and # of data bytes is <" << fDataNBytes << ">" << endl;
            //cout << "  *interleaved # of bytes is <" << fInterleavedRecordNBytes << ">" << endl;
            //cout << "  *separate # of bytes is <" << fSeparateRecordNBytes << ">" << endl;

            fWriteFunction = &Monarch2::InterleavedToInterleaved;
        }
        else
        {
            throw M2Exception() << "unable to write a header with acquisition mode <" << fHeader->GetAcquisitionMode() << "> and format mode <" << fHeader->GetFormatMode() << ">";
            return;
        }

        fState = eReady;
        return;
    }

    void Monarch2::SetInterface( InterfaceModeType aMode ) const
    {
        if( aMode == sInterfaceInterleaved )
        {
            if( fHeader->GetAcquisitionMode() == 1 /* the FormatMode is ignored for single-channel data */ )
            {
                fReadFunction = &Monarch2::InterleavedFromSingle;
            }
            else if( fHeader->GetAcquisitionMode() == 2 && fHeader->GetFormatMode() == sFormatMultiInterleaved )
            {
                fReadFunction = &Monarch2::InterleavedFromInterleaved;
            }
            else if( fHeader->GetAcquisitionMode() == 2 && fHeader->GetFormatMode() == sFormatMultiSeparate )
            {
                fReadFunction = &Monarch2::InterleavedFromSeparate;
            }
        }
        if( aMode == sInterfaceSeparate )
        {
            if( fHeader->GetAcquisitionMode() == 1 /* the FormatMode is ignored for single-channel data */ )
            {
                fReadFunction = &Monarch2::SeparateFromSingle;
            }
            else if( fHeader->GetAcquisitionMode() == 2 && fHeader->GetFormatMode() == sFormatMultiInterleaved )
            {
                fReadFunction = &Monarch2::SeparateFromInterleaved;
            }
            else if( fHeader->GetAcquisitionMode() == 2 && fHeader->GetFormatMode() == sFormatMultiSeparate )
            {
                fReadFunction = &Monarch2::SeparateFromSeparate;
            }
        }
        return;
    }

    void Monarch2::SetInterface( InterfaceModeType aMode )
    {
        if( aMode == sInterfaceInterleaved )
        {
            if( fHeader->GetAcquisitionMode() == 1 /* the FormatMode is ignored for single-channel data */ )
            {
                fWriteFunction = &Monarch2::InterleavedToSingle;
            }
            else if( fHeader->GetAcquisitionMode() == 2 && fHeader->GetFormatMode() == sFormatMultiInterleaved )
            {
                fWriteFunction = &Monarch2::InterleavedToInterleaved;
            }
            else if( fHeader->GetAcquisitionMode() == 2 && fHeader->GetFormatMode() == sFormatMultiSeparate )
            {
                fWriteFunction = &Monarch2::InterleavedToSeparate;
            }
        }
        if( aMode == sInterfaceSeparate )
        {
            if( fHeader->GetAcquisitionMode() == 1 /* the FormatMode is ignored for single-channel data */ )
            {
                fWriteFunction = &Monarch2::SeparateToSingle;
            }
            else if( fHeader->GetAcquisitionMode() == 2 && fHeader->GetFormatMode() == sFormatMultiInterleaved )
            {
                fWriteFunction = &Monarch2::SeparateToInterleaved;
            }
            else if( fHeader->GetAcquisitionMode() == 2 && fHeader->GetFormatMode() == sFormatMultiSeparate )
            {
                fWriteFunction = &Monarch2::SeparateToSeparate;
            }
        }
        return;
    }

    bool Monarch2::ReadRecord( int anOffset ) const
    {
        return (this->*fReadFunction)( anOffset );
    }

    bool Monarch2::InterleavedFromSingle( int anOffset ) const
    {
        if( anOffset != 0 )
        {
            long int aByteOffset = anOffset * fInterleavedRecordNBytes;
            if( fIO->Seek( aByteOffset ) == false )
            {
                if( fIO->Done() != true )
                {
                    std::cout << "could not seek to requested position" << std::endl;
                }
                return false;
            }
        }

        if( fIO->Read( fRecordInterleavedBytes, fInterleavedRecordNBytes ) == false )
        {
            if( fIO->Done() != true )
            {
                throw M2Exception() << "could not read next single record";
            }
            return false;
        }

        return true;
    }

    bool Monarch2::InterleavedFromSeparate( int anOffset ) const
    {
        if( anOffset != 0 )
        {
            long int aByteOffset = anOffset * 2 * fSeparateRecordNBytes;
            if( fIO->Seek( aByteOffset ) == false )
            {
                if( fIO->Done() != true )
                {
                    std::cout << "could not seek to requested position" << std::endl;
                }
                return false;
            }
        }

        if( fIO->Read( fRecordSeparateOneBytes, fSeparateRecordNBytes ) == false )
        {
            if( fIO->Done() != true )
            {
                throw M2Exception() << "could not read next channel one record";
            }
            return false;
        }

        if( fIO->Read( fRecordSeparateTwoBytes, fSeparateRecordNBytes ) == false )
        {
            if( fIO->Done() != true )
            {
                throw M2Exception() << "could not read next channel two record";
            }
            return false;
        }

        fRecordInterleaved->fTime = fRecordSeparateOne->fTime;
        fRecordInterleaved->fRecordId = fRecordSeparateOne->fRecordId;
        fRecordInterleaved->fTime = fRecordSeparateOne->fTime;
        Zip( fDataSize, fDataTypeSize, fRecordSeparateOne->fData, fRecordSeparateTwo->fData, fRecordInterleaved->fData );

        return true;
    }

    bool Monarch2::InterleavedFromInterleaved( int anOffset ) const
    {
        if( anOffset != 0 )
        {
            long int aByteOffset = anOffset * fInterleavedRecordNBytes;
            if( fIO->Seek( aByteOffset ) == false )
            {
                if( fIO->Done() != true )
                {
                    std::cout << "could not seek to requested position" << std::endl;
                }
                return false;
            }
        }

        if( fIO->Read( fRecordInterleavedBytes, fInterleavedRecordNBytes ) == false )
        {
            if( fIO->Done() != true )
            {
                throw M2Exception() << "could not read next interleaved record";
            }
            return false;
        }

        return true;
    }

    bool Monarch2::SeparateFromSingle( int anOffset ) const
    {
        if( anOffset != 0 )
        {
            long int aByteOffset = anOffset * fSeparateRecordNBytes;
            if( fIO->Seek( aByteOffset ) == false )
            {
                if( fIO->Done() != true )
                {
                    std::cout << "could not seek to requested position" << std::endl;
                }
                return false;
            }
        }

        if( fIO->Read( fRecordSeparateOneBytes, fSeparateRecordNBytes ) == false )
        {
            if( fIO->Done() != true )
            {
                throw M2Exception() << "could not read next channel one record";
            }
            return false;
        }

        return true;
    }

    bool Monarch2::SeparateFromSeparate( int anOffset ) const
    {
        if( anOffset != 0 )
        {
            long int aByteOffset = anOffset * 2 * fSeparateRecordNBytes;
            if( fIO->Seek( aByteOffset ) == false )
            {
                if( fIO->Done() != true )
                {
                    std::cout << "could not seek to requested position" << std::endl;
                }
                return false;
            }
        }

        if( fIO->Read( fRecordSeparateOneBytes, fSeparateRecordNBytes ) == false )
        {
            if( fIO->Done() != true )
            {
                throw M2Exception() << "could not read next channel one record";
            }
            return false;
        }

        if( fIO->Read( fRecordSeparateTwoBytes, fSeparateRecordNBytes ) == false )
        {
            if( fIO->Done() != true )
            {
                throw M2Exception() << "could not read next channel two record";
            }
            return false;
        }

        return true;
    }

    bool Monarch2::SeparateFromInterleaved( int anOffset ) const
    {
        if( anOffset != 0 )
        {
            long int aByteOffset = anOffset * fInterleavedRecordNBytes;
            if( fIO->Seek( aByteOffset ) == false )
            {
                if( fIO->Done() != true )
                {
                    std::cout << "could not seek to requested position" << std::endl;
                }
                return false;
            }
        }

        if( fIO->Read( fRecordInterleavedBytes, fInterleavedRecordNBytes ) == false )
        {
            if( fIO->Done() != true )
            {
                throw M2Exception() << "could not read next interleaved record";
            }
            return false;
        }

        fRecordSeparateOne->fTime = fRecordInterleaved->fTime;
        fRecordSeparateTwo->fTime = fRecordInterleaved->fTime;
        fRecordSeparateOne->fRecordId = fRecordInterleaved->fRecordId;
        fRecordSeparateTwo->fRecordId = fRecordInterleaved->fRecordId;
        fRecordSeparateOne->fTime = fRecordInterleaved->fTime;
        fRecordSeparateTwo->fTime = fRecordInterleaved->fTime;
        Unzip( fDataSize, fDataTypeSize, fRecordSeparateOne->fData, fRecordSeparateTwo->fData, fRecordInterleaved->fData );

        return true;
    }

    bool Monarch2::WriteRecord()
    {
        return (this->*fWriteFunction)();
    }

    bool Monarch2::InterleavedToSingle()
    {
        if( fIO->Write( fRecordInterleavedBytes, fInterleavedRecordNBytes ) == false )
        {
            throw M2Exception() << "could not write single record";
            return false;
        }

        return true;
    }

    bool Monarch2::InterleavedToSeparate()
    {
        fRecordSeparateOne->fTime = fRecordInterleaved->fTime;
        fRecordSeparateTwo->fTime = fRecordInterleaved->fTime;
        fRecordSeparateOne->fRecordId = fRecordInterleaved->fRecordId;
        fRecordSeparateTwo->fRecordId = fRecordInterleaved->fRecordId;
        Unzip( fDataSize, fDataTypeSize, fRecordSeparateOne->fData, fRecordSeparateTwo->fData, fRecordInterleaved->fData );

        if( fIO->Write( fRecordSeparateOneBytes, fSeparateRecordNBytes ) == false )
        {
            throw M2Exception() << "could not write next channel one record";
            return false;
        }

        if( fIO->Write( fRecordSeparateTwoBytes, fSeparateRecordNBytes ) == false )
        {
            throw M2Exception() << "could not write next channel two record";
            return false;
        }

        return true;
    }

    bool Monarch2::InterleavedToInterleaved()
    {
        if( fIO->Write( fRecordInterleavedBytes, fInterleavedRecordNBytes ) == false )
        {
            throw M2Exception() << "could not write interleaved record";
            return false;
        }

        return true;
    }

    bool Monarch2::SeparateToSingle()
    {
        if( fIO->Write( fRecordSeparateOneBytes, fSeparateRecordNBytes ) == false )
        {
            throw M2Exception() << "could not write single record";
            return false;
        }

        return true;
    }

    bool Monarch2::SeparateToSeparate()
    {
        if( fIO->Write( fRecordSeparateOneBytes, fSeparateRecordNBytes ) == false )
        {
            throw M2Exception() << "could not write next channel one record";
            return false;
        }

        if( fIO->Write( fRecordSeparateTwoBytes, fSeparateRecordNBytes ) == false )
        {
            throw M2Exception() << "could not write next channel two record";
            return false;
        }

        return true;
    }

    bool Monarch2::SeparateToInterleaved()
    {
        fRecordInterleaved->fTime = fRecordSeparateOne->fTime;
        fRecordInterleaved->fRecordId = fRecordSeparateOne->fRecordId;
        fRecordInterleaved->fTime = fRecordSeparateOne->fTime;
        Zip( fDataSize, fDataTypeSize, fRecordSeparateOne->fData, fRecordSeparateTwo->fData, fRecordInterleaved->fData );

        if( fIO->Write( fRecordInterleavedBytes, fInterleavedRecordNBytes ) == false )
        {
            throw M2Exception() << "could not write interleaved record";
            return false;
        }

        return true;
    }

    void Monarch2::Close() const
    {
        if( fIO->Close() == false )
        {
            throw M2Exception() << "could not close file";
        }
        return;
    }

    void Monarch2::Close()
    {
        if( fIO->Close() == false )
        {
            throw M2Exception() << "could not close file";
        }
        return;
    }

}
