/*
 * M4Monarch.cc
 *
 *  Created on: Apr 12, 2023
 *      Author: N.S. Oblath
 */
#define M4_API_EXPORTS

#include "M4Monarch.hh"
#include "logger.hh"

#include "z5includes.hh"

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
            fFile(nullptr),
            fHeader(),
            fMutexPtr( new std::mutex() )
    {
//std::cout << "Monarch4::Monarch4() default CTOR\n";
    }

    /*************************************************************************
    * @brief Destroy the Monarch 4:: Monarch 4 object
    * 
    *************************************************************************/
    Monarch4::~Monarch4()
    {
//std::cout << "Monarch4::~Monarch4(): default DTOR\n";
        if( fState == eOpenToRead || fState == eReadyToRead)
        {
            FinishReading();
        } 
        if( fState == eOpenToWrite || fState == eReadyToWrite) 
        {
            FinishWriting();
        }

        while( ! fStreams.empty() )
        {
            delete fStreams.back();
            fStreams.pop_back();
        }
//std::cout << "Monarch4::~Monarch4(): Monarch4 component destroyed\n";
    }

    /*************************************************************************
    * @brief Report current state of Monarch4 component WRT file
    * 
    * @return Monarch4::State 
    *   - eOpenToRead: state when monarch has a file open but hasn't read the header
    *   - eOpenToWrite: state when monarch has a file open but hasn't written the header
    *   - eReadyToRead: state when monarch has dealt with the header and is reading records
    *   - eReadyToWrite:  state when monarch has dealt with the header and is writing records
    *   - eClosed: state when monarch has no file
    *************************************************************************/
    Monarch4::State Monarch4::GetState() const
    {
        return fState;
    }

    /*************************************************************************
    * @brief Create new M4Monarch component for read-only
    * 
    * @param[in] aFilename path/filename
    * @return const Monarch4* Pointer to newly created object
    *************************************************************************/
    const Monarch4* Monarch4::OpenForReading( const string& aFilename )
    {
std::cout << "Monarch4::OpenForReading()\n";        
        Monarch4* tMonarch4 = new Monarch4();

        try
        {
            // Create a new z5/Zarr File
            // tMonarch4->fFile = std::make_unique< z5::filesystem::handle::File >( aFilename, z5::FileMode::r );
            tMonarch4->fFile = new z5FileHandle( aFilename, z5::FileMode::r );
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

        // Create the new M4Header
        // tMonarch4->fHeader = std::make_unique< M4Header >();
        tMonarch4->fHeader = std::make_shared< M4Header >();

        tMonarch4->fHeader->Filename() = aFilename;
        tMonarch4->fState = eOpenToRead;

std::cout << "Monarch4::OpenForReading(): newly created Monarch4 component\n";        
        return tMonarch4;   // return a newly created Monarch4 component
    }

    /*************************************************************************
    * @brief Create new Monarch4 component for write
    * 
    * @param[in] aFilename  path/filename
    * @return Monarch4* Pointer to newly created object 
    * @note Open fails is (filename) exists 
    *************************************************************************/
    Monarch4* Monarch4::OpenForWriting( const string& aFilename )
    {
std::cout << "Monarch4::OpenForWriting()\n"; 

        // Create new Monarch4 object
        Monarch4* tMonarch4 = new Monarch4();

        try
        {
            // Create a new z5/Zarr File
            tMonarch4->fFile = new z5FileHandle( aFilename, z5::FileMode::w );
            z5::createFile( *tMonarch4->fFile, true );
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

        // Create the new M4Header
        // tMonarch4->fHeader = std::make_unique< M4Header >();
        tMonarch4->fHeader = std::make_shared< M4Header >();

        tMonarch4->fHeader->Filename() = aFilename;
        tMonarch4->fState = eOpenToWrite;

std::cout << "Monarch4::OpenForWriting(): newly created Monarch4 component\n";        
        return tMonarch4;       // return a newly created Monarch4 component
    }

    /*************************************************************************
    * @brief 
    * 
    *************************************************************************/
    void Monarch4::ReadHeader() const
    {
std::cout << "Monarch4::ReadHeader()\n";
        if( fState != eOpenToRead )
        {
            throw M4Exception() << "File not opened to read";
        }

        // Read the header information from the file (run header, plus all stream and channel headers)
        fHeader->ReadFromFile( fFile );
#if 0
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
#endif
        fState = eReadyToRead;
std::cout << "Monarch4::ReadHeader(): void\n";
    }

    /*************************************************************************
    * @brief 
    * 
    *************************************************************************/
    void Monarch4::WriteHeader()
    {
std::cout << "Monarch4::WriteHeader()\n";
        if( fState != eOpenToWrite )
        {
            throw M4Exception() << "File not opened to write";
        }

        // Write the run-configuration headers to the file
        // This will create the following groups: 
        //  file root directory
        //      channel_coherence, 
        //      channels/channel0..channelN-1, 
        //      channel_streams
        //      streams/stream0..streamN-1
///@todo Handle nulptr fHeader
        fHeader->WriteToFile( fFile );
        
        z5GroupHandle* tStreamsGroup = fHeader->GetStreamsGroup();

        try
        {
std::cout << "\n\nCreate the stream objects based on the configuration from the header\n\n";

            // Create the stream objects based on the configuration from the header
            for( M4Header::M4StreamHeaders::const_iterator streamIt = fHeader->StreamHeaders().begin();
                    streamIt != fHeader->StreamHeaders().end();
                    ++streamIt )
            {                
                fStreams.push_back( new M4Stream( *streamIt, tStreamsGroup ) );     // Create new M4Stream
                fStreams.back()->SetMutex( fMutexPtr );
            }
        }
        // catch( H5::Exception& e )
        // {
        //     throw M4Exception() << "HDF5 error while creating stream objects:\n\t" << e.getDetailMsg() << " (function: " << e.getFuncName() << ")";
        // }
        catch( M4Exception& e )
        {
            throw;
        }
        fState = eReadyToWrite;
std::cout << "Monarch4::WriteHeader(): void\n";
    }

    /*************************************************************************
    * @brief 
    * @note Deconstruct internal components?
    *************************************************************************/
    void Monarch4::FinishReading() const
    {
std::cout << "Monarch4::FinishReading()\n";        
        std::string filename = fHeader != nullptr ? fHeader->Filename() : std::string();
        
        LDEBUG( mlog, "Finishing reading <" << filename << ">" );
        try
        {
            // release ownership of M4Header object 
            //delete fHeader;
            fHeader.reset();  
            //fHeader = nullptr;

            // release ownership of M4Stream objects
            for( std::vector< M4Stream* >::iterator streamIt = fStreams.begin(); streamIt != fStreams.end(); ++streamIt )
            {
                const_cast< const M4Stream* >(*streamIt)->Close();

                delete *streamIt;
                *streamIt = nullptr;
            }

            // release ownership of M4Monarch
            // fFile->reset();      
            // fFile = nullptr;
        }
        catch( std::exception& e )
        {
            throw M4Exception() << "Error while closing file <" << filename << ">:\n" << e.what();
        }
        fState = eClosed;
std::cout << "Monarch4::FinishReading(): void\n";        
    }

    /*************************************************************************
    * @brief 
    * @note Deconstruct internal components?
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
