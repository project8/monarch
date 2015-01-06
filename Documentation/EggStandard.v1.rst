Egg v1 File Standard
====================

Usage history: 2011

Relevant software:
- Katydid maintains the ability to read these files (called "2011" egg files)
- Very old versions of Monarch would be able to read and write these files.

File Structure
--------------

1. Prelude
2. Header
3. Records


Prelude
-------
This is 8-byte word describing the size of the size of the header in bytes.

Header
------
The header consists of information describing the contents of the file formatted in XML as follows::

  <header>
    <data_format id="[Frame ID size]" ts="[timestamp size]" data="[record size]" />
    <digitizer rate="[acq'n rate]" />
    <run length="[length]" />
  </header>

The data_format node describes the size of the various parts of the record in bytes.

The digitizer node includes the acquisition rate in MHz.

The run node includes the length of the run in ms.

Record
------

Following the header, records are written to disk sequentially.  

The header for each record includes an ID number and timestamp.  

The data section of the record is the array of digitized 1-byte samples.