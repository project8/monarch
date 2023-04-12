####################
# monarch4 library #
####################

set( MONARCH4_HEADERFILES
    M4Constants.hh
    M4DataInterface.hh
    M4Exception.hh
    M4Header.hh
    M4IToA.hh
    M4MemberVariable.hh
    M4Monarch.hh
    M4Record.hh
    M4Stream.hh
    M4Types.hh
    M4Version.hh
)

set( MONARCH4_SOURCEFILES
    M4Exception.cc
    M4Header.cc
    M4IToA.cc
    M4Monarch.cc
    M4Record.cc
    M4Stream.cc
)

configure_file( M4Version.cc.in ${CMAKE_CURRENT_BINARY_DIR}/M4Version.cc )
set( MONARCH4_SOURCEFILES
    ${MONARCH4_SOURCEFILES}
    ${CMAKE_CURRENT_BINARY_DIR}/M4Version.cc
)


pbuilder_library(
    TARGET Monarch4
    SOURCES ${MONARCH4_SOURCEFILES}
    PUBLIC_EXTERNAL_LIBRARIES ${PUBLIC_EXT_LIBS_M4}
    COMPILE_DEFINITIONS ${HDF5_DEFINITIONS}
)

pbuilder_component_install_and_export( 
    COMPONENT M4Library
    LIBTARGETS Monarch4
)

pbuilder_install_headers( ${MONARCH4_HEADERFILES} )

# Executables
add_subdirectory( Executables )
