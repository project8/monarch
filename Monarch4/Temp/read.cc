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

string z5FileName = "readme2.zr";

int main() {

    // get handle to a File on the filesystem
    // z5::filesystem::handle::File f("readme2.zr");
    z5::filesystem::handle::File f(z5FileName);

    // file: readme2.zr
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

    // constants for data
    const int totalChannels = 2;
    const int recSize = 16;
    const int datasetNRec = 100;
    const int chunkNRec = 10;
    const int maxRecs = datasetNRec / chunkNRec;
    z5::types::ShapeType readOffset = { 0, 0, 0 };

    // Using nStreams from above (loaded from stream attributes)
    // simulate data taking by storing data in each of the stream's dataset
    z5::types::ShapeType writeOffset = { 0, 0, 0 };
    xt::xarray< int16_t >::shape_type readShape = { 1, recSize, 1 };
    xt::xarray< int16_t > readArray( readShape, 0.0 );

    // Read the stream records from file
    std::cout << "Reading records" << std::endl;
    for( int iStr = 0; iStr < nStreams; ++iStr )
    {
        std::stringstream str;
        std::string streamXName;
        // z5::filesystem::handle::Group streamXHandle;
        // z5::filesystem::handle::Group acqHandle;

        // Get handle to stream[iStr]: streamXHandle
        str << "stream" << iStr;        // stream name: "stream0", "stream1", ...
        streamXName = str.str();
std::cout << streamXName << "[" << iStr << "]\n";
        // z5::filesystem::handle::Group streamXHandle = z5::filesystem::handle::Group( streamsHandle, streamXName );
        
        const std::string dsXName = "streams/" + streamXName + "/acquisitions/data";        
        z5::filesystem::handle::File fX(z5FileName);
        auto dsXHandle = z5::filesystem::handle::Dataset(fX, dsXName);
cout << "openDataset(): " <<  z5FileName << "/" << dsXName << endl;       
        auto dsX = z5::openDataset(fX, dsXName);

std::cout << "readSubarray()\n";
        // read array from roi (values that were not written before are filled with a fill-value)
        // z5::multiarray::readSubarray<float>(dsX, readArray, readOffset.begin());
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
// cat readme2.zr/streams/stream0/acquisitions/.zarray 
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


#if 0
    for( int iStr = 0; iStr < nStreams; ++iStr )
    {
        std::stringstream str;
        std::string streamXName;
        // z5::filesystem::handle::Group streamXHandle;
        // z5::filesystem::handle::Group acqHandle;

        // Get handle to stream[iStr]: streamXHandle
        str << "stream" << iStr;        // stream name: "stream0", "stream1", ...
        streamXName = str.str();
std::cout << streamXName << "[" << iStr << "]\n";
        z5::filesystem::handle::Group streamXHandle = z5::filesystem::handle::Group( streamsHandle, streamXName );
        
        // Get handle to streamX "acquisition" group
        z5::filesystem::handle::Group acqXHandle = z5::filesystem::handle::Group( streamXHandle, "acquisitions" );

        std::vector< size_t > datasetShape = { datasetNRec, recSize, 1 };
        std::vector< size_t > chunkShape = {chunkNRec, recSize, 1 };

std::cout << "createDataset()\n";
        z5::DatasetMetadata::DatasetMetadata dsMeta = z5::DatasetMetadata::DatasetMetadata("int16",datasetShape,chunkShape,true);
        auto dsX = z5::createDataset( acqXHandle, "dataSetX", dsMeta );
std::cout << "readSubarray()\n";
        // read array from roi (values that were not written before are filled with a fill-value)
        z5::multiarray::readSubarray<float>(dsX, readArray, readOffset.begin());

        // // Get "acquisition/data" group
        // z5::filesystem::handle::Group dataXHandle = z5::filesystem::handle::Group( acqXHandle, "data" );

        // z5::DatasetMetadata metaXData;
        // auto dsX = z5::createDatasetMetadata(acqXHandle,"data",metaXData);

//         nlohmann::json dataXAttributes;
//         z5::readAttributes(acqXHandle,dataXAttributes);
// std::cout << dataXAttributes << std::endl;

        // // Read shape attributes
        // nlohmann::json shapeXAttr;
        // // z5::readAttributes( dataXHandle, shapeXAttr );
        // z5::readAttributes( acqXHandle, shapeXAttr );
        // std::cout << "Attributes: ";
        // std::cout << shapeXAttr << std::endl;

        // Read Dataset for streamX:  readme2.zr/streams/stream0/acquisitions/.zarray 
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


        // virtual bool z5::Dataset::readChunk()

// cout << "read the attributes of dataset: /data\n";
//     nlohmann::json dataXAttributes;   // object to receive attributes
//     z5::readAttributes(dsHandle, dataXAttributes);
//     std::cout << "Attributes: ";
//     std::cout << attributesOut << std::endl;

        // nlohmann::json dataXGroupAttr;
        // z5::readAttributes( dataXHandle, dataXGroupAttr );
        // std::cout << "\tStream " << iStr << " attributes: " << dataXGroupAttr << std::endl;

        // const std::string acqDataName = "data";
        // const int recSize = 16;
        // const int datasetNRec = 100;
        // const int chunkNRec = 10;
        // const int maxRecs = datasetNRec / chunkNRec;
        // z5::createDataset( acqXHandle, acqDataName, "int16", datasetShape, chunkShape );

        // // create a new Dataset for the data of this stream
        // acqDatasets.emplace_back( z5::createDataset( acqHandle, acqDataName, "int16", datasetShape, chunkShape ) );
        
        // // get handle for the dataset for this stream
        // acqDataHandles.push_back( z5::filesystem::handle::Dataset( acqHandle, dsName ) );



        // //TODO get handle to Dataset
        // //TODO create xarray buffer for Dataset

        // // Read streamX dataset into xarray
        // std::cout << "Read records from stream " << iStr << std::endl;
        // for( writeOffset[0] = 0; writeOffset[0] < maxRecs; writeOffset[0] += 1 )
        // {
        //     z5::multiarray::readSubarray< int16_t >( acqDatasets[iStr], arrayPrototype, writeOffset.begin() );
        // }
    }


#endif

