Testing Monarch3
================

Overview
--------

Monarch3 includes two categories of tests:

**C++ test executables** (``M3WriteTest``, ``M3ReadTest``, ``M3WriteSpeedTest``,
``M3MultithreadingTest``) are built when ``Monarch_ENABLE_TESTING`` is ON.
They exercise the C++ library directly and do not require Python.

**Python validation tests** live in ``Monarch3/Tests/Validation/`` and require both
``Monarch_ENABLE_TESTING`` and ``Monarch_BUILD_PYTHON`` to be ON.  They use
`pytest <https://docs.pytest.org>`_ as the test runner and verify cross-language
compatibility between the C++ library and the Python binding.  The validation tests
are registered with CTest so they run automatically alongside the C++ tests.


Building for Testing
--------------------

To build all tests — C++ executables and Python validation — configure CMake with::

   cmake \
       -DMonarch_BUILD_MONARCH3=ON \
       -DMonarch_ENABLE_TESTING=ON \
       -DMonarch_BUILD_PYTHON=ON \
       /path/to/monarch/source

   make

.. note::
   ``Monarch_BUILD_PYTHON`` requires `pybind11 <https://pybind11.readthedocs.io>`_
   and Python 3 development headers (``python3-dev`` or equivalent).
   ``numpy`` and ``pytest`` must also be available in the active Python environment.

To build only the C++ test executables (no Python binding or validation tests)::

   cmake -DMonarch_BUILD_MONARCH3=ON -DMonarch_ENABLE_TESTING=ON /path/to/monarch/source
   make


Running Tests
-------------

**Via CTest** (recommended; runs everything registered in the build)::

   cd build
   ctest --output-on-failure

This runs the C++ test executables and, if the Python binding was built, the pytest
validation suite as a single CTest entry named ``monarch3_validation``.

**Running the Python validation tests directly** (after building and installing)::

   cd build
   source bin/add_lib_python_path.sh   # makes monarch3.so importable
   pytest Monarch3/Tests/Validation/ -v

Run a specific test file::

   pytest Monarch3/Tests/Validation/test_cpp_write_python_read.py -v

Run a specific test function::

   pytest Monarch3/Tests/Validation/test_cpp_write_python_read.py::test_stream0_data -v


Validation
----------

The validation tests verify cross-language compatibility: that egg v3 files written by
C++ are correctly read by Python, and that files written by Python are correctly read by
C++.  All three test modules use the same gold-standard 4-stream data structure defined
in the ``M3WriteTest`` C++ executable.

Gold-Standard Data
~~~~~~~~~~~~~~~~~~

Every validation test is written against a common set of known values:

+--------+----------------------------+---------+-----------+------+------+----------+---------------------+
| Stream | Source                     | Channels| Format    | Rate | Rec  | Type     | Record values       |
+========+============================+=========+===========+======+======+==========+=====================+
| 0      | "1-channel device"         | 1       | —         | 500  | 10   | uint8    | rec0=1, rec1=10     |
+--------+----------------------------+---------+-----------+------+------+----------+---------------------+
| 1      | "2-channel device"         | 2       | interleaved| 250 | 5    | uint16   | rec0=(1,2),         |
|        |                            |         |           |      |      |          | rec1=(1000,2000),   |
|        |                            |         |           |      |      |          | rec2=(10000,20000)  |
+--------+----------------------------+---------+-----------+------+------+----------+---------------------+
| 2      | "3-channel device"         | 3       | separate  | 100  | 5    | uint8    | rec0=(1,2,3),       |
|        |                            |         |           |      |      |          | rec1=(10,20,30)     |
+--------+----------------------------+---------+-----------+------+------+----------+---------------------+
| 3      | "Floating-point device"    | 1       | —         | 100  | 10   | float32  | rec0=π, rec1=e      |
+--------+----------------------------+---------+-----------+------+------+----------+---------------------+

Header fields: ``run_duration=8675309``, ``timestamp="Stardate 33515"``,
``description="Bigger on the inside"``.

Stream 1 has two acquisitions: acquisition 0 contains record 0; acquisition 1 contains
records 1 and 2.  Stream 3 has two acquisitions, one record each.

The shared constants are defined in ``conftest.py`` and imported by all test modules.

Test Scenarios
~~~~~~~~~~~~~~

**C++ write → Python read** (``test_cpp_write_python_read.py``)

The fixture ``cpp_written_egg`` invokes the ``M3WriteTest`` executable to produce a
known-good egg file in a temporary directory.  The tests then open that file with the
Python binding and verify:

- Header fields (``run_duration``, ``timestamp``, ``description``, ``n_streams``)
- Per-stream structure (channel count, acquisition count, record count, record size)
- Sample values for each stream and channel, including float comparison with
  ``numpy.isclose``
- Offset-based record navigation for stream 1: forward skips, rewinds, and requests
  past the end or before the beginning of the file (mirrors the checks in
  ``M3ReadTest.cc``)

**Python write → C++ read** (``test_python_write_cpp_read.py``)

The helper function ``write_standard_egg(path)`` uses the Python binding to write the
same 4-stream structure as ``M3WriteTest``.  The test then runs the ``M3ReadTest``
executable against that file and asserts that it exits with return code 0.  Since
``M3ReadTest`` checks record counts, acquisition counts, channel counts, and data values
internally, a clean exit confirms byte-level compatibility.

**Python write → Python read** (``test_python_write_python_read.py``)

Uses ``write_standard_egg`` (imported from ``test_python_write_cpp_read.py``) to produce
a file, then reads it back entirely with the Python binding.  This isolates the binding
from the C++ executables and verifies that the write and read paths are internally
consistent.  The same structure and value checks as the C++ write → Python read scenario
are applied.
