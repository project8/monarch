cmake_minimum_required( VERSION 2.8 )

#########
# setup #
#########

project( Monarch )

include( ${PROJECT_SOURCE_DIR}/PackageBuilder.cmake )
pbuilder_prepare_project( 2 0 0 )

include_directories( ${CMAKE_SOURCE_DIR}/Include )


########################
# monarch dependencies #
########################

find_package( Protobuf )
include_directories( ${PROTOBUF_INCLUDE_DIR} )
protobuf_generate_cpp( Monarch_Protobuf_Source Monarch_Protobuf_Header ${CMAKE_CURRENT_SOURCE_DIR}/Protobuf/MonarchHeader.proto )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
pbuilder_add_ext_libraries( ${PROTOBUF_LIBRARIES} )


###################
# monarch library #
###################

set( MONARCHCORE_HEADERFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/MonarchTypes.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/MonarchException.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/MonarchHeader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/MonarchRecord.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/MonarchIO.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/Monarch.hpp
    ${Monarch_Protobuf_Header}
)

set( MONARCHCORE_SOURCEFILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/MonarchException.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/MonarchHeader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/MonarchRecord.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/MonarchIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Monarch.cpp
    ${Monarch_Protobuf_Source}
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
