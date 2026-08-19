How to use Monarch3 from Python
================================

The ``monarch3`` Python module is a pybind11 binding of the Monarch3 C++ library.
It exposes the same read/write workflow as the C++ API but with Pythonic conventions:
method and property names use ``snake_case``, raw data buffers are returned as
``numpy.ndarray`` views (``uint8`` by default), and files can be managed with Python's
``with`` statement.

.. note::
   Record data is returned as a ``numpy.ndarray`` of ``uint8`` bytes.
   Use ``array.view(dtype)`` to reinterpret the buffer as the appropriate element type
   (e.g. ``np.uint16``, ``np.float32``).  The element type and size are available from
   the stream header's ``data_type_size`` and ``data_format`` properties.

Thread safety follows the same rules as the C++ library: calling ``read_record()`` and
``write_record()`` is thread-safe (the GIL is released during disk I/O), but all other
operations are not thread-safe.


Constants
---------

The following constants are provided by the ``monarch3`` module:

**Data format**

* ``sDigitizedUS`` -- unsigned integer samples
* ``sDigitizedS``  -- signed integer samples
* ``sAnalog``      -- floating-point samples

**Bit alignment**

* ``sBitsAlignedLeft``  -- significant bits are aligned to the MSB of the sample word
* ``sBitsAlignedRight`` -- significant bits are aligned to the LSB of the sample word

**Channel format (multi-channel streams)**

* ``sInterleaved`` -- channel samples are interleaved within the stream record
* ``sSeparate``    -- each channel has its own contiguous block within the stream record


Reading Egg3 Files
------------------

1. Open the file::

      m = monarch3.Monarch3.open_for_reading( filename )

   Or use the context manager (recommended; closes the file automatically)::

      with monarch3.Monarch3.open_for_reading( filename ) as m:
          ...

2. Read the header::

      m.read_header()

3. Inspect the header::

      hdr = m.get_header()
      print( hdr.egg_version )
      print( hdr.run_duration )
      print( hdr.timestamp )
      print( hdr.description )

   Stream and channel metadata are available through header lists::

      stream_hdr = hdr.stream_headers[0]
      print( stream_hdr.n_channels, stream_hdr.acquisition_rate, stream_hdr.record_size )
      print( stream_hdr.data_type_size, stream_hdr.data_format )

      channel_hdr = hdr.channel_headers[0]
      print( channel_hdr.voltage_offset, channel_hdr.voltage_range )

4. Get a stream::

      stream = m.get_stream( stream_index )
      n_records = stream.n_records_in_file

5. Access data record by record.
   Use ``get_channel_data( channel )`` to obtain a ``numpy.ndarray`` of ``uint8`` bytes
   for a single channel, or ``get_stream_data()`` for the full (potentially interleaved)
   stream buffer::

      while stream.read_record():
          arr = stream.get_channel_data( 0 )
          # reinterpret bytes as the actual element type, e.g.:
          samples = arr.view( np.uint16 )
          print( samples )

   The ``offset`` parameter to ``read_record`` controls navigation within the file
   (same semantics as the C++ API):

   * ``offset == 0`` (default): advance to the next record
   * ``offset == -1``: re-read the current record
   * ``offset < -1``: step backward
   * ``offset > 0``: skip forward

   ``read_record()`` returns ``True`` on success and ``False`` when the requested
   position is past the end (or before the beginning) of the file.

6. Close the file (not needed when using the ``with`` statement)::

      m.finish_reading()


Writing Egg3 Files
------------------

1. Open the file::

      m = monarch3.Monarch3.open_for_writing( filename )

   Or use the context manager::

      with monarch3.Monarch3.open_for_writing( filename ) as m:
          ...

2. Configure the header::

      hdr = m.get_header()
      hdr.filename     = filename
      hdr.run_duration = 1000       # milliseconds
      hdr.timestamp    = "2024-01-01T00:00:00"
      hdr.description  = "My data"

3. Add streams.
   For a single-channel stream::

      stream_num = hdr.add_stream(
          source         = "my-digitizer",
          acq_rate       = 200,             # MHz
          rec_size       = 4096,            # samples per record
          sample_size    = 1,               # elements per sample
          data_type_size = 2,               # bytes per element
          data_format    = monarch3.sDigitizedUS,
          bit_depth      = 14,
          bit_alignment  = monarch3.sBitsAlignedRight,
      )

   For a multi-channel stream::

      stream_num = hdr.add_stream(
          source         = "my-2ch-digitizer",
          n_channels     = 2,
          channel_format = monarch3.sInterleaved,
          acq_rate       = 200,
          rec_size       = 4096,
          sample_size    = 1,
          data_type_size = 2,
          data_format    = monarch3.sDigitizedUS,
          bit_depth      = 14,
          bit_alignment  = monarch3.sBitsAlignedRight,
      )

4. Write the header (this creates the HDF5 structure and allocates stream objects)::

      m.write_header()

5. Get the stream object::

      stream = m.get_stream( stream_num )

6. For each record, fill the data buffer and write to disk::

      # get a writable uint8 view; reinterpret as the target element type
      buf = stream.get_channel_data( 0 ).view( np.uint16 )
      buf[:] = my_samples          # numpy array of uint16

      stream.write_record( is_new_acquisition=True )   # True = start new acquisition
      stream.write_record( is_new_acquisition=False )  # False = continue acquisition

7. Close the file (not needed when using the ``with`` statement)::

      m.finish_writing()


Complete Examples
-----------------

**Writing a file**

.. code-block:: python

   import monarch3
   import numpy as np

   filename = "output.egg"

   with monarch3.Monarch3.open_for_writing( filename ) as m:
       hdr = m.get_header()
       hdr.filename     = filename
       hdr.run_duration = 1000
       hdr.timestamp    = "2024-01-01T00:00:00"
       hdr.description  = "Example egg file"

       stream_num = hdr.add_stream(
           source         = "my-digitizer",
           acq_rate       = 200,
           rec_size       = 1024,
           sample_size    = 1,
           data_type_size = 1,
           data_format    = monarch3.sDigitizedUS,
           bit_depth      = 8,
           bit_alignment  = monarch3.sBitsAlignedLeft,
       )

       m.write_header()

       stream = m.get_stream( stream_num )

       # Acquisition 0, two records
       buf = stream.get_channel_data( 0 )
       buf[:] = 42
       stream.write_record( True )

       buf[:] = 100
       stream.write_record( False )


**Reading a file**

.. code-block:: python

   import monarch3
   import numpy as np

   with monarch3.Monarch3.open_for_reading( "output.egg" ) as m:
       m.read_header()
       hdr = m.get_header()
       print( "Egg version:", hdr.egg_version )
       print( "Timestamp:  ", hdr.timestamp )

       stream_hdr = hdr.stream_headers[0]
       dtype = np.uint8  # choose based on stream_hdr.data_type_size and data_format

       stream = m.get_stream( 0 )
       print( f"Stream 0: {stream.n_records_in_file} record(s)" )

       while stream.read_record():
           arr = stream.get_channel_data( 0 ).view( dtype )
           print( f"  acq={stream.acquisition_id}  rec={stream.record_count_in_acq}  data={arr}" )
