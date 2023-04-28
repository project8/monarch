########################
# monarch4 executables #
########################

set( Monarch4_PROGRAMS )

set( sources 
    read.cc
    readme.cc
    write.cc
)

pbuilder_executables( 
    SOURCES ${sources}
    TARGETS_VAR Monarch4_PROGRAMS
    PUBLIC_EXTERNAL_LIBS ${PUBLIC_EXT_LIBS_M4}
)

pbuilder_component_install_and_export( 
    COMPONENT M4Executables
    EXETARGETS ${Monarch4_PROGRAMS}
)
