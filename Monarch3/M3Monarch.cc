/*
 * MMonarch.cc
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 */

#include "MMonarch.hh"

#include "MLogger.hh"

using std::string;

namespace monarch
{
    MLOGGER( mlog, "MMonarch" );

    Monarch::Monarch() :
                fState( eClosed ),
                fFile( NULL ),
                fHeader( NULL )
    {
        // Turn of HDF5 automatic exception printing to allow exceptions to be handled properly
        // And they will be handled properly, always, right????
        H5::Exception::dontPrint();
    }

    Monarch::~Monarch()
    {
        if( fHeader != NULL )
        {
            delete fHeader;
            fHeader = NULL;
        }

        while( ! fStreams.empty() )
        {
            delete fStreams.back();
            fStreams.pop_back();
        }
    }

    const Monarch* Monarch::OpenForReading( const string& aFilename )
    {
        Monarch* tMonarch = new Monarch();

        tMonarch->fFile = new H5::H5File( aFilename.c_str(), H5F_ACC_RDONLY );
        if( tMonarch->fFile == NULL )
        {
            delete tMonarch;
            throw MException() << "Could not open <" << aFilename << "> for reading";
            return NULL;
        }
        MDEBUG( mlog, "Opened egg file <" << aFilename << "> for reading" );

        tMonarch->fHeader = new MHeader();
        tMonarch->fHeader->SetFilename( aFilename );

        tMonarch->fState = eOpen;

        return tMonarch;
    }

    Monarch* Monarch::OpenForWriting( const string& aFilename )
    {
        Monarch* tMonarch = new Monarch();

        tMonarch->fFile = new H5::H5File( aFilename.c_str(), H5F_ACC_TRUNC );
        if( tMonarch->fFile == NULL )
        {
            delete tMonarch;
            throw MException() << "Could not open <" << aFilename << "> for writing";
            return NULL;
        }
        MDEBUG( mlog, "Opened egg file <" << aFilename << "> for writing" );

        tMonarch->fHeader = new MHeader();
        tMonarch->fHeader->SetFilename( aFilename );

        tMonarch->fState = eOpen;

        return tMonarch;
    }

    void Monarch::ReadHeader() const
    {
        // Read the header information from the file (run header, plus all stream and channel headers)
        try
        {
            fHeader->ReadFromHDF5( fFile );
        }
        catch( H5::Exception& e )
        {
            throw MException() << "HDF5 error while reading the header:\n\t" << e.getCDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        catch( MException& e )
        {
            throw( e );
        }


        H5::Group* tStreamsGroup = fHeader->GetStreamsGroup();

        try
        {
            // Create the stream objects based on the configuration from the header
            for( MHeader::MStreamHeaders::const_iterator streamIt = fHeader->GetStreamHeaders().begin();
                    streamIt != fHeader->GetStreamHeaders().end();
                    ++streamIt )
            {
                fStreams.push_back( new MStream( *streamIt, tStreamsGroup ) );
            }
        }
        catch( H5::Exception& e )
        {
            throw MException() << "HDF5 error while creating stream objects for reading:\n\t" << e.getDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        catch( MException& e )
        {
            throw( e );
        }

        fState = eReady;
        return;
    }

    void Monarch::WriteHeader()
    {
        // Write the header to the file
        // This will create the following groups: run, streams, and channels
        try
        {
            fHeader->WriteToHDF5( fFile );
        }
        catch( H5::Exception& e )
        {
            throw MException() << "HDF5 error while writing header:\n\t" << e.getDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        catch( MException& e )
        {
            throw( e );
        }


        H5::Group* tStreamsGroup = fHeader->GetStreamsGroup();

        try
        {
            // Create the stream objects based on the configuration from the header
            for( MHeader::MStreamHeaders::const_iterator streamIt = fHeader->GetStreamHeaders().begin();
                    streamIt != fHeader->GetStreamHeaders().end();
                    ++streamIt )
            {
                fStreams.push_back( new MStream( *streamIt, tStreamsGroup ) );
            }
        }
        catch( H5::Exception& e )
        {
            throw MException() << "HDF5 error while creating stream objects:\n\t" << e.getDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        catch( MException& e )
        {
            throw( e );
        }

        fState = eReady;
        return;
    }

    void Monarch::Close() const
    {
        MDEBUG( mlog, "const Monarch::Close()" );
        try
        {
            for( std::vector< MStream* >::iterator streamIt = fStreams.begin(); streamIt != fStreams.end(); ++streamIt )
            {
                const_cast< const MStream* >(*streamIt)->Close();
                delete *streamIt;
                *streamIt = NULL;
            }
        }
        catch( H5::Exception& e )
        {
            throw MException() << "Error while closing: " << e.getDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        return;
    }

    void Monarch::Close()
    {
        MDEBUG( mlog, "non-const Monarch::Close()" );
        try
        {
            for( std::vector< MStream* >::iterator streamIt = fStreams.begin(); streamIt != fStreams.end(); ++streamIt )
            {
                (*streamIt)->Close();
                delete *streamIt;
                *streamIt = NULL;
            }
        }
        catch( H5::Exception& e )
        {
            throw MException() << "Error while closing: " << e.getDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        return;
    }

}
