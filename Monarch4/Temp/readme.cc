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
  z5::filesystem::handle::File f("data_readme.zr");
  // if you wanted to use a different backend, for example AWS, you
  // would need to use this instead:
  // z5::s3::handle::File f;

  // create the file in zarr format
  const bool createAsZarr = true;
  z5::createFile(f, createAsZarr);

  // create a new zarr dataset
  const std::string dsName = "data";
  std::vector<size_t> shape = { 1000, 1000, 1000 };
  std::vector<size_t> chunks = { 100, 100, 100 };
  auto ds = z5::createDataset(f, dsName, "float32", shape, chunks);

  // write array to roi
  z5::types::ShapeType offset1 = { 50, 100, 150 };
  xt::xarray<float>::shape_type shape1 = { 150, 200, 100 };
  xt::xarray<float> array1(shape1, 42.0);
  z5::multiarray::writeSubarray<float>(ds, array1, offset1.begin());

  // read array from roi (values that were not written before are filled with a fill-value)
  z5::types::ShapeType offset2 = { 100, 100, 100 };
  xt::xarray<float>::shape_type shape2 = { 300, 200, 75 };
  xt::xarray<float> array2(shape2);
  z5::multiarray::readSubarray<float>(ds, array2, offset2.begin());

  // get handle for the dataset
  const auto dsHandle = z5::filesystem::handle::Dataset(f, dsName);

  // read and write json attributes
  nlohmann::json attributesIn;
  attributesIn["bar"] = "foo";
  attributesIn["pi"] = 3.141593;
  z5::writeAttributes(dsHandle, attributesIn);

  nlohmann::json attributesOut;
  z5::readAttributes(dsHandle, attributesOut);
  
  return 0;
}
