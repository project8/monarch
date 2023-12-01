/*************************************************************************
* @file writeMonarch.cc
* @author your name (you@domain.com)
* @brief Write Zarr file using Monarch4 components
* @version 0.1
* @date 2023-09-11 Initial adaptation from write.cc
* 
* 
*************************************************************************/

#include "M4Monarch.hh"

#if 0
#include "nlohmann/json.hpp"
#include "xtensor/xarray.hpp"
#include "xtensor/xio.hpp"

// factory functions to create files, groups and datasets
#include "z5/factory.hxx"
// handles for z5 filesystem objects
#include "z5/filesystem/handle.hxx"
// io for xtensor multi-arrays
#include "z5/multiarray/xtensor_access.hxx"
// attribute functionality
#include "z5/attributes.hxx"
#endif

// Shorter typedefs for z5 types
typedef z5::filesystem::handle::File z5FileHandle;
typedef z5::filesystem::handle::Dataset z5DatasetHandle;
typedef z5::filesystem::handle::Group z5GroupHandle;

using namespace std;
using namespace monarch4;

LOGGER( mlog, "M4WriteTest" );

string tFilename = "readme4.zr";    // read/write Zarr with Monarch4 components

int main() {

    shared_ptr< Monarch4 > tWriteTest( Monarch4::OpenForWriting( tFilename ) );

    if (tWriteTest != nullptr)
    {
        cout << "writeMonarch: SUCCESS: Zarr file opened for writing\n";

        M4Header* tHeader = tWriteTest->GetHeader();
        tHeader->Filename() = tFilename;
        tHeader->SetRunDuration( 8675309 );
        tHeader->Timestamp() = "Stardate 33515";
        tHeader->Description() = "Bigger on the inside";

        // number of samples in each stream
        unsigned tSSSamples = 10;
        unsigned tDSSamples = 5;
        unsigned tTSSamples = 5;
        unsigned tFlSSamples = 10;

        unsigned tSingleStreamNum = tHeader->AddStream( "1-channel device", 500, tSSSamples, 1, 1, sDigitizedUS, 8, sBitsAlignedLeft );
        unsigned tDoubleStreamNum = tHeader->AddStream( "2-channel device", 2, sInterleaved, 250, tDSSamples, 1, 2, sDigitizedUS, 16, sBitsAlignedLeft );
        unsigned tTripleStreamNum = tHeader->AddStream( "3-channel device", 3, sSeparate, 100, tTSSamples, 1, 1, sDigitizedUS, 8, sBitsAlignedLeft );
        unsigned tFloatStreamNum = tHeader->AddStream( "Floating-point device", 100, tFlSSamples, 1, 4, sAnalog, 8, sBitsAlignedLeft );

        tWriteTest->WriteHeader();

        // Programmer's Note: *tHeader into mlog (ostream) dumps header info to terminal!
//      LINFO( mlog, "Wrote header:\n" << *tHeader );
        LINFO( mlog, "Writing data" );

        // Stream 0
        M4Stream* tSingleStream = tWriteTest->GetStream( tSingleStreamNum );
std::cout << "tSingleStream: " << tSingleStream << std::endl;
        byte_type* tSSData = tSingleStream->GetStreamRecord()->GetData();
std::cout << "tSSData: " << tSSData << std::endl;
        if (tSingleStream != nullptr && tSSData != nullptr)
        {
          std::cout << "stream and data pointers okay\n";
        }
//      for( unsigned iSample = 0; iSample < tSSSamples; ++iSample )
//      {
//          tSSData[ iSample ] = 1;
//      }
//      if( ! tSingleStream->WriteRecord( true ) )
//      {
//          LERROR( mlog, "Unable to write the record!" );
//          return RETURN_ERROR;
//      }
//
//      for( unsigned iSample = 0; iSample < tSSSamples; ++iSample )
//      {
//          tSSData[ iSample ] = 10;
//      }
//      if( ! tSingleStream->WriteRecord( false ) )
//      {
//          LERROR( mlog, "Unable to write the record!" );
//          return RETURN_ERROR;
//      }
    }
    else 
    {
      LINFO( mlog, "writeMonarch: Error creating WriteTest\n" );
    }

#if 0



        // Stream 1
        M4Stream* tDoubleStream = tWriteTest->GetStream( tDoubleStreamNum );
        M4DataWriter< uint16_t > tDSData0( tDoubleStream->GetChannelRecord( 0 )->GetData(), 2, sDigitizedUS );
        M4DataWriter< uint16_t > tDSData1( tDoubleStream->GetChannelRecord( 1 )->GetData(), 2, sDigitizedUS );
        for( unsigned iSample = 0; iSample < tDSSamples; ++iSample )
        {
            tDSData0.set_at( 1, iSample );
            tDSData1.set_at( 2, iSample );
        }
        if( ! tDoubleStream->WriteRecord( true ) )
        {
            LERROR( mlog, "Unable to write the record!" );
            return RETURN_ERROR;
        }

        for( unsigned iSample = 0; iSample < tDSSamples; ++iSample )
        {
            tDSData0.set_at( 1000, iSample );
            tDSData1.set_at( 2000, iSample );
        }
        if( ! tDoubleStream->WriteRecord( true ) )
        {
            LERROR( mlog, "Unable to write the record!" );
            return RETURN_ERROR;
        }

        for( unsigned iSample = 0; iSample < tDSSamples; ++iSample )
        {
            tDSData0.set_at( 10000, iSample );
            tDSData1.set_at( 20000, iSample );
        }
        if( ! tDoubleStream->WriteRecord( false ) )
        {
            LERROR( mlog, "Unable to write the record!" );
            return RETURN_ERROR;
        }


        // Stream 2
        M4Stream* tTripleStream = tWriteTest->GetStream( tTripleStreamNum );
        byte_type* tTSData0 = tTripleStream->GetChannelRecord( 0 )->GetData();
        byte_type* tTSData1 = tTripleStream->GetChannelRecord( 1 )->GetData();
        byte_type* tTSData2 = tTripleStream->GetChannelRecord( 2 )->GetData();
        for( unsigned iSample = 0; iSample < tTSSamples; ++iSample )
        {
            tTSData0[ iSample ] = 1;
            tTSData1[ iSample ] = 2;
            tTSData2[ iSample ] = 3;
        }
        if( ! tTripleStream->WriteRecord( true ) )
        {
            LERROR( mlog, "Unable to write the record!" );
            return RETURN_ERROR;
        }

        for( unsigned iSample = 0; iSample < tTSSamples; ++iSample )
        {
            tTSData0[ iSample ] = 10;
            tTSData1[ iSample ] = 20;
            tTSData2[ iSample ] = 30;
        }
        if( ! tTripleStream->WriteRecord( false ) )
        {
            LERROR( mlog, "Unable to write the record!" );
            return RETURN_ERROR;
        }



        // Stream 3
        M4Stream* tFloatStream = tWriteTest->GetStream( tFloatStreamNum );
        M4DataWriter< float > tFlSData( tFloatStream->GetChannelRecord( 0 )->GetData(), 4, sAnalog );
        for( unsigned iSample = 0; iSample < tFlSSamples; ++iSample )
        {
            tFlSData.set_at( 3.1415926535898, iSample );
        }
        if( ! tFloatStream->WriteRecord( true ) )
        {
            LERROR( mlog, "Unable to write the record!" );
            return RETURN_ERROR;
        }

        for( unsigned iSample = 0; iSample < tFlSSamples; ++iSample )
        {
            tFlSData.set_at( 2.71828182846, iSample );
        }
        if( ! tFloatStream->WriteRecord( true ) )
        {
            LERROR( mlog, "Unable to write the record!" );
            return RETURN_ERROR;
        }




        // Stream 4
        /*
        M4Stream* tFlCompStream = tWriteTest->GetStream( tFlCompStreamNum );
        M4ComplexDataWriter< f8_complex > tFlCompSData0( tFlCompStream->GetChannelRecord( 0 )->GetData(), 8, sAnalog, 2 );
        M4ComplexDataWriter< f8_complex > tFlCompSData1( tFlCompStream->GetChannelRecord( 1 )->GetData(), 8, sAnalog, 2 );
        M4ComplexDataWriter< f8_complex > tFlCompSData2( tFlCompStream->GetChannelRecord( 2 )->GetData(), 8, sAnalog, 2 );
        M4ComplexDataWriter< f8_complex > tFlCompSData3( tFlCompStream->GetChannelRecord( 3 )->GetData(), 8, sAnalog, 2 );
        M4ComplexDataWriter< f8_complex > tFlCompSData4( tFlCompStream->GetChannelRecord( 3 )->GetData(), 8, sAnalog, 2 );
        f8_complex value0, value1, value2, value3, value4;
        value0[ 0 ] = 0.0; value0[ 1 ] = 0.0;
        value1[ 0 ] = 1.1; value1[ 1 ] = 1.001;
        value2[ 0 ] = 2.2; value1[ 1 ] = 2.002;
        value3[ 0 ] = 3.3; value1[ 1 ] = 3.003;
        value4[ 0 ] = 4.4; value1[ 1 ] = 4.004;
        for( unsigned iSample = 0; iSample < tFlCompSamples; ++iSample )
        {
            tFlCompSData0.set_at( value0, iSample );
            tFlCompSData1.set_at( value1, iSample );
            tFlCompSData2.set_at( value2, iSample );
            tFlCompSData3.set_at( value3, iSample );
            tFlCompSData4.set_at( value4, iSample );
        }
        if( ! tFlCompStream->WriteRecord( true ) )
        {
            LERROR( mlog, "Unable to write the record!" );
            return RETURN_ERROR;
        }

        value0[ 0 ] = -0.0; value0[ 1 ] = -0.0;
        value1[ 0 ] = -1.1; value1[ 1 ] = -1.001;
        value2[ 0 ] = -2.2; value1[ 1 ] = -2.002;
        value3[ 0 ] = -3.3; value1[ 1 ] = -3.003;
        value4[ 0 ] = -4.4; value1[ 1 ] = -4.004;
        for( unsigned iSample = 0; iSample < tFlCompSamples; ++iSample )
        {
            tFlCompSData0.set_at( value0, iSample );
            tFlCompSData1.set_at( value1, iSample );
            tFlCompSData2.set_at( value2, iSample );
            tFlCompSData3.set_at( value3, iSample );
            tFlCompSData4.set_at( value4, iSample );
        }
        if( ! tFlCompStream->WriteRecord( false ) )
        {
            LERROR( mlog, "Unable to write the record!" );
            return RETURN_ERROR;
        }
        */

        tWriteTest->FinishWriting();
        LINFO( mlog, "File closed" );
    }
    catch( M4Exception& e )
    {
        LERROR( mlog, "Exception thrown during write test:\n" << e.what() );
        return RETURN_ERROR;
    }
#endif
    std::cout << "writeMonarch: End of test\n";
    return 0;
}
