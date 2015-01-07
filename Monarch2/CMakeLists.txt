####################
# monarch2 library #
####################

set( MONARCH2_HEADERFILES
    M2Exception.hh
    M2Header.hh
    M2IO.hh
    M2Logger.hh
    M2Monarch.hh
    M2Record.hh
    M2Types.hh
)

set( MONARCH2_SOURCEFILES
    M2Exception.cc
    M2Header.cc
    M2IO.cc
    M2Logger.cc
    M2Monarch.cc
    M2Version.cc
)

configure_file( M2Version.hh.in ${CMAKE_BINARY_DIR}/M2Version.hh )
set( MONARCH2_HEADERFILES
    ${MONARCH2_HEADERFILES}
    ${CMAKE_BINARY_DIR}/M2Version.hh
)
include_directories( BEFORE ${CMAKE_CURRENT_BINARY_DIR} )

add_library( Monarch2 ${MONARCH2_SOURCEFILES} )
target_link_libraries( Monarch2 MonarchProto ${EXTERNAL_LIBRARIES} )

pbuilder_install_headers( ${MONARCH2_HEADERFILES} )
pbuilder_install_libraries( Monarch2 )


########################
# monarch2 executables #
########################

add_executable( Monarch2Dump Monarch2Dump.cc )
target_link_libraries( Monarch2Dump Monarch2 MonarchProto ${EXTERNAL_LIBRARIES})

add_executable( Monarch2Info Monarch2Info.cc )
target_link_libraries( Monarch2Info Monarch2 MonarchProto ${EXTERNAL_LIBRARIES})

add_executable( Monarch2TimeCheck Monarch2TimeCheck.cc )
target_link_libraries( Monarch2TimeCheck Monarch2 MonarchProto ${EXTERNAL_LIBRARIES})

pbuilder_install_executables (
    Monarch2Dump
    Monarch2Info
    Monarch2TimeCheck
)
