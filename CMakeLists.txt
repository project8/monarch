cmake_minimum_required( VERSION 2.8 )

#########
# setup #
#########

project( Monarch )

include( ${PROJECT_SOURCE_DIR}/PackageBuilder.cmake )
pbuilder_prepare_project( 2 6 6 )
cmake_policy (SET CMP0011 NEW)

include_directories( BEFORE ${PROJECT_SOURCE_DIR}/Source )

# Egg file version
set( Egg_VERSION "3.0.0b" )


########################
# monarch dependencies #
########################

# if git is used, get the commit SHA1
find_package( Git )
if( GIT_FOUND )
    execute_process( COMMAND ${GIT_EXECUTABLE} rev-parse -q HEAD  OUTPUT_VARIABLE Monarch_GIT_COMMIT  OUTPUT_STRIP_TRAILING_WHITESPACE )
    execute_process( COMMAND ${GIT_EXECUTABLE} describe --tags --long  OUTPUT_VARIABLE Monarch_GIT_DESCRIBE  OUTPUT_STRIP_TRAILING_WHITESPACE )
endif( GIT_FOUND )

find_package( HDF5 COMPONENTS CXX )
include_directories( ${HDF5_INCLUDE_DIR} )
pbuilder_add_ext_libraries( ${HDF5_CXX_LIBRARIES} )

find_package( Protobuf )
include_directories( ${PROTOBUF_INCLUDE_DIR} )
pbuilder_add_ext_libraries( ${PROTOBUF_LIBRARIES} )
include_directories( BEFORE ${CMAKE_CURRENT_BINARY_DIR}/Protobuf )
add_subdirectory( Protobuf )

add_subdirectory( libthorax )
pbuilder_add_submodule_libraries( ${libthorax_LIBRARIES} )
# add to the RPATH to be used when installing, but only if it's not a system directory
list( FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${libthorax_LIBRARY_DIR}" isSystemDir )
if ( "${isSystemDir}" STREQUAL "-1" )
   list( APPEND CMAKE_INSTALL_RPATH ${libthorax_LIBRARY_DIR} )
endif( "${isSystemDir}" STREQUAL "-1" )
include_directories( BEFORE 
    ${PROJECT_SOURCE_DIR}/libthorax/cpp
    ${PROJECT_SOURCE_DIR}/libthorax
)


#############################
# libraries and executables #
#############################

add_subdirectory( Monarch2 )
add_subdirectory( Monarch3 )

#######################
# installation config #
#######################

pbuilder_install_config_files()

pbuilder_variables_for_parent()
