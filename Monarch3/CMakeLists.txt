####################
# monarch3 library #
####################

set( MONARCH3_HEADERFILES
    MException.hh
    MHeader.hh
    MIToA.hh
    MLogger.hh
    MMemberVariable.hh
    MMonarch.hh
    MRecord.hh
    MStream.hh
    MTypes.hh
)

set( MONARCH3_SOURCEFILES
    MException.cc
    MHeader.cc
    MIToA.cc
    MLogger.cc
    MMonarch.cc
    MRecord.cc
    MStream.cc
    MVersion.cc
)

configure_file( MVersion.hh.in ${CMAKE_BINARY_DIR}/MVersion.hh )
set( MONARCH3_HEADERFILES
    ${MONARCH3_HEADERFILES}
    ${CMAKE_BINARY_DIR}/MVersion.hh
)
include_directories( BEFORE ${CMAKE_BINARY_DIR} )

add_library( Monarch3 ${MONARCH3_SOURCEFILES} )
target_link_libraries( Monarch3 ${EXTERNAL_LIBRARIES} )

pbuilder_install_headers( ${MONARCH3_HEADERFILES} )
pbuilder_install_libraries( Monarch3 )


########################
# monarch3 executables #
########################

#add_executable( Monarch3Dump MonarchDump.cpp )
#target_link_libraries( Monarch3Dump Monarch3 ${EXTERNAL_LIBRARIES})

add_executable( Monarch3Info MonarchInfo.cc )
target_link_libraries( Monarch3Info Monarch3 ${EXTERNAL_LIBRARIES})

add_executable( Monarch3WriteTest MonarchWriteTest.cc )
target_link_libraries( Monarch3WriteTest Monarch3 ${EXTERNAL_LIBRARIES})

#add_executable( Monarch3TimeCheck MonarchTimeCheck.cpp )
#target_link_libraries( Monarch3TimeCheck Monarch3 ${EXTERNAL_LIBRARIES})

pbuilder_install_executables (
    #Monarch3Dump
    Monarch3Info
    Monarch3WriteTest
    #Monarch3TimeCheck
)
