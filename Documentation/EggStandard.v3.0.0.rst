Egg v3.0.0.b4 File Standard
=========================

Usage history: 1/2015 - 

Relevant software:
- Monarch v3

Files are written in HDF5 format.  
All group, dataset, and attribute names are lower case, with words separated by underscores.
Numerical values in the file are stored in little-endian format.

File Structure
--------------

Version 3 egg files are designed to accept data from multiple sources that may or may not 
be taking data in a synchronized way.  It is also can also record both integer and floating-point data.

Data from each source are divided into *acquisitions* and *records*.  
A *record* consists of a set number of data samples.  
An *acquisition* is the set of records that are contiguous in time.

A single source of data (e.g. a digitizer channel) is called a *channel* in the egg file.  
However, some devices produce data from multiple channels combined together; 
therefore data are actually recorded in the egg file in *streams*, 
where each stream contains one or more channel of data.

In a stream with multiple channels, the individual samples from each channel can either be *separate* or *interleaved*.  
For example, one record of data from a device with two channels, A and B, 
recorded separately in a single stream would look like this in the stream::

    AAAAABBBBB

One record of data from a device with two channels, C and D, 
recorded interleaved in a single stream would look like this::

    CDCDCDCDCD

In these cases, the record length for each channel is 5, and the record length for each stream is 10.

Header information describing the run is stored in the file attributes.  
Channel and stream header information is stored in the attributes for each channel and stream group.

::

 File (w/ attr)
 |
 +-- channels (group)
 |   |
 |   +-- channel0 (group w/ attr)
 |   |
 |   +-- channel1 (group w/ attr)
 |   |
 |   +-- . . .
 |
 +-- streams (group)
     |
     +-- stream0 (group w/ attr)
     |   |
     |   +-- acquisitions (group)
     |       |
     |       +-- 0 (dataset w/ attr)
     |       |
     |       +-- 1 (dataset w/ attr)
     |       |
     |       +-- . . .
     |
     +-- stream1 (group w/ attr)
         |
         +-- acquisitions (group)
             |
             +-- 0 (dataset w/ attr)
             |
             +-- 1 (dataset w/ attr)
             |
             +-- . . .



File Attributes
---------------------

These attributes constitute the header for the file, and describe its contents. 
They consist of information that apply to the all streams and channels.

- egg_version (string) -- Egg file version
- filename (string) -- Filename
- run_duration (uint32) -- Length of the run in ms
- timestamp (string) -- Timestamp string
- description (string) -- Description of the run
- n_channels (uint32) -- Number of channels present
- n_streams (uint32) -- Number of streams used
- channel_streams (vector< uint32 >) -- Map of channel number to stream number
- channel_coherence (vector< vector< bool > >) -- Matrix of channel coherence (stored as uint8_t)


Stream Attributes
-------------------------

Each stream group has a set of attributes needed to fully describe the stream.  
Information there is common to all of the channels included in that stream.

- number (uint32) -- Sequential integer used to uniquely identify each stream in the file; the stream's group will be named "stream[Number]".
- source (string) -- Device used to produce the stream
- n_channels (uint32) -- Number of channels in the stream
- channel_format (uint32) -- Whether the samples from multiple channels are interleaved or separate (either is valid for single-channel streams)

  * 0 == interleaved
  * 1 == separate
  
- acquisition_rate (uint32) -- Acquisition rate in MHz
- record_size (uint32) -- Number of samples in each channel record (stream record size = # channels * record size)
- data_type_size (uint32) -- Number of bytes used to represent each sample (whether integer or floating point)
- data_format_type (uint32) -- Whether the data is analog or digitized

  * 0 == digitized
  * 1 == analog
  
- bit_depth (uint32) -- Number of bits with which the data was digitized
- n_acquisitions (uint32) -- Number of acquisitions recorded
- n_records (uint32) -- Number of records recorded



Channel Attributes
---------------------------

While much of the information regarding multiple channels in a single stream 
will be the same for all of those channels, some can certainly be different.
Therefore each channel is given its own header information.

Some information is duplicated between channel and stream headers.  
This is for convenience, since when writing files the user cares 
about how the data is divided into streams, and when reading files most 
users will care about how the data is divided into channels.

- number (uint32) -- Sequential integer used to uniquely identify each channel in the file; the channel's group will be named "channel[Number]".
- source (string) -- Device used to produce the channel
- acquisition_rate (uint32) -- Acquisition rate in MHz
- record_size (uint32) -- Number of samples in each channel record (stream record size = # channels * record size)
- data_type_size (uint32) -- Number of bytes used to represent each sample (whether integer or floating point)
- data_format_type (uint32) -- Whether the data is analog or digitized

  * 0 == digitized
  * 1 == analog
  
- bit_depth (uint32) -- Number of bits with which the data was digitized
- voltage_offset (double) -- Voltage value represented by an ADC value of 0 (the minimum voltage for unsigned digital data, and the center voltage for signed digital data)
- voltage_range (double) -- Voltage range accepted above the minimum
- dac_gain (double) -- Gain value needed to convert digital values to analog (analog = digital * gain + voltage_min)
- frequency_min (double) -- For band-pass data, minimum frequency recorded
- frequency_range (double) -- For band-pass data, range of frequencies recorded above the minimum


Acquisitions
-------------------

Each stream contains an "acquisitions" group that holds the acquisition datasets.
Each acquisition dataset is named simply with the number of the acquisition, 
starting from 0 and incrementing for each new acquisition.

Each acquisition has a single attribute:

- n_records (uint32) -- Number of records in that acquisition

Here are some examples how data might be arranged in an acquisition dataset:

- Single channel; channel record size == 10; 3 records::

    AAAAAAAAAA  <-- record 0
    AAAAAAAAAA  <-- record 1
    AAAAAAAAAA  <-- record 2
  
- Two channels; channel record size == 5; separate samples; 2 records::

    AAAAABBBBB
    AAAAABBBBB
  
- Three channels; channel record size == 5; interleaved samples; 2 records::

    ABCABCABCABCABC
    ABCABCABCABCABC
