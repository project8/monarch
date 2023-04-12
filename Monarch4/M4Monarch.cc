/*
 * M4Monarch.cc
 *
 *  Created on: Apr 12, 2023
 *      Author: N.S. Oblath
 */
#define M4_API_EXPORTS

#include "M4Monarch.hh"

#include "logger.hh"

using std::string;

namespace monarch4
{
    LOGGER( mlog, "MMonarch4" );

    Monarch4::Monarch4() :
            fState( eClosed ),
            fFile( nullptr ),
            fHeader( nullptr ),
            fMutexPtr( new std::mutex() )
    {
    }

    Monarch4::~Monarch4()
    {
        if( fState == eOpenToRead || fState == eReadyToRead) FinishReading();
        if( fState == eOpenToWrite || fState == eReadyToWrite) FinishWriting();

        if( fHeader != nullptr )
        {
            delete fHeader;
            fHeader = nullptr;
        }

        while( ! fStreams.empty() )
        {
            delete fStreams.back();
            fStreams.pop_back();
        }

        if( fFile != nullptr )
        {
            delete fFile;
            fFile = nullptr;
        }
    }

    Monarch4::State Monarch4::GetState() const
    {
        return fState;
    }

    const Monarch4* Monarch4::OpenForReading( const string& aFilename )
    {
        Monarch4* tMonarch4 = new Monarch4();

        try
        {
            tMonarch4->fFile = new H5::H5File( aFilename.c_str(), H5F_ACC_RDONLY );
        }
        catch( H5::Exception& e )
        {
            delete tMonarch4;
            throw M4Exception() << "Could not open <" << aFilename << "> for reading; an H5::Exception was thrown: " << e.getCDetailMsg();
            return nullptr;
        }
        catch( std::exception& e )
        {
            delete tMonarch4;
            throw M4Exception() << "Could not open <" << aFilename << "> for reading; a std::exception was thrown: " << e.what();
            return nullptr;
        }
        if( tMonarch4->fFile == nullptr )
        {
            delete tMonarch4;
            throw M4Exception() << "Could not open <" << aFilename << "> for reading";
            return nullptr;
        }
        LDEBUG( mlog, "Opened egg file <" << aFilename << "> for reading" );

        tMonarch4->fHeader = new M4Header();
        tMonarch4->fHeader->Filename() = aFilename;

        tMonarch4->fState = eOpenToRead;

        return tMonarch4;
    }

    Monarch4* Monarch4::OpenForWriting( const string& aFilename )
    {
        Monarch4* tMonarch4 = new Monarch4();

        try
        {
            tMonarch4->fFile = new H5::H5File( aFilename.c_str(), H5F_ACC_TRUNC );
        }
        catch( H5::Exception& e )
        {
            delete tMonarch4;
            throw M4Exception() << "Could not open <" << aFilename << "> for writing; an H5::Exception was thrown: " << e.getCDetailMsg();
            return nullptr;
        }
        catch( std::exception& e )
        {
            delete tMonarch4;
            throw M4Exception() << "Could not open <" << aFilename << "> for writing; a std::exception was thrown: " << e.what();
            return nullptr;
        }
        if( tMonarch4->fFile == nullptr )
        {
            delete tMonarch4;
            throw M4Exception() << "Could not open <" << aFilename << "> for writing";
            return nullptr;
        }
        LDEBUG( mlog, "Opened egg file <" << aFilename << "> for writing" );

        tMonarch4->fHeader = new M4Header();
        tMonarch4->fHeader->Filename() = aFilename;

        tMonarch4->fState = eOpenToWrite;

        return tMonarch4;
    }

    void Monarch4::ReadHeader() const
    {
        if( fState != eOpenToRead )
        {
            throw M4Exception() << "File not opened to read";
        }

        // Read the header information from the file (run header, plus all stream and channel headers)
        try
        {
            fHeader->ReadFromHDF5( fFile );
        }
        catch( H5::Exception& e )
        {
            throw M4Exception() << "HDF5 error while reading the header:\n\t" << e.getCDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        catch( M4Exception& e )
        {
            throw;
        }


        H5::Group* tStreamsGroup = fHeader->GetStreamsGroup();

        try
        {
            // Create the stream objects based on the configuration from the header
            for( M4Header::M4StreamHeaders::const_iterator streamIt = fHeader->StreamHeaders().begin();
                    streamIt != fHeader->StreamHeaders().end();
                    ++streamIt )
            {
                fStreams.push_back( new M4Stream( *streamIt, tStreamsGroup ) );
                fStreams.back()->SetMutex( fMutexPtr );
            }
        }
        catch( H5::Exception& e )
        {
            throw M4Exception() << "HDF5 error while creating stream objects for reading:\n\t" << e.getDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        catch( M4Exception& e )
        {
            throw;
        }

        fState = eReadyToRead;
        return;
    }

    void Monarch4::WriteHeader()
    {
        if( fState != eOpenToWrite )
        {
            throw M4Exception() << "File not opened to write";
        }

        // Write the header to the file
        // This will create the following groups: run, streams, and channels
        try
        {
            fHeader->WriteToHDF5( fFile );
        }
        catch( H5::Exception& e )
        {
            throw M4Exception() << "HDF5 error while writing header:\n\t" << e.getDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        catch( M4Exception& e )
        {
            throw;
        }

        H5::Group* tStreamsGroup = fHeader->GetStreamsGroup();

        try
        {
            // Create the stream objects based on the configuration from the header
            for( M4Header::M4StreamHeaders::const_iterator streamIt = fHeader->StreamHeaders().begin();
                    streamIt != fHeader->StreamHeaders().end();
                    ++streamIt )
            {
                fStreams.push_back( new M4Stream( *streamIt, tStreamsGroup ) );
                fStreams.back()->SetMutex( fMutexPtr );
            }
        }
        catch( H5::Exception& e )
        {
            throw M4Exception() << "HDF5 error while creating stream objects:\n\t" << e.getDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        catch( M4Exception& e )
        {
            throw;
        }

        fState = eReadyToWrite;
        return;
    }

    void Monarch4::FinishReading() const
    {
        LDEBUG( mlog, "Finishing reading <" << fHeader->Filename() << ">" );
        try
        {
            if( fHeader != nullptr )
            {
                delete fHeader;
                fHeader = nullptr;
            }
            for( std::vector< M4Stream* >::iterator streamIt = fStreams.begin(); streamIt != fStreams.end(); ++streamIt )
            {
                const_cast< const M4Stream* >(*streamIt)->Close();
                delete *streamIt;
                *streamIt = nullptr;
            }
            if( fFile != nullptr )
            {
                fFile->close();
                delete fFile;
                fFile = nullptr;
            }
        }
        catch( H5::Exception& e )
        {
            throw M4Exception() << "Error while closing: " << e.getDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        fState = eClosed;
        return;
    }

    void Monarch4::FinishWriting()
    {
        LINFO( mlog, "Finishing writing <" << fHeader->Filename() << ">" );
        try
        {
            if( fHeader != nullptr )
            {
                delete fHeader;
                fHeader = nullptr;
            }
            for( std::vector< M4Stream* >::iterator streamIt = fStreams.begin(); streamIt != fStreams.end(); ++streamIt )
            {
                (*streamIt)->Close();
                delete *streamIt;
                *streamIt = nullptr;
            }
            if( fFile != nullptr )
            {
                fFile->close();
                delete fFile;
                fFile = nullptr;
            }
            fFile = nullptr;
        }
        catch( H5::Exception& e )
        {
            throw M4Exception() << "Error while closing: " << e.getDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        fState = eClosed;
        return;
    }

}
