# test_python_write_python_read.py
# Pure Python round-trip: write with the Python binding, read back with the Python
# binding.  Isolates the binding from the C++ executables.

import numpy as np
import monarch3

from conftest import (
    HEADER_RUN_DURATION, HEADER_DESCRIPTION,
    STREAM0_N_CHANNELS, STREAM0_N_ACQUISITIONS, STREAM0_N_RECORDS, STREAM0_REC_SIZE, STREAM0_VALUES,
    STREAM1_N_CHANNELS, STREAM1_N_ACQUISITIONS, STREAM1_N_RECORDS, STREAM1_REC_SIZE, STREAM1_VALUES,
    STREAM2_N_CHANNELS, STREAM2_N_ACQUISITIONS, STREAM2_N_RECORDS, STREAM2_REC_SIZE, STREAM2_VALUES,
    STREAM3_N_CHANNELS, STREAM3_N_ACQUISITIONS, STREAM3_N_RECORDS, STREAM3_REC_SIZE, STREAM3_VALUES,
)
from test_python_write_cpp_read import write_standard_egg


def test_header_round_trip(tmp_egg):
    write_standard_egg(tmp_egg)
    with monarch3.Monarch3.open_for_reading(tmp_egg) as m:
        m.read_header()
        hdr = m.get_header()
        assert hdr.run_duration == HEADER_RUN_DURATION
        assert hdr.description  == HEADER_DESCRIPTION
        assert hdr.n_streams    == 4


def test_stream0_round_trip(tmp_egg):
    write_standard_egg(tmp_egg)
    with monarch3.Monarch3.open_for_reading(tmp_egg) as m:
        m.read_header()
        stream = m.get_stream(0)
        assert stream.n_channels        == STREAM0_N_CHANNELS
        assert stream.n_acquisitions    == STREAM0_N_ACQUISITIONS
        assert stream.n_records_in_file == STREAM0_N_RECORDS
        assert stream.channel_record_size == STREAM0_REC_SIZE
        for expected_val in STREAM0_VALUES:
            assert stream.read_record(), "Unexpected end of stream 0"
            arr = stream.get_channel_data(0).view(np.uint8)
            assert np.all(arr == expected_val), \
                f"Stream 0: expected all {expected_val}, got {arr}"
        assert not stream.read_record(), "Stream 0 should be exhausted"


def test_stream1_round_trip(tmp_egg):
    write_standard_egg(tmp_egg)
    with monarch3.Monarch3.open_for_reading(tmp_egg) as m:
        m.read_header()
        stream = m.get_stream(1)
        assert stream.n_channels        == STREAM1_N_CHANNELS
        assert stream.n_acquisitions    == STREAM1_N_ACQUISITIONS
        assert stream.n_records_in_file == STREAM1_N_RECORDS
        assert stream.channel_record_size == STREAM1_REC_SIZE
        for v0, v1 in STREAM1_VALUES:
            assert stream.read_record(), "Unexpected end of stream 1"
            ch0 = stream.get_channel_data(0).view(np.uint16)
            ch1 = stream.get_channel_data(1).view(np.uint16)
            assert np.all(ch0 == v0), f"Stream 1 ch0: expected {v0}, got {ch0}"
            assert np.all(ch1 == v1), f"Stream 1 ch1: expected {v1}, got {ch1}"


def test_stream2_round_trip(tmp_egg):
    write_standard_egg(tmp_egg)
    with monarch3.Monarch3.open_for_reading(tmp_egg) as m:
        m.read_header()
        stream = m.get_stream(2)
        assert stream.n_channels        == STREAM2_N_CHANNELS
        assert stream.n_acquisitions    == STREAM2_N_ACQUISITIONS
        assert stream.n_records_in_file == STREAM2_N_RECORDS
        assert stream.channel_record_size == STREAM2_REC_SIZE
        for v0, v1, v2 in STREAM2_VALUES:
            assert stream.read_record(), "Unexpected end of stream 2"
            assert np.all(stream.get_channel_data(0).view(np.uint8) == v0)
            assert np.all(stream.get_channel_data(1).view(np.uint8) == v1)
            assert np.all(stream.get_channel_data(2).view(np.uint8) == v2)


def test_stream3_round_trip(tmp_egg):
    write_standard_egg(tmp_egg)
    with monarch3.Monarch3.open_for_reading(tmp_egg) as m:
        m.read_header()
        stream = m.get_stream(3)
        assert stream.n_channels        == STREAM3_N_CHANNELS
        assert stream.n_acquisitions    == STREAM3_N_ACQUISITIONS
        assert stream.n_records_in_file == STREAM3_N_RECORDS
        assert stream.channel_record_size == STREAM3_REC_SIZE
        for expected_val in STREAM3_VALUES:
            assert stream.read_record(), "Unexpected end of stream 3"
            arr = stream.get_channel_data(0).view(np.float32)
            assert np.all(np.isclose(arr, expected_val, rtol=1e-6)), \
                f"Stream 3: expected ~{expected_val}, got {arr}"
