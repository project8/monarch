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

    // get handle to a File on the filesystem
    z5::filesystem::handle::File f("readme2.zr");

    // readme

cout << "open the zarr dataset from file: /data\n";
    const std::string dsName = "data";
    const auto dsHandle = z5::filesystem::handle::Dataset(f, dsName);
    auto ds = z5::openDataset(f, dsName);

cout << "read array from roi (values that were not written before are filled with a fill-value)\n";
    z5::types::ShapeType offset2 = { 100, 100, 100 };
    xt::xarray<float>::shape_type shape2 = { 300, 200, 75 };
    
    xt::xarray<float> array2(shape2);
    z5::multiarray::readSubarray<float>(ds, array2, offset2.begin());

    // Dump the data array
    std::cout << "\nData:" << std::endl;
    std::cout << array2 << std::endl;

cout << "read the attributes of dataset: /data\n";
    nlohmann::json attributesOut;   // object to receive attributes
    z5::readAttributes(dsHandle, attributesOut);
    std::cout << "Attributes: ";
    std::cout << attributesOut << std::endl;

    // channels
    auto channelsHandle = z5::filesystem::handle::Group( f, "channels" );
    nlohmann::json chGroupAttr;
    z5::readAttributes( channelsHandle, chGroupAttr );
    std::cout << "\nChannels" << std::endl;
    std::cout << "N channels: " << chGroupAttr["nChannels"] << endl << endl;

    int nChannels = chGroupAttr["nChannels"];
    for( int iCh = 0; iCh < nChannels; ++iCh )
    {
        std::stringstream str;
        str << "channel" << iCh;
        std::string name( str.str() );
        
        std::cout << "Extracting channel <" << name << ">" << endl;
        z5::filesystem::handle::Group channelHandle = z5::filesystem::handle::Group( channelsHandle, name );
        
        nlohmann::json oneChGroupAttr;
        z5::readAttributes( channelHandle, oneChGroupAttr );
        std::cout << "\tChannel " << iCh << " attributes: " << oneChGroupAttr << std::endl;
    }

    // streams
    auto streamsHandle = z5::filesystem::handle::Group( f, "streams" );
    nlohmann::json strGroupAttr;
    
    // Extract the stream attribute: nStreams to indicate how many streams are stored
    z5::readAttributes( streamsHandle, strGroupAttr );
    std::cout << "\nStreams" << std::endl;
    std::cout << "N streams: " << strGroupAttr["nStreams"] << endl << endl;

    // Extract each of the stream attributes
    int nStreams = strGroupAttr["nStreams"];
    for( int iStr = 0; iStr < nStreams; ++iStr )
    {
        std::stringstream str;
        str << "stream" << iStr;
        std::string name( str.str() );
        std::cout << "Extracting stream <" << name << ">" << endl;
        
        z5::filesystem::handle::Group streamHandle = z5::filesystem::handle::Group( streamsHandle, name );
        
        nlohmann::json oneStrGroupAttr;
        z5::readAttributes( streamHandle, oneStrGroupAttr );
        std::cout << "\tStream " << iStr << " attributes: " << oneStrGroupAttr << std::endl;
    }

    return 0;
}