# Minimum cmake verison 3.12 for Scarab
cmake_minimum_required (VERSION 3.12)

#########
# setup #
#########

cmake_policy( SET CMP0048 NEW ) # version in project()
project( Monarch VERSION 3.8.2 )

list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/Scarab/cmake)
include( PackageBuilder )

set( CMAKE_CXX_STANDARD 17 )

# Egg file version
set( Egg_VERSION "4.0.0" )
add_definitions( -DEgg_VERSION=${Egg_VERSION} )

# Monarch library options
set( Monarch_BUILD_MONARCH2 FALSE CACHE BOOL "Build Monarch2 library (requires Protobuf)" )
set( Monarch_BUILD_MONARCH3 FALSE CACHE BOOL "Build Monarch3 library (requires HDF5)" )
set( Monarch_BUILD_MONARCH4 TRUE CACHE BOOL "Build Monarch4 library (requires Zarr)" )


########################
# monarch dependencies #
########################

set( PUBLIC_EXT_LIBS_M2 )
set( PUBLIC_EXT_LIBS_M3 )
set( PUBLIC_EXT_LIBS_M4 )
set( PRIVATE_EXT_LIBS_M4 )


######
# Zarr
######

if( Monarch_BUILD_MONARCH4 )
#    set( Monarch_BUILD_MONARCH3 TRUE CACHE BOOL "Build Monarch3 library (requires HDF5)" )
    message("Build Monarch4 library (requires Zarr)")

    find_package( z5 REQUIRED )
    find_package(xtl REQUIRED)
    find_package( xtensor REQUIRED )
    include_directories( BEFORE 
        ${PROJECT_SOURCE_DIR}/Monarch4 
        ${z5_INCLUDE_DIRS}
    )
    list( APPEND PUBLIC_EXT_LIBS_M4 
        nlohmann_json::nlohmann_json
        xtensor
    )

endif( Monarch_BUILD_MONARCH4 )


######
# HDF5
######

if( Monarch_BUILD_MONARCH3 )
    # Removed C component from find_package on 9/9/16, as it was causing problems on linux builds, and the C interface isn't used
    find_package( HDF5 REQUIRED COMPONENTS CXX )
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/Monarch3 )

    # HDF5 doesn't currently define imported targets.  Therefore we have to do it ourselves.

    # Library variables returned by the FindHDF5 module are paths to the actual libraries.
    # We'll store all libraries, regardless of platform, in a single list.
    set( ALL_HDF5_LIBRARIES )
    if( WIN32 )

        # In testing on 3/24/16 I found that the .dlls were being found, not the .libs.  I don't understand why, but this fixes the problem if it occurs.
        # HDF5_CXX_LIBRARIES was blank in testing, so I commented out the first line.
        #string( REPLACE ".dll" ".lib" HDF5_CXX_LIBRARIES ${HDF5_CXX_LIBRARIES} )
        string( REPLACE ".dll" ".lib" HDF5_C_LIBRARY ${HDF5_C_LIBRARY} )
        string( REPLACE ".dll" ".lib" HDF5_CXX_LIBRARY ${HDF5_CXX_LIBRARY} )

        list( APPEND ALL_HDF5_LIBRARIES ${HDF5_C_LIBRARY} ${HDF5_CXX_LIBRARY} )

        message( STATUS "HDF5_C_LIBRARY: ${HDF5_C_LIBRARY}" )
        message( STATUS "HDF5_CXX_LIBRARY: ${HDF5_CXX_LIBRARY}" )

    else()

        list( APPEND ALL_HDF5_LIBRARIES ${HDF5_CXX_LIBRARIES} )
        message( STATUS "HDF5_CXX_LIBRARIES: ${HDF5_CXX_LIBRARIES}" )

    endif()

    # Now we create an imported library for the C++ library, and assign all other libraries as dependencies
    add_library( HDF5CXXLib SHARED IMPORTED )
    foreach( LIBRARY_PATH IN LISTS ALL_HDF5_LIBRARIES )
        get_filename_component( LIBRARY ${LIBRARY_PATH} NAME_WE )
        string(TOLOWER ${LIBRARY} LIBRARY_LOWER ) # avoid any potential case variation
        if( LIBRARY_LOWER STREQUAL "libhdf5_cpp" )
            # this is the C++ library
            set_target_properties( HDF5CXXLib PROPERTIES IMPORTED_LOCATION ${LIBRARY_PATH} )

            # We want the relevant include directories to be propagated as part of the HDF5 library properties
            # HDF5_INCLUDE_DIR is deprecated, but is placed here to ensure the directory is picked up if an older version of cmake is used
            target_include_directories( HDF5CXXLib INTERFACE ${HDF5_INCLUDE_DIRS} ${HDF5_INCLUDE_DIR} )
        else()
            # this is a dependency library
            add_library( ${LIBRARY} SHARED IMPORTED )
            set_target_properties(  ${LIBRARY} PROPERTIES IMPORTED_LOCATION ${LIBRARY_PATH} )

            # We want the dependency libraries to be propagated as part of the HDF5 library properties
            target_link_libraries( HDF5CXXLib INTERFACE ${LIBRARY} )
        endif()
    endforeach()

    # Add the imported library to the set of external libraries
    list( APPEND PUBLIC_EXT_LIBS_M3 HDF5CXXLib )
    
    # The following is a work-around to provide compatibility with the HDF5 1.8 API and the HDF5 1.10 API
    # It should be removed in favor of the 1.10 API once we're able to make 1.10 a requirement
    # If you upgrade your version of HDF5, you'll need to clear your CMake cache before rebuilding
    # First, test for the version of HDF5 in use
    include( CheckCXXSourceCompiles )
    set( CMAKE_REQUIRED_QUIET FALSE )
    set( CMAKE_REQUIRED_LIBRARIES ${HDF5_CXX_LIBRARIES} )
    set( CMAKE_REQUIRED_INCLUDES ${HDF5_INCLUDE_DIRS} )
    check_cxx_source_compiles("
        #include <H5Cpp.h>
        int main()
        {
            H5::CommonFG* tester = new H5::H5File(\"new_file.h5\", H5F_ACC_TRUNC);
            tester->openGroup(\"my_group\");
            return 0;
        }
        " HAVE_OLD_HDF5 )
    
    # Then set preprocessor macros accordingly
    if( ${HAVE_OLD_HDF5} )
        message( STATUS "HDF5 is version 1.10.0 or older" )
        set( HDF5_DEFINITIONS -DHAS_ATTR_IFC=H5::H5Location -DHAS_GRP_IFC=H5::CommonFG)
    else( ${HAVE_OLD_HDF5} )
        message( STATUS "HDF5 is version 1.10.1 or newer" )
        set( HDF5_DEFINITIONS -DHAS_ATTR_IFC=H5::H5Object -DHAS_GRP_IFC=H5::H5Object )
    endif( ${HAVE_OLD_HDF5} )

endif( Monarch_BUILD_MONARCH3 )

list( APPEND PUBLIC_EXT_LIBS_M4 ${PUBLIC_EXT_LIBS_M3} )

##########
# Protobuf
##########

if( Monarch_BUILD_MONARCH2 )
    find_package( Protobuf REQUIRED )
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/Monarch2 ${PROJECT_BINARY_DIR}/Monarch2 ${PROJECT_BINARY_DIR} )

    list( APPEND PUBLIC_EXT_LIBS_M2 
        protobuf::libprotobuf 
        protobuf::libprotobuf-lite 
        protobuf::libprotoc
    )
endif( Monarch_BUILD_MONARCH2 )


#####################
# Prepare for Build #
#####################

pbuilder_prepare_project()


########
# Scarab
########

pbuilder_add_submodule( Scarab Scarab )

# All parts of Monarch use scarab
pbuilder_use_sm_library( Scarab Scarab )


#############################
# libraries and executables #
#############################

if( Monarch_BUILD_MONARCH2 )
    add_subdirectory( Monarch2 )
endif( Monarch_BUILD_MONARCH2 )

if( Monarch_BUILD_MONARCH3 )
    add_subdirectory( Monarch3 )
endif( Monarch_BUILD_MONARCH3 )

if( Monarch_BUILD_MONARCH4 )
    add_subdirectory( Monarch4 )
endif( Monarch_BUILD_MONARCH4 )


##################
# package config #
##################

configure_file( ${PROJECT_SOURCE_DIR}/MonarchConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/MonarchConfig.cmake @ONLY )

pbuilder_do_package_config()
