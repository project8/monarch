# CMakeLists.txt for Monarch/Protobuf
# Author: N. Oblath
# Created: March 26, 2014
# For older versions of FindProtobuf.cmake, the protobuf_generate_cpp is supposed to be used in the same directory as 
# the relevant add_library or add_executable.  Therefore a separate proto library is made.

set( PROTO_FILES
    MonarchHeader.proto
)

protobuf_generate_cpp( Monarch_Protobuf_Sources Monarch_Protobuf_Headers ${PROTO_FILES} )

add_library( MonarchProto ${Monarch_Protobuf_Sources} )
target_link_libraries( MonarchProto ${EXTERNAL_LIBRARIES} )
pbuilder_install_libraries( MonarchProto )
pbuilder_install_headers( ${Monarch_Protobuf_Headers} )

