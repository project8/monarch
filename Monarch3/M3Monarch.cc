/*
 * M3Monarch.cc
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 */

#include "M3Monarch.hh"

#include "M3Logger.hh"

using std::string;

namespace monarch3
{
    M3LOGGER( mlog, "MMonarch3" );

    Monarch3::Monarch3() :
                fState( eClosed ),
                fFile( NULL ),
                fHeader( NULL )
    {
        // Turn of HDF5 automatic exception printing to allow exceptions to be handled properly
        // And they will be handled properly, always, right????
        H5::Exception::dontPrint();
    }

    Monarch3::~Monarch3()
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

    const Monarch3* Monarch3::OpenForReading( const string& aFilename )
    {
        Monarch3* tMonarch3 = new Monarch3();

        tMonarch3->fFile = new H5::H5File( aFilename.c_str(), H5F_ACC_RDONLY );
        if( tMonarch3->fFile == NULL )
        {
            delete tMonarch3;
            throw M3Exception() << "Could not open <" << aFilename << "> for reading";
            return NULL;
        }
        M3DEBUG( mlog, "Opened egg file <" << aFilename << "> for reading" );

        tMonarch3->fHeader = new M3Header();
        tMonarch3->fHeader->SetFilename( aFilename );

        tMonarch3->fState = eOpen;

        return tMonarch3;
    }

    Monarch3* Monarch3::OpenForWriting( const string& aFilename )
    {
        Monarch3* tMonarch3 = new Monarch3();

        tMonarch3->fFile = new H5::H5File( aFilename.c_str(), H5F_ACC_TRUNC );
        if( tMonarch3->fFile == NULL )
        {
            delete tMonarch3;
            throw M3Exception() << "Could not open <" << aFilename << "> for writing";
            return NULL;
        }
        M3DEBUG( mlog, "Opened egg file <" << aFilename << "> for writing" );

        tMonarch3->fHeader = new M3Header();
        tMonarch3->fHeader->SetFilename( aFilename );

        tMonarch3->fState = eOpen;

        return tMonarch3;
    }

    void Monarch3::ReadHeader() const
    {
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
            throw( e );
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
            }
        }
        catch( H5::Exception& e )
        {
            throw M3Exception() << "HDF5 error while creating stream objects for reading:\n\t" << e.getDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        catch( M3Exception& e )
        {
            throw( e );
        }

        fState = eReady;
        return;
    }

    void Monarch3::WriteHeader()
    {
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
            throw( e );
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
            }
        }
        catch( H5::Exception& e )
        {
            throw M3Exception() << "HDF5 error while creating stream objects:\n\t" << e.getDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        catch( M3Exception& e )
        {
            throw( e );
        }

        fState = eReady;
        return;
    }

    void Monarch3::Close() const
    {
        M3DEBUG( mlog, "const Monarch3::Close()" );
        try
        {
            for( std::vector< M3Stream* >::iterator streamIt = fStreams.begin(); streamIt != fStreams.end(); ++streamIt )
            {
                const_cast< const M3Stream* >(*streamIt)->Close();
                delete *streamIt;
                *streamIt = NULL;
            }
        }
        catch( H5::Exception& e )
        {
            throw M3Exception() << "Error while closing: " << e.getDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        return;
    }

    void Monarch3::Close()
    {
        M3DEBUG( mlog, "non-const Monarch3::Close()" );
        try
        {
            for( std::vector< M3Stream* >::iterator streamIt = fStreams.begin(); streamIt != fStreams.end(); ++streamIt )
            {
                (*streamIt)->Close();
                delete *streamIt;
                *streamIt = NULL;
            }
        }
        catch( H5::Exception& e )
        {
            throw M3Exception() << "Error while closing: " << e.getDetailMsg() << " (function: " << e.getFuncName() << ")";
        }
        return;
    }

}
