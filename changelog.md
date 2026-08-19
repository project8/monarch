# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

Types of changes: Added, Changed, Deprecated, Removed, Fixed, Security

## [Unreleased] ([3.9.0] - 2026-08-19)

### Added

- Python interface for Monarch3 via pybind11 (`Monarch3/python/`), exposing `M3Header`,
  `M3StreamHeader`, `M3ChannelHeader`, `M3Stream`, `M3Record`, and `Monarch3` to Python
  as the `monarch3` module; record data is returned as zero-copy numpy `uint8` arrays
- Context manager (`with` statement) support for `Monarch3` objects; `FinishReading()`
  or `FinishWriting()` is called automatically on exit
- `Monarch_BUILD_PYTHON` CMake option to enable the Python binding (requires pybind11 and
  Python 3 development headers)
- Validation test suite (`Monarch3/Tests/Validation/`) using pytest and CTest:
  - C++ write → Python read (`test_cpp_write_python_read.py`)
  - Python write → C++ read (`test_python_write_cpp_read.py`)
  - Python write → Python read (`test_python_write_python_read.py`)
- `Monarch_ENABLE_TESTING` CMake option now activates `enable_testing()` and the CTest
  validation suite in addition to the existing C++ test executables
- Documentation pages: `UsageMonarch3Python.rst` and `TestingMonarch3.rst`
- GitHub Actions workflow (`.github/workflows/run_tests.yaml`) with separate jobs for
  Monarch3 (including Python validation tests) and Monarch2, plus a Release job

### Fixed

- `M3ReadTest`: offset-navigation test (Test 2) now passes `aIfNewAcqStartAtFirstRec=false`
  when crossing an acquisition boundary to land on the exact target record, and corrects
  a `ReadRecord(-2)` call that would step before the start of the file
- `M3ReadTest`: Test 3 (stream 2 skip) likewise uses `aIfNewAcqStartAtFirstRec=false` for
  the initial offset skip

### Changed

- HDF5 minimum version raised to 1.10.1; the v1.8 API compatibility workaround has been
  removed from `CMakeLists.txt`
