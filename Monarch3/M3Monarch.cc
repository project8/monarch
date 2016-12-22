/*
 * M3Monarch.cc
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 */
#define M3_API_EXPORTS

#include "M3Monarch.hh"

#include "logger.hh"

using std::string;

namespace monarch3
{
    LOGGER( mlog, "MMonarch3" );

    Monarch3::Monarch3() :
            fState( eClosed ),
            fFile( nullptr ),
            fHeader( nullptr ),
            fMutexPtr( new std::mutex() )
    {
    }

    Monarch3::~Monarch3()
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

    const Monarch3* Monarch3::OpenForReading( const string& aFilename )
    {
        Monarch3* tMonarch3 = new Monarch3();

        try
        {
            tMonarch3->fFile = new H5::H5File( aFilename.c_str(), H5F_ACC_RDONLY );
        }
        catch( H5::Exception& e )
        {
            delete tMonarch3;
            throw M3Exception() << "Could not open <" << aFilename << "> for reading; an H5::Exception was thrown: " << e.getCDetailMsg();
            return nullptr;
        }
        catch( std::exception& e )
        {
            delete tMonarch3;
            throw M3Exception() << "Could not open <" << aFilename << "> for reading; a std::exception was thrown: " << e.what();
            return nullptr;
        }
        if( tMonarch3->fFile == nullptr )
        {
            delete tMonarch3;
            throw M3Exception() << "Could not open <" << aFilename << "> for reading";
            return nullptr;
        }
        LDEBUG( mlog, "Opened egg file <" << aFilename << "> for reading" );

        tMonarch3->fHeader = new M3Header();
        tMonarch3->fHeader->SetFilename( aFilename );

        tMonarch3->fState = eOpenToRead;

        return tMonarch3;
    }

    Monarch3* Monarch3::OpenForWriting( const string& aFilename )
    {
        Monarch3* tMonarch3 = new Monarch3();

        try
        {
            tMonarch3->fFile = new H5::H5File( aFilename.c_str(), H5F_ACC_TRUNC );
        }
        catch( H5::Exception& e )
        {
            delete tMonarch3;
            throw M3Exception() << "Could not open <" << aFilename << "> for writing; an H5::Exception was thrown: " << e.getCDetailMsg();
            return nullptr;
        }
        catch( std::exception& e )
        {
            delete tMonarch3;
            throw M3Exception() << "Could not open <" << aFilename << "> for writing; a std::exception was thrown: " << e.what();
            return nullptr;
        }
        if( tMonarch3->fFile == nullptr )
        {
            delete tMonarch3;
            throw M3Exception() << "Could not open <" << aFilename << "> for writing";
            return nullptr;
        }
        LDEBUG( mlog, "Opened egg file <" << aFilename << "> for writing" );

        tMonarch3->fHeader = new M3Header();
        tMonarch3->fHeader->SetFilename( aFilename );

        tMonarch3->fState = eOpenToWrite;

        return tMonarch3;
    }

    void Monarch3::ReadHeader() const
    {
        if( fState != eOpenToRead )
        {
            throw M3Exception() << "File not opened to read";
        }

        // Read the header information from the file (run header, plus all stream and channel headers)
        try
        {
            fHeader->ReadFromHDF5( fFile );
        }
        catch( H5::Exception& e )
        {
            throw M3Exception() << "HDF5 error while reading the header:\n\t" << e.getCDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        catch( M3Exception& e )
        {
            throw;
        }


        H5::Group* tStreamsGroup = fHeader->GetStreamsGroup();

        try
        {
            // Create the stream objects based on the configuration from the header
            for( M3Header::M3StreamHeaders::const_iterator streamIt = fHeader->GetStreamHeaders().begin();
                    streamIt != fHeader->GetStreamHeaders().end();
                    ++streamIt )
            {
                fStreams.push_back( new M3Stream( *streamIt, tStreamsGroup ) );
                fStreams.back()->SetMutex( fMutexPtr );
            }
        }
        catch( H5::Exception& e )
        {
            throw M3Exception() << "HDF5 error while creating stream objects for reading:\n\t" << e.getDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        catch( M3Exception& e )
        {
            throw;
        }

        fState = eReadyToRead;
        return;
    }

    void Monarch3::WriteHeader()
    {
        if( fState != eOpenToWrite )
        {
            throw M3Exception() << "File not opened to write";
        }

        // Write the header to the file
        // This will create the following groups: run, streams, and channels
        try
        {
            fHeader->WriteToHDF5( fFile );
        }
        catch( H5::Exception& e )
        {
            throw M3Exception() << "HDF5 error while writing header:\n\t" << e.getDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        catch( M3Exception& e )
        {
            throw;
        }

        H5::Group* tStreamsGroup = fHeader->GetStreamsGroup();

        try
        {
            // Create the stream objects based on the configuration from the header
            for( M3Header::M3StreamHeaders::const_iterator streamIt = fHeader->GetStreamHeaders().begin();
                    streamIt != fHeader->GetStreamHeaders().end();
                    ++streamIt )
            {
                fStreams.push_back( new M3Stream( *streamIt, tStreamsGroup ) );
                fStreams.back()->SetMutex( fMutexPtr );
            }
        }
        catch( H5::Exception& e )
        {
            throw M3Exception() << "HDF5 error while creating stream objects:\n\t" << e.getDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        catch( M3Exception& e )
        {
            throw;
        }

        fState = eReadyToWrite;
        return;
    }

    void Monarch3::FinishReading() const
    {
        LDEBUG( mlog, "Finishing reading" );
        try
        {
            if( fHeader != nullptr )
            {
                delete fHeader;
                fHeader = nullptr;
            }
            for( std::vector< M3Stream* >::iterator streamIt = fStreams.begin(); streamIt != fStreams.end(); ++streamIt )
            {
                const_cast< const M3Stream* >(*streamIt)->Close();
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
            throw M3Exception() << "Error while closing: " << e.getDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        fState = eClosed;
        return;
    }

    void Monarch3::FinishWriting()
    {
        LDEBUG( mlog, "Finishing writing" );
        try
        {
            if( fHeader != nullptr )
            {
                delete fHeader;
                fHeader = nullptr;
            }
            for( std::vector< M3Stream* >::iterator streamIt = fStreams.begin(); streamIt != fStreams.end(); ++streamIt )
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
            throw M3Exception() << "Error while closing: " << e.getDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        fState = eClosed;
        return;
    }

}
