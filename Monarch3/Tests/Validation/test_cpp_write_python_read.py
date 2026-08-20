# test_cpp_write_python_read.py
# Validates that the Python binding can correctly read a file produced by the C++
# M3WriteTest executable.  The structure and values checked here mirror M3ReadTest.cc.

import numpy as np
import pytest
import monarch3

from conftest import (
    HEADER_RUN_DURATION, HEADER_TIMESTAMP, HEADER_DESCRIPTION,
    STREAM0_N_CHANNELS, STREAM0_N_ACQUISITIONS, STREAM0_N_RECORDS, STREAM0_REC_SIZE, STREAM0_VALUES,
    STREAM1_N_CHANNELS, STREAM1_N_ACQUISITIONS, STREAM1_N_RECORDS, STREAM1_REC_SIZE, STREAM1_VALUES,
    STREAM2_N_CHANNELS, STREAM2_N_ACQUISITIONS, STREAM2_N_RECORDS, STREAM2_REC_SIZE, STREAM2_VALUES,
    STREAM3_N_CHANNELS, STREAM3_N_ACQUISITIONS, STREAM3_N_RECORDS, STREAM3_REC_SIZE, STREAM3_VALUES,
)


# ---- Header ----

def test_header(cpp_written_egg):
    with monarch3.Monarch3.open_for_reading(cpp_written_egg) as m:
        m.read_header()
        hdr = m.get_header()
        assert hdr.run_duration == HEADER_RUN_DURATION
        assert hdr.timestamp    == HEADER_TIMESTAMP
        assert hdr.description  == HEADER_DESCRIPTION
        assert hdr.n_streams    == 4
        assert hdr.n_channels   == 7  # 1 + 2 + 3 + 1


# ---- Stream 0: single-channel uint8 ----

def test_stream0_structure(cpp_written_egg):
    with monarch3.Monarch3.open_for_reading(cpp_written_egg) as m:
        m.read_header()
        stream = m.get_stream(0)
        assert stream.n_channels        == STREAM0_N_CHANNELS
        assert stream.n_acquisitions    == STREAM0_N_ACQUISITIONS
        assert stream.n_records_in_file == STREAM0_N_RECORDS
        assert stream.channel_record_size == STREAM0_REC_SIZE


def test_stream0_data(cpp_written_egg):
    with monarch3.Monarch3.open_for_reading(cpp_written_egg) as m:
        m.read_header()
        stream = m.get_stream(0)
        for expected_val in STREAM0_VALUES:
            assert stream.read_record(), "Unexpected end of stream 0"
            arr = stream.get_channel_data(0).view(np.uint8)
            assert np.all(arr == expected_val), \
                f"Stream 0: expected all {expected_val}, got {arr}"
        assert not stream.read_record(), "Stream 0 should be exhausted"


# ---- Stream 1: two-channel uint16, interleaved, offset navigation ----

def test_stream1_structure(cpp_written_egg):
    with monarch3.Monarch3.open_for_reading(cpp_written_egg) as m:
        m.read_header()
        stream = m.get_stream(1)
        assert stream.n_channels        == STREAM1_N_CHANNELS
        assert stream.n_acquisitions    == STREAM1_N_ACQUISITIONS
        assert stream.n_records_in_file == STREAM1_N_RECORDS
        assert stream.channel_record_size == STREAM1_REC_SIZE


def test_stream1_data_sequential(cpp_written_egg):
    with monarch3.Monarch3.open_for_reading(cpp_written_egg) as m:
        m.read_header()
        stream = m.get_stream(1)
        for v0, v1 in STREAM1_VALUES:
            assert stream.read_record(), "Unexpected end of stream 1"
            ch0 = stream.get_channel_data(0).view(np.uint16)
            ch1 = stream.get_channel_data(1).view(np.uint16)
            assert np.all(ch0 == v0), f"Stream 1 ch0: expected {v0}, got {ch0}"
            assert np.all(ch1 == v1), f"Stream 1 ch1: expected {v1}, got {ch1}"


def test_stream1_offset_navigation(cpp_written_egg):
    """Verify seeking with non-zero offsets (mirrors M3ReadTest Test 2).

    Key: ReadRecord uses aIfNewAcqStartAtFirstRec=True by default, which snaps to the
    first record of any new acquisition entered.  Pass False to land exactly on the
    offset-targeted record when crossing an acquisition boundary.
    """
    with monarch3.Monarch3.open_for_reading(cpp_written_egg) as m:
        m.read_header()
        stream = m.get_stream(1)

        # Read record 0 (offset=0 from start); acq 0, rec 0
        assert stream.read_record(0), "Expected record 0"
        ch0 = stream.get_channel_data(0).view(np.uint16)
        assert np.all(ch0 == STREAM1_VALUES[0][0]), f"Expected {STREAM1_VALUES[0][0]}, got {ch0}"

        # Skip forward with offset=1 crossing to acquisition 1.
        # aIfNewAcqStartAtFirstRec=False: land on the exact target (file rec 2, acq 1 rec 1,
        # values 10000/20000) rather than snapping back to the start of acq 1.
        assert stream.read_record(1, False), "Expected record 2"
        ch0 = stream.get_channel_data(0).view(np.uint16)
        assert np.all(ch0 == STREAM1_VALUES[2][0]), f"Expected {STREAM1_VALUES[2][0]}, got {ch0}"

        # Reread record 2 (offset=-1)
        assert stream.read_record(-1), "Expected reread of record 2"
        ch0 = stream.get_channel_data(0).view(np.uint16)
        assert np.all(ch0 == STREAM1_VALUES[2][0]), f"Expected {STREAM1_VALUES[2][0]}, got {ch0}"

        # Step back to record 1 (offset=-2)
        assert stream.read_record(-2), "Expected record 1"
        ch0 = stream.get_channel_data(0).view(np.uint16)
        assert np.all(ch0 == STREAM1_VALUES[1][0]), f"Expected {STREAM1_VALUES[1][0]}, got {ch0}"

        # Request past end of file
        assert not stream.read_record(5), "Expected False for out-of-bounds forward seek"

        # Request before beginning of file
        assert not stream.read_record(-5), "Expected False for out-of-bounds backward seek"


# ---- Stream 2: three-channel uint8, separate ----

def test_stream2_structure(cpp_written_egg):
    with monarch3.Monarch3.open_for_reading(cpp_written_egg) as m:
        m.read_header()
        stream = m.get_stream(2)
        assert stream.n_channels        == STREAM2_N_CHANNELS
        assert stream.n_acquisitions    == STREAM2_N_ACQUISITIONS
        assert stream.n_records_in_file == STREAM2_N_RECORDS
        assert stream.channel_record_size == STREAM2_REC_SIZE


def test_stream2_data(cpp_written_egg):
    with monarch3.Monarch3.open_for_reading(cpp_written_egg) as m:
        m.read_header()
        stream = m.get_stream(2)
        for v0, v1, v2 in STREAM2_VALUES:
            assert stream.read_record(), "Unexpected end of stream 2"
            assert np.all(stream.get_channel_data(0).view(np.uint8) == v0)
            assert np.all(stream.get_channel_data(1).view(np.uint8) == v1)
            assert np.all(stream.get_channel_data(2).view(np.uint8) == v2)


def test_stream2_skip_to_second_record(cpp_written_egg):
    """Skip directly to record 1 using offset=1 (mirrors M3ReadTest Test 3).

    Stream 2 has only 1 acquisition, so aIfNewAcqStartAtFirstRec does not matter here;
    the snap-to-first-in-acq only fires when entering a *new* acquisition.  However,
    the very first ReadRecord call always treats the stream as entering a new acquisition,
    so we must pass False to land on the exact offset target (rec 1) rather than rec 0.
    """
    with monarch3.Monarch3.open_for_reading(cpp_written_egg) as m:
        m.read_header()
        stream = m.get_stream(2)
        assert stream.read_record(1, False), "Expected record 1 via skip"
        v0, v1, v2 = STREAM2_VALUES[1]
        assert np.all(stream.get_channel_data(0).view(np.uint8) == v0)
        assert np.all(stream.get_channel_data(1).view(np.uint8) == v1)
        assert np.all(stream.get_channel_data(2).view(np.uint8) == v2)

        # Verify that a backward seek past the beginning returns False
        assert not stream.read_record(-3), "Expected False for out-of-bounds backward seek"


# ---- Stream 3: single-channel float32 ----

def test_stream3_structure(cpp_written_egg):
    with monarch3.Monarch3.open_for_reading(cpp_written_egg) as m:
        m.read_header()
        stream = m.get_stream(3)
        assert stream.n_channels        == STREAM3_N_CHANNELS
        assert stream.n_acquisitions    == STREAM3_N_ACQUISITIONS
        assert stream.n_records_in_file == STREAM3_N_RECORDS
        assert stream.channel_record_size == STREAM3_REC_SIZE


def test_stream3_data(cpp_written_egg):
    with monarch3.Monarch3.open_for_reading(cpp_written_egg) as m:
        m.read_header()
        stream = m.get_stream(3)
        for expected_val in STREAM3_VALUES:
            assert stream.read_record(), "Unexpected end of stream 3"
            arr = stream.get_channel_data(0).view(np.float32)
            assert np.all(np.isclose(arr, expected_val, rtol=1e-6)), \
                f"Stream 3: expected ~{expected_val}, got {arr}"
