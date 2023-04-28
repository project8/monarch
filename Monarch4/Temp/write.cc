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

    const int totalStreams = 3;
    int nStreams = 0;
    std::vector< z5::filesystem::handle::Group > streamHandles;
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

        ++nStreams;
    }

    nlohmann::json strGroupAttr;
    strGroupAttr["nStreams"] = nStreams;
    z5::writeAttributes(streamsHandle, strGroupAttr);

    // take data
    const int recSize = 1024;


    return 0;
}
