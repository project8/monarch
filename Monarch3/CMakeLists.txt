####################
# monarch3 library #
####################

set( MONARCH3_HEADERFILES
    M3DataInterface.hh
    M3Exception.hh
    M3Header.hh
    M3IToA.hh
    M3Logger.hh
    M3MemberVariable.hh
    M3Monarch.hh
    M3Record.hh
    M3Stream.hh
    M3Types.hh
)

set( MONARCH3_SOURCEFILES
    M3Exception.cc
    M3Header.cc
    M3IToA.cc
    M3Logger.cc
    M3Monarch.cc
    M3Record.cc
    M3Stream.cc
    M3Version.cc
)

configure_file( M3Version.hh.in ${PROJECT_BINARY_DIR}/M3Version.hh )
set( MONARCH3_HEADERFILES
    ${MONARCH3_HEADERFILES}
    ${PROJECT_BINARY_DIR}/M3Version.hh
)
include_directories( BEFORE ${PROJECT_BINARY_DIR} )

add_library( Monarch3 ${MONARCH3_SOURCEFILES} )
target_link_libraries( Monarch3 ${EXTERNAL_LIBRARIES} )

pbuilder_install_headers( ${MONARCH3_HEADERFILES} )
pbuilder_install_libraries( Monarch3 )


########################
# monarch3 executables #
########################

#add_executable( Monarch3Dump Monarch3Dump.cpp )
#target_link_libraries( Monarch3Dump Monarch3 ${EXTERNAL_LIBRARIES})

add_executable( Monarch3Info Monarch3Info.cc )
target_link_libraries( Monarch3Info Monarch3 ${EXTERNAL_LIBRARIES})

add_executable( Monarch3ReadTest Monarch3ReadTest.cc )
target_link_libraries( Monarch3ReadTest Monarch3 ${EXTERNAL_LIBRARIES})

add_executable( Monarch3WriteTest Monarch3WriteTest.cc )
target_link_libraries( Monarch3WriteTest Monarch3 ${EXTERNAL_LIBRARIES})

#add_executable( Monarch3TimeCheck Monarch3TimeCheck.cpp )
#target_link_libraries( Monarch3TimeCheck Monarch3 ${EXTERNAL_LIBRARIES})

pbuilder_install_executables (
    #Monarch3Dump
    Monarch3Info
    Monarch3ReadTest
    Monarch3WriteTest
    #Monarch3TimeCheck
)
