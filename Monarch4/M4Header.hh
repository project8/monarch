/*
 * M4Header.hh
 *
 *  Created on: Apr 12, 2023
 *      Author: N.S. Oblath
 */

#ifndef M4HEADER_HH_
#define M4HEADER_HH_

#include "M4Constants.hh"
#include "logger.hh"
#include "M4MemberVariable.hh"
// #include "M4Types.hh"

#include "z5includes.hh"

// Search for definitions: in <prj> directory monarch/
// $ grep -R HAS_GRP_IFC *
// $ grep -R HAS_ATTR_IFC *
//
// HAS_ATTR_IFC and HAS_GRP_IFC was found in monarch/CMakeLists.txt, used in compiler CXX_DEFINES
// set( HDF5_DEFINITIONS -DHAS_ATTR_IFC=H5::H5Location -DHAS_GRP_IFC=H5::CommonFG)
// set( HDF5_DEFINITIONS -DHAS_ATTR_IFC=H5::H5Object -DHAS_GRP_IFC=H5::H5Object )
//
// HAS_GRP_IFC looks to be similar to z5 Group concept - hold streams, channels, etc.
// HAS_ATTR_IFC looks to be something that can have attributes, z5 Group can have attributes
//
// For initial development, we will define them here to see they compile/build
#define HAS_GRP_IFC z5::filesystem::handle::Group
#define HAS_ATTR_IFC z5::filesystem::handle::Group

#include <string>
#include <vector>

namespace monarch4
{
    LOGGER( mlog_mheader, "M4Header.hh" );

    /*!
     @class M4StreamHeader
     @author N. S. Oblath

     @brief Single-stream header information

     @details
    */
    class M4_API M4StreamHeader
    {
        public:
            M4StreamHeader();
            M4StreamHeader( const std::string& aSource, uint32_t aNumber, uint32_t aNChannels, uint32_t aFirstChannel, uint32_t aFormat,
                           uint32_t anAcqRate, uint32_t aRecSize, uint32_t aSampleSize,
                           uint32_t aDataTypeSize, uint32_t aDataFormat,
                           uint32_t aBitDepth, uint32_t aBitAlignment );
            M4StreamHeader( const M4StreamHeader& orig );
            ~M4StreamHeader();

            M4MEMBERVARIABLE_PTR( char, Label );
            M4MEMBERVARIABLE_NOSET( uint32_t, Number );
            void SetNumber( uint32_t aNumber ) const; /// In addition to setting the number, sets the label to "stream[aNumber]"
            M4MEMBERVARIABLE_REF( std::string, Source );
            M4MEMBERVARIABLE( uint32_t, NChannels );
            M4MEMBERVARIABLE_REF_CONST( std::vector< uint32_t >, Channels );
            M4MEMBERVARIABLE( uint32_t, ChannelFormat );
            M4MEMBERVARIABLE( uint32_t, AcquisitionRate );
            M4MEMBERVARIABLE( uint32_t, RecordSize );
            M4MEMBERVARIABLE( uint32_t, SampleSize );
            M4MEMBERVARIABLE( uint32_t, DataTypeSize );
            M4MEMBERVARIABLE( uint32_t, DataFormat );
            M4MEMBERVARIABLE( uint32_t, BitDepth );
            M4MEMBERVARIABLE( uint32_t, BitAlignment );
            M4MEMBERVARIABLE( uint32_t, NAcquisitions );
            M4MEMBERVARIABLE( uint32_t, NRecords );
            
        public:
            void WriteToFile( z5GroupHandle aGroup  );
            void ReadFromFile( z5GroupHandle aGroup, const std::string& aLabel ) const;


        private:
            // void WriteChannels( HAS_ATTR_IFC* aLoc );
            // void ReadChannels( const HAS_ATTR_IFC* aLoc ) const;
            void WriteChannels( z5GroupHandle aGroup );
            void ReadChannels( const z5GroupHandle aGroup ) const;            
   };

    /*!
     @class M4ChannelHeader
     @author N. S. Oblath

     @brief Single-channel header information

     @details
    */
    class M4_API M4ChannelHeader
    {
        public:
            M4ChannelHeader();
            M4ChannelHeader( const std::string& aSource, uint32_t aNumber,
                            uint32_t anAcqRate, uint32_t aRecSize, uint32_t aSampleSize,
                            uint32_t aDataTypeSize, uint32_t aDataFormat,
                            uint32_t aBitDepth, uint32_t aBitAlignment );
            M4ChannelHeader( const M4ChannelHeader& orig );
            ~M4ChannelHeader();

            M4MEMBERVARIABLE_PTR( char, Label );
            M4MEMBERVARIABLE_NOSET( uint32_t, Number );
            void SetNumber( uint32_t aNumber ) const; /// In addition to setting the number, sets the label to "channel[aNumber]"
            M4MEMBERVARIABLE_REF( std::string, Source );
            M4MEMBERVARIABLE( uint32_t, AcquisitionRate );
            M4MEMBERVARIABLE( uint32_t, RecordSize );
            M4MEMBERVARIABLE( uint32_t, SampleSize );
            M4MEMBERVARIABLE( uint32_t, DataTypeSize );
            M4MEMBERVARIABLE( uint32_t, DataFormat );
            M4MEMBERVARIABLE( uint32_t, BitDepth );
            M4MEMBERVARIABLE( uint32_t, BitAlignment );
            M4MEMBERVARIABLE( double, VoltageOffset );
            M4MEMBERVARIABLE( double, VoltageRange );
            M4MEMBERVARIABLE( double, DACGain );
            M4MEMBERVARIABLE( double, FrequencyMin );
            M4MEMBERVARIABLE( double, FrequencyRange );

        public:
            // void WriteToFile( HAS_GRP_IFC* aParent );
            // void ReadFromFile( const HAS_GRP_IFC* aParent, const std::string& aLabel ) const;
            void WriteToFile( z5GroupHandle aGroup  );
            void ReadFromFile( const z5GroupHandle aGroup , const std::string& aLabel ) const;
    };

    /*!
     @class M4Header
     @author N. S. Oblath

     @brief Egg file header information

     @details
     General run setup can be configured by setting variables in this header object.

     The stream structure of the data is also configured using the AddStream functions.
    */
    class M4_API M4Header
    {
        public:
            typedef std::vector< M4ChannelHeader > M4ChannelHeaders;
            typedef std::vector< M4StreamHeader > M4StreamHeaders;

        public:
            M4Header();
            ~M4Header();

            void CopyBasicInfo( const M4Header& aOrig );

            M4MEMBERVARIABLE_REF( std::string, EggVersion );
            M4MEMBERVARIABLE_REF( std::string, Filename );
            M4MEMBERVARIABLE( uint32_t, RunDuration );
            M4MEMBERVARIABLE_REF( std::string, Timestamp );
            M4MEMBERVARIABLE_REF( std::string, Description );
            M4MEMBERVARIABLE( uint32_t, NChannels );
            M4MEMBERVARIABLE( uint32_t, NStreams );
            M4MEMBERVARIABLE_REF_CONST( std::vector< uint32_t >, ChannelStreams );
            M4MEMBERVARIABLE_REF_CONST( std::vector< std::vector< bool > >, ChannelCoherence );
            void SetCoherence( unsigned aChanA, unsigned aChanB, bool aCoherence );
            M4MEMBERVARIABLE_REF_CONST( std::vector< M4ChannelHeader >, ChannelHeaders );
            std::vector< M4ChannelHeader >& GetChannelHeaders();
            M4MEMBERVARIABLE_REF_CONST( std::vector< M4StreamHeader >, StreamHeaders );
            std::vector< M4StreamHeader >& GetStreamHeaders();

        public:
            /// Add a stream with one channel with aRecSize samples per record
            /// Returns the channel number assigned/used by this stream
            unsigned AddStream( const std::string& aSource,
                                uint32_t anAcqRate, uint32_t aRecSize, uint32_t aSampleSize,
                                uint32_t aDataTypeSize, uint32_t aDataFormat,
                                uint32_t aBitDepth, uint32_t aBitAlignment,
                                std::vector< unsigned >* aChanVec = NULL );

            /// Add a stream with multiple channels with aRecSize samples per record
            /// Returns vector of the channel numbers assigned/used by this stream
            unsigned AddStream( const std::string& aSource, uint32_t aNChannels, uint32_t aFormat,
                                uint32_t anAcqRate, uint32_t aRecSize, uint32_t aSampleSize,
                                uint32_t aDataTypeSize, uint32_t aDataFormat,
                                uint32_t aBitDepth, uint32_t aBitAlignment,
                                std::vector< unsigned >* aChanVec = NULL );
        public:
            // void ReadFromFile( const z5::filesystem::handle::File* aFile ) const;
            void WriteToFile( z5FileHandle* aFile );
            void ReadFromFile( const z5FileHandle* aFile ) const;

            const z5GroupHandle* GetStreamsGroup() const;
            z5GroupHandle* GetStreamsGroup();

            const z5GroupHandle* GetChannelsGroup() const;
            z5GroupHandle* GetChannelsGroup();

        private:
            // void WriteChannelStreams( HAS_ATTR_IFC* aLoc );
            // void ReadChannelStreams( const HAS_ATTR_IFC* aLoc ) const;
            void WriteChannelStreams( z5FileHandle* aFile );
            void ReadChannelStreams( const z5FileHandle* aFile ) const;

            // void WriteChannelCoherence( HAS_ATTR_IFC* aLoc );
            // void ReadChannelCoherence( const HAS_ATTR_IFC* aLoc ) const;
            void WriteChannelCoherence( z5FileHandle* aFile  );
            void ReadChannelCoherence( const z5FileHandle* aFile ) const;

            mutable z5FileHandle* fFile;            // <root>
            mutable z5GroupHandle* fStreamsGroup;   // <root>/"streams"
            mutable z5GroupHandle* fChannelsGroup;  // <root>/"channels"

        public:
            static void WriteScalarToHDF5( HAS_ATTR_IFC* aLoc, const std::string& aName, const std::string& aValue );
            template< typename XType >
            static void WriteScalarToHDF5( HAS_ATTR_IFC* aLoc, const std::string& aName, XType aValue );

            //template< typename XArrayType >
            //static void Write1DToHDF5( HAS_GRP_IFC* aLoc, const std::string& aName, const XArrayType& anArray );

            template< typename XType >
            static XType ReadScalarFromHDF5( const HAS_ATTR_IFC* aLoc, const std::string& aName );

            template< typename XType >
            static XType ReadScalarFromHDF5( const HAS_ATTR_IFC* aLoc, const std::string& aName, const XType& aDefault );

            //template< typename XArrayType >
            //static void Read1DFromHDF5( const HAS_ATTR_IFC* aLoc, const std::string& aName, XArrayType& anArray );
    };

    inline const z5GroupHandle* M4Header::GetStreamsGroup() const
    {
        return fStreamsGroup;
    }

    inline z5GroupHandle* M4Header::GetStreamsGroup()
    {
        return fStreamsGroup;
    }

    inline const z5GroupHandle* M4Header::GetChannelsGroup() const
    {
        return fChannelsGroup;
    }

    inline z5GroupHandle* M4Header::GetChannelsGroup()
    {
        return fChannelsGroup;
    }

    inline std::vector< M4ChannelHeader >& M4Header::GetChannelHeaders()
    {
        return fChannelHeaders;
    }

    inline std::vector< M4StreamHeader >& M4Header::GetStreamHeaders()
    {
        return fStreamHeaders;
    }

#if 0
    inline void M4Header::WriteScalarToHDF5( HAS_ATTR_IFC* aLoc, const std::string& aName, const std::string& aValue )
    {
        LTRACE( mlog_mheader, "Writing string to new scalar metadata <" << aName << ">: " << aValue << "; size = " << aValue.size() );
    // aName.c_str() and aValue.c_str() are used because while using the std::string itself, the value was getting mangled
    H5::DataType tType = MH5Type< std::string >::H5( aValue );
        aLoc->createAttribute( aName.c_str(), tType, H5::DataSpace( H5S_SCALAR ) ).write( tType, aValue.c_str() );
        return;
    }

    template< typename XType >
    void M4Header::WriteScalarToHDF5( HAS_ATTR_IFC* aLoc, const std::string& aName, XType aValue )
    {
        LTRACE( mlog_mheader, "Writing value to new scalar metadata <" << aName << ">: " << aValue );
        // aName.c_str() is used because while using the std::string itself, the value was getting mangled
    aLoc->createAttribute( aName.c_str(), MH5Type< XType >::H5(), H5::DataSpace( H5S_SCALAR ) ).write( MH5Type< XType >::Native(), &aValue );
        return;
    }
#endif
/*
    template< typename XArrayType >
    void M4Header::Write1DToHDF5( HAS_GRP_IFC* aLoc, const std::string& aName, const XArrayType& anArray )
    {
        typedef typename XArrayType::value_type XValueType;
        LDEBUG( mlog_mheader, "Writing vector to new 1-D metadata <" << aName << ">; size = " << anArray.size() );
        hsize_t tDims[ 1 ] = { anArray.size() };
        std::cout << "tDims[0] = " << tDims[0] << std::endl;
        //H5::ArrayType tTypeNative( MH5Type< XValueType >::Native(), 1, tDims );
        //H5::ArrayType tTypeH5( MH5Type< XValueType >::H5(), 1, tDims );
        H5::DataSpace dspace( 1, tDims, tDims );
        XValueType* buffer = new XValueType( anArray.size() );
        for( unsigned i = 0; i < anArray.size(); ++i )
        {
            buffer[ i ] = anArray[ i ];
            std::cout << "writing bin " << i << ": " << buffer[i] << " <-- " << anArray[i] << std::endl;
        }
        //aLoc->createAttribute( aName, MH5Type< XValueType >::H5(), dspace ).write( MH5Type< XValueType >::Native(), buffer );
        aLoc->createDataSet( aName, MH5Type< XValueType >::H5(), dspace ).write( buffer, MH5Type< XValueType >::Native() );
        delete [] buffer;
        return;
    }
*/
#if 0
    // Read functions
    // read specialization for strings
    template<>
    inline std::string M4Header::ReadScalarFromHDF5( const HAS_ATTR_IFC* aLoc, const std::string& aName )
    {
        //std::string tValue;
    char tBuffer[ 65536 ]; // this array size matches the maximum standard attribute size according to the HDF5 documentation
        H5::Attribute* tAttr = new H5::Attribute( aLoc->openAttribute( aName.c_str() ) );
        //tAttr->read( tAttr->getDataType(), tValue );
    tAttr->read( tAttr->getDataType(), tBuffer );
        delete tAttr;
    std::string tValue( tBuffer );
        LTRACE( mlog_mheader, "Reading string <" << aName << ">: " << tValue << "; size = " << tValue.size() );
        return tValue;
    }

    // templated read function
    template< typename XType >
    XType M4Header::ReadScalarFromHDF5( const HAS_ATTR_IFC* aLoc, const std::string& aName )
    {
        XType tValue;
        H5::Attribute* tAttr = new H5::Attribute( aLoc->openAttribute( aName.c_str() ) );
        tAttr->read( tAttr->getDataType(), &tValue );
        delete tAttr;
        LTRACE( mlog_mheader, "Reading value <" << aName << ">: " << tValue );
        return tValue;
    }


    // Read functions with default values

    // read specialization for strings
    template<>
    inline std::string M4Header::ReadScalarFromHDF5( const HAS_ATTR_IFC* aLoc, const std::string& aName, const std::string& aDefaultValue )
    {
        if( ! aLoc->attrExists( aName.c_str() ) ) return aDefaultValue;
        //std::string tValue;
        char tBuffer[ 65536 ]; // this array size matches the maximum standard attribute size according to the HDF5 documentation
        H5::Attribute* tAttr = new H5::Attribute( aLoc->openAttribute( aName.c_str() ) );
        //tAttr->read( tAttr->getDataType(), tValue );
        tAttr->read( tAttr->getDataType(), tBuffer );
        delete tAttr;
        std::string tValue( tBuffer );
        LTRACE( mlog_mheader, "Reading string <" << aName << ">: " << tValue << "; size = " << tValue.size() );
        return tValue;
    }

    // templated read function
    template< typename XType >
    XType M4Header::ReadScalarFromHDF5( const HAS_ATTR_IFC* aLoc, const std::string& aName, const XType& aDefaultValue )
    {
        if( ! aLoc->attrExists( aName.c_str() ) ) return aDefaultValue;
        XType tValue;
        H5::Attribute* tAttr = new H5::Attribute( aLoc->openAttribute( aName.c_str() ) );
        tAttr->read( tAttr->getDataType(), &tValue );
        delete tAttr;
        LTRACE( mlog_mheader, "Reading value <" << aName << ">: " << tValue );
        return tValue;
    }
#endif    
/*
    template< typename XArrayType >
    void M4Header::Read1DFromHDF5( const HAS_ATTR_IFC* aLoc, const std::string& aName, XArrayType& anArray )
    {
        typedef typename XArrayType::value_type XValueType;
        H5::Attribute tAttr( aLoc->openAttribute( aName ) );
        H5::DataSpace tDataspace( tAttr.getSpace() );
        if( tDataspace.getSimpleExtentNdims() != 1 )
        {
            throw M4Exception() << "Attribute <" << aName << "> has " << tDataspace.getSimpleExtentNdims() << " dimensions; 1 dimension was expected";
        }
        hsize_t tDims[ 1 ];
        tDataspace.getSimpleExtentDims( tDims );
        XValueType* buffer = new XValueType( tDims[0] );
        LDEBUG( mlog_mheader, "Reading 1-D metadata <" << aName << "> to vector; size = " << tDims[0] );
        tAttr.read( tAttr.getDataType(), buffer );
        for( unsigned i = 0; i < anArray.size(); ++i )
        {
            anArray[ i ] = buffer[ i ];
        }
        delete [] buffer;
        return;
    }
*/
}

// Pretty printing methods
M4_API std::ostream& operator<<( std::ostream& out, const monarch4::M4StreamHeader& hdr );
M4_API std::ostream& operator<<( std::ostream& out, const monarch4::M4ChannelHeader& hdr );
M4_API std::ostream& operator<<( std::ostream& out, const monarch4::M4Header& hdr );

#endif
