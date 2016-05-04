# Minimum cmake verison 3.1 required for the variable CMAKE_CXX_STANDARD
cmake_minimum_required (VERSION 3.1)

#########
# setup #
#########

cmake_policy( SET CMP0048 NEW ) # version in project()
project( Monarch VERSION 3.2.5 )

list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/Scarab/cmake)

include( PackageBuilder )
pbuilder_prepare_project()

# Egg file version
set( Egg_VERSION "3.2.0" )
add_definitions( -DEgg_VERSION=${Egg_VERSION} )

set( Monarch_BUILD_MONARCH2 FALSE CACHE BOOL "Build Monarch2 library (requires Protobuf)" )
set( Monarch_BUILD_MONARCH3 TRUE CACHE BOOL "Build Monarch3 library (requires HDF5)" )

set( Monarch_ENABLE_DOC_BUILD FALSE CACHE BOOL "Enable the options to build the documentation (requires Doxygen and Sphynx)" )

########################
# monarch dependencies #
########################

########
# Scarab
########

pbuilder_add_submodule( Scarab Scarab/library )


######
# HDF5
######

if( Monarch_BUILD_MONARCH3 )
    find_package( HDF5 REQUIRED COMPONENTS C CXX )
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/Monarch3 ${PROJECT_BINARY_DIR} )
    include_directories( BEFORE ${HDF5_INCLUDE_DIR} )
    if( WIN32 )
        # In testing on 3/24/16 I found that the .dlls were being found, not the .libs.  I don't understand why, but this fixes the problem if it occurs.
        # HDF5_CXX_LIBRARIES was blank in testing, so I commented out the first line.
        #string( REPLACE ".dll" ".lib" HDF5_CXX_LIBRARIES ${HDF5_CXX_LIBRARIES} )
        string( REPLACE ".dll" ".lib" HDF5_C_LIBRARY ${HDF5_C_LIBRARY} )
        string( REPLACE ".dll" ".lib" HDF5_CXX_LIBRARY ${HDF5_CXX_LIBRARY} )
    endif( WIN32 )
    # It seems that all 3 of the following lines are needed to make Monarch compile on both Mac/Linux and Windows
    pbuilder_add_ext_libraries( ${HDF5_CXX_LIBRARIES} )
    pbuilder_add_ext_libraries( ${HDF5_C_LIBRARY} )
    pbuilder_add_ext_libraries( ${HDF5_CXX_LIBRARY} )
    message( STATUS "HDF5_CXX_LIBRARIES: ${HDF5_CXX_LIBRARIES}" )
    message( STATUS "HDF5_C_LIBRARY: ${HDF5_C_LIBRARY}" )
    message( STATUS "HDF5_CXX_LIBRARY: ${HDF5_CXX_LIBRARY}" )
endif( Monarch_BUILD_MONARCH3 )


##########
# Protobuf
##########

if( Monarch_BUILD_MONARCH2 )
    find_package( Protobuf REQUIRED )
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/Monarch2 ${PROJECT_BINARY_DIR} )
    include_directories( ${PROTOBUF_INCLUDE_DIR} )
    pbuilder_add_ext_libraries( ${PROTOBUF_LIBRARIES} )
    include_directories( BEFORE ${CMAKE_CURRENT_BINARY_DIR}/Protobuf )
    add_subdirectory( Protobuf )
endif( Monarch_BUILD_MONARCH2 )



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
