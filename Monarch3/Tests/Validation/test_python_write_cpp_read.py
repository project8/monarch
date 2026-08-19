# test_python_write_cpp_read.py
# Validates that a file written by the Python binding is correctly read by the C++
# M3ReadTest executable.  The Python write reproduces the same 4-stream structure as
# M3WriteTest so that M3ReadTest's built-in checks pass without modification.

import numpy as np
import subprocess
import monarch3

from conftest import (
    HEADER_RUN_DURATION, HEADER_TIMESTAMP, HEADER_DESCRIPTION,
    STREAM0_REC_SIZE, STREAM0_VALUES,
    STREAM1_REC_SIZE, STREAM1_VALUES,
    STREAM2_REC_SIZE, STREAM2_VALUES,
    STREAM3_REC_SIZE, STREAM3_VALUES,
)


def write_standard_egg(path):
    """Write the same 4-stream structure as M3WriteTest using the Python binding.

    This is also imported by test_python_write_python_read.py to avoid
    duplicating the write logic.
    """
    with monarch3.Monarch3.open_for_writing(path) as m:
        hdr = m.get_header()
        hdr.filename     = path
        hdr.run_duration = HEADER_RUN_DURATION
        hdr.timestamp    = HEADER_TIMESTAMP
        hdr.description  = HEADER_DESCRIPTION

        s0 = hdr.add_stream(
            "1-channel device",
            500, STREAM0_REC_SIZE, 1, 1,
            monarch3.sDigitizedUS, 8, monarch3.sBitsAlignedLeft,
        )
        s1 = hdr.add_stream(
            "2-channel device",
            2, monarch3.sInterleaved,
            250, STREAM1_REC_SIZE, 1, 2,
            monarch3.sDigitizedUS, 16, monarch3.sBitsAlignedLeft,
        )
        s2 = hdr.add_stream(
            "3-channel device",
            3, monarch3.sSeparate,
            100, STREAM2_REC_SIZE, 1, 1,
            monarch3.sDigitizedUS, 8, monarch3.sBitsAlignedLeft,
        )
        s3 = hdr.add_stream(
            "Floating-point device",
            100, STREAM3_REC_SIZE, 1, 4,
            monarch3.sAnalog, 8, monarch3.sBitsAlignedLeft,
        )
        m.write_header()

        # Stream 0: 2 records in 1 acquisition
        stream = m.get_stream(s0)
        for i, val in enumerate(STREAM0_VALUES):
            stream.get_channel_data(0)[:] = val
            stream.write_record(i == 0)

        # Stream 1: 3 records in 2 acquisitions
        #   acq 0: rec 0 (is_new=True)
        #   acq 1: rec 1 (is_new=True), rec 2 (is_new=False)
        stream = m.get_stream(s1)
        for i, (v0, v1) in enumerate(STREAM1_VALUES):
            stream.get_channel_data(0).view(np.uint16)[:] = v0
            stream.get_channel_data(1).view(np.uint16)[:] = v1
            is_new = (i == 0 or i == 1)
            stream.write_record(is_new)

        # Stream 2: 2 records in 1 acquisition
        stream = m.get_stream(s2)
        for i, (v0, v1, v2) in enumerate(STREAM2_VALUES):
            stream.get_channel_data(0)[:] = v0
            stream.get_channel_data(1)[:] = v1
            stream.get_channel_data(2)[:] = v2
            stream.write_record(i == 0)

        # Stream 3: 2 records, each in its own acquisition
        stream = m.get_stream(s3)
        for val in STREAM3_VALUES:
            stream.get_channel_data(0).view(np.float32)[:] = val
            stream.write_record(True)


def test_python_write_cpp_read(tmp_egg):
    """Write the standard 4-stream file from Python, then validate with M3ReadTest."""
    write_standard_egg(tmp_egg)

    result = subprocess.run(
        ["M3ReadTest", tmp_egg],
        capture_output=True,
        text=True,
    )
    assert result.returncode == 0, (
        f"M3ReadTest failed on Python-written file (return code {result.returncode}):\n"
        f"stdout:\n{result.stdout}\n"
        f"stderr:\n{result.stderr}"
    )
