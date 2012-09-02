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

dmake_library_declare( Monarch . )
dmake_library_headers( Monarch
    MonarchTypes.hpp
    Monarch.hpp
    MonarchIO.hpp
    MonarchHeader.hpp
    MonarchRecord.hpp
)
list( APPEND Monarch_HEADERS ${Monarch_Protobuf_Header} )

dmake_library_sources( Monarch
    Monarch.cpp
    MonarchIO.cpp
    MonarchHeader.cpp
    MonarchRecord.cpp
)
list( APPEND Monarch_SOURCES ${Monarch_Protobuf_Source} )

#######################
# monarch executables #
#######################

dmake_executable_declare( MonarchIOTest . )
dmake_executable_sources( MonarchIOTest
    MonarchIOTest.cpp
)

dmake_executable_declare( MonarchTest . )
dmake_executable_sources( MonarchTest
    MonarchTest.cpp
)

dmake_executable_declare( MonarchInfo . )
dmake_executable_sources( MonarchInfo
    MonarchInfo.cpp
)

#####################
# bees installation #
#####################

dmake_project_end()
target_link_libraries( Monarch ${PROTOBUF_LIBRARIES} )
