/*
 * M4Monarch.cc
 *
 *  Created on: Apr 12, 2023
 *      Author: N.S. Oblath
 */
#define M4_API_EXPORTS

#include "M4Monarch.hh"

#include "logger.hh"

#include "z5/factory.hxx"

using std::string;

namespace monarch4
{
    LOGGER( mlog, "MMonarch4" );

    /*************************************************************************
    * @brief Construct a new Monarch 4:: Monarch 4 object
    * 
    *************************************************************************/
    Monarch4::Monarch4() :
            fState( eClosed ),
            hFile(nullptr),
            fHeader(),
            fMutexPtr( new std::mutex() )
    {
    }

    /*************************************************************************
    * @brief Destroy the Monarch 4:: Monarch 4 object
    * 
    *************************************************************************/
    Monarch4::~Monarch4()
    {
        if( fState == eOpenToRead || fState == eReadyToRead) FinishReading();
        if( fState == eOpenToWrite || fState == eReadyToWrite) FinishWriting();

        while( ! fStreams.empty() )
        {
            delete fStreams.back();
            fStreams.pop_back();
        }
    }

    /*************************************************************************
    * @brief 
    * 
    * @return Monarch4::State 
    *************************************************************************/
    Monarch4::State Monarch4::GetState() const
    {
        return fState;
    }

    /*************************************************************************
    * @brief 
    * 
    * @param aFilename 
    * @return const Monarch4* 
    *************************************************************************/
    const Monarch4* Monarch4::OpenForReading( const string& aFilename )
    {
        Monarch4* tMonarch4 = new Monarch4();

        try
        {
            // tMonarch4->fFile = std::make_unique< z5::filesystem::handle::File >( aFilename, z5::FileMode::r );
// z5::filesystem::handle::File::File()
            tMonarch4->hFile = new z5FileHandle( aFilename, z5::FileMode::r );
            z5::createFile( *tMonarch4->hFile, true );

        }
        catch( std::exception& e )
        {
            delete tMonarch4;
            throw M4Exception() << "Could not open <" << aFilename << "> for reading; a std::exception was thrown:\n" << e.what();
            return nullptr;
        }

        // if( tMonarch4->hFile == nullptr )
        // {
        //     delete tMonarch4;
        //     throw M4Exception() << "Could not open <" << aFilename << "> for reading";
        //     return nullptr;
        // }
        LDEBUG( mlog, "Opened egg file <" << aFilename << "> for reading" );

        // tMonarch4->fHeader = std::make_unique< M4Header >();
        tMonarch4->fHeader = std::make_shared< M4Header >();
        tMonarch4->fHeader->Filename() = aFilename;

        tMonarch4->fState = eOpenToRead;

        return tMonarch4;
    }

    /*************************************************************************
    * @brief 
    * 
    * @param aFilename 
    * @return Monarch4* 
    *************************************************************************/
    Monarch4* Monarch4::OpenForWriting( const string& aFilename )
    {
        Monarch4* tMonarch4 = new Monarch4();

        try
        {
// z5::filesystem::handle::File f( "readme2.zr", z5::FileMode::modes::a );
// z5FileHandle f( "readme2.zr", z5::FileMode::modes::a );

            tMonarch4->hFile = new z5FileHandle( aFilename, z5::FileMode::w );
            z5::createFile( *tMonarch4->hFile, true );

        }
        catch( std::exception& e )
        {
            delete tMonarch4;
            throw M4Exception() << "Could not open <" << aFilename << "> for writing; a std::exception was thrown:\n" << e.what();
            return nullptr;
        }

        // if( tMonarch4->fFile == nullptr )
        // {
        //     delete tMonarch4;
        //     throw M4Exception() << "Could not open <" << aFilename << "> for writing";
        //     return nullptr;
        // }

        LDEBUG( mlog, "Opened egg file <" << aFilename << "> for writing" );

        // tMonarch4->fHeader = std::make_unique< M4Header >();
        tMonarch4->fHeader = std::make_shared< M4Header >();
        tMonarch4->fHeader->Filename() = aFilename;

        tMonarch4->fState = eOpenToWrite;

        return tMonarch4;
    }
#if 0
    void Monarch4::ReadHeader() const
    {
        if( fState != eOpenToRead )
        {
            throw M4Exception() << "File not opened to read";
        }

        // Read the header information from the file (run header, plus all stream and channel headers)
        fHeader->ReadFromFile( fFile.get() );

        z5::filesystem::handle::Group* tStreamsGroup = fHeader->GetStreamsGroup();

        // TODO: investigate z5/Zarr exceptions
        // try
        // {
            // Create the stream objects based on the configuration from the header
            for( M4Header::M4StreamHeaders::const_iterator streamIt = fHeader->StreamHeaders().begin();
                    streamIt != fHeader->StreamHeaders().end();
                    ++streamIt )
            {
                fStreams.push_back( new M4Stream( *streamIt, tStreamsGroup ) );
                fStreams.back()->SetMutex( fMutexPtr );
            }
        // }
        // catch( H5::Exception& e )
        // {
        //     throw M4Exception() << "HDF5 error while creating stream objects for reading:\n\t" << e.getDetailMsg() << " (function: " << e.getFuncName() << ")";
        // }
        // catch( M4Exception& e )
        // {
        //     throw;
        // }

        fState = eReadyToRead;
        return;
    }

#endif
    /*************************************************************************
    * @brief 
    * 
    *************************************************************************/
    void Monarch4::WriteHeader()
    {
        if( fState != eOpenToWrite )
        {
            throw M4Exception() << "File not opened to write";
        }

        // Write the header to the file
        // This will create the following groups: run, streams, and channels
        fHeader->WriteToFile( *hFile );
#if 0
        z5::filesystem::handle::Group* tStreamsGroup = fHeader->GetStreamsGroup();

        // try
        // {
            // Create the stream objects based on the configuration from the header
            for( M4Header::M4StreamHeaders::const_iterator streamIt = fHeader->StreamHeaders().begin();
                    streamIt != fHeader->StreamHeaders().end();
                    ++streamIt )
            {
                fStreams.push_back( new M4Stream( *streamIt, tStreamsGroup ) );
                fStreams.back()->SetMutex( fMutexPtr );
            }
        // }
        // catch( H5::Exception& e )
        // {
        //     throw M4Exception() << "HDF5 error while creating stream objects:\n\t" << e.getDetailMsg() << " (function: " << e.getFuncName() << ")";
        // }
        // catch( M4Exception& e )
        // {
        //     throw;
        // }
#endif
        fState = eReadyToWrite;
    }

    /*************************************************************************
    * @brief 
    * 
    *************************************************************************/
    void Monarch4::FinishReading() const
    {
        std::string filename = fHeader != nullptr ? fHeader->Filename() : std::string();
        
        LDEBUG( mlog, "Finishing reading <" << filename << ">" );
        try
        {
            //delete fHeader;
            fHeader.reset();  // release ownership on the object 
            //fHeader = nullptr;
            for( std::vector< M4Stream* >::iterator streamIt = fStreams.begin(); streamIt != fStreams.end(); ++streamIt )
            {
                const_cast< const M4Stream* >(*streamIt)->Close();
                delete *streamIt;
                *streamIt = nullptr;
            }
// How to properly release shared_ptr?
            //delete fFile;
            //fFile = nullptr;
        }
        catch( std::exception& e )
        {
            throw M4Exception() << "Error while closing file <" << filename << ">:\n" << e.what();
        }
        fState = eClosed;
    }

    /*************************************************************************
    * @brief 
    * 
    *************************************************************************/
    void Monarch4::FinishWriting()
    {
        std::string filename = fHeader != nullptr ? fHeader->Filename() : std::string();
        LINFO( mlog, "Finishing writing <" << filename << ">" );
        try
        {
            //delete fHeader;
            //fHeader = nullptr;
            fHeader.reset();  // release ownership on the object
            for( std::vector< M4Stream* >::iterator streamIt = fStreams.begin(); streamIt != fStreams.end(); ++streamIt )
            {
                (*streamIt)->Close();
                delete *streamIt;
                *streamIt = nullptr;
            }
            //delete fFile;
            //fFile = nullptr;
        }
        catch( std::exception& e )
        {
            throw M4Exception() << "Error while closing file <" << filename << ">:\n" << e.what();
        }
        fState = eClosed;
        return;
    }

}
