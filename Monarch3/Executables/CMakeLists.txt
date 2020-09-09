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
        #PUBLIC_EXTERNAL_LIBRARIES ${PUBLIC_EXT_LIBS}
        #PRIVATE_EXTERNAL_LIBRARIES ${PRIVATE_EXT_LIBS}
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
        TARGETS_VAR Monarch3_PROGRAMS
        PROJECT_LIBRARIES ${lib_dependencies}
        #PUBLIC_EXTERNAL_LIBRARIES ${PUBLIC_EXT_LIBS}
        #PRIVATE_EXTERNAL_LIBRARIES ${PRIVATE_EXT_LIBS}
    )

endif( Monarch_ENABLE_TESTING )

if( Monarch3_PROGRAMS )
    pbuilder_component_install_and_export( 
        COMPONENT M3Executables
        EXETARGETS ${Monarch3_PROGRAMS}
    )
endif()
