Monarch
=======

Monarch is the library for file I/O used by the Project 8 collaboration.


Monarch3
--------

The newest version of the Monarch library, Monarch3, produces egg v3 files, 
which is an HDF5-based file format.


Monarch2
--------

The Monarch2 library, which produces egg v2 files (Google Protocol Buffers-based), 
can also be built.


Requirements
------------

- CMake 3.0 or higher
- HDF5 1.8 or higher (Monarch3 only)
- Google Protocol Buffers (Monarch2 only)


Installation
------------

Monarch is installed using CMake
We recommend an out-of-source build:
```
    mkdir build
    cd build
```

To configure the installation:
```
    cmake ..
```

The .. argument points to the top-level Monarch directory, in this case
assuming that the build is taking place in the "build" subdirectory.
ccmake, or cmake-gui may also be used, of course.

To build and install:
```
    make
    make install
```

The install prefix is specified by the CMake variable CMAKE_INSTALL_PREFIX.
The library, binaries, and header files will be installed in the 
lib, bin, and include subdirectories. The default install prefix is the
build directory.

Any CMake-based projects that will depend on Monarch can include 
the MonarchConfig.cmake file that is located in the build directory.


Development
-----------

The Git workflow used is git-flow:
* http://nvie.com/posts/a-successful-git-branching-model/
We suggest that you use the aptly-named git extension, git-flow, which is available from commonly-used package managers:
* https://github.com/nvie/gitflow

