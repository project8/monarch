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


###################
# monarch library #
###################

set( MONARCHCORE_HEADERFILES
    Source/MException.hh
    Source/MHeader.hh
    Source/MIToA.hh
    Source/MLogger.hh
    Source/MMemberVariable.hh
    Source/MMonarch.hh
    Source/MRecord.hh
    Source/MStream.hh
    Source/MTypes.hh
)

set( MONARCHCORE_SOURCEFILES
    Source/MException.cc
    Source/MHeader.cc
    Source/MIToA.cc
    Source/MLogger.cc
    Source/MMonarch.cc
    Source/MRecord.cc
    Source/MStream.cc
    Source/MVersion.cc
)

configure_file( Source/MVersion.hh.in ${CMAKE_CURRENT_BINARY_DIR}/MVersion.hh )
set( MONARCHCORE_HEADERFILES
    ${MONARCHCORE_HEADERFILES}
    ${CMAKE_CURRENT_BINARY_DIR}/MVersion.hh
)
include_directories( BEFORE ${CMAKE_CURRENT_BINARY_DIR} )

add_library( MonarchCore ${MONARCHCORE_SOURCEFILES} )
target_link_libraries( MonarchCore MonarchProto ${EXTERNAL_LIBRARIES} )

pbuilder_install_headers( ${MONARCHCORE_HEADERFILES} )
pbuilder_install_libraries( MonarchCore )


#######################
# monarch executables #
#######################

#add_executable( MonarchDump Source/MonarchDump.cpp )
#target_link_libraries( MonarchDump MonarchCore MonarchProto ${EXTERNAL_LIBRARIES})

add_executable( MonarchInfo Source/MonarchInfo.cc )
target_link_libraries( MonarchInfo MonarchCore MonarchProto ${EXTERNAL_LIBRARIES})

add_executable( MonarchWriteTest Source/MonarchWriteTest.cc )
target_link_libraries( MonarchWriteTest MonarchCore MonarchProto ${EXTERNAL_LIBRARIES})

#add_executable( MonarchTimeCheck Source/MonarchTimeCheck.cpp )
#target_link_libraries( MonarchTimeCheck MonarchCore MonarchProto ${EXTERNAL_LIBRARIES})

pbuilder_install_executables (
    #MonarchDump
    MonarchInfo
    MonarchWriteTest
    #MonarchTimeCheck
)

#############################
# libraries and executables #
#############################

add_subdirectory( Monarch2 )

#######################
# installation config #
#######################

pbuilder_install_config_files()

pbuilder_variables_for_parent()
