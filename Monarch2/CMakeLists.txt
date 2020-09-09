
###########################
# protobuf c++ generation #
###########################

set( PROTO_FILES
    Protobuf/MonarchHeader.proto
)

protobuf_generate_cpp( Monarch_Protobuf_Sources Monarch_Protobuf_Headers ${PROTO_FILES} )


####################
# monarch2 library #
####################

set( MONARCH2_HEADERFILES
    ${Monarch_Protobuf_Sources}
    M2Exception.hh
    M2Header.hh
    M2IO.hh
    M2Monarch.hh
    M2Record.hh
    M2Types.hh
    M2Version.hh
)

set( MONARCH2_SOURCEFILES
    ${Monarch_Protobuf_Sources}
    M2Exception.cc
    M2Header.cc
    M2IO.cc
    M2Monarch.cc
)

configure_file( M2Version.cc.in ${CMAKE_CURRENT_BINARY_DIR}/M2Version.cc )
set( MONARCH2_SOURCEFILES
    ${MONARCH2_SOURCEFILES}
    ${CMAKE_CURRENT_BINARY_DIR}/M2Version.cc
)


pbuilder_library(
    TARGET Monarch2
    SOURCES ${MONARCH2_SOURCEFILES}
    PUBLIC_EXTERNAL_LIBRARIES ${PUBLIC_EXT_LIBS_M2}
)

pbuilder_component_install_and_export( 
    COMPONENT M2Library
    LIBTARGETS Monarch2
)

pbuilder_install_headers( ${MONARCH2_HEADERFILES} )


########################
# monarch2 executables #
########################

set( Monarch2_PROGRAMS )

if( Monarch_ENABLE_EXECUTABLES )

    set( lib_dependencies 
        Monarch2
    )
    
    set( sources
        Monarch2Info.cc
        Monarch2Dump.cc
    )
    
    pbuilder_executables( 
        SOURCES ${sources}
        TARGETS_VAR Monarch2_PROGRAMS
        PROJECT_LIBRARIES ${lib_dependencies}
    )

endif( Monarch_ENABLE_EXECUTABLES )

if( Monarch_ENABLE_TESTING )

    set( lib_dependencies 
        Monarch2
    )
    
    set( sources
        Monarch2TimeCheck.cc
    )
    
    pbuilder_executables( 
        SOURCES ${sources}
        TARGETS_VAR Monarch2_PROGRAMS
        PROJECT_LIBRARIES ${lib_dependencies}
    )
    
endif( Monarch_ENABLE_TESTING )

if( Monarch2_PROGRAMS )
    pbuilder_component_install_and_export( 
        COMPONENT M2Executables
        EXETARGETS ${Monarch2_PROGRAMS}
    )
endif()
