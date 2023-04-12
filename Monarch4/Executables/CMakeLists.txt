########################
# monarch4 executables #
########################

set( Monarch4_PROGRAMS )

if( Monarch_ENABLE_EXECUTABLES )

    set( lib_dependencies 
        Monarch4
    )
    
    set( sources
        M4Info.cc
        #Monarch4Dump.cc
    )
    
    pbuilder_executables( 
        SOURCES ${sources}
        TARGETS_VAR Monarch4_PROGRAMS
        PROJECT_LIBRARIES ${lib_dependencies}
    )

endif( Monarch_ENABLE_EXECUTABLES )

if( Monarch_ENABLE_TESTING )

    set( lib_dependencies 
        Monarch4
    )
    
    set( sources
        M4MultithreadingTest.cc
        M4ReadTest.cc
        M4WriteTest.cc
        M4WriteSpeedTest.cc
        #Monarch4TimeCheck.cc
    )
    
    pbuilder_executables( 
        SOURCES ${sources}
        TARGETS_VAR Monarch4_TEST_PROGRAMS
        PROJECT_LIBRARIES ${lib_dependencies}
    )

    list( APPEND Monarch4_PROGRAMS ${Monarch4_TEST_PROGRAMS} )

endif( Monarch_ENABLE_TESTING )

if( Monarch4_PROGRAMS )
    pbuilder_component_install_and_export( 
        COMPONENT M4Executables
        EXETARGETS ${Monarch4_PROGRAMS}
    )
endif()
