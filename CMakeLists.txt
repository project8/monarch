# CMakeLists for Monarch
# Author: N. Oblath

#############
# Setup stuff
#############

# minimum cmake version 2.6 required by the scripts to get Git information
cmake_minimum_required (VERSION 2.6)

# define the project
project (Monarch)

# Setup the default install prefix
# This gets set to the binary directory upon first configuring.
# If the user changes the prefix, but leaves the flag OFF, then it will remain as the user specified.
# If the user wants to reset the prefix to the default (i.e. the binary directory), then the flag should be set ON.
if (NOT DEFINED SET_INSTALL_PREFIX_TO_DEFAULT)
    set (SET_INSTALL_PREFIX_TO_DEFAULT ON)
endif (NOT DEFINED SET_INSTALL_PREFIX_TO_DEFAULT)
if (SET_INSTALL_PREFIX_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR} CACHE PATH "Install prefix" FORCE)
    set (SET_INSTALL_PREFIX_TO_DEFAULT OFF CACHE BOOL "Reset default install path when when configuring" FORCE)
endif (SET_INSTALL_PREFIX_TO_DEFAULT)

# install subdirectories
set (INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "Install directory for headers")
set (LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE PATH "Install directory for libraries")
set (BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Install directory for binaries")

# build shared libraries
set (BUILD_SHARED_LIBS ON)

# find Protocol Buffers
find_package (Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})
set (LIBS ${LIBS} ${PROTOBUF_LIBRARIES})


##############
# Build things
##############

# Lists of header and source files

set (MONARCH_PROTOBUF_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MonarchHeader.proto
)

set (MONARCH_HEADER_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Monarch.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/MonarchIO.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/MonarchHeader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/MonarchRecord.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/MonarchTypes.hpp
)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set (MONARCH_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Monarch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MonarchIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MonarchHeader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MonarchRecord.cpp
)

# Generate the Protocol Buffers cpp file
include_directories(${CMAKE_CURRENT_BINARY_DIR})
protobuf_generate_cpp (GENERATED_PROTO_SOURCE_FILES GENERATED_PROTO_HEADER_FILES ${MONARCH_PROTOBUF_HEADER_FILES})
install (FILES ${GENERATED_PROTO_HEADER_FILES}  DESTINATION ${INCLUDE_INSTALL_DIR})

# Build the Monarch library
add_library (monarch ${MONARCH_SOURCE_FILES} ${GENERATED_PROTO_SOURCE_FILES})
set_target_properties(monarch PROPERTIES INSTALL_NAME_DIR ${LIB_INSTALL_DIR})
target_link_libraries (monarch ${LIBS})

# Build the test executables
add_executable (MonarchIOTest ${CMAKE_CURRENT_SOURCE_DIR}/src/MonarchIOTest.cpp)
target_link_libraries (MonarchIOTest monarch)
add_executable (MonarchTest ${CMAKE_CURRENT_SOURCE_DIR}/src/MonarchTest.cpp)
target_link_libraries (MonarchTest monarch)

install (FILES ${MONARCH_HEADER_FILES}  DESTINATION ${INCLUDE_INSTALL_DIR})
install (TARGETS monarch  DESTINATION ${LIB_INSTALL_DIR})
install (TARGETS MonarchIOTest MonarchTest DESTINATION ${BIN_INSTALL_DIR})
