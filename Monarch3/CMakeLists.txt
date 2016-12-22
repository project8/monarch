####################
# monarch3 library #
####################

set( MONARCH3_HEADERFILES
    M3Constants.hh
    M3DataInterface.hh
    M3Exception.hh
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


########################
# monarch3 executables #
########################

if( Monarch_ENABLE_EXECUTABLES )

    set( lib_dependencies 
        Monarch3
    )
    
    set( programs
        Monarch3Info
        #Monarch3Dump
    )
    
    pbuilder_executables( programs lib_dependencies )

endif( Monarch_ENABLE_EXECUTABLES )

if( Monarch_ENABLE_TESTING )

    set( lib_dependencies 
        Monarch3
    )
    
    set( programs
        Monarch3MultithreadingTest
        Monarch3ReadTest
        Monarch3WriteTest
        Monarch3WriteSpeedTest
        #Monarch3TimeCheck
    )
    
    pbuilder_executables( programs lib_dependencies )
    
endif( Monarch_ENABLE_TESTING )
