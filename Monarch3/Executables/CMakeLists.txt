########################
# monarch3 executables #
########################

if( Monarch_ENABLE_EXECUTABLES )

    set( lib_dependencies 
        Monarch3
    )
    
    set( programs
        M3Info
        #Monarch3Dump
    )
    
    pbuilder_executables( programs lib_dependencies )

endif( Monarch_ENABLE_EXECUTABLES )

if( Monarch_ENABLE_TESTING )

    set( lib_dependencies 
        Monarch3
    )
    
    set( programs
        M3MultithreadingTest
        M3ReadTest
        M3WriteTest
        M3WriteSpeedTest
        #Monarch3TimeCheck
    )
    
    pbuilder_executables( programs lib_dependencies )
    
endif( Monarch_ENABLE_TESTING )
