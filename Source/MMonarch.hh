/*
 * MMonarch.hh
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 */

#ifndef MMONARCH_HH_
#define MMONARCH_HH_

#include "MException.hh"
#include "MHeader.hh"
#include "MLogger.hh"

#include "H5Cpp.h"

#include <string>

namespace monarch
{
    MLOGGER( mlog_mmonarch, "MMonarch.h" );

    /*!
     @class Monarch
     @author N. S. Oblath (v3), D. Furse (original)

     @brief Egg file read/write access

     @details
    */
    class Monarch
    {
            //***********************
            // constructors and state
            //***********************

        private:
            //private to force use of static constructor methods
            Monarch();

            //current state of monarch
            typedef enum
            {
                eOpen, // state when monarch has a file open but hasn't written/read the header
                eReady, // state when monarch has dealt with the header and is writing/reading records
                eClosed // state when monarch has no file
            } State;
            mutable State fState;

        public:
            ~Monarch();

            //********************************
            // methods for reading (all const)
            //********************************

        public:

            /// This static method opens the file for reading.
            /// If the file exists and can be read, this returns a prepared monarch pointer, and memory is allocated for the header.
            /// Upon successful return monarch is in the eOpen state.
            static const Monarch* OpenForReading( const std::string& filename );

            /// This method extracts the header information from the file.
            /// If the header read correctly, this returns and the header may be examined, and memory is allocated for the record.
            /// Upon successful return monarch is in the eReady state.
            /// An exception is thrown if the header is not read.
            void ReadHeader() const;

            //get the pointer to the header.
            const MHeader* GetHeader() const;

            /*
            //set the interface type to use
            void SetInterface( InterfaceModeType aMode ) const;

            //this method parses the file for a next record.
            //if the record demarshalled correctly, this returns true and the record is refreshed with content.
            //when the end of the file is reached, this will return false.
            bool ReadRecord( int anOffset = 0 ) const;

            //get the pointer to the current interleaved record.
            const MonarchRecordBytes* GetRecordInterleaved() const;

            //get the pointer to the current separate channel one record.
            const MonarchRecordBytes* GetRecordSeparateOne() const;

            //get the pointer to the current separate channel two record.
            const MonarchRecordBytes* GetRecordSeparateTwo() const;
            */

            //close the file pointer.
            void Close() const;

            //*********************************
            // methods for writing (none const)
            //*********************************

        public:

            //this static method opens the file for writing.
            //if the file exists and can be written, this returns a prepared monarch pointer, and memory is allocated for the header.
            //upon successful return monarch is in the eOpen state.
            static Monarch* OpenForWriting( const std::string& filename );

            //this method marshals the current header to the file.
            //if the header marshalled correctly, this returns true, memory is allocated for the record(s).
            //upon successful return monarch is in the eReady state.
            void WriteHeader();

            //get the pointer to the header.
            MHeader* GetHeader();

            /*
            //set the interface type to use.
            void SetInterface( InterfaceModeType aMode );

            //this method marshals the current record into the file.
            //if the record marshalled correctly, this returns true.
            bool WriteRecord();

            //get the pointer to the current interleaved record.
            MonarchRecordBytes* GetRecordInterleaved();

            //get the pointer to the current separate channel one record.
            MonarchRecordBytes* GetRecordSeparateOne();

            //get the pointer to the current separate channel two record.
            MonarchRecordBytes* GetRecordSeparateTwo();
            */
            //close the file pointer
            void Close();

        private:
            //void WriteMetadata( H5::Group* a_group, const std::string& a_name, const std::string& a_value );
            //template< typename XType >
            //void WriteMetadata( H5::Group* a_group, const std::string& a_name, XType a_value );
            void WriteScalarMetadata( H5::Group* aGroup, const std::string& aName, const std::string& aValue );
            template< typename XType >
            void WriteScalarMetadata( H5::Group* aGroup, const std::string& aName, XType aValue );

            template< typename XType >
            void Write1DMetadata( H5::Group* aGroup, const std::string& aName, const std::vector< XType >& aValue );

            template< typename XType >
            XType ReadMetadata( H5::Group* aGroup, const std::string& aName ) const;

            //the MonarchIO class wraps a bare C file pointer.
            //MonarchIO* fIO;

            // the HDF5 file
            mutable H5::H5File* fFile;

            // the header
            mutable MHeader* fHeader;
/*
            //size of the native type of the records in bytes
            mutable size_t fDataTypeSize;

            //number of bytes in the data array in a record
            mutable size_t fDataNBytes;
            //number of samples in the data array in a record
            mutable size_t fDataSize;

            //number of bytes in an interleaved record
            mutable size_t fInterleavedRecordNBytes;

            //pointer to a MonarchRecordBytes occupying the interleaved record
            mutable MonarchRecordBytes* fRecordInterleaved;
            //pointer to the bytes that hold the interleaved record
            mutable byte_type* fRecordInterleavedBytes;

            //number of bytes in a separate record
            mutable size_t fSeparateRecordNBytes;

            //pointer to a MonarchRecordBytes occupying the first separate record
            mutable MonarchRecordBytes* fRecordSeparateOne;
            //pointer to the bytes that hold the first separate record
            mutable byte_type* fRecordSeparateOneBytes;

            //pointer to a MonarchRecordBytes occupying the second separate record
            mutable MonarchRecordBytes* fRecordSeparateTwo;
            //pointer to the bytes that hold the second separate record
            mutable byte_type* fRecordSeparateTwoBytes;

            //the private read functions
            mutable bool (Monarch::*fReadFunction)( int anOffset ) const;
            bool InterleavedFromSingle( int anOffset ) const;
            bool InterleavedFromSeparate( int anOffset ) const;
            bool InterleavedFromInterleaved( int anOffset ) const;
            bool SeparateFromSingle( int anOffset ) const;
            bool SeparateFromSeparate( int anOffset ) const;
            bool SeparateFromInterleaved( int anOffset ) const;

            //the private write functions
            mutable bool (Monarch::*fWriteFunction)();
            bool InterleavedToSingle();
            bool InterleavedToSeparate();
            bool InterleavedToInterleaved();
            bool SeparateToSingle();
            bool SeparateToSeparate();
            bool SeparateToInterleaved();

        private:
#ifdef __GNUG__
            static void Zip( const size_t aSize, const size_t aDataTypeSize, const byte_type* __restrict__ aRecordOne, const byte_type* __restrict__ aRecordTwo, byte_type* __restrict__ anInterleavedRecord );
#else
            static void Zip( const size_t aSize, const size_t aDataTypeSize, const byte_type*  aRecordOne, const byte_type*  aRecordTwo, byte_type*  anInterleavedRecord );
#endif

#ifdef __GNUG__
            static void Unzip( const size_t aSize, const size_t aDataTypeSize, byte_type* __restrict__ aRecordOne, byte_type* __restrict__ aRecordTwo, const byte_type* __restrict__ anInterleavedRecord );
#else
            static void Unzip( const size_t aSize, const size_t aDataTypeSize, byte_type*  aRecordOne, byte_type*  aRecordTwo, const byte_type*  anInterleavedRecord );
#endif
*/
    };

    // Write functions

    /*
    inline void Monarch::WriteMetadata( H5::Group* a_group, const std::string& a_name, const std::string& a_value )
    {
        a_group->openDataSet( a_name.c_str() ).write( a_value, MH5TypeAccess< std::string >::GetType( a_value ) );
        MDEBUG( mlog_mmonarch, "Writing string to existing metadata: " << a_value << "; size = " << a_value.size() );
        return;
    }
    */
    /*
    template< typename XType >
    void Monarch::WriteMetadata( H5::Group* a_group, const std::string& a_name, XType a_value )
    {
        a_group->openDataSet( a_name.c_str() ).write( &a_value, MH5TypeAccess< XType >::GetType() );
        MDEBUG( mlog_mmonarch, "Writing value to existing metadata: " << a_value );
        return;
    }
    */

    inline void Monarch::WriteScalarMetadata( H5::Group* aGroup, const std::string& aName, const std::string& aValue )
    {
        H5::DataSpace dspace( H5S_SCALAR );
        H5::DSetCreatPropList plist;
        H5::DataType tType = MH5TypeAccess< std::string >::GetType( aValue );
        aGroup->createDataSet(aName, tType, dspace, plist ).write( aValue, tType );
        MDEBUG( mlog_mmonarch, "Writing string to new scalar metadata: " << aValue << "; size = " << aValue.size() );
        return;
    }

    template< typename XType >
    void Monarch::WriteScalarMetadata( H5::Group* aGroup, const std::string& aName, XType aValue )
    {
        H5::DataSpace dspace( H5S_SCALAR );
        H5::DSetCreatPropList plist;
        H5::DataType tType = MH5TypeAccess< XType >::GetType();
        aGroup->createDataSet(aName, tType, dspace, plist ).write( &aValue, tType );
        MDEBUG( mlog_mmonarch, "Writing value to new scalar metadata: " << aValue );
        return;
    }

    template< typename XType >
    inline void Monarch::Write1DMetadata( H5::Group* aGroup, const std::string& aName, const std::vector< XType >& aValue )
    {
        MDEBUG( mlog_mmonarch, "Writing vector to new 1-D metadata; size = " << aValue.size() );
        hsize_t tDims[ 1 ] = { aValue.size() };
        H5::DataSpace dspace( 1, tDims );
        H5::DSetCreatPropList plist;
        H5::DataType tType = MH5TypeAccess< XType >::GetType();
        H5::DataSet tDataset = aGroup->createDataSet(aName, tType, dspace, plist );
        XType* buffer = new XType( aValue.size() );
        MWARN( mlog_mmonarch, tDims << "  " << buffer );
        for( unsigned i = 0; i < aValue.size(); ++i )
        {
            buffer[ i ] = aValue[ i ];
        }
        tDataset.write( buffer, tType );
        delete [] buffer;
        return;
    }




    // Read functions

    // read specialization for strings
    template<>
    std::string Monarch::ReadMetadata( H5::Group* aGroup, const std::string& aName ) const
    {
        std::string tValue;
        H5::DataSet tDataset = aGroup->openDataSet( aName );
        tDataset.read( tValue, MH5TypeAccess< std::string >::GetType( tDataset.getInMemDataSize() ) );
        MDEBUG( mlog_mmonarch, "Reading string: " << tValue << "; size = " << tValue.size() );
        return tValue;
    }

    // templated read function
    template< typename XType >
    XType Monarch::ReadMetadata( H5::Group* aGroup, const std::string& aName ) const
    {
        XType tValue;
        aGroup->openDataSet( aName ).read( &tValue, MH5TypeAccess< XType >::GetType() );
        MDEBUG( mlog_mmonarch, "Reading value: " << tValue );
        return tValue;
    }




    inline const MHeader* Monarch::GetHeader() const
    {
        return fHeader;
    }
    inline MHeader* Monarch::GetHeader()
    {
        return fHeader;
    }
/*
    inline const MonarchRecordBytes* Monarch::GetRecordSeparateOne() const
    {
        return fRecordSeparateOne;
    }
    inline MonarchRecordBytes* Monarch::GetRecordSeparateOne()
    {
        return fRecordSeparateOne;
    }

    inline const MonarchRecordBytes* Monarch::GetRecordSeparateTwo() const
    {
        return fRecordSeparateTwo;
    }
    inline MonarchRecordBytes* Monarch::GetRecordSeparateTwo()
    {
        return fRecordSeparateTwo;
    }

    inline const MonarchRecordBytes* Monarch::GetRecordInterleaved() const
    {
        return fRecordInterleaved;
    }
    inline MonarchRecordBytes* Monarch::GetRecordInterleaved()
    {
        return fRecordInterleaved;
    }

#ifdef __GNUG__
    inline void Monarch::Zip( const size_t aSize, const size_t aDataTypeSize, const byte_type* __restrict__ aRecordOne, const byte_type* __restrict__ aRecordTwo, byte_type* __restrict__ anInterleavedRecord )
#else
    inline void Monarch::Zip( const size_t aSize, const size_t aDataTypeSize, const byte_type* aRecordOne, const byte_type* aRecordTwo, byte_type* anInterleavedRecord )
#endif
    {
        for( size_t anIndex = 0; anIndex < aSize; anIndex++ )
        {
            *anInterleavedRecord = *aRecordOne;
            anInterleavedRecord += aDataTypeSize;
            aRecordOne += aDataTypeSize;

            *anInterleavedRecord = *aRecordTwo;
            anInterleavedRecord += aDataTypeSize;
            aRecordTwo += aDataTypeSize;
        }
    }

#ifdef __GNUG__
    inline void Monarch::Unzip( const size_t aSize, const size_t aDataTypeSize, byte_type* __restrict__ aRecordOne, byte_type* __restrict__ aRecordTwo, const byte_type* __restrict__ anInterleavedRecord )
#else
    inline void Monarch::Unzip( const size_t aSize, const size_t aDataTypeSize, byte_type* aRecordOne, byte_type* aRecordTwo, const byte_type* anInterleavedRecord )
#endif
    {
        for( size_t anIndex = 0; anIndex < aSize; anIndex++ )
        {
            *aRecordOne = *anInterleavedRecord;
            anInterleavedRecord += aDataTypeSize;
            aRecordOne += aDataTypeSize;

            *aRecordTwo = *anInterleavedRecord;
            anInterleavedRecord += aDataTypeSize;
            aRecordTwo += aDataTypeSize;
        }
    }
*/
}

#endif /* MMONARCH_HH_ */
