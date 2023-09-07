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

using namespace std;

int main() {

cout << "get handle to a File on the filesystem: f\n";
    z5::filesystem::handle::File f( "readme2.zr", z5::FileMode::modes::a );

cout << "create the file in zarr format\n";
    const bool createAsZarr = true;
    z5::createFile(f, createAsZarr);

cout << "create a new zarr dataset: /data<float32>[1000,1000,1000]\n";
    const std::string dsName = "data";
    std::vector<size_t> shape = { 1000, 1000, 1000 };
    std::vector<size_t> chunks = { 100, 100, 100 };
    auto ds = z5::createDataset(f, dsName, "float32", shape, chunks);
    
cout << "get handle for the dataset\n";
    const auto dsHandle = z5::filesystem::handle::Dataset(f, dsName);

cout << "write sub-array to roi\n";
    // initialize sub-array with the 'The Meaning of Life'
    z5::types::ShapeType offset1 = { 50, 100, 150 };
    xt::xarray<float>::shape_type shape1 = { 150, 200, 100 };
    xt::xarray<float> array1(shape1, 42.0);

    // write sub-array to dataset
    z5::multiarray::writeSubarray<float>(ds, array1, offset1.begin());

cout << "create json attributes for the dataset\n";
    nlohmann::json attributesIn;
    attributesIn["bar"] = "foo";
    attributesIn["pi"] = 3.141593;

    // Write attributes to the dataset
    z5::writeAttributes(dsHandle, attributesIn);
    // channels

    // Create a group to hold channels
    z5::createGroup(f, "channels");
    auto channelsHandle = z5::filesystem::handle::Group(f, "channels");

    const int totalChannels = 2;
    int nChannels = 0;
    for( int iCh = 0; iCh < totalChannels; ++iCh )
    {
        std::stringstream str;
        str << "channel" << iCh;
        std::string name( str.str() );
cout << "Create channel: " << name << endl;        
        
        z5::createGroup( channelsHandle, name );
        z5::filesystem::handle::Group channelHandle = z5::filesystem::handle::Group( channelsHandle, name );
        
        nlohmann::json oneChGroupAttr;
        oneChGroupAttr["name"] = name;
        z5::writeAttributes(channelHandle, oneChGroupAttr);
        ++nChannels;
    }

    nlohmann::json chGroupAttr;
    chGroupAttr["nChannels"] = nChannels;
    z5::writeAttributes(channelsHandle, chGroupAttr);

    // streams
cout << "Create streams group\n";
    z5::createGroup(f, "streams");
    auto streamsHandle = z5::filesystem::handle::Group(f, "streams");

    // constants for data
    const std::string acqDataName = "data";
    const int recSize = 16;
    const int datasetNRec = 100;
    const int chunkNRec = 10;
    const int maxRecs = datasetNRec / chunkNRec;
    
    std::cout << "Will write <" << maxRecs << "> records" << std::endl;
    std::vector< size_t > datasetShape = { datasetNRec, recSize, 1 };
    std::vector< size_t > chunkShape = {chunkNRec, recSize, 1 };

    const int totalStreams = 3;
    int nStreams = 0;

    // Handles to each stream
    std::vector< z5::filesystem::handle::Group > streamHandles;

    // Handles to each dataset
    std::vector< std::unique_ptr< z5::Dataset > > acqDatasets;

    // std::vector< const z5::filesystem::handle::Dataset > acqDataHandles;
    std::vector< z5::filesystem::handle::Dataset > acqDataHandles;
    for( int iStr = 0; iStr < totalStreams; ++iStr )
    {
        std::stringstream str;
        
        str << "stream" << iStr;
        std::string name( str.str() );
cout << "create group: " << name << endl;
        z5::createGroup( streamsHandle, name );
        
        streamHandles.emplace_back( z5::filesystem::handle::Group( streamsHandle, name ) );
        
        // Create stream group attribute: 'name'
        nlohmann::json oneStrGroupAttr;
        oneStrGroupAttr["name"] = name;
        z5::writeAttributes(streamHandles.back(), oneStrGroupAttr);

        // Create stream group attribute: 'acquisitions', and handle
        z5::createGroup( streamHandles.back(), "acquisitions" );
        z5::filesystem::handle::Group acqHandle = z5::filesystem::handle::Group( streamHandles.back(), "acquisitions" );

        // create a new Dataset for the data of this stream
        acqDatasets.emplace_back( z5::createDataset( acqHandle, acqDataName, "int16", datasetShape, chunkShape ) );
        
        // get handle for the dataset for this stream
        acqDataHandles.push_back( z5::filesystem::handle::Dataset( acqHandle, dsName ) );

        ++nStreams;
    }

    // Create streams attribute: 'nStreams'
    nlohmann::json strGroupAttr;
    strGroupAttr["nStreams"] = nStreams;
    z5::writeAttributes(streamsHandle, strGroupAttr);

    // simulate data taking by storing data in each of the stream's dataset
    z5::types::ShapeType writeOffset = { 0, 0, 0 };
    xt::xarray< int16_t >::shape_type writeShape = { 2, recSize, 1 };
    // xt::xarray< int16_t > arrayPrototype( writeShape, 0 );
    
    xt::xarray< int16_t > arrayStream0( writeShape, 0 );
    xt::xarray< int16_t > arrayStream1( writeShape, 1 );
    xt::xarray< int16_t > arrayStream2( writeShape, 2 );

    // Create unique recognizable data in each stream
    for (int c = 0; c<recSize; ++c)
    {
        arrayStream0.at(0,c,0) = (int16_t)c;
        arrayStream0.at(1,c,0) = (int16_t)c + (int16_t)16;

        arrayStream1.at(0,c,0) = (int16_t)c + (int16_t)32;
        arrayStream1.at(1,c,0) = (int16_t)c + (int16_t)48;

        arrayStream2.at(0,c,0) = (int16_t)c + (int16_t)64;
        arrayStream2.at(1,c,0) = (int16_t)c + (int16_t)80;
    }
    cout << "arrayStream0: \n" << arrayStream0 << endl;
    cout << "\narrayStream1: \n" << arrayStream1 << endl;
    cout << "\narrayStream2: \n" << arrayStream2 << endl;

    // std::cout << "Writing records" << std::endl;
    // for( int iStr = 0; iStr < totalStreams; ++iStr )
    // {
    //     std::cout << "Writing records for stream " << iStr << std::endl;
    //     // PROGRAMMER'S NOTE: z5::types::ShapeType writeOffset[0]
    //     for( writeOffset[0] = 0; writeOffset[0] < maxRecs; writeOffset[0] += 1 )
    //     {
    //         std::cout << "Write record: " <<  writeOffset[0] << endl;
    //         z5::multiarray::writeSubarray< int16_t >( acqDatasets[iStr], arrayPrototype, writeOffset.begin() );
    //     }
    // }

    std::cout << "Writing stream0 record\n";
    z5::multiarray::writeSubarray< int16_t >( acqDatasets[0], arrayStream0, writeOffset.begin() );

    std::cout << "Writing stream1 record\n";
    z5::multiarray::writeSubarray< int16_t >( acqDatasets[1], arrayStream1, writeOffset.begin() );
    
    std::cout << "Writing stream2 record\n";
    z5::multiarray::writeSubarray< int16_t >( acqDatasets[2], arrayStream2, writeOffset.begin() );

    return 0;
}
