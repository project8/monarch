#ifdef _WIN32
#define NOMINMAX
#endif

#include "M4DataInterface.hh"
#include "M4Monarch.hh"
#include "application.hh"
#include "logger.hh"
#include "M4Record.hh"

#include <algorithm>
#include <cstring> // for strcmp
#include <sstream>

// Shorter typedefs for z5 types
typedef z5::filesystem::handle::File z5FileHandle;
typedef z5::filesystem::handle::Dataset z5DatasetHandle;
typedef z5::filesystem::handle::Group z5GroupHandle;

using namespace std;
using namespace monarch4;

string tFilename = "readme3.zr";    // read/write Zarr with Monarch4 components

int main() {

    shared_ptr< const Monarch4 > tReadTest( Monarch4::OpenForReading( tFilename ) );

    if (tReadTest != nullptr)
    {
        cout << "SUCCESS: Zarr file opened for reading\n";

        // M4Header* tHeader = tWriteTest->GetHeader();
        // tHeader->Filename() = tFilename;
        // tHeader->SetRunDuration( 8675309 );
        // tHeader->Timestamp() = "Stardate 33515";
        // tHeader->Description() = "Bigger on the inside";

        tReadTest->ReadHeader(); 

        const M4Header* hdr = (const M4Header*)tReadTest->GetHeader();
        std::cout << *hdr;
       
    }
#if 0    
    try
    {
        LINFO( mlog, "Opening file <" << tFilename );
        std::shared_ptr< const Monarch4 > tReadTest( Monarch4::OpenForReading( tFilename ) );

        LINFO( mlog, "Reading header" );
        tReadTest->ReadHeader();

        const M4Header* tReadHeader = tReadTest->GetHeader();
        LINFO( mlog, *tReadHeader );

        if( tHeaderOnly )
        {
            tReadTest->FinishReading();
            return RETURN_SUCCESS;
        }

        LINFO( mlog, "Reading data" );

        LINFO( mlog, "Test 1: reading 2 sequential 1-channel records from stream 0");
        LINFO( mlog, "\tRecord 0 has values '1'" );
        LINFO( mlog, "\tRecord 1 has values '10'" );

        const M4Stream* tStream0 = tReadTest->GetStream( 0 );
        unsigned tNAcquisitions0 = tStream0->GetNAcquisitions();
        unsigned tNRecords0 = tStream0->GetNRecordsInFile();
        LINFO( mlog, "Stream 0 has " << tNAcquisitions0 << " acquisitions and " << tNRecords0 << " records" );
        const M4StreamHeader& tStrHeader0 = tReadHeader->StreamHeaders().at( 0 );
        unsigned tNChannels0 = tStrHeader0.GetNChannels();
        //unsigned tRecSize0 = tStrHeader0.GetRecordSize();
        LINFO( mlog, "Stream 0 has " << tNChannels0 << " channel(s) stored in format mode " << tStrHeader0.GetChannelFormat() );
        if( tNAcquisitions0 != 1 || tNChannels0 != 1 || tNRecords0 != 2 )
        {
            LERROR( mlog, "Invalid number of acquisitions (1 expected), channels (1 expected), or records (2 expected)" );
            return RETURN_ERROR;
        }

        for( unsigned iRec = 0; iRec < tNRecords0; ++iRec )
        {
            LINFO( mlog, "Checking record " << iRec );
            if( ! ReadRecordCheck( tStream0, 0, tStrHeader0.GetDataFormat() ) )
            {
                LERROR( mlog, "Failed read record check" );
                return RETURN_ERROR;
            }
        }

        LINFO( mlog, "Test 1 complete\n" );



        LINFO( mlog, "Test 2: using non-zero reading offsets");
        LINFO( mlog, "\tRecord 0 has values '1' and '2'" );
        LINFO( mlog, "\tRecord 1 has values '1000' and '2000'" );
        LINFO( mlog, "\tRecord 2 has values '10000' and '20000'" );

        const M4Stream* tStream1 = tReadTest->GetStream( 1 );
        unsigned tNAcquisitions1 = tStream1->GetNAcquisitions();
        unsigned tNRecords1 = tStream1->GetNRecordsInFile();
        LINFO( mlog, "Stream 1 has " << tNAcquisitions1 << " acquisitions and " << tNRecords1 << " records" );
        const M4StreamHeader& tStrHeader1 = tReadHeader->StreamHeaders().at( 1 );
        unsigned tNChannels1 = tStrHeader1.GetNChannels();
        //unsigned tRecSize1 = tStrHeader1.GetRecordSize();
        LINFO( mlog, "Stream 1 has " << tNChannels1 << " channel(s) stored in format mode " << tStrHeader1.GetChannelFormat() );
        if( tNAcquisitions1 != 2 || tNChannels1 != 2 || tNRecords1 != 3 )
        {
            LERROR( mlog, "Invalid number of acquisitions (2 expected), channels (2 expected), or records (3 expected)" );
            return RETURN_ERROR;
        }

        LINFO( mlog, "Read the first record (record 0)" );
        if( ! ReadRecordCheck( tStream1, 0, tStrHeader1.GetDataFormat() ) )
        {
            LERROR( mlog, "Failed read record check" );
            return RETURN_ERROR;
        }

        LINFO( mlog, "Skip to the third record, crossing to the next acquisition (record 2; acquisition 1)" );
        if( ! ReadRecordCheck( tStream1, 1, tStrHeader1.GetDataFormat() ) )
        {
            LERROR( mlog, "Failed read record check" );
            return RETURN_ERROR;
        }

        LINFO( mlog, "Reread the third record (record 2; acquisition 1)" );
        if( ! ReadRecordCheck( tStream1, -1, tStrHeader1.GetDataFormat() ) )
        {
            LERROR( mlog, "Failed read record check" );
            return RETURN_ERROR;
        }

        LINFO( mlog, "Go backwards to the second record (record 1; acquisition 1)" );
        if( ! ReadRecordCheck( tStream1, -2, tStrHeader1.GetDataFormat() ) )
        {
            LERROR( mlog, "Failed read record check" );
            return RETURN_ERROR;
        }

        LINFO( mlog, "Go backwards to the first record (record 1; acquisition 0)" );
        if( ! ReadRecordCheck( tStream1, -2, tStrHeader1.GetDataFormat() ) )
        {
            LERROR( mlog, "Failed read record check" );
            return RETURN_ERROR;
        }

        LINFO( mlog, "Reread the first record (record 1; acquisition 0)" );
        if( ! ReadRecordCheck( tStream1, -1, tStrHeader1.GetDataFormat() ) )
        {
            LERROR( mlog, "Failed read record check" );
            return RETURN_ERROR;
        }

        LINFO( mlog, "Request record beyond the end of the file" );
        if( tStream1->ReadRecord( 5 ) )
        {
            LERROR( mlog, "Record read did not fail" );
            return RETURN_ERROR;
        }

        LINFO( mlog, "Test 2 complete\n" );



        LINFO( mlog, "Test 3: using non-zero reading offset to skip the first record");
        LINFO( mlog, "\tRecord 0 has values '1', '2', and '3'" );
        LINFO( mlog, "\tRecord 1 has values '10', '20', and '30'" );

        const M4Stream* tStream2 = tReadTest->GetStream( 2 );
        unsigned tNAcquisitions2 = tStream2->GetNAcquisitions();
        unsigned tNRecords2 = tStream2->GetNRecordsInFile();
        LINFO( mlog, "Stream 2 has " << tNAcquisitions2 << " acquisitions and " << tNRecords2 << " records" );
        const M4StreamHeader& tStrHeader2 = tReadHeader->StreamHeaders().at( 2 );
        unsigned tNChannels2 = tStrHeader2.GetNChannels();
        //unsigned tRecSize2 = tStrHeader2.GetRecordSize();
        LINFO( mlog, "Stream 2 has " << tNChannels2 << " channel(s) stored in format mode " << tStrHeader2.GetChannelFormat() );
        if( tNAcquisitions2 != 1 || tNChannels2 != 3 || tNRecords2 != 2 )
        {
            LERROR( mlog, "Invalid number of acquisitions (1 expected), channels (3 expected), or records (2 expected)" );
            return RETURN_ERROR;
        }

        LINFO( mlog, "Skipping immediately to the second record (record 1)" );
        if( ! ReadRecordCheck( tStream2, 1, tStrHeader2.GetDataFormat() ) )
        {
            LERROR( mlog, "Failed read record check" );
            return RETURN_ERROR;
        }

        LINFO( mlog, "Request record before the beginning of the file" );
        if( tStream2->ReadRecord( -3 ) )
        {
            LERROR( mlog, "Record read did not fail" );
            return RETURN_ERROR;
        }

        LINFO( mlog, "Test 3 complete\n" );


        LINFO( mlog, "Test 4: reading 2 sequential 1-channel floating-point records from stream 3");
        LINFO( mlog, "\tRecord 0 has values '3.1415926535898'" );
        LINFO( mlog, "\tRecord 1 has values '2.71828182846'" );

        const M4Stream* tStreaM4 = tReadTest->GetStream( 3 );
        unsigned tNAcquisitions3 = tStreaM4->GetNAcquisitions();
        unsigned tNRecords3 = tStreaM4->GetNRecordsInFile();
        LINFO( mlog, "Stream 3 has " << tNAcquisitions3 << " acquisitions and " << tNRecords3 << " records" );
        const M4StreamHeader& tStrHeader3 = tReadHeader->StreamHeaders().at( 3 );
        unsigned tNChannels3 = tStrHeader3.GetNChannels();
        //unsigned tRecSize3 = tStrHeader3.GetRecordSize();
        LINFO( mlog, "Stream 3 has " << tNChannels3 << " channel(s) stored in format mode " << tStrHeader3.GetChannelFormat() );
        if( tNAcquisitions3 != 2 || tNChannels3 != 1 || tNRecords3 != 2 )
        {
            LERROR( mlog, "Invalid number of acquisitions (2 expected), channels (1 expected), or records (2 expected)" );
            return RETURN_ERROR;
        }

        for( unsigned iRec = 0; iRec < tNRecords3; ++iRec )
        {
            LINFO( mlog, "Checking record " << iRec );
            if( ! ReadRecordCheck( tStreaM4, 0, tStrHeader3.GetDataFormat() ) )
            {
                LERROR( mlog, "Failed read record check" );
                return RETURN_ERROR;
            }
        }

        LINFO( mlog, "Test 4 complete\n" );

/*
        LINFO( mlog, "Test 5: reading 2 sequential 5-channel interleaved complex floating-point records from stream 4");
        LINFO( mlog, "\tRecord 0 always has values '(0, 0)'" );
        LINFO( mlog, "\tRecord 1 has values '(1.1, 1.001) and (-1.1, -1.001)'" );
        LINFO( mlog, "\tRecord 2 has values '(2.2, 2.002) and (-2.2, -2.002)'" );
        LINFO( mlog, "\tRecord 3 has values '(3.3, 3.003) and (-3.3, -3.003)'" );
        LINFO( mlog, "\tRecord 4 has values '(4.4, 4.004) and (-4.4, -4.004)'" );

        const M4Stream* tStream4 = tReadTest->GetStream( 4 );
        unsigned tNAcquisitions4 = tStream4->GetNAcquisitions();
        unsigned tNRecords4 = tStream4->GetNRecordsInFile();
        LINFO( mlog, "Stream 4 has " << tNAcquisitions4 << " acquisitions and " << tNRecords4 << " records" );
        const M4StreamHeader& tStrHeader4 = tReadHeader->GetStreamHeaders().at( 3 );
        unsigned tNChannels4 = tStrHeader4.GetNChannels();
        //unsigned tRecSize4 = tStrHeader4.GetRecordSize();
        LINFO( mlog, "Stream 4 has " << tNChannels4 << " channel(s) stored in format mode " << tStrHeader4.GetChannelFormat() );
        if( tNAcquisitions4 != 1 || tNChannels4 != 5 || tNRecords4 != 2 )
        {
            LERROR( mlog, "Invalid number of acquisitions (1 expected), channels (5 expected), or records (2 expected)" );
            return RETURN_ERROR;
        }

        for( unsigned iRec = 0; iRec < tNRecords4; ++iRec )
        {
            LINFO( mlog, "Checking record " << iRec );
            if( ! ReadRecordCheck( tStream4, 0, tStrHeader4.GetDataFormat() ) )
            {
                LERROR( mlog, "Failed read record check" );
                return RETURN_ERROR;
            }
        }

        LINFO( mlog, "Test 5 complete\n" );
*/



        tReadTest->FinishReading();
    }
    catch( M4Exception& e )
    {
        LERROR( mlog, "Exception thrown during file reading:\n" << e.what() );
        return RETURN_ERROR;
    }
#endif
    return RETURN_SUCCESS;
}
#if 0
bool ReadRecordCheck( const M4Stream* aStream, int aOffset, unsigned aDataFormat )
{
    if( ! aStream->ReadRecord( aOffset ) )
    {
        LERROR( mlog, "Failed to read record" );
        return false;
    }

    switch( aDataFormat )
    {
        case sDigitizedUS:
            if( ! PrintChannelsUInt( aStream ) )
            {
                LERROR( mlog, "Failed to print channels (digitized)" );
                return false;
            }
            break;
        case sDigitizedS:
            if( ! PrintChannelsUInt( aStream ) )
            {
                LERROR( mlog, "Failed to print channels (digitized)" );
                return false;
            }
            break;
        case sAnalog:
            switch( aStream->GetSampleSize() )
            {
                case 1:
                    if( ! PrintChannelsFloat( aStream ) )
                    {
                        LERROR( mlog, "Problem printing channels (float)" );
                        return 0;
                    }
                    break;
                default:
                    if( ! PrintChannelsFloatComplex( aStream ) )
                    {
                        LERROR( mlog, "Problem printing channels (float-complex)" );
                        return 0;
                    }
                    break;
            }
            break;
        default:
            LERROR( mlog, "Invalid data format" );
            return false;
    }

    return true;
}

bool PrintChannelsUInt( const M4Stream* aStream )
{
    unsigned tRecSize = aStream->GetChannelRecordSize();
    const unsigned tMaxSamples = 30;
    for( unsigned iChan = 0; iChan < aStream->GetNChannels(); ++iChan )
    {
        const M4DataReader< uint64_t > tDataInterface( aStream->GetChannelRecord( iChan )->GetData(),
                aStream->GetDataTypeSize(), sDigitizedUS );
        stringstream tDataOut;
        for( unsigned iSample = 0; iSample < std::min( tMaxSamples, tRecSize ); ++iSample )
        {
            tDataOut << tDataInterface.at( iSample );
            if( iSample != tRecSize - 1 ) tDataOut << ", ";
        }
        if( tRecSize > tMaxSamples ) tDataOut << " . . .";
        LINFO( mlog, "\tChannel " << iChan << ": " << tDataOut.str() );
    }
    return true;
}

bool PrintChannelsInt( const M4Stream* aStream )
{
    unsigned tRecSize = aStream->GetChannelRecordSize();
    const unsigned tMaxSamples = 30;
    for( unsigned iChan = 0; iChan < aStream->GetNChannels(); ++iChan )
    {
        const M4DataReader< int64_t > tDataInterface( aStream->GetChannelRecord( iChan )->GetData(),
                aStream->GetDataTypeSize(), sDigitizedS );
        stringstream tDataOut;
        for( unsigned iSample = 0; iSample < std::min( tMaxSamples, tRecSize ); ++iSample )
        {
            tDataOut << tDataInterface.at( iSample );
            if( iSample != tRecSize - 1 ) tDataOut << ", ";
        }
        if( tRecSize > tMaxSamples ) tDataOut << " . . .";
        LINFO( mlog, "\tChannel " << iChan << ": " << tDataOut.str() );
    }
    return true;
}

bool PrintChannelsFloat( const M4Stream* aStream )
{
    unsigned tRecSize = aStream->GetChannelRecordSize();
    const unsigned tMaxSamples = 30;
    for( unsigned iChan = 0; iChan < aStream->GetNChannels(); ++iChan )
    {
        const M4DataReader< double > tDataInterface( aStream->GetChannelRecord( iChan )->GetData(),
                aStream->GetDataTypeSize(), sAnalog );
        stringstream tDataOut;
        for( unsigned iSample = 0; iSample < std::min( tMaxSamples, tRecSize ); ++iSample )
        {
            tDataOut << tDataInterface.at( iSample );
            if( iSample != tRecSize - 1 ) tDataOut << ", ";
        }
        if( tRecSize > tMaxSamples ) tDataOut << " . . .";
        LINFO( mlog, "\tChannel " << iChan << ": " << tDataOut.str() );
    }
    return true;
}

bool PrintChannelsFloatComplex( const M4Stream* aStream )
{
    // NOTE: this implementation is specific to f4_complex and f8_complex, and therefore assumes only 2 elements in each sample
    const unsigned tMaxSamples = 30;
    unsigned tRecSize = aStream->GetChannelRecordSize();
    switch( aStream->GetDataTypeSize() )
    {
        case 4:
            for( unsigned iChan = 0; iChan < aStream->GetNChannels(); ++iChan )
            {
                const M4ComplexDataReader< f4_complex > tDataInterface( aStream->GetChannelRecord( iChan )->GetData(),
                        aStream->GetDataTypeSize(), sAnalog, aStream->GetSampleSize() );
                stringstream tDataOut;
                for( unsigned iSample = 0; iSample < std::min( tMaxSamples, tRecSize ); ++iSample )
                {
                    tDataOut << "(" << tDataInterface.at( iSample )[ 0 ] << ", " << tDataInterface.at( iSample )[ 1 ] << ")";
                    if( iSample != tRecSize - 1 ) tDataOut << ", ";
                }
                if( tRecSize > tMaxSamples ) tDataOut << " . . .";
                LINFO( mlog, "\tChannel " << iChan << ": " << tDataOut.str() );
            }
            break;
        case 8:
            for( unsigned iChan = 0; iChan < aStream->GetNChannels(); ++iChan )
            {
                const M4ComplexDataReader< f8_complex > tDataInterface( aStream->GetChannelRecord( iChan )->GetData(),
                        aStream->GetDataTypeSize(), sAnalog, aStream->GetSampleSize() );
                stringstream tDataOut;
                for( unsigned iSample = 0; iSample < std::min( tMaxSamples, tRecSize ); ++iSample )
                {
                    tDataOut << "(" << tDataInterface.at( iSample )[ 0 ] << ", " << tDataInterface.at( iSample )[ 1 ] << ")";
                    if( iSample != tRecSize - 1 ) tDataOut << ", ";
                }
                if( tRecSize > tMaxSamples ) tDataOut << " . . .";
                LINFO( mlog, "\tChannel " << iChan << ": " << tDataOut.str() );
            }
            break;
        default:
            LERROR( mlog, "Cannot print channels for complex floating-point data with type size " << aStream->GetDataTypeSize() );
            return false;
            break;
    }
    return true;
}
#endif
