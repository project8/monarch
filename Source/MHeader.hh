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
            MStreamHeader( const std::string& aSource, uint32_t aNumber, uint32_t aNChannels, MultiChannelFormatType aFormat,
                           uint32_t anAcqRate, uint32_t aRecSize,
                           uint32_t aDataTypeSize, DataFormatType aDataFormat,
                           uint32_t aBitDepth );
            MStreamHeader( const MStreamHeader& orig );
            ~MStreamHeader();

            MMEMBERVARIABLE( char*, Label );

            MMEMBERVARIABLE_NOSET( uint32_t, Number );
            void SetNumber( uint32_t aNumber ) const; /// In addition to setting the number, sets the label to "stream[aNumber]"

            MMEMBERVARIABLE( std::string, Source );

            MMEMBERVARIABLE( uint32_t, NChannels );

            MMEMBERVARIABLE( MultiChannelFormatType, ChannelFormat );

            MMEMBERVARIABLE( uint32_t, AcquisitionRate );

            MMEMBERVARIABLE( uint32_t, RecordSize );

            MMEMBERVARIABLE( uint32_t, DataTypeSize );

            MMEMBERVARIABLE( DataFormatType, DataFormat );

            MMEMBERVARIABLE( uint32_t, BitDepth );

            MMEMBERVARIABLE( uint32_t, NAcquisitions );

            MMEMBERVARIABLE( uint32_t, NRecords );

        public:
            void WriteToHDF5( H5::CommonFG* aParent );
            void ReadFromHDF5( const H5::CommonFG* aParent, const std::string& aLabel ) const;

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
            MChannelHeader( const std::string& aSource, uint32_t aNumber,
                            uint32_t anAcqRate, uint32_t aRecSize,
                            uint32_t aDataTypeSize, DataFormatType aDataFormat,
                            uint32_t aBitDepth );
            MChannelHeader( const MChannelHeader& orig );
            ~MChannelHeader();

            MMEMBERVARIABLE( char*, Label );

            MMEMBERVARIABLE_NOSET( uint32_t, Number );
            void SetNumber( uint32_t aNumber ) const; /// In addition to setting the number, sets the label to "channel[aNumber]"

            MMEMBERVARIABLE( std::string, Source );

            MMEMBERVARIABLE( uint32_t, AcquisitionRate );

            MMEMBERVARIABLE( uint32_t, RecordSize );

            MMEMBERVARIABLE( uint32_t, DataTypeSize );

            MMEMBERVARIABLE( DataFormatType, DataFormat );

            MMEMBERVARIABLE( uint32_t, BitDepth );

            MMEMBERVARIABLE( double, VoltageMin );

            MMEMBERVARIABLE( double, VoltageRange );

            MMEMBERVARIABLE( double, FrequencyMin );

            MMEMBERVARIABLE( double, FrequencyRange );

        public:
            void WriteToHDF5( H5::CommonFG* aParent );
            void ReadFromHDF5( const H5::CommonFG* aParent, const std::string& aLabel ) const;

    };


    /*!
     @class MHeader
     @author N. S. Oblath

     @brief Egg file header information

     @details
    */
    class MHeader
    {
        public:
            typedef std::vector< MChannelHeader > MChannelHeaders;
            typedef std::vector< MStreamHeader > MStreamHeaders;

        public:
            MHeader();
            ~MHeader();

            MMEMBERVARIABLEREF( std::string, SchemaVersion );

            MMEMBERVARIABLEREF( std::string, Filename );

            MMEMBERVARIABLE( uint32_t, RunDuration );

            MMEMBERVARIABLEREF( std::string, Timestamp );

            MMEMBERVARIABLEREF( std::string, Description );

            MMEMBERVARIABLE( uint32_t, NChannels );

            MMEMBERVARIABLE( uint32_t, NStreams );

            MMEMBERVARIABLEREF_NOSET( std::vector< uint32_t >, ChannelStreams );

            MMEMBERVARIABLEREF_NOSET( std::vector< MChannelHeader >, ChannelHeaders );

            MMEMBERVARIABLEREF_NOSET( std::vector< MStreamHeader >, StreamHeaders );

            // TODO: channel coherence

        public:
            /// Add a stream with one channel with aRecSize samples per record
            /// Returns the stream number (used to address the stream later)
            unsigned AddStream( const std::string& aSource,
                                uint32_t anAcqRate, uint32_t aRecSize,
                                uint32_t aDataTypeSize, DataFormatType aDataFormat,
                                uint32_t aBitDepth );
            /// Add a stream with multiple channels with aRecSize samples per record
            /// Returns the stream number (used to address the stream later)
            unsigned AddStream( const std::string& aSource, uint32_t aNChannels, MultiChannelFormatType aFormat,
                                uint32_t anAcqRate, uint32_t aRecSize,
                                uint32_t aDataTypeSize, DataFormatType aDataFormat,
                                uint32_t aBitDepth );

        public:
            void WriteToHDF5( H5::H5File* aFile );
            void ReadFromHDF5( const H5::H5File* aFile ) const;

            const H5::Group* GetStreamsGroup() const;
            H5::Group* GetStreamsGroup();

            const H5::Group* GetChannelsGroup() const;
            H5::Group* GetChannelsGroup();

        private:
            mutable H5::H5File* fFile;
            mutable H5::Group* fStreamsGroup;
            mutable H5::Group* fChannelsGroup;

        public:
            static void WriteScalarToHDF5( H5::H5Location* aLoc, const std::string& aName, const std::string& aValue );
            template< typename XType >
            static void WriteScalarToHDF5( H5::H5Location* aLoc, const std::string& aName, XType aValue );

            template< typename XArrayType >
            static void Write1DToHDF5( H5::H5Location* aLoc, const std::string& aName, const XArrayType& anArray );

            template< typename XType >
            static XType ReadScalarFromHDF5( const H5::H5Location* aLoc, const std::string& aName );

            template< typename XArrayType >
            static void Read1DFromHDF5( const H5::H5Location* aLoc, const std::string& aName, XArrayType& anArray );

    };

    inline const H5::Group* MHeader::GetStreamsGroup() const
    {
        return fStreamsGroup;
    }

    inline H5::Group* MHeader::GetStreamsGroup()
    {
        return fStreamsGroup;
    }

    inline const H5::Group* MHeader::GetChannelsGroup() const
    {
        return fChannelsGroup;
    }

    inline H5::Group* MHeader::GetChannelsGroup()
    {
        return fChannelsGroup;
    }


    inline void MHeader::WriteScalarToHDF5( H5::H5Location* aLoc, const std::string& aName, const std::string& aValue )
    {
        H5::DataType tType = MH5Type< std::string >::H5( aValue );
        aLoc->createAttribute( aName, tType, H5::DataSpace( H5S_SCALAR ) ).write( tType, aValue );
        MDEBUG( mlog_mheader, "Writing string to new scalar metadata <" << aName << ">: " << aValue << "; size = " << aValue.size() );
        return;
    }

    template< typename XType >
    void MHeader::WriteScalarToHDF5( H5::H5Location* aLoc, const std::string& aName, XType aValue )
    {
        aLoc->createAttribute( aName, MH5Type< XType >::H5(), H5::DataSpace( H5S_SCALAR ) ).write( MH5Type< XType >::Native(), &aValue );
        MDEBUG( mlog_mheader, "Writing value to new scalar metadata <" << aName << ">: " << aValue );
        return;
    }

    template< typename XArrayType >
    inline void MHeader::Write1DToHDF5( H5::H5Location* aLoc, const std::string& aName, const XArrayType& anArray )
    {
        typedef typename XArrayType::value_type XValueType;
        MDEBUG( mlog_mheader, "Writing vector to new 1-D metadata <" << aName << ">; size = " << anArray.size() );
        hsize_t tDims[ 1 ] = { anArray.size() };
        H5::ArrayType tTypeNative( MH5Type< XValueType >::Native(), 1, tDims );
        H5::ArrayType tTypeH5( MH5Type< XValueType >::H5(), 1, tDims );
        H5::DataSpace dspace( 1, tDims );
        XValueType* buffer = new XValueType( anArray.size() );
        for( unsigned i = 0; i < anArray.size(); ++i )
        {
            buffer[ i ] = anArray[ i ];
            std::cout << "writing bin " << i << ": " << buffer[i] << " <-- " << anArray[i] << std::endl;
        }
        aLoc->createAttribute( aName, tTypeH5, dspace ).write( tTypeNative, buffer );
        delete [] buffer;
        return;
    }


    // Read functions

    // read specialization for strings
    template<>
    inline std::string MHeader::ReadScalarFromHDF5( const H5::H5Location* aLoc, const std::string& aName )
    {
        std::string tValue;
        H5::Attribute* tAttr = new H5::Attribute( aLoc->openAttribute( aName ) );
        tAttr->read( tAttr->getDataType(), tValue );
        delete tAttr;
        MDEBUG( mlog_mheader, "Reading string <" << aName << ">: " << tValue << "; size = " << tValue.size() );
        return tValue;
    }

    // templated read function
    template< typename XType >
    XType MHeader::ReadScalarFromHDF5( const H5::H5Location* aLoc, const std::string& aName )
    {
        XType tValue;
        H5::Attribute* tAttr = new H5::Attribute( aLoc->openAttribute( aName ) );
        tAttr->read( tAttr->getDataType(), &tValue );
        delete tAttr;
        MDEBUG( mlog_mheader, "Reading value <" << aName << ">: " << tValue );
        return tValue;
    }

    template< typename XArrayType >
    void MHeader::Read1DFromHDF5( const H5::H5Location* aLoc, const std::string& aName, XArrayType& anArray )
    {
        typedef typename XArrayType::value_type XValueType;
        H5::Attribute tAttr( aLoc->openAttribute( aName ) );
        H5::DataSpace tDataspace( tAttr.getSpace() );
        if( tDataspace.getSimpleExtentNdims() != 1 )
        {
            throw MException() << "Attribute <" << aName << "> has " << tDataspace.getSimpleExtentNdims() << " dimensions; 1 dimension was expected";
        }
        hsize_t tDims[ 1 ];
        tDataspace.getSimpleExtentDims( tDims );
        XValueType* buffer = new XValueType( tDims[0] );
        MDEBUG( mlog_mheader, "Reading 1-D metadata <" << aName << "> to vector; size = " << tDims[0] );
        tAttr.read( tAttr.getDataType(), buffer );
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
