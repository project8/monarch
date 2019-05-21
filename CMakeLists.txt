# Minimum cmake verison 3.1 required for the variable CMAKE_CXX_STANDARD
cmake_minimum_required (VERSION 3.1)

#########
# setup #
#########

cmake_policy( SET CMP0048 NEW ) # version in project()
project( Monarch VERSION 3.5.8 )

list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/Scarab/cmake)

include( PackageBuilder )
pbuilder_prepare_project()

# Egg file version
set( Egg_VERSION "3.2.0" )
add_definitions( -DEgg_VERSION=${Egg_VERSION} )

set( Monarch_BUILD_MONARCH2 FALSE CACHE BOOL "Build Monarch2 library (requires Protobuf)" )
set( Monarch_BUILD_MONARCH3 TRUE CACHE BOOL "Build Monarch3 library (requires HDF5)" )


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
    # Removed C component from find_package on 9/9/16, as it was causing problems on linux builds, and the C interface isn't used
    find_package( HDF5 REQUIRED COMPONENTS CXX )
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/Monarch3 ${PROJECT_BINARY_DIR} )
    # HDF5_INCLUDE_DIR is deprecated, but is placed here to ensure the directory is picked up if an older version of cmake is used
    include_directories( BEFORE ${HDF5_INCLUDE_DIRS} ${HDF5_INCLUDE_DIR} )
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
    message( STATUS "HDF5_INCLUDE_DIR: ${HDF5_INCLUDE_DIRS}" )
    
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
        add_definitions( -DHAS_ATTR_IFC=H5::H5Location -DHAS_GRP_IFC=H5::CommonFG )
    else( ${HAVE_OLD_HDF5} )
        message( STATUS "HDF5 is version 1.10.1 or newer" )
        add_definitions( -DHAS_ATTR_IFC=H5::H5Object -DHAS_GRP_IFC=H5::H5Object )
    endif( ${HAVE_OLD_HDF5} )

endif( Monarch_BUILD_MONARCH3 )


##########
# Protobuf
##########

if( Monarch_BUILD_MONARCH2 )
    find_package( Protobuf REQUIRED )
    include_directories( BEFORE ${PROJECT_SOURCE_DIR}/Monarch2 ${PROJECT_BINARY_DIR}/Monarch2 ${PROJECT_BINARY_DIR} )
    include_directories( ${PROTOBUF_INCLUDE_DIR} )
    pbuilder_add_ext_libraries( ${PROTOBUF_LIBRARIES} )
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
