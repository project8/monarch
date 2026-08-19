# conftest.py
# Shared fixtures and constants for Monarch3 validation tests.
# All data constants match the exact values written by the M3WriteTest C++ executable.

import pytest
import subprocess

# ---- Gold-standard data constants from M3WriteTest ----

# Header
HEADER_RUN_DURATION = 8675309
HEADER_TIMESTAMP    = "Stardate 33515"
HEADER_DESCRIPTION  = "Bigger on the inside"

# Stream 0: 1 channel, uint8, 10 samples/record, 1 acquisition, 2 records
STREAM0_N_CHANNELS     = 1
STREAM0_N_ACQUISITIONS = 1
STREAM0_N_RECORDS      = 2
STREAM0_REC_SIZE       = 10
STREAM0_VALUES         = [1, 10]   # one uniform value per record

# Stream 1: 2 channels, uint16, interleaved, 5 samples/record, 2 acquisitions, 3 records
#   acq 0: rec 0   (WriteRecord true)
#   acq 1: rec 1   (WriteRecord true), rec 2 (WriteRecord false)
STREAM1_N_CHANNELS     = 2
STREAM1_N_ACQUISITIONS = 2
STREAM1_N_RECORDS      = 3
STREAM1_REC_SIZE       = 5
STREAM1_VALUES         = [
    (1,     2),       # rec 0: ch0=1,   ch1=2
    (1000,  2000),    # rec 1: ch0=1000, ch1=2000
    (10000, 20000),   # rec 2: ch0=10000, ch1=20000
]

# Stream 2: 3 channels, uint8, separate, 5 samples/record, 1 acquisition, 2 records
STREAM2_N_CHANNELS     = 3
STREAM2_N_ACQUISITIONS = 1
STREAM2_N_RECORDS      = 2
STREAM2_REC_SIZE       = 5
STREAM2_VALUES         = [
    (1,  2,  3),    # rec 0: ch0=1, ch1=2,  ch2=3
    (10, 20, 30),   # rec 1: ch0=10, ch1=20, ch2=30
]

# Stream 3: 1 channel, float32, 10 samples/record, 2 acquisitions, 2 records (one each)
STREAM3_N_CHANNELS     = 1
STREAM3_N_ACQUISITIONS = 2
STREAM3_N_RECORDS      = 2
STREAM3_REC_SIZE       = 10
STREAM3_VALUES         = [3.1415926535898, 2.71828182846]  # pi, e

# ---- Fixtures ----

@pytest.fixture
def tmp_egg(tmp_path):
    """Provide a temporary .egg file path, cleaned up automatically after the test."""
    return str(tmp_path / "test.egg")


@pytest.fixture(scope="session")
def cpp_written_egg(tmp_path_factory):
    """Run M3WriteTest once per session to produce the gold-standard egg file.

    Session scope avoids spawning multiple concurrent M3WriteTest processes (which
    can be killed by macOS resource throttling when pytest runs tests back-to-back).
    """
    path = str(tmp_path_factory.mktemp("cpp_written") / "cpp_written.egg")
    result = subprocess.run(
        ["M3WriteTest", path],
        capture_output=True,
        text=True,
    )
    assert result.returncode == 0, (
        f"M3WriteTest failed with return code {result.returncode}:\n"
        f"stdout: {result.stdout}\n"
        f"stderr: {result.stderr}"
    )
    return path
