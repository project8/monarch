How to use Monarch3 in C++
==========================

Thread safety: Reading and writing records (via ``M3Stream::ReadRecord()`` and ``M3Stream::WriteRecord()``, respectively) are thread-safe
except that the HDF5 C library (on which the C++ library is built) is inherently non-thread-safe.  Though multi-threaded writing may
work even most of the time, it is inherently unstable.
All other operations in Monarch (besides writing and reading records) are explicitly not thread-safe.


Constants
---------

The following constants are defined in the ``monarch3`` namespace (``M3Constants.hh``):

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

1. Open the file: ``Monarch3::OpenForReading( [filename] )``

2. Read the header: ``Monarch3::ReadHeader()``

3. Get the pointer to the header and inspect it: ``Monarch3::GetHeader()``

   Key fields on ``M3Header``:

   * ``GetEggVersion()`` / ``SetEggVersion()``
   * ``GetFilename()``
   * ``GetRunDuration()`` -- run duration in milliseconds
   * ``GetTimestamp()``
   * ``GetDescription()``

   Stream and channel metadata are available through header vectors:

   .. code-block:: cpp

      const M3Header* hdr = monarch->GetHeader();

      // Stream-level metadata
      const M3StreamHeader& streamHdr = hdr->GetStreamHeaders()[0];
      streamHdr.GetNChannels();
      streamHdr.GetAcquisitionRate();
      streamHdr.GetRecordSize();
      streamHdr.GetDataTypeSize();
      streamHdr.GetDataFormat();

      // Channel-level metadata
      const M3ChannelHeader& chanHdr = hdr->GetChannelHeaders()[0];
      chanHdr.GetVoltageOffset();
      chanHdr.GetVoltageRange();

4. Get the pointer(s) to the stream(s) in the file: ``Monarch3::GetStream( [stream number] )``

   The total number of records in the stream is available via ``M3Stream::GetNRecordsInFile()``.

5. Setup to access the data in a stream.  You can access either the record for the entire stream with ``M3Stream::GetStreamRecord()``,
   or for individual channels with ``M3Stream::GetChannelRecord( [channel number] )``.
   If you have only one channel in the stream, the distinction between those is irrelevant.
   The record objects have a function ``M3Record::GetData()`` to get the raw byte data array.
   There are three ways in which you can interact with the data array:

   * If you want to access the data as an array of bytes (e.g. because either your data is of type ``uint8_t``, or you want to use ``memcpy``), you can use the pointer returned by ``M3Record::GetData()``;
   * If you want to access the data as an array of other integer or floating-point data types, you can pass the data pointer from ``M3Record::GetData()`` to an ``M3DataReader`` object, along with the data type size and data format flag. The type of the values that are returned is specified as a template argument for ``M3DataReader``; it doesn't have to match the data type in the data array exactly, but it should have at least as many bytes as the data elements, and if the data elements are integer, it should be an integer, and if the data elements are floating-point, it should be floating-point.
   * If you want to access the data as an array of complex floating-point data types, you can pass the data pointer from ``M3Record::GetData()`` to an ``M3ComplexDataReader`` object, along with the data type size and data format flag (you can also specify the element size, but for complex data it should be the default, 2).  The type of the values that are returned is specified as a template argument for ``M3ComplexDataReader``; it should either be ``f4_complex`` or ``f8_complex``, or the equivalent.

6. When moving from record to record in the file, the memory used for the data stays the same, but it gets updated with new values.
   To move to a new record use the ``M3Stream::ReadRecord( [offset] )`` function.  The offset parameter allows you to move forward and
   backward within the file. If the last record read was ``[J]`` (``= -1`` for a just-opened file), ``ReadRecord`` will access the ``[J+1+offset]`` record.
   This means that the offset parameter has the following meanings:

   * if ``offset == 0`` (default), the next record will be accessed;
   * if ``offset == -1``, the current record will be reread;
   * ``offset < -1`` will go backwards in the file;
   * ``offset > 0`` will skip forward in the file.

   The outcomes from the call are:

   * returns ``true`` if the move was successful;
   * returns ``false`` if the move was unsuccessful because it goes past the end (or beginning) of the file;
   * throws an ``M3Exception`` if there was an error.

   After each successful ``ReadRecord``, the current acquisition and record position are available via:

   * ``M3Stream::GetAcquisitionId()`` -- index of the current acquisition
   * ``M3Stream::GetRecordCountInAcq()`` -- record index within the current acquisition

   A typical read loop:

   .. code-block:: cpp

      M3Stream* stream = monarch->GetStream( 0 );
      const M3Record* record = stream->GetChannelRecord( 0 );

      // Data type from the stream header
      unsigned dataTypeSize = stream->GetDataTypeSize();
      uint32_t dataFormat = hdr->GetStreamHeaders()[0].GetDataFormat();

      M3DataReader< uint16_t > reader( record->GetData(), dataTypeSize, dataFormat );

      while( stream->ReadRecord() )
      {
          unsigned recSize = stream->GetChannelRecordSize();
          for( unsigned i = 0; i < recSize; ++i )
          {
              uint16_t sample = reader.at( i );
              // process sample ...
          }
      }

7. When you're finished reading, use ``Monarch3::FinishReading()`` to close the file.


Writing Egg3 Files
------------------

1. Open the file: ``Monarch3::OpenForWriting( [filename] )``

2. Get the pointer to the header and configure it: ``Monarch3::GetHeader()``

   .. code-block:: cpp

      M3Header* hdr = monarch->GetHeader();
      hdr->SetFilename( filename );
      hdr->SetRunDuration( 1000 );                     // milliseconds
      hdr->SetTimestamp( "2024-01-01T00:00:00" );
      hdr->SetDescription( "My data" );

3. Add streams using the ``AddStream`` functions.  Both overloads return the stream number,
   which is used to address the stream after the header is written.

   For a single-channel stream::

      unsigned streamNum = hdr->AddStream(
          source,           // std::string: digitizer label
          acqRate,          // uint32_t: acquisition rate (MHz)
          recSize,          // uint32_t: samples per record
          sampleSize,       // uint32_t: elements per sample (1 for real, 2 for complex)
          dataTypeSize,     // uint32_t: bytes per element
          dataFormat,       // uint32_t: sDigitizedUS, sDigitizedS, or sAnalog
          bitDepth,         // uint32_t: number of significant bits per sample
          bitAlignment      // uint32_t: sBitsAlignedLeft or sBitsAlignedRight
      );

   For a multi-channel stream::

      unsigned streamNum = hdr->AddStream(
          source,           // std::string: digitizer label
          nChannels,        // uint32_t: number of channels
          channelFormat,    // uint32_t: sInterleaved or sSeparate
          acqRate,          // uint32_t: acquisition rate (MHz)
          recSize,          // uint32_t: samples per record per channel
          sampleSize,       // uint32_t: elements per sample
          dataTypeSize,     // uint32_t: bytes per element
          dataFormat,       // uint32_t: sDigitizedUS, sDigitizedS, or sAnalog
          bitDepth,         // uint32_t: number of significant bits per sample
          bitAlignment      // uint32_t: sBitsAlignedLeft or sBitsAlignedRight
      );

4. Write the header information (this creates the HDF5 structure and allocates stream objects): ``Monarch3::WriteHeader()``

5. Get the pointer(s) to the stream(s) in the file: ``Monarch3::GetStream( [stream number] )``

6. Setup to access the data in a stream.  You can access either the record for the entire stream with ``M3Stream::GetStreamRecord()``,
   or for individual channels with ``M3Stream::GetChannelRecord( [channel number] )``.
   If you have only one channel in the stream, the distinction between those is irrelevant.
   The record objects have a function ``M3Record::GetData()`` to get the raw byte data array.
   There are three ways in which you can interact with the data array:

   * If you want to access the data as an array of bytes (e.g. because either your data is of type ``uint8_t``, or you want to use ``memcpy``), you can use the pointer returned by ``M3Record::GetData()``;
   * If you want to access the data as an array of other integer or floating-point data types, you can pass the data pointer from ``M3Record::GetData()`` to an ``M3DataWriter`` object, along with the data type size and data format flag. The type of the values that are passed to the writer is specified as a template argument for ``M3DataWriter``; it doesn't have to match the data type in the data array exactly, but it should be no larger than the data elements, and if the data elements are integer, it should be an integer, and if the data elements are floating-point, it should be floating-point.
   * If you want to access the data as an array of complex floating-point data types, you can pass the data pointer from ``M3Record::GetData()`` to an ``M3ComplexDataWriter`` object, along with the data type size and data format flag (you can also specify the element size, but for complex data it should be the default, 2).  The type of the values that are returned is specified as a template argument for ``M3ComplexDataWriter``; it should either be ``f4_complex`` or ``f8_complex``, or the equivalent.

7. For each record, copy the data to the stream data memory using the access method you chose above, and then write to disk with ``M3Stream::WriteRecord( [is new acquisition?] )``.
   When a record is from a different acquisition than the previous record, the flag passed to ``WriteRecord`` should be ``true``; otherwise it should be ``false``.
   The outcomes from the call are:

   * returns ``true`` if the write was successful;
   * throws an ``M3Exception`` if there was an error;
   * (should never return ``false``).

8. When you're finished writing, use ``Monarch3::FinishWriting()`` to close the file.


Complete Examples
-----------------

**Writing a file**

.. code-block:: cpp

   #include "M3Monarch.hh"
   #include "M3DataInterface.hh"
   #include <cstring>

   using namespace monarch3;

   int main()
   {
       std::string filename = "output.egg";

       Monarch3* monarch = Monarch3::OpenForWriting( filename );

       M3Header* hdr = monarch->GetHeader();
       hdr->SetFilename( filename );
       hdr->SetRunDuration( 1000 );
       hdr->SetTimestamp( "2024-01-01T00:00:00" );
       hdr->SetDescription( "Example egg file" );

       unsigned streamNum = hdr->AddStream(
           "my-digitizer",
           200,                   // acquisition rate (MHz)
           1024,                  // samples per record
           1,                     // elements per sample
           1,                     // bytes per element (uint8)
           sDigitizedUS,
           8,                     // bit depth
           sBitsAlignedLeft
       );

       monarch->WriteHeader();

       M3Stream* stream = monarch->GetStream( streamNum );
       M3Record* record = stream->GetChannelRecord( 0 );
       unsigned recSize = stream->GetChannelRecordSize();

       M3DataWriter< uint8_t > writer( record->GetData(), 1, sDigitizedUS );

       // Acquisition 0, first record
       for( unsigned i = 0; i < recSize; ++i ) writer.set_at( 42, i );
       stream->WriteRecord( true );    // true = new acquisition

       // Acquisition 0, second record
       for( unsigned i = 0; i < recSize; ++i ) writer.set_at( 100, i );
       stream->WriteRecord( false );   // false = continue acquisition

       monarch->FinishWriting();
       delete monarch;
       return 0;
   }


**Reading a file**

.. code-block:: cpp

   #include "M3Monarch.hh"
   #include "M3DataInterface.hh"
   #include <iostream>

   using namespace monarch3;

   int main()
   {
       Monarch3* monarch = Monarch3::OpenForReading( "output.egg" );
       monarch->ReadHeader();

       const M3Header* hdr = monarch->GetHeader();
       std::cout << "Egg version: " << hdr->GetEggVersion() << "\n";
       std::cout << "Timestamp:   " << hdr->GetTimestamp()  << "\n";

       const M3StreamHeader& streamHdr = hdr->GetStreamHeaders()[0];
       unsigned dataTypeSize = streamHdr.GetDataTypeSize();
       uint32_t dataFormat   = streamHdr.GetDataFormat();

       M3Stream* stream = monarch->GetStream( 0 );
       std::cout << "Stream 0: " << stream->GetNRecordsInFile() << " record(s)\n";

       const M3Record* record = stream->GetChannelRecord( 0 );
       M3DataReader< uint8_t > reader( record->GetData(), dataTypeSize, dataFormat );

       while( stream->ReadRecord() )
       {
           unsigned recSize = stream->GetChannelRecordSize();
           std::cout << "  acq=" << stream->GetAcquisitionId()
                     << "  rec=" << stream->GetRecordCountInAcq()
                     << "  data[0]=" << (int)reader.at( 0 ) << "\n";
       }

       monarch->FinishReading();
       delete monarch;
       return 0;
   }
