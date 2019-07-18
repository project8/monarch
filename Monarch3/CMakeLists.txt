####################
# monarch3 library #
####################

set( MONARCH3_HEADERFILES
    M3Constants.hh
    M3DataInterface.hh
    M3Exception.hh
    M3HDF5Mutex.hh
    M3Header.hh
    M3IToA.hh
    M3MemberVariable.hh
    M3Monarch.hh
    M3Record.hh
    M3Stream.hh
    M3Types.hh
    M3Version.hh
)

set( MONARCH3_SOURCEFILES
    M3Exception.cc
    M3HDF5Mutex.cc
    M3Header.cc
    M3IToA.cc
    M3Monarch.cc
    M3Record.cc
    M3Stream.cc
)

configure_file( M3Version.cc.in ${CMAKE_CURRENT_BINARY_DIR}/M3Version.cc )
set( MONARCH3_SOURCEFILES
    ${MONARCH3_SOURCEFILES}
    ${CMAKE_CURRENT_BINARY_DIR}/M3Version.cc
)

pbuilder_library( Monarch3 MONARCH3_SOURCEFILES "" )

pbuilder_install_headers( ${MONARCH3_HEADERFILES} )

add_subdirectory( Executables )
