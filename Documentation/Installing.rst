Getting and Installing Monarch
==============================

Prerequisites
-------------

All builds require:

* CMake 3.12 or higher
* A C++11-capable compiler
* Git (to clone the repository and fetch submodules)

Additional prerequisites depend on which components you want to build:

+---------------------+------------------------------------------+
| Component           | Additional requirement                   |
+=====================+==========================================+
| Monarch3 (default)  | HDF5 ≥ 1.10.1 (C++ component)           |
+---------------------+------------------------------------------+
| Monarch2 (optional) | Google Protocol Buffers                  |
+---------------------+------------------------------------------+
| Python bindings     | Python 3, pybind11, numpy                |
+---------------------+------------------------------------------+


Getting the Source
------------------

Clone the repository from GitHub, including the required Scarab submodule::

    git clone https://github.com/project8/monarch.git
    cd monarch
    git submodule update --init --recursive

The ``--recursive`` flag is important: Monarch depends on the
`Scarab <https://github.com/project8/scarab>`_ library (vendored as a
submodule in the ``Scarab/`` directory) for its CMake build infrastructure.


Building and Installing
-----------------------

Monarch uses an out-of-source CMake build. Create a ``build`` directory,
configure, compile, and install:

.. code-block:: bash

    mkdir build
    cd build
    cmake [options] ..
    make
    make install

The install prefix defaults to the ``build`` directory itself. To change it,
pass ``-DCMAKE_INSTALL_PREFIX=/your/install/path`` to ``cmake``.

After installation, the ``lib/``, ``bin/``, and ``include/`` subdirectories
will be populated under the install prefix. A ``MonarchConfig.cmake`` file is
written to the build directory so that downstream CMake projects can locate
Monarch with ``find_package(Monarch)``.


C++-only build
~~~~~~~~~~~~~~

This is the default configuration: it builds the Monarch3 C++ library
(Monarch2 and the Python bindings are off by default).

.. code-block:: bash

    cmake ..
    make
    make install

To be explicit, or to confirm the defaults::

    cmake \
        -DMonarch_BUILD_MONARCH3=ON \
        -DMonarch_BUILD_MONARCH2=OFF \
        -DMonarch_BUILD_PYTHON=OFF \
        ..


C++ and Python build
~~~~~~~~~~~~~~~~~~~~

To also build the Python bindings for Monarch3, enable ``Monarch_BUILD_PYTHON``.
This requires Python 3 development headers and `pybind11
<https://pybind11.readthedocs.io>`_::

    cmake \
        -DMonarch_BUILD_PYTHON=ON \
        ..
    make
    make install

After installation, add the Python binding to your ``PYTHONPATH``::

    source bin/add_lib_python_path.sh

or set the path manually::

    export PYTHONPATH=/your/install/path/lib:$PYTHONPATH

You can then ``import monarch3`` from Python.

.. note::
   pybind11 can typically be installed via your system package manager
   (e.g. ``brew install pybind11`` on macOS, ``apt install pybind11-dev``
   on Debian/Ubuntu) or via pip (``pip install pybind11``).


CMake Options Reference
-----------------------

+----------------------------------+---------+--------------------------------------------+
| Option                           | Default | Description                                |
+==================================+=========+============================================+
| ``Monarch_BUILD_MONARCH3``       | ON      | Build the Monarch3 (HDF5-based) library    |
+----------------------------------+---------+--------------------------------------------+
| ``Monarch_BUILD_MONARCH2``       | OFF     | Build the Monarch2 (Protobuf-based) library|
+----------------------------------+---------+--------------------------------------------+
| ``Monarch_BUILD_PYTHON``         | OFF     | Build the Monarch3 Python bindings         |
+----------------------------------+---------+--------------------------------------------+
| ``Monarch_ENABLE_TESTING``       | OFF     | Build the test executables                 |
+----------------------------------+---------+--------------------------------------------+
| ``CMAKE_INSTALL_PREFIX``         | build/  | Installation root directory                |
+----------------------------------+---------+--------------------------------------------+
