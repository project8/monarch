cmake_minimum_required( VERSION 2.8 )

#########
# setup #
#########

project( Monarch )

include( ${PROJECT_SOURCE_DIR}/PackageBuilder.cmake )
pbuilder_prepare_project( 2 5 2 )
cmake_policy (SET CMP0011 NEW)

include_directories( ${PROJECT_SOURCE_DIR}/Source )


########################
# monarch dependencies #
########################

# if git is used, get the commit SHA1
find_package( Git )
if( GIT_FOUND )
    execute_process( COMMAND ${GIT_EXECUTABLE} rev-parse -q HEAD  OUTPUT_VARIABLE Monarch_GIT_COMMIT  OUTPUT_STRIP_TRAILING_WHITESPACE )
    execute_process( COMMAND ${GIT_EXECUTABLE} describe --tags --long  OUTPUT_VARIABLE Monarch_GIT_DESCRIBE  OUTPUT_STRIP_TRAILING_WHITESPACE )
endif( GIT_FOUND )

set( PROTO_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Protobuf/MonarchHeader.proto
)
find_package( Protobuf )
include_directories( ${PROTOBUF_INCLUDE_DIR} )
pbuilder_add_ext_libraries( ${PROTOBUF_LIBRARIES} )
protobuf_generate_cpp( Monarch_Protobuf_Source Monarch_Protobuf_Header ${PROTO_FILES} )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_subdirectory (libthorax)
pbuilder_add_submodule_libraries (${libthorax_LIBRARIES})
# add to the RPATH to be used when installing, but only if it's not a system directory
list (FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${libthorax_LIBRARY_DIR}" isSystemDir)
if ("${isSystemDir}" STREQUAL "-1")
   list (APPEND CMAKE_INSTALL_RPATH ${libthorax_LIBRARY_DIR})
endif ("${isSystemDir}" STREQUAL "-1")
include_directories (${PROJECT_SOURCE_DIR}/libthorax/cpp)
include_directories (${PROJECT_SOURCE_DIR}/libthorax)


###################
# monarch library #
###################

set( MONARCHCORE_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Monarch.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/MonarchException.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/MonarchHeader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/MonarchIO.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/MonarchLogger.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/MonarchRecord.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/MonarchTypes.hpp
    ${Monarch_Protobuf_Header}
)

set( MONARCHCORE_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Monarch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/MonarchException.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/MonarchHeader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/MonarchIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/MonarchLogger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/MonarchVersion.cpp
    ${Monarch_Protobuf_Source}
)

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/Source/MonarchVersion.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/MonarchVersion.hpp )
set( MONARCHCORE_HEADERFILES
    ${MONARCHCORE_HEADERFILES}
    ${CMAKE_CURRENT_BINARY_DIR}/MonarchVersion.hpp
)

add_library( MonarchCore ${MONARCHCORE_SOURCEFILES} )
target_link_libraries( MonarchCore ${EXTERNAL_LIBRARIES} )

pbuilder_install_headers( ${MONARCHCORE_HEADERFILES} )
pbuilder_install_libraries( MonarchCore )


#######################
# monarch executables #
#######################

add_executable( MonarchDump ${CMAKE_CURRENT_SOURCE_DIR}/Source/MonarchDump.cpp )
target_link_libraries( MonarchDump MonarchCore ${EXTERNAL_LIBRARIES})

add_executable( MonarchInfo ${CMAKE_CURRENT_SOURCE_DIR}/Source/MonarchInfo.cpp )
target_link_libraries( MonarchInfo MonarchCore ${EXTERNAL_LIBRARIES})

add_executable( MonarchTimeCheck ${CMAKE_CURRENT_SOURCE_DIR}/Source/MonarchTimeCheck.cpp )
target_link_libraries( MonarchTimeCheck MonarchCore ${EXTERNAL_LIBRARIES})

pbuilder_install_executables (
    MonarchDump
    MonarchInfo
    MonarchTimeCheck
)


#######################
# installation config #
#######################

pbuilder_install_config_files()

pbuilder_variables_for_parent()
