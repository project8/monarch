/*
 * M3Header.hh
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 */

#ifndef M3HEADER_HH_
#define M3HEADER_HH_

#include "M3Constants.hh"
#include "logger.hh"
#include "M3MemberVariable.hh"
#include "M3Types.hh"

#include "H5Cpp.h"

#include <string>
#include <vector>

namespace monarch3
{
    LOGGER( mlog_mheader, "M3Header.hh" );

    /*!
     @class M3StreamHeader
     @author N. S. Oblath

     @brief Single-stream header information

     @details
    */
    class M3_API M3StreamHeader
    {
        public:
            M3StreamHeader();
            M3StreamHeader( const std::string& aSource, uint32_t aNumber, uint32_t aNChannels, uint32_t aFirstChannel, uint32_t aFormat,
                           uint32_t anAcqRate, uint32_t aRecSize, uint32_t aSampleSize,
                           uint32_t aDataTypeSize, uint32_t aDataFormat,
                           uint32_t aBitDepth, uint32_t aBitAlignment );
            M3StreamHeader( const M3StreamHeader& orig );
            ~M3StreamHeader();

            M3MEMBERVARIABLE_PTR( char, Label );

            M3MEMBERVARIABLE_NOSET( uint32_t, Number );
            void SetNumber( uint32_t aNumber ) const; /// In addition to setting the number, sets the label to "stream[aNumber]"

            M3MEMBERVARIABLE_REF( std::string, Source );

            M3MEMBERVARIABLE( uint32_t, NChannels );

            M3MEMBERVARIABLE_REF_CONST( std::vector< uint32_t >, Channels );

            M3MEMBERVARIABLE( uint32_t, ChannelFormat );

            M3MEMBERVARIABLE( uint32_t, AcquisitionRate );

            M3MEMBERVARIABLE( uint32_t, RecordSize );

            M3MEMBERVARIABLE( uint32_t, SampleSize );

            M3MEMBERVARIABLE( uint32_t, DataTypeSize );

            M3MEMBERVARIABLE( uint32_t, DataFormat );

            M3MEMBERVARIABLE( uint32_t, BitDepth );

            M3MEMBERVARIABLE( uint32_t, BitAlignment );

            M3MEMBERVARIABLE( uint32_t, NAcquisitions );

            M3MEMBERVARIABLE( uint32_t, NRecords );

        public:
            void WriteToHDF5( HAS_GRP_IFC* aParent );
            void ReadFromHDF5( const HAS_GRP_IFC* aParent, const std::string& aLabel ) const;

        private:
            void WriteChannels( HAS_ATTR_IFC* aLoc );
            void ReadChannels( const HAS_ATTR_IFC* aLoc ) const;
   };

    /*!
     @class M3ChannelHeader
     @author N. S. Oblath

     @brief Single-channel header information

     @details
    */
    class M3_API M3ChannelHeader
    {
        public:
            M3ChannelHeader();
            M3ChannelHeader( const std::string& aSource, uint32_t aNumber,
                            uint32_t anAcqRate, uint32_t aRecSize, uint32_t aSampleSize,
                            uint32_t aDataTypeSize, uint32_t aDataFormat,
                            uint32_t aBitDepth, uint32_t aBitAlignment );
            M3ChannelHeader( const M3ChannelHeader& orig );
            ~M3ChannelHeader();

            M3MEMBERVARIABLE_PTR( char, Label );

            M3MEMBERVARIABLE_NOSET( uint32_t, Number );
            void SetNumber( uint32_t aNumber ) const; /// In addition to setting the number, sets the label to "channel[aNumber]"

            M3MEMBERVARIABLE_REF( std::string, Source );

            M3MEMBERVARIABLE( uint32_t, AcquisitionRate );

            M3MEMBERVARIABLE( uint32_t, RecordSize );

            M3MEMBERVARIABLE( uint32_t, SampleSize );

            M3MEMBERVARIABLE( uint32_t, DataTypeSize );

            M3MEMBERVARIABLE( uint32_t, DataFormat );

            M3MEMBERVARIABLE( uint32_t, BitDepth );

            M3MEMBERVARIABLE( uint32_t, BitAlignment );

            M3MEMBERVARIABLE( double, VoltageOffset );

            M3MEMBERVARIABLE( double, VoltageRange );

            M3MEMBERVARIABLE( double, DACGain );

            M3MEMBERVARIABLE( double, FrequencyMin );

            M3MEMBERVARIABLE( double, FrequencyRange );

        public:
            void WriteToHDF5( HAS_GRP_IFC* aParent );
            void ReadFromHDF5( const HAS_GRP_IFC* aParent, const std::string& aLabel ) const;

    };


    /*!
     @class M3Header
     @author N. S. Oblath

     @brief Egg file header information

     @details
     General run setup can be configured by setting variables in this header object.

     The stream structure of the data is also configured using the AddStream functions.
    */
    class M3_API M3Header
    {
        public:
            typedef std::vector< M3ChannelHeader > M3ChannelHeaders;
            typedef std::vector< M3StreamHeader > M3StreamHeaders;

        public:
            M3Header();
            ~M3Header();

            void CopyBasicInfo( const M3Header& aOrig );

            M3MEMBERVARIABLE_REF( std::string, EggVersion );

            M3MEMBERVARIABLE_REF( std::string, Filename );

            M3MEMBERVARIABLE( uint32_t, RunDuration );

            M3MEMBERVARIABLE_REF( std::string, Timestamp );

            M3MEMBERVARIABLE_REF( std::string, Description );

            M3MEMBERVARIABLE( uint32_t, NChannels );

            M3MEMBERVARIABLE( uint32_t, NStreams );

            M3MEMBERVARIABLE_REF_CONST( std::vector< uint32_t >, ChannelStreams );

            M3MEMBERVARIABLE_REF_CONST( std::vector< std::vector< bool > >, ChannelCoherence );
            void SetCoherence( unsigned aChanA, unsigned aChanB, bool aCoherence );

            M3MEMBERVARIABLE_REF_CONST( std::vector< M3ChannelHeader >, ChannelHeaders );
            std::vector< M3ChannelHeader >& GetChannelHeaders();

            M3MEMBERVARIABLE_REF_CONST( std::vector< M3StreamHeader >, StreamHeaders );
            std::vector< M3StreamHeader >& GetStreamHeaders();

        public:
            /// Add a stream with one channel with aRecSize samples per record
            /// Returns the stream number (used to address the stream later)
            unsigned AddStream( const std::string& aSource,
                                uint32_t anAcqRate, uint32_t aRecSize, uint32_t aSampleSize,
                                uint32_t aDataTypeSize, uint32_t aDataFormat,
                                uint32_t aBitDepth, uint32_t aBitAlignment,
                                std::vector< unsigned >* aChanVec = NULL );
            /// Add a stream with multiple channels with aRecSize samples per record
            /// Returns the stream number (used to address the stream later)
            unsigned AddStream( const std::string& aSource, uint32_t aNChannels, uint32_t aFormat,
                                uint32_t anAcqRate, uint32_t aRecSize, uint32_t aSampleSize,
                                uint32_t aDataTypeSize, uint32_t aDataFormat,
                                uint32_t aBitDepth, uint32_t aBitAlignment,
                                std::vector< unsigned >* aChanVec = NULL );

        public:
            void WriteToHDF5( H5::H5File* aFile );
            void ReadFromHDF5( const H5::H5File* aFile ) const;

            const H5::Group* GetStreamsGroup() const;
            H5::Group* GetStreamsGroup();

            const H5::Group* GetChannelsGroup() const;
            H5::Group* GetChannelsGroup();

        private:
            void WriteChannelStreams( HAS_ATTR_IFC* aLoc );
            void ReadChannelStreams( const HAS_ATTR_IFC* aLoc ) const;

            void WriteChannelCoherence( HAS_ATTR_IFC* aLoc );
            void ReadChannelCoherence( const HAS_ATTR_IFC* aLoc ) const;

            mutable H5::H5File* fFile;
            mutable H5::Group* fStreamsGroup;
            mutable H5::Group* fChannelsGroup;

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

    inline const H5::Group* M3Header::GetStreamsGroup() const
    {
        return fStreamsGroup;
    }

    inline H5::Group* M3Header::GetStreamsGroup()
    {
        return fStreamsGroup;
    }

    inline const H5::Group* M3Header::GetChannelsGroup() const
    {
        return fChannelsGroup;
    }

    inline H5::Group* M3Header::GetChannelsGroup()
    {
        return fChannelsGroup;
    }

    inline std::vector< M3ChannelHeader >& M3Header::GetChannelHeaders()
    {
        return fChannelHeaders;
    }

    inline std::vector< M3StreamHeader >& M3Header::GetStreamHeaders()
    {
        return fStreamHeaders;
    }


    inline void M3Header::WriteScalarToHDF5( HAS_ATTR_IFC* aLoc, const std::string& aName, const std::string& aValue )
    {
        LTRACE( mlog_mheader, "Writing string to new scalar metadata <" << aName << ">: " << aValue << "; size = " << aValue.size() );
		// aName.c_str() and aValue.c_str() are used because while using the std::string itself, the value was getting mangled
		H5::DataType tType = MH5Type< std::string >::H5( aValue );
        aLoc->createAttribute( aName.c_str(), tType, H5::DataSpace( H5S_SCALAR ) ).write( tType, aValue.c_str() );
        return;
    }

    template< typename XType >
    void M3Header::WriteScalarToHDF5( HAS_ATTR_IFC* aLoc, const std::string& aName, XType aValue )
    {
        LTRACE( mlog_mheader, "Writing value to new scalar metadata <" << aName << ">: " << aValue );
        // aName.c_str() is used because while using the std::string itself, the value was getting mangled
		aLoc->createAttribute( aName.c_str(), MH5Type< XType >::H5(), H5::DataSpace( H5S_SCALAR ) ).write( MH5Type< XType >::Native(), &aValue );
        return;
    }
/*
    template< typename XArrayType >
    void M3Header::Write1DToHDF5( HAS_GRP_IFC* aLoc, const std::string& aName, const XArrayType& anArray )
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

    // Read functions

    // read specialization for strings
    template<>
    inline std::string M3Header::ReadScalarFromHDF5( const HAS_ATTR_IFC* aLoc, const std::string& aName )
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
    XType M3Header::ReadScalarFromHDF5( const HAS_ATTR_IFC* aLoc, const std::string& aName )
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
    inline std::string M3Header::ReadScalarFromHDF5( const HAS_ATTR_IFC* aLoc, const std::string& aName, const std::string& aDefaultValue )
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
    XType M3Header::ReadScalarFromHDF5( const HAS_ATTR_IFC* aLoc, const std::string& aName, const XType& aDefaultValue )
    {
        if( ! aLoc->attrExists( aName.c_str() ) ) return aDefaultValue;
        XType tValue;
        H5::Attribute* tAttr = new H5::Attribute( aLoc->openAttribute( aName.c_str() ) );
        tAttr->read( tAttr->getDataType(), &tValue );
        delete tAttr;
        LTRACE( mlog_mheader, "Reading value <" << aName << ">: " << tValue );
        return tValue;
    }
/*
    template< typename XArrayType >
    void M3Header::Read1DFromHDF5( const HAS_ATTR_IFC* aLoc, const std::string& aName, XArrayType& anArray )
    {
        typedef typename XArrayType::value_type XValueType;
        H5::Attribute tAttr( aLoc->openAttribute( aName ) );
        H5::DataSpace tDataspace( tAttr.getSpace() );
        if( tDataspace.getSimpleExtentNdims() != 1 )
        {
            throw M3Exception() << "Attribute <" << aName << "> has " << tDataspace.getSimpleExtentNdims() << " dimensions; 1 dimension was expected";
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
M3_API std::ostream& operator<<( std::ostream& out, const monarch3::M3StreamHeader& hdr );
M3_API std::ostream& operator<<( std::ostream& out, const monarch3::M3ChannelHeader& hdr );
M3_API std::ostream& operator<<( std::ostream& out, const monarch3::M3Header& hdr );

#endif
