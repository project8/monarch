"""
monarch3 -- Python interface to the Monarch3 egg-file library.

The ``monarch3`` module is a pybind11 binding of the Monarch3 C++ library.
It exposes the same read/write workflow as the C++ API but with Pythonic
conventions: method and property names use ``snake_case``, raw data buffers
are returned as :class:`numpy.ndarray` views (``uint8`` by default), and
files can be managed with Python's ``with`` statement.

.. note::
   This stub module is used when the compiled ``monarch3`` extension is not
   available (e.g. during documentation builds on ReadTheDocs).  It exists
   solely to allow :mod:`sphinx.ext.autodoc` to introspect the API surface.
   All method bodies are intentionally empty.

Thread safety follows the same rules as the C++ library: calling
:meth:`M3Stream.read_record` and :meth:`M3Stream.write_record` is
thread-safe (the GIL is released during disk I/O), but all other operations
are not thread-safe.
"""

# ---------------------------------------------------------------------------
# Constants
# ---------------------------------------------------------------------------

#: Unsigned integer sample format.
sDigitizedUS: int = 0

#: Signed integer sample format.
sDigitizedS: int = 1

#: Floating-point (analog) sample format.
sAnalog: int = 2

#: Significant bits are aligned to the MSB of the sample word.
sBitsAlignedLeft: int = 0

#: Significant bits are aligned to the LSB of the sample word.
sBitsAlignedRight: int = 1

#: Channel samples are interleaved within the stream record.
sInterleaved: int = 0

#: Each channel occupies its own contiguous block within the stream record.
sSeparate: int = 1


# ---------------------------------------------------------------------------
# Exception
# ---------------------------------------------------------------------------

class Monarch3Exception(Exception):
    """Raised by the monarch3 library on I/O errors and invalid operations."""
    pass


# ---------------------------------------------------------------------------
# Header classes
# ---------------------------------------------------------------------------

class M3StreamHeader:
    """Header information for a single data stream."""

    @property
    def number(self) -> int:
        """Stream index within the file."""
        ...

    @property
    def source(self) -> str:
        """Source identifier string (e.g. digitizer name)."""
        ...

    @property
    def n_channels(self) -> int:
        """Number of channels in this stream."""
        ...

    @property
    def channels(self):
        """List of channel indices belonging to this stream."""
        ...

    @property
    def channel_format(self) -> int:
        """Channel layout: :data:`sInterleaved` or :data:`sSeparate`."""
        ...

    @property
    def acquisition_rate(self) -> int:
        """Acquisition rate in MHz."""
        ...

    @property
    def record_size(self) -> int:
        """Number of samples per record (per channel)."""
        ...

    @property
    def sample_size(self) -> int:
        """Number of elements per sample (1 for real, 2 for complex)."""
        ...

    @property
    def data_type_size(self) -> int:
        """Size in bytes of each sample element."""
        ...

    @property
    def data_format(self) -> int:
        """Data format: :data:`sDigitizedUS`, :data:`sDigitizedS`, or :data:`sAnalog`."""
        ...

    @property
    def bit_depth(self) -> int:
        """Number of significant bits per sample."""
        ...

    @property
    def bit_alignment(self) -> int:
        """Bit alignment: :data:`sBitsAlignedLeft` or :data:`sBitsAlignedRight`."""
        ...

    @property
    def n_acquisitions(self) -> int:
        """Number of acquisitions recorded in this stream."""
        ...

    @property
    def n_records(self) -> int:
        """Total number of records across all acquisitions in this stream."""
        ...

    def __repr__(self) -> str: ...


class M3ChannelHeader:
    """Header information for a single data channel."""

    @property
    def number(self) -> int:
        """Channel index within the file."""
        ...

    @property
    def source(self) -> str:
        """Source identifier string (e.g. digitizer name)."""
        ...

    @property
    def acquisition_rate(self) -> int:
        """Acquisition rate in MHz."""
        ...

    @property
    def record_size(self) -> int:
        """Number of samples per record."""
        ...

    @property
    def sample_size(self) -> int:
        """Number of elements per sample (1 for real, 2 for complex)."""
        ...

    @property
    def data_type_size(self) -> int:
        """Size in bytes of each sample element."""
        ...

    @property
    def data_format(self) -> int:
        """Data format: :data:`sDigitizedUS`, :data:`sDigitizedS`, or :data:`sAnalog`."""
        ...

    @property
    def bit_depth(self) -> int:
        """Number of significant bits per sample."""
        ...

    @property
    def bit_alignment(self) -> int:
        """Bit alignment: :data:`sBitsAlignedLeft` or :data:`sBitsAlignedRight`."""
        ...

    @property
    def voltage_offset(self) -> float:
        """Voltage offset of the channel in volts."""
        ...

    @property
    def voltage_range(self) -> float:
        """Voltage range of the channel in volts."""
        ...

    @property
    def dac_gain(self) -> float:
        """DAC gain of the channel."""
        ...

    @property
    def frequency_min(self) -> float:
        """Minimum frequency in Hz."""
        ...

    @property
    def frequency_range(self) -> float:
        """Frequency range in Hz."""
        ...

    def __repr__(self) -> str: ...


class M3Header:
    """Egg file header: run metadata and stream/channel configuration."""

    # ------------------------------------------------------------------
    # Read-only properties
    # ------------------------------------------------------------------

    @property
    def egg_version(self) -> str:
        """Egg file format version string."""
        ...

    @property
    def n_channels(self) -> int:
        """Total number of channels across all streams."""
        ...

    @property
    def n_streams(self) -> int:
        """Total number of streams in the file."""
        ...

    @property
    def stream_headers(self):
        """List of :class:`M3StreamHeader` objects, one per stream."""
        ...

    @property
    def channel_headers(self):
        """List of :class:`M3ChannelHeader` objects, one per channel."""
        ...

    # ------------------------------------------------------------------
    # Read-write properties
    # ------------------------------------------------------------------

    @property
    def filename(self) -> str:
        """Output filename stored in the header."""
        ...

    @filename.setter
    def filename(self, value: str) -> None: ...

    @property
    def run_duration(self) -> int:
        """Run duration in milliseconds."""
        ...

    @run_duration.setter
    def run_duration(self, value: int) -> None: ...

    @property
    def timestamp(self) -> str:
        """Run timestamp string (ISO 8601 recommended)."""
        ...

    @timestamp.setter
    def timestamp(self, value: str) -> None: ...

    @property
    def description(self) -> str:
        """Free-text description of the run."""
        ...

    @description.setter
    def description(self, value: str) -> None: ...

    # ------------------------------------------------------------------
    # Methods
    # ------------------------------------------------------------------

    def add_stream(self,
                   source: str,
                   acq_rate: int,
                   rec_size: int,
                   sample_size: int,
                   data_type_size: int,
                   data_format: int,
                   bit_depth: int,
                   bit_alignment: int,
                   chan_vec=None) -> int:
        """Add a single-channel stream; returns the stream number.

        :param source: Digitizer or source identifier.
        :param acq_rate: Acquisition rate in MHz.
        :param rec_size: Number of samples per record.
        :param sample_size: Number of elements per sample (1 for real data).
        :param data_type_size: Bytes per element (e.g. 2 for ``uint16``).
        :param data_format: :data:`sDigitizedUS`, :data:`sDigitizedS`, or :data:`sAnalog`.
        :param bit_depth: Number of significant bits per sample.
        :param bit_alignment: :data:`sBitsAlignedLeft` or :data:`sBitsAlignedRight`.
        :param chan_vec: Optional list of channel indices to assign; ``None`` for automatic.
        :returns: Stream number (index used with :meth:`Monarch3.get_stream`).
        """
        ...

    def add_stream(self,  # noqa: F811  (overload)
                   source: str,
                   n_channels: int,
                   channel_format: int,
                   acq_rate: int,
                   rec_size: int,
                   sample_size: int,
                   data_type_size: int,
                   data_format: int,
                   bit_depth: int,
                   bit_alignment: int,
                   chan_vec=None) -> int:
        """Add a multi-channel stream; returns the stream number.

        :param source: Digitizer or source identifier.
        :param n_channels: Number of channels in the stream.
        :param channel_format: :data:`sInterleaved` or :data:`sSeparate`.
        :param acq_rate: Acquisition rate in MHz.
        :param rec_size: Number of samples per record per channel.
        :param sample_size: Number of elements per sample.
        :param data_type_size: Bytes per element.
        :param data_format: :data:`sDigitizedUS`, :data:`sDigitizedS`, or :data:`sAnalog`.
        :param bit_depth: Number of significant bits per sample.
        :param bit_alignment: :data:`sBitsAlignedLeft` or :data:`sBitsAlignedRight`.
        :param chan_vec: Optional list of channel indices to assign; ``None`` for automatic.
        :returns: Stream number (index used with :meth:`Monarch3.get_stream`).
        """
        ...

    def __repr__(self) -> str: ...


# ---------------------------------------------------------------------------
# Record class
# ---------------------------------------------------------------------------

class M3Record:
    """A single data record: record ID, timestamp, and raw data bytes."""

    @property
    def record_id(self) -> int:
        """Record ID (``uint64``)."""
        ...

    @property
    def time(self) -> int:
        """Timestamp in nanoseconds since the start of the run (``uint64``)."""
        ...

    def get_data(self, nbytes: int):
        """Return a writable :class:`numpy.ndarray` (``uint8``) view of the data buffer.

        This is a zero-copy view: the array does not own the data.
        The caller must ensure the record and its parent stream remain alive.

        :param nbytes: Number of bytes to expose; should equal
            :attr:`M3Stream.channel_record_n_bytes` (or
            :attr:`M3Stream.stream_record_n_bytes` for the stream record).
        :returns: ``numpy.ndarray`` of dtype ``uint8``.
            Use :meth:`numpy.ndarray.view` to reinterpret as the actual element type.
        """
        ...


# ---------------------------------------------------------------------------
# Stream class
# ---------------------------------------------------------------------------

class M3Stream:
    """Read/write access for a single data stream."""

    # ------------------------------------------------------------------
    # State query properties
    # ------------------------------------------------------------------

    @property
    def n_channels(self) -> int:
        """Number of channels in this stream."""
        ...

    @property
    def n_acquisitions(self) -> int:
        """Number of acquisitions in this stream (valid after :meth:`read_record` or after writing)."""
        ...

    @property
    def acquisition_id(self) -> int:
        """ID of the most recently accessed acquisition."""
        ...

    @property
    def record_count_in_acq(self) -> int:
        """Number of records read/written in the current acquisition."""
        ...

    @property
    def n_records_in_file(self) -> int:
        """Total number of records across all acquisitions in the file."""
        ...

    @property
    def n_records_in_acquisition(self) -> int:
        """Number of records in the current acquisition."""
        ...

    @property
    def data_type_size(self) -> int:
        """Size in bytes of each sample element."""
        ...

    @property
    def sample_size(self) -> int:
        """Number of elements per sample (1 for real, 2 for complex)."""
        ...

    @property
    def channel_record_size(self) -> int:
        """Number of samples in a channel record."""
        ...

    @property
    def channel_record_n_bytes(self) -> int:
        """Size in bytes of a channel record data buffer."""
        ...

    @property
    def stream_record_size(self) -> int:
        """Number of samples in the full (potentially interleaved) stream record."""
        ...

    @property
    def stream_record_n_bytes(self) -> int:
        """Size in bytes of the full stream record data buffer."""
        ...

    @property
    def is_interleaved(self) -> bool:
        """``True`` if multi-channel data is stored interleaved in the file."""
        ...

    # ------------------------------------------------------------------
    # Record reading
    # ------------------------------------------------------------------

    def read_record(self, offset: int = 0, if_new_acq_start_at_first_rec: bool = True) -> bool:
        """Read a record from the file.

        Assuming the last record read was ``[J]``, reads record ``[J+1+offset]``.

        :param offset:
            * ``0`` (default): advance to the next record.
            * ``-1``: re-read the current record.
            * ``< -1``: step backward in the file.
            * ``> 0``: skip forward in the file.
        :param if_new_acq_start_at_first_rec: If ``True`` (default), when
            stepping into a new acquisition the first record of that
            acquisition is read regardless of the offset.  Set to ``False``
            for backwards-compatible behaviour.
        :returns: ``True`` on success; ``False`` when the requested position
            is past the end (or before the beginning) of the file.
        :raises Monarch3Exception: On I/O error.
        """
        ...

    def get_stream_record(self) -> M3Record:
        """Return the stream-level record object (all channels interleaved)."""
        ...

    def get_channel_record(self, channel: int) -> M3Record:
        """Return the record object for the given channel index.

        :param channel: Zero-based channel index.
        """
        ...

    # ------------------------------------------------------------------
    # Convenience numpy data access
    # ------------------------------------------------------------------

    def get_stream_data(self):
        """Return a writable :class:`numpy.ndarray` (``uint8``) view of the full stream record.

        Call after :meth:`read_record`.
        Use :meth:`numpy.ndarray.view` to reinterpret the buffer as the
        actual element type (e.g. ``arr.view(np.uint16)``).

        :returns: ``numpy.ndarray`` of dtype ``uint8`` with length
            :attr:`stream_record_n_bytes`.
        """
        ...

    def get_channel_data(self, channel: int):
        """Return a writable :class:`numpy.ndarray` (``uint8``) view of a single channel's data.

        Call after :meth:`read_record` (reading) or before :meth:`write_record` (writing).
        Use :meth:`numpy.ndarray.view` to reinterpret the buffer as the
        actual element type (e.g. ``arr.view(np.uint16)``).

        :param channel: Zero-based channel index.
        :returns: ``numpy.ndarray`` of dtype ``uint8`` with length
            :attr:`channel_record_n_bytes`.
        """
        ...

    # ------------------------------------------------------------------
    # Record writing
    # ------------------------------------------------------------------

    def write_record(self, is_new_acquisition: bool) -> bool:
        """Write the current record contents to the file.

        Fill the data buffer via :meth:`get_channel_data` (or
        :meth:`get_stream_data`) before calling this method.

        :param is_new_acquisition: ``True`` to start a new acquisition group;
            ``False`` to continue the current acquisition.
        :returns: ``True`` on success.
        :raises Monarch3Exception: On I/O error.
        """
        ...


# ---------------------------------------------------------------------------
# Top-level file handle
# ---------------------------------------------------------------------------

class Monarch3:
    """Top-level egg v3 file handle.

    Use :meth:`open_for_reading` or :meth:`open_for_writing` to obtain an
    instance.  Supports the context manager protocol (``with`` statement),
    which automatically calls :meth:`finish_reading` or
    :meth:`finish_writing` on exit.

    Example (reading)::

        with monarch3.Monarch3.open_for_reading("data.egg") as m:
            m.read_header()
            stream = m.get_stream(0)
            while stream.read_record():
                arr = stream.get_channel_data(0).view(np.uint16)
                ...

    Example (writing)::

        with monarch3.Monarch3.open_for_writing("out.egg") as m:
            hdr = m.get_header()
            hdr.run_duration = 1000
            hdr.add_stream("src", 200, 4096, 1, 2, monarch3.sDigitizedUS, 14,
                           monarch3.sBitsAlignedRight)
            m.write_header()
            stream = m.get_stream(0)
            buf = stream.get_channel_data(0).view(np.uint16)
            buf[:] = my_samples
            stream.write_record(True)
    """

    @staticmethod
    def open_for_reading(filename: str) -> "Monarch3":
        """Open an existing egg file for reading.

        :param filename: Path to the egg file.
        :returns: A :class:`Monarch3` instance in the ``eOpenToRead`` state.
        :raises Monarch3Exception: If the file cannot be opened.
        """
        ...

    @staticmethod
    def open_for_writing(filename: str) -> "Monarch3":
        """Create or overwrite an egg file for writing.

        :param filename: Path to the egg file.
        :returns: A :class:`Monarch3` instance in the ``eOpenToWrite`` state.
        :raises Monarch3Exception: If the file cannot be created.
        """
        ...

    def __enter__(self) -> "Monarch3":
        """Enter the context manager; returns ``self``."""
        ...

    def __exit__(self, exc_type, exc_val, exc_tb) -> bool:
        """Exit the context manager.

        Calls :meth:`finish_reading` or :meth:`finish_writing` as appropriate.
        Does not suppress exceptions (always returns ``False``).
        """
        ...

    @property
    def state(self) -> int:
        """Current state of the file handle."""
        ...

    # ------------------------------------------------------------------
    # Reading interface
    # ------------------------------------------------------------------

    def read_header(self) -> None:
        """Read header information from the file.

        Must be called after :meth:`open_for_reading` and before accessing
        streams or header data.

        :raises Monarch3Exception: On I/O error.
        """
        ...

    def get_header(self) -> M3Header:
        """Return the file header.

        When called on a file opened for reading, the header is read-only
        (modifying it has no effect on the file).  When called on a file
        opened for writing, the returned object is mutable and should be
        configured before calling :meth:`write_header`.

        :returns: The :class:`M3Header` for this file.
        """
        ...

    def get_stream(self, stream: int) -> M3Stream:
        """Return the stream object for the given stream index.

        :param stream: Zero-based stream index.
        :returns: The :class:`M3Stream` for the requested stream.
        """
        ...

    def finish_reading(self) -> None:
        """Close the file after reading.

        Not needed when using the ``with`` statement.
        """
        ...

    # ------------------------------------------------------------------
    # Writing interface
    # ------------------------------------------------------------------

    def write_header(self) -> None:
        """Write the header to the file and prepare streams for writing.

        Must be called after configuring the header (via :meth:`get_header`)
        and adding all streams (via :meth:`M3Header.add_stream`).

        :raises Monarch3Exception: On I/O error.
        """
        ...

    def finish_writing(self) -> None:
        """Flush and close the file after writing.

        Not needed when using the ``with`` statement.
        """
        ...
