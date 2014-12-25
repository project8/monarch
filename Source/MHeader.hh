/*
 * MHeader.hh
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 */

#ifndef MHEADER_HH_
#define MHEADER_HH_

#include "MLogger.hh"
#include "MMemberVariable.hh"
#include "MTypes.hh"

#include "H5Cpp.h"

#include <string>
#include <vector>

namespace monarch
{
    MLOGGER( mlog_mheader, "MHeader.hh" );

    /*!
     @class MStreamHeader
     @author N. S. Oblath

     @brief Single-stream header information

     @details
    */
    class MStreamHeader
    {
        public:
            MStreamHeader();
            MStreamHeader( const std::string& aSource, unsigned aNChannels,
                           unsigned anAcqRate, unsigned aRecSize,
                           unsigned aDataTypeSize, DataFormatType aDataFormat,
                           unsigned aBitDepth );
            MStreamHeader( const MStreamHeader& orig );
            ~MStreamHeader();

            MEMBERVARIABLE( unsigned, Number );

            MEMBERVARIABLE( std::string, Source );

            MEMBERVARIABLE( unsigned, NChannels );

            MEMBERVARIABLE( unsigned, AcquisitionRate );

            MEMBERVARIABLE( unsigned, RecordSize );

            MEMBERVARIABLE( unsigned, DataTypeSize );

            MEMBERVARIABLE( DataFormatType, DataFormat );

            MEMBERVARIABLE( unsigned, BitDepth );

    };

    /*!
     @class MChannelHeader
     @author N. S. Oblath

     @brief Single-channel header information

     @details
    */
    class MChannelHeader
    {
        public:
            MChannelHeader();
            MChannelHeader( const std::string& aSource,
                            unsigned anAcqRate, unsigned aRecSize,
                            unsigned aDataTypeSize, DataFormatType aDataFormat,
                            unsigned aBitDepth );
            MChannelHeader( const MChannelHeader& orig );
            ~MChannelHeader();

            MEMBERVARIABLE( unsigned, Number );

            MEMBERVARIABLE( std::string, Source );

            MEMBERVARIABLE( unsigned, AcquisitionRate );

            MEMBERVARIABLE( unsigned, RecordSize );

            MEMBERVARIABLE( unsigned, DataTypeSize );

            MEMBERVARIABLE( DataFormatType, DataFormat );

            MEMBERVARIABLE( unsigned, BitDepth );

            MEMBERVARIABLE( double, VoltageMin );

            MEMBERVARIABLE( double, VoltageRange );

            MEMBERVARIABLE( double, FrequencyMin );

            MEMBERVARIABLE( double, FrequencyRange );

    };


    /*!
     @class MHeader
     @author N. S. Oblath

     @brief Egg file header information

     @details
    */
    class MHeader
    {
            friend class Monarch;

        public:
            MHeader();
            ~MHeader();

            MEMBERVARIABLEREF( std::string, SchemaVersion );

            MEMBERVARIABLEREF( std::string, Filename );

            MEMBERVARIABLE( unsigned, RunDuration );

            MEMBERVARIABLEREF( std::string, Timestamp );

            MEMBERVARIABLEREF( std::string, Description );

            MEMBERVARIABLE( unsigned, NChannels );

            MEMBERVARIABLE( unsigned, NStreams );

            MEMBERVARIABLEREF_NOSET( std::vector< unsigned >, ChannelStreams );

            MEMBERVARIABLEREF_NOSET( std::vector< MChannelHeader >, ChannelHeaders );

            MEMBERVARIABLEREF_NOSET( std::vector< MStreamHeader >, StreamHeaders );

            // TODO: channel coherence

        public:
            /// Add a stream with one channel with aRecSize samples per record
            /// Returns the stream number (used to address the stream later)
            unsigned AddStream( const std::string& aSource,
                                unsigned anAcqRate, unsigned aRecSize,
                                unsigned aDataTypeSize, DataFormatType aDataFormat,
                                unsigned aBitDepth );
            /// Add a stream with multiple channels with aRecSize samples per record
            /// Returns the stream number (used to address the stream later)
            unsigned AddStream( const std::string& aSource, unsigned aNChannels,
                                unsigned anAcqRate, unsigned aRecSize,
                                unsigned aDataTypeSize, DataFormatType aDataFormat,
                                unsigned aBitDepth );

        public:
            void WriteToHDF5( H5::Group* aGroup ) const;
            void ReadFromHDF5( const H5::Group* aGroup );

        private:
            void WriteScalarToHDF5( H5::Group* aGroup, const std::string& aName, const std::string& aValue ) const;
            template< typename XType >
            void WriteScalarToHDF5( H5::Group* aGroup, const std::string& aName, XType aValue ) const;

            template< typename XType >
            void Write1DToHDF5( H5::Group* aGroup, const std::string& aName, const std::vector< XType >& anArray ) const;

            template< typename XType >
            XType ReadScalarFromHDF5( const H5::Group* aGroup, const std::string& aName );

            template< typename XType >
            void Read1DFromHDF5( const H5::Group* aGroup, const std::string& aName, std::vector< XType >& anArray );

    };

    inline void MHeader::WriteScalarToHDF5( H5::Group* aGroup, const std::string& aName, const std::string& aValue ) const
    {
        H5::DataSpace dspace( H5S_SCALAR );
        H5::DSetCreatPropList plist;
        H5::DataType tType = MH5TypeAccess< std::string >::GetType( aValue );
        aGroup->createDataSet(aName, tType, dspace, plist ).write( aValue, tType );
        MDEBUG( mlog_mheader, "Writing string to new scalar metadata: " << aValue << "; size = " << aValue.size() );
        return;
    }

    template< typename XType >
    void MHeader::WriteScalarToHDF5( H5::Group* aGroup, const std::string& aName, XType aValue ) const
    {
        H5::DataSpace dspace( H5S_SCALAR );
        H5::DSetCreatPropList plist;
        H5::DataType tType = MH5TypeAccess< XType >::GetType();
        aGroup->createDataSet(aName, tType, dspace, plist ).write( &aValue, tType );
        MDEBUG( mlog_mheader, "Writing value to new scalar metadata: " << aValue );
        return;
    }

    template< typename XType >
    inline void MHeader::Write1DToHDF5( H5::Group* aGroup, const std::string& aName, const std::vector< XType >& anArray ) const
    {
        MDEBUG( mlog_mheader, "Writing vector to new 1-D metadata; size = " << anArray.size() );
        hsize_t tDims[ 1 ] = { anArray.size() };
        H5::DataSpace dspace( 1, tDims );
        H5::DSetCreatPropList plist;
        H5::DataType tType = MH5TypeAccess< XType >::GetType();
        H5::DataSet tDataset = aGroup->createDataSet(aName, tType, dspace, plist );
        XType* buffer = new XType( anArray.size() );
        for( unsigned i = 0; i < anArray.size(); ++i )
        {
            buffer[ i ] = anArray[ i ];
        }
        tDataset.write( buffer, tType );
        delete [] buffer;
        return;
    }

    // Read functions

    // read specialization for strings
    template<>
    inline std::string MHeader::ReadScalarFromHDF5( const H5::Group* aGroup, const std::string& aName )
    {
        std::string tValue;
        H5::DataSet tDataset = aGroup->openDataSet( aName );
        tDataset.read( tValue, MH5TypeAccess< std::string >::GetType( tDataset.getInMemDataSize() ) );
        MDEBUG( mlog_mheader, "Reading string: " << tValue << "; size = " << tValue.size() );
        return tValue;
    }

    // templated read function
    template< typename XType >
    XType MHeader::ReadScalarFromHDF5( const H5::Group* aGroup, const std::string& aName )
    {
        XType tValue;
        aGroup->openDataSet( aName ).read( &tValue, MH5TypeAccess< XType >::GetType() );
        MDEBUG( mlog_mheader, "Reading value: " << tValue );
        return tValue;
    }

    template< typename XType >
    void MHeader::Read1DFromHDF5( const H5::Group* aGroup, const std::string& aName, std::vector< XType >& anArray )
    {
        H5::DataSet tDataset = aGroup->openDataSet( aName );
        H5::DataSpace tDataspace = tDataset.getSpace();
        if( tDataspace.getSimpleExtentNdims() != 1 )
        {
            throw MException() << "Dataset <" << aName << "> has " << tDataspace.getSimpleExtentNdims() << " dimensions; 1 dimension was expected";
        }
        hsize_t tDataSize[ 1 ];
        tDataspace.getSimpleExtentDims( tDataSize );
        XType* buffer = new XType( tDataSize[0] );
        MDEBUG( mlog_mheader, "Reading 1-D metadata to vector; size = " << tDataSize[0] );
        tDataset.read( buffer, MH5TypeAccess< XType >::GetType() );
        anArray.resize( tDataSize[0] );
        for( unsigned i = 0; i < anArray.size(); ++i )
        {
            anArray[ i ] = buffer[ i ];
        }
        delete [] buffer;
        return;
    }

}

// Pretty printing methods
std::ostream& operator<<( std::ostream& out, const monarch::MStreamHeader& hdr );
std::ostream& operator<<( std::ostream& out, const monarch::MChannelHeader& hdr );
std::ostream& operator<<( std::ostream& out, const monarch::MHeader& hdr );

#endif