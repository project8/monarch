How to use Monarch3
===================

Thread safety: Reading and writing records (via M3Stream::ReadRecord() and M3Stream::WriteRecord(), respectively) are thread-safe.  All other operations are not thread-safe.

Reading Egg3 Files
------------------

1. Open the file: ``Monarch3::OpenForReading( [filename] )``
2. Access the header information: ``Monarch3::ReadHeader()``
3. Get the pointer to the header, and use as needed: ``Monarch3::GetHeader()``
4. Get the pointer(s) to the stream(s) in the file: ``Monarch3::GetStream( [stream number] )``
5. Setup to access the data in a stream.  You can acces either the record for the entire stream with ``Monarch3::GetStreamRecord()``, 
   or for individual channels with ``Monarch3::GetChannelRecord( [record number] )``.  
   If you have only one channel in the stream, the distinction between those is irrelevant.
   The record objects have a function ``M3Record::GetData()`` to get the data array.
   There are two ways in which you can interact with the data array:

   * If you want to access the data as an array of bytes (e.g. because either your data is of type ``uint8_t``, or you want to use ``memcpy``), you can use the pointer returned by ``M3Record::GetData()``;
   * If you want to access the data as an array of other integer or floating-point data types, you can pass the data pointer from ``M3Record::GetData()`` to an ``M3DataReader`` object, along with the data type size and data format flag. The type of the values that are returned is specified as a template argument for ``M3DataReader``; it doesn't have to match the data type in the data array exactly, but it should have at least as many bytes as the data elements, and if the data elements are integer, it should be an integer, and if the data elements are floating-point, it should be floating-point.
   * If you want to access the data as an array of complex floating-point data types, you can pass the data pointer from ``M3Record::GetData()`` to an ``M3ComplexDataReader`` object, along with the data type size and data format flag (you can also specify the element size, but for complex data it should be the default, 2).  The type of the values that are returned is specified as a template argument for ``M3ComplexDataReader``; it should either be f4_complex or f8_complex, or the equivalent.
   
6. When moving from record to record in the file, the memory used for the data stays the same, but it gets updated with new values.  
   To move to a new record use the ``M3Stream::ReadRecord( [offset] )`` function.  The offset parameter allows you to move forward and 
   backward within the file. If the last record read was ``[J]`` (``= -1`` for a just-opened file), ReadRecord will access the ``[J+1+offset]`` record.
   This means that the offset parameter has the following meanings:
   
   * if ``offset == 0`` (default), the next record will be accessed;
   * if ``offset == -1``, the current record will be reread;
   * ``offset < -1`` will go backwards in the file;
   * ``offset > 0`` will skip forward in the file.
   
   The outcomes from the call are:
   
   * returns true if the move was successful;
   * returns false if the move was unsuccessful because it goes past the end (or beginning) of the file;
   * throws an M3Exception if there was an error.
   
7. When you're finished reading, use ``Monarch3::FinishReading()`` to close the file.


Writing Egg3 Files
------------------

1. Open the file: ``Monarch3::OpenForWriting( [filename] )``
2. Get the pointer to the header, and use as needed: ``Monarch3::GetHeader()``
3. Fill in the header information and setup the streams.  For the latter, use the AddStream functions to add streams with one or multiple channels.
4. Write the header information: ``Monarch3::WriteHeader()``
5. Get the pointer(s) to the stream(s) in the file: ``Monarch3::GetStream( [stream number] )``
6. Setup to access the data in a stream.  You can acces either the record for the entire stream with ``Monarch3::GetStreamRecord()``, 
   or for individual channels with ``Monarch3::GetChannelRecord( [record number] )``.  
   If you have only one channel in the stream, the distinction between those is irrelevant.
   The record objects have a function ``M3Record::GetData()`` to get the data array.
   There are two ways in which you can interact with the data array:

   * If you want to access the data as an array of bytes (e.g. because either your data is of type ``uint8_t``, or you want to use ``memcpy``), you can use the pointer returned by ``M3Record::GetData()``;
   * If you want to access the data as an array of other integer or floating-point data types, you can pass the data pointer from ``M3Record::GetData()`` to an ``M3DataWriter`` object, along with the data type size and data format flag. The type of the values that are passed to the writer is specified as a template argument for ``M3DataWriter``; it doesn't have to match the data type in the data array exactly, but it should be no larger than the data elements, and if the data elements are integer, it should be an integer, and if the data elements are floating-point, it should be floating-point.
   * If you want to access the data as an array of complex floating-point data types, you can pass the data pointer from ``M3Record::GetData()`` to an ``M3ComplexDataWriter`` object, along with the data type size and data format flag (you can also specify the element size, but for complex data it should be the default, 2).  The type of the values that are returned is specified as a template argument for ``M3ComplexDataWriter``; it should either be ``f4_complex`` or ``f8_complex``, or the equivalent.
   
7. For each record, copy the data to the stream data memory using the access method you chose above, and then write to disk with ``M3Stream::WriteRecord( [is new acquisition?] )``.  
   When a record is from a different acquisition than the previous record, the flag passed to ``WriteRecord`` should be ``true``; otherwise it should be ``false``.
   The outcomes from the call are:
   
   * returns true if the write was successful;
   * throws an M3Exception if there was an error;
   * (should never return false).

8. When you're finished writing, use ``Monarch3::FinishWriting()`` to close the file.