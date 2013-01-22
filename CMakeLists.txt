##################
# get that gravy #
##################

cmake_minimum_required( VERSION 2.8 )
include( $ENV{HOME}/.dmake/dmake.cmake )

#########
# setup #
#########

dmake_project_begin( Monarch 0 1 0 )

########################
# monarch dependencies #
########################

find_package( Protobuf )
protobuf_generate_cpp( Monarch_Protobuf_Source Monarch_Protobuf_Header ${Monarch_ROOT_DIRECTORY}/Protobuf/MonarchHeader.proto )

###################
# monarch library #
###################

dmake_library_declare( MonarchCore . )
dmake_library_headers( MonarchCore
    MonarchTypes.hpp
    MonarchException.hpp
    MonarchHeader.hpp
    MonarchRecord.hpp
    MonarchIO.hpp
    Monarch.hpp
)
dmake_library_headers_external( MonarchCore
    ${Monarch_Protobuf_Header}
)

dmake_library_sources( MonarchCore
    MonarchException.cpp
    MonarchHeader.cpp
    MonarchRecord.cpp
    MonarchIO.cpp
    Monarch.cpp
)
dmake_library_sources_external( MonarchCore
    ${Monarch_Protobuf_Source}
)
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

#######################
# monarch executables #
#######################

dmake_executable_declare( MonarchDump . )
dmake_executable_sources( MonarchDump
    MonarchDump.cpp
)

dmake_executable_declare( MonarchInfo . )
dmake_executable_sources( MonarchInfo
    MonarchInfo.cpp
)

########################
# monarch installation #
########################

dmake_project_end()
target_link_libraries( MonarchCore ${PROTOBUF_LIBRARIES} )
