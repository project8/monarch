# Minimum cmake verison 3.12 for Scarab
cmake_minimum_required (VERSION 3.12)

#########
# setup #
#########

cmake_policy( SET CMP0048 NEW ) # version in project()
project( Monarch VERSION 3.8.7 )

list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/Scarab/cmake)
include( PackageBuilder )

# Egg file version
set( Egg_VERSION "3.2.0" )
add_definitions( -DEgg_VERSION=${Egg_VERSION} )

# Monarch library options
set( Monarch_BUILD_MONARCH2 FALSE CACHE BOOL "Build Monarch2 library (requires Protobuf)" )
set( Monarch_BUILD_MONARCH3 TRUE CACHE BOOL "Build Monarch3 library (requires HDF5)" )
set( Monarch_BUILD_PYTHON FALSE CACHE BOOL "Build Monarch3 Python bindings (requires pybind11)" )


########################
# monarch dependencies #
########################

set( PUBLIC_EXT_LIBS_M2 )
set( PUBLIC_EXT_LIBS_M3 )

######
# HDF5
######

if( Monarch_BUILD_MONARCH3 )
    # Removed C component from find_package on 9/9/16, as it was causing problems on linux builds, and the C interface isn't used
    find_package( HDF5 REQUIRED COMPONENTS CXX )
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/Monarch3 )

    if( HDF5_VERSION VERSION_LESS "1.10.1" )
        message( FATAL_ERROR "HDF5 must be v1.10.1 or newer" )
    elseif( HDF5_VERSION VERSION_GREATER_EQUAL "2.0.0" )
        list( APPEND PUBLIC_EXT_LIBS_M3 hdf5::hdf5_cpp )
    else() # 1.10.1 ... <2.0.0

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
    
    endif()

    # The following macros were originally introduced to handle compatibility with the HDF5 1.8 API and the HDF5 1.10.1 API
    # 1.10 is now the minimum version (as of 8/19/2026) so the workaround for v1.8 has been removed.
    # But these definitions still need to be used until the source code is adjusted.
    message( STATUS "HDF5 is version 1.10.1 or newer" )
    set( HDF5_DEFINITIONS -DHAS_ATTR_IFC=H5::H5Object -DHAS_GRP_IFC=H5::H5Object )

endif( Monarch_BUILD_MONARCH3 )


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


##########
# Python #
##########

if( Monarch_BUILD_PYTHON )
    find_package( Python3 REQUIRED COMPONENTS Interpreter Development )
    set( PYBIND11_PYTHON_VERSION ${Python3_VERSION} CACHE STRING "" )
    find_package( pybind11 REQUIRED )
    include_directories( ${Python3_INCLUDE_DIRS} )
endif( Monarch_BUILD_PYTHON )


#####################
# Prepare for Build #
#####################

pbuilder_prepare_project()

if( Monarch_ENABLE_TESTING )
    enable_testing()
endif( Monarch_ENABLE_TESTING )


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


##################
# package config #
##################

configure_file( ${PROJECT_SOURCE_DIR}/MonarchConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/MonarchConfig.cmake @ONLY )

pbuilder_do_package_config()
