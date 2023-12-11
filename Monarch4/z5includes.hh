/*************************************************************************//**
* @file z5includes.hh
* @brief z5/Zarr types and definitions
******************************************************************************
* @copyright 
*
* This material was prepared as an account of work sponsored by an agency of
* the United States Government. Neither the United States Government nor the
* United States Department of Energy, nor Battelle, nor any of their employees,
* nor any jurisdiction or organization that has cooperated in the development
* of these materials, makes any warranty, express or implied, or assumes any
* legal liability or responsibility for the accuracy, completeness, or usefulness
* or any information, apparatus, product, software, or process disclosed, or
* represents that its use would not infringe privately owned rights. Reference
* herein to any specific commercial product, process, or service by trade name,
* trademark, manufacturer, or otherwise does not necessarily constitute or imply
* its endorsement, recommendation, or favoring by the United States Government
* or any agency thereof, or Battelle Memorial Institute. The views and opinions
* of authors expressed herein do not necessarily state or reflect those of the
* United States Government or any agency thereof.
*
*               PACIFIC NORTHWEST NATIONAL LABORATORY
*                        operated by BATTELLE
*               for the UNITED STATES DEPARTMENT OF ENERGY
*                   under Contract DE-AC05-76RL01830
*
* @author dunn874
******************************************************************************/

#include "z5/types/types.hxx"         // z5 data types

#include "z5/factory.hxx"     // factory functions to create files, groups and datasets
#include "z5/filesystem/handle.hxx"     // handles for z5 filesystem objects
#include "z5/multiarray/xtensor_access.hxx"   // io for xtensor multi-arrays
#include "z5/attributes.hxx"    // attribute functionality

// Shorter typedefs for z5 types
//typedef z5::filesystem::handle::File z5FileHandle;
//typedef z5::filesystem::handle::Dataset z5DatasetHandle;
//typedef z5::filesystem::handle::Group z5GroupHandle;
//typedef z5::filesystem::handle::File::GroupType z5GroupType;
//typedef z5::types::ShapeType z5ShapeType;
//typedef z5::types::Datatype z5Datatype;

// More modern equivalent form since we are merely aliasing instead of creating new types:  xxx_t 
// One c++/STL advantage: Using can be used with a template whereas typedef cannot
using z5FileHandle = z5::filesystem::handle::File;
using z5DatasetHandle = z5::filesystem::handle::Dataset;
using z5GroupHandle = z5::filesystem::handle::Group;
using z5GroupType = z5::filesystem::handle::File::GroupType;
using z5ShapeType = z5::types::ShapeType;
using z5Datatype = z5::types::Datatype;

