# CMakeLists.txt for Monarch3/python
# Author: N.S. Oblath

message( "Building Monarch3 Python bindings" )

set( dir ${CMAKE_CURRENT_SOURCE_DIR} )

set( PYBINDING_HEADERFILES
    ${dir}/monarch3_binding_helpers.hh
    ${dir}/m3constants_pybind.hh
    ${dir}/m3header_pybind.hh
    ${dir}/m3record_pybind.hh
    ${dir}/m3stream_pybind.hh
    ${dir}/m3monarch_pybind.hh
)

set( PYBINDING_SOURCEFILES
    ${dir}/monarch3_namespace_pybind.cc
)

set( PYBINDING_PROJECT_LIBRARIES Monarch3 )

pbuilder_add_pybind11_module(
    MODULE_NAME monarch3
    SOURCE_FILES ${PYBINDING_SOURCEFILES}
    PROJECT_LIBRARIES ${PYBINDING_PROJECT_LIBRARIES}
)

pbuilder_install_headers( ${PYBINDING_HEADERFILES} )

if( NOT DEFINED PBUILDER_PY_INSTALL_IN_SITELIB )
    message( STATUS "Installing add_lib_python_path.sh since python modules will not be installed in the site-package directory" )
    configure_file( add_lib_python_path.sh.in add_lib_python_path.sh @ONLY )
    install( FILES ${CMAKE_CURRENT_BINARY_DIR}/add_lib_python_path.sh DESTINATION ${BIN_INSTALL_DIR} )
endif( NOT DEFINED PBUILDER_PY_INSTALL_IN_SITELIB )

message( "Done with Monarch3 Python bindings" )
