# Minimum cmake verison 3.0 required by the way CMake handles rpath information on Mac OSX.
cmake_minimum_required (VERSION 3.0)
cmake_policy (SET CMP0042 NEW)

#########
# setup #
#########

project( Monarch )

list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
#set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake" )
message(STATUS "path with append is:${CMAKE_MODULE_PATH}")
include( PackageBuilder)
pbuilder_prepare_project( 2 6 7 )
cmake_policy (SET CMP0011 NEW)

# Egg file version
set( Egg_VERSION "3.0.0b4" )

set( Monarch_BUILD_MONARCH2 FALSE CACHE BOOL "Build Monarch2 library (requires Protobuf)" )
set( Monarch_BUILD_MONARCH3 TRUE CACHE BOOL "Build Monarch3 library (requires HDF5)" )

set( Monarch_ENABLE_DOC_BUILD FALSE CACHE BOOL "Enable the options to build the documentation (requires Doxygen and Sphynx)" )

########################
# monarch dependencies #
########################

# if git is used, get the commit SHA1
find_package( Git )
if( GIT_FOUND )
    execute_process( COMMAND ${GIT_EXECUTABLE} rev-parse -q HEAD  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE Monarch_GIT_COMMIT  OUTPUT_STRIP_TRAILING_WHITESPACE )
    execute_process( COMMAND ${GIT_EXECUTABLE} describe --tags --long  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE Monarch_GIT_DESCRIBE  OUTPUT_STRIP_TRAILING_WHITESPACE )
endif( GIT_FOUND )

if( Monarch_BUILD_MONARCH3 )
    find_package( HDF5 REQUIRED COMPONENTS CXX )
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/Monarch3 ${PROJECT_BINARY_DIR} )
    include_directories( BEFORE ${HDF5_INCLUDE_DIR} )
    pbuilder_add_ext_libraries( ${HDF5_CXX_LIBRARIES} )
endif( Monarch_BUILD_MONARCH3 )

if( Monarch_BUILD_MONARCH2 )
    find_package( Protobuf REQUIRED )
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/Monarch2 ${PROJECT_BINARY_DIR} )
    include_directories( ${PROTOBUF_INCLUDE_DIR} )
    pbuilder_add_ext_libraries( ${PROTOBUF_LIBRARIES} )
    include_directories( BEFORE ${CMAKE_CURRENT_BINARY_DIR}/Protobuf )
    add_subdirectory( Protobuf )
endif( Monarch_BUILD_MONARCH2 )

add_subdirectory( libthorax )
pbuilder_add_submodule_libraries( ${libthorax_LIBRARIES} )
include_directories( ${libthorax_DEP_INCLUDE_DIRS} )
message(STATUS "from monarch, libthorax dep include dirs: ${libthorax_DEP_INCLUDE_DIRS}")
# add to the RPATH to be used when installing, but only if it's not a system directory
list( FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${libthorax_LIBRARY_DIR}" isSystemDir )
if ( "${isSystemDir}" STREQUAL "-1" )
   list( APPEND CMAKE_INSTALL_RPATH ${libthorax_LIBRARY_DIR} )
endif( "${isSystemDir}" STREQUAL "-1" )


#############################
# libraries and executables #
#############################

if( Monarch_BUILD_MONARCH2 )
    add_subdirectory( Monarch2 )
endif( Monarch_BUILD_MONARCH2 )

if( Monarch_BUILD_MONARCH3 )
    add_subdirectory( Monarch3 )
endif( Monarch_BUILD_MONARCH3 )

#######################
# installation config #
#######################

pbuilder_install_config_files()

pbuilder_variables_for_parent()

#################
# documentation #
#################

if( Monarch_ENABLE_DOC_BUILD )
    # Doxygen documentation can be built with "make doc" (it's not included with the "all" make target)
    # By default it builds in the source tree.
    add_subdirectory (Documentation)
endif( Monarch_ENABLE_DOC_BUILD )
