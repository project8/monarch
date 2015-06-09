Egg v2 File Standard
====================

Usage history: 2012-2014

Relevant software:
- Monarch v2.6.6 or newer (Monarch v3 will retain the ability to read Egg v2 files)

File Structure
--------------

1. Prelude
2. Header
3. Records


Prelude
-------
This is 4-byte word describing the size of the size of the header in bytes.

Header
------
The header is de/serialized using Google Protocol Buffers.

The fields are specified in MonarchHeader.proto, and include the following:

- filename (string; required) -- Filename
- acqRate (double; required) -- Acquisition rate in MHz
- acqMode (uint32; required) -- Acquisition mode describing how many channels were used:

  * 1 == one channel
  * 2 == two channels

- acqTime (uint32; required) -- Run length in ms
- recSize (uint32; required) -- Number of bytes in each record (sum of both channels in 2-channel mode)
- runDate (string; optional) -- Timestamp string
- runInfo (string; optional) -- Description of the run; in files created by Mantis this typically included the Mantis client and server configurations in JSON format.
- runSource (enum; optional) -- Run source information (not typically used)

  * 0 == Mantis
  * 1 == Simulation

- runType (enum; optional) -- Run content information (not typically used)

  * 0 == Background
  * 1 == Signal
  * 999 == Other

- formatMode (enum; optional) -- How data from multiple channels was arranged:

  * 0 == Single
  * 1 == Multiple, separate
  * 2 == Multiple, interleaved

- dataTypeSize (uint32; optional) -- Size of the integer data type used to make the data
- bitDepth (uint32; optional) -- Number of bits used to digitize each sample
- voltageMin (double; optional) -- Minimum voltage accepted by the digitizer
- voltageRange (double; optional) -- Voltage range accepted by the digitizer (above the minimum)

Record
------

Following the header, records are written to disk sequentially.  

The header for each record includes an acquisition ID, a record ID, and a timestamp.  The acquisition ID is used to determine which records are part of the same acquisition (i.e. are contiguous in time).  The record ID is an integer that counts up sequentially for each record.  The timestamp is a integer number of nanoseconds from the start of the run.  

The data section of the record is the array of digitized samples.   By default, the Monarch interface presents the data array as a byte array (uint8_t), though the actual data can be larger integer types (e.g. uint16_t).