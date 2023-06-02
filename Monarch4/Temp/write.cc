#include "nlohmann/json.hpp"
#include "xtensor/xarray.hpp"

// factory functions to create files, groups and datasets
#include "z5/factory.hxx"
// handles for z5 filesystem objects
#include "z5/filesystem/handle.hxx"
// io for xtensor multi-arrays
#include "z5/multiarray/xtensor_access.hxx"
// attribute functionality
#include "z5/attributes.hxx"

int main() {

    // get handle to a File on the filesystem
    z5::filesystem::handle::File f( "readme2.zr", z5::FileMode::modes::a );

    // create the file in zarr format
    const bool createAsZarr = true;
    z5::createFile(f, createAsZarr);

    // create a new zarr dataset
    const std::string dsName = "data";
    std::vector<size_t> shape = { 1000, 1000, 1000 };
    std::vector<size_t> chunks = { 100, 100, 100 };
    auto ds = z5::createDataset(f, dsName, "float32", shape, chunks);
    // get handle for the dataset
    const auto dsHandle = z5::filesystem::handle::Dataset(f, dsName);

    // write array to roi
    z5::types::ShapeType offset1 = { 50, 100, 150 };
    xt::xarray<float>::shape_type shape1 = { 150, 200, 100 };
    xt::xarray<float> array1(shape1, 42.0);
    z5::multiarray::writeSubarray<float>(ds, array1, offset1.begin());

    // write json attributes
    nlohmann::json attributesIn;
    attributesIn["bar"] = "foo";
    attributesIn["pi"] = 3.141593;
    z5::writeAttributes(dsHandle, attributesIn);

    // channels

    z5::createGroup(f, "channels");
    auto channelsHandle = z5::filesystem::handle::Group(f, "channels");

    const int totalChannels = 2;
    int nChannels = 0;
    for( int iCh = 0; iCh < totalChannels; ++iCh )
    {
        std::stringstream str;
        str << "channel" << iCh;
        std::string name( str.str() );
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

    z5::createGroup(f, "streams");
    auto streamsHandle = z5::filesystem::handle::Group(f, "streams");

    // constants for data
    const std::string acqDataName = "data";
    const int recSize = 16;
    const int datasetNRec = 100;
    const int chunkNRec = 10;
    const int maxRecs = datasetNRec / chunkNRec;
    std::vector< size_t > datasetShape = { datasetNRec, recSize, 1 };
    std::vector< size_t > chunkShape = {chunkNRec, recSize, 1 };

    const int totalStreams = 3;
    int nStreams = 0;
    std::vector< z5::filesystem::handle::Group > streamHandles;
    std::vector< std::unique_ptr< z5::Dataset > > acqDatasets;
    std::vector< const z5::filesystem::handle::Dataset > acqDataHandles;
    for( int iStr = 0; iStr < totalStreams; ++iStr )
    {
        std::stringstream str;
        str << "stream" << iStr;
        std::string name( str.str() );
        z5::createGroup( streamsHandle, name );
        streamHandles.emplace_back( z5::filesystem::handle::Group( streamsHandle, name ) );
        nlohmann::json oneStrGroupAttr;
        oneStrGroupAttr["name"] = name;
        z5::writeAttributes(streamHandles.back(), oneStrGroupAttr);

        z5::createGroup( streamHandles.back(), "acquisitions" );
        z5::filesystem::handle::Group acqHandle = z5::filesystem::handle::Group( streamHandles.back(), "acquisitions" );

        // create a new zarr dataset
        acqDatasets.emplace_back( z5::createDataset( acqHandle, acqDataName, "int16", datasetShape, chunkShape ) );
        // get handle for the dataset
        acqDataHandles.push_back( z5::filesystem::handle::Dataset( acqHandle, dsName ) );

        ++nStreams;
    }

    nlohmann::json strGroupAttr;
    strGroupAttr["nStreams"] = nStreams;
    z5::writeAttributes(streamsHandle, strGroupAttr);

    // simulate data taking
    z5::types::ShapeType writeOffset = { 0, 0, 0 };
    xt::xarray< int16_t >::shape_type writeShape = { 1, recSize, 1 };
    xt::xarray< int16_t > arrayPrototype( writeShape, 42.0 );

    for( int iStr = 0; iStr < totalStreams; ++iStr )
    {
        for( ; writeOffset[0] < maxRecs; writeOffset[0] += 1 )
        {
            z5::multiarray::writeSubarray< int16_t >( acqDatasets[iStr], arrayPrototype, writeOffset.begin() );
        }
    }

    return 0;
}
