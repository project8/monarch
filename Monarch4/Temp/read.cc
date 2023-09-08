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

// Shorter typedefs for z5 types
typedef z5::filesystem::handle::File z5FileHandle;
typedef z5::filesystem::handle::Dataset z5DatasetHandle;
typedef z5::filesystem::handle::Group z5GroupHandle;

using namespace std;

string z5FileName = "readme2.zr";

int main() {

    // get handle to a File on the filesystem
    // z5::filesystem::handle::File f("readme2.zr");
    z5::filesystem::handle::File f(z5FileName);

    // file: readme2.zr
    // Group: /data
    cout << "open the zarr dataset from file: /data\n";
    const std::string dsName = "data";
// const auto dsHandle = z5::filesystem::handle::Dataset(f, dsName);
    const auto dsHandle = z5DatasetHandle(f, dsName);
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

    // Group: /channels
// auto channelsHandle = z5::filesystem::handle::Group( f, "channels" );
    auto channelsHandle = z5GroupHandle( f, "channels" );
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
// z5::filesystem::handle::Group channelHandle = z5::filesystem::handle::Group( channelsHandle, name );
        z5GroupHandle channelHandle = z5GroupHandle( channelsHandle, name );
        
        nlohmann::json oneChGroupAttr;
        z5::readAttributes( channelHandle, oneChGroupAttr );
        std::cout << "\tChannel " << iCh << " attributes: " << oneChGroupAttr << std::endl;
    }

    // Group: /streams attributes
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
        
// z5::filesystem::handle::Group streamHandle = z5::filesystem::handle::Group( streamsHandle, name );
        z5GroupHandle streamHandle = z5GroupHandle( streamsHandle, name );
        
        nlohmann::json oneStrGroupAttr;
        z5::readAttributes( streamHandle, oneStrGroupAttr );
        std::cout << "\tStream " << iStr << " attributes: " << oneStrGroupAttr << std::endl;
    }

    // constants for data
    const int totalChannels = 2;
    const int recSize = 16;
    const int datasetNRec = 100;
    const int chunkNRec = 10;
    const int maxRecs = datasetNRec / chunkNRec;
    z5::types::ShapeType readOffset = { 0, 0, 0 };

    // Reads streamX into the same buffer:  readArray[]
    const int numRecords = 2;
    xt::xarray< int16_t >::shape_type readShape = { numRecords, recSize, 1 };
    xt::xarray< int16_t > readArray( readShape, 0.0 );

    // Read the stream records from file
    std::cout << "Reading streamX/acquisition records\n";
    for( int iStr = 0; iStr < nStreams; ++iStr )
    {
        std::stringstream str;
        std::string streamXName;

        // Get handle to stream[iStr]: streamXHandle
        str << "stream" << iStr;        // stream name: "stream0", "stream1", ...
        streamXName = str.str();
        std::cout << streamXName << "[" << iStr << "]\n";

        // Dataset is opened by path name: dsXName       
        std::string dsXName = "streams/" + streamXName + "/acquisitions/data";   

        // Handle the file at the end of the path:  as base path    
// z5::filesystem::handle::File fX(z5FileName);
        z5FileHandle fX(z5FileName);
// auto dsXHandle = z5::filesystem::handle::Dataset(fX, dsXName);
        auto dsXHandle = z5DatasetHandle(fX, dsXName);
        
        // Group: /streams/streamX/acquisitions/data
// auto acqXHandle = z5::filesystem::handle::Group( fX, dsXName );
        auto acqXHandle = z5GroupHandle( fX, dsXName );
        
        // Open the dataset from path
        cout << "openDataset(): " <<  z5FileName << "/" << dsXName << endl;       
        auto dsX = z5::openDataset(fX, dsXName);

        // Read into array buffer from dataset
        z5::multiarray::readSubarray<int16_t>(dsX, readArray, readOffset.begin());

        // Dump array buffer to screen
        std::cout << readArray << std::endl;
    }
}


// Zarr file-directory format
//
// (base) dunn874@we36418u:~/Projects/project8/monarch/build$ tree -a readme2.zr/
// readme2.zr/
// ├── channels
// │   ├── channel0
// │   │   ├── .zattrs
// │   │   └── .zgroup
// │   ├── channel1
// │   │   ├── .zattrs
// │   │   └── .zgroup
// │   ├── .zattrs
// │   └── .zgroup
// ├── data
// │   ├── 0.1.1
// │   ├── 0.1.2
// │   ├── 0.2.1
// │   ├── 0.2.2
// │   ├── 1.1.1
// │   ├── 1.1.2
// │   ├── 1.2.1
// │   ├── 1.2.2
// │   ├── .zarray
// │   └── .zattrs
// ├── streams
// │   ├── stream0
// │   │   ├── acquisitions
// │   │   │   ├── data
// │   │   │   │   ├── 0.0.0
// │   │   │   │   └── .zarray
// │   │   │   └── .zgroup
// │   │   ├── .zattrs
// │   │   └── .zgroup
// │   ├── stream1
// │   │   ├── acquisitions
// │   │   │   ├── data
// │   │   │   │   ├── 0.0.0
// │   │   │   │   └── .zarray
// │   │   │   └── .zgroup
// │   │   ├── .zattrs
// │   │   └── .zgroup
// │   ├── stream2
// │   │   ├── acquisitions
// │   │   │   ├── data
// │   │   │   │   ├── 0.0.0
// │   │   │   │   └── .zarray
// │   │   │   └── .zgroup
// │   │   ├── .zattrs
// │   │   └── .zgroup
// │   ├── .zattrs
// │   └── .zgroup
// └── .zgroup

// Data record attributes
// cat readme2.zr/streams/stream0/acquisitions/data/.zarray 
// {
//     "chunks": [
//         10,
//         16,
//         1
//     ],
//     "compressor": null,
//     "dimension_separator": ".",
//     "dtype": "<i2",
//     "fill_value": 0.0,
//     "filters": null,
//     "order": "C",
//     "shape": [
//         100,
//         16,
//         1
//     ],
//     "zarr_format": 2
// }



