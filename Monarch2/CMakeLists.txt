####################
# monarch2 library #
####################

set( MONARCH2_HEADERFILES
    M2Exception.hh
    M2Header.hh
    M2IO.hh
    M2Monarch.hh
    M2Record.hh
    M2Types.hh
)

set( MONARCH2_SOURCEFILES
    M2Exception.cc
    M2Header.cc
    M2IO.cc
    M2Monarch.cc
    M2Version.cc
)

configure_file( M2Version.hh.in ${PROJECT_BINARY_DIR}/M2Version.hh )
set( MONARCH2_HEADERFILES
    ${MONARCH2_HEADERFILES}
    ${PROJECT_BINARY_DIR}/M2Version.hh
)

pbuilder_library( Monarch2 MONARCH2_SOURCEFILES "" )

pbuilder_install_headers( ${MONARCH2_HEADERFILES} )


########################
# monarch2 executables #
########################

if( Monarch_ENABLE_EXECUTABLES )

    set( lib_dependencies 
        Monarch2
    )
    
    set( programs
        Monarch2Info
        Monarch2Dump
    )
    
    pbuilder_executables( programs lib_dependencies )

endif( Monarch_ENABLE_EXECUTABLES )

if( Monarch_ENABLE_TESTING )

    set( lib_dependencies 
        Monarch2
    )
    
    set( programs
        Monarch2TimeCheck
    )
    
    pbuilder_executables( programs lib_dependencies )
    
endif( Monarch_ENABLE_TESTING )
