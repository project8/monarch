########################
# monarch3 executables #
########################

set( Monarch3_PROGRAMS )

if( Monarch_ENABLE_EXECUTABLES )

    set( lib_dependencies 
        Monarch3
    )
    
    set( sources
        M3Info.cc
        #Monarch3Dump.cc
    )
    
    pbuilder_executables( 
        SOURCES ${sources}
        TARGETS_VAR Monarch3_PROGRAMS
        PROJECT_LIBRARIES ${lib_dependencies}
    )

endif( Monarch_ENABLE_EXECUTABLES )

if( Monarch_ENABLE_TESTING )

    set( lib_dependencies 
        Monarch3
    )
    
    set( sources
        M3MultithreadingTest.cc
        M3ReadTest.cc
        M3WriteTest.cc
        M3WriteSpeedTest.cc
        #Monarch3TimeCheck.cc
    )
    
    pbuilder_executables( 
        SOURCES ${sources}
        TARGETS_VAR Monarch3_TEST_PROGRAMS
        PROJECT_LIBRARIES ${lib_dependencies}
    )

    list( APPEND Monarch3_PROGRAMS ${Monarch3_TEST_PROGRAMS} )

endif( Monarch_ENABLE_TESTING )

if( Monarch3_PROGRAMS )
    pbuilder_component_install_and_export( 
        COMPONENT M3Executables
        EXETARGETS ${Monarch3_PROGRAMS}
    )
endif()
