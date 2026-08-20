#ifndef M3STREAM_PYBIND_HH_
#define M3STREAM_PYBIND_HH_

#include "M3Stream.hh"

#include "monarch3_binding_helpers.hh"

#include "pybind11/pybind11.h"
#include "pybind11/numpy.h"

namespace monarch3_pybind
{

    std::list< std::string > export_stream( pybind11::module& mod )
    {
        std::list< std::string > all_items;

        all_items.push_back( "M3Stream" );
        pybind11::class_< monarch3::M3Stream >( mod, "M3Stream",
                "Read/write access for a single data stream" )

            // ---- State query properties ----
            .def_property_readonly( "n_channels",
                &monarch3::M3Stream::GetNChannels,
                "Number of channels in this stream" )
            .def_property_readonly( "n_acquisitions",
                &monarch3::M3Stream::GetNAcquisitions,
                "Number of acquisitions in this stream (valid after ReadRecord or after writing)" )
            .def_property_readonly( "acquisition_id",
                &monarch3::M3Stream::GetAcquisitionId,
                "ID of the most recently accessed acquisition" )
            .def_property_readonly( "record_count_in_acq",
                &monarch3::M3Stream::GetRecordCountInAcq,
                "Number of records read/written in the current acquisition" )
            .def_property_readonly( "n_records_in_file",
                &monarch3::M3Stream::GetNRecordsInFile,
                "Total number of records across all acquisitions in the file" )
            .def_property_readonly( "n_records_in_acquisition",
                &monarch3::M3Stream::GetNRecordsInAcquisition,
                "Number of records in the current acquisition" )
            .def_property_readonly( "data_type_size",
                &monarch3::M3Stream::GetDataTypeSize,
                "Size in bytes of each sample element" )
            .def_property_readonly( "sample_size",
                &monarch3::M3Stream::GetSampleSize,
                "Number of elements per sample (1 for real, 2 for complex)" )
            .def_property_readonly( "channel_record_size",
                &monarch3::M3Stream::GetChannelRecordSize,
                "Number of samples in a channel record" )
            .def_property_readonly( "channel_record_n_bytes",
                &monarch3::M3Stream::GetChannelRecordNBytes,
                "Size in bytes of a channel record data buffer" )
            .def_property_readonly( "stream_record_size",
                &monarch3::M3Stream::GetStreamRecordSize,
                "Number of samples in the full (interleaved) stream record" )
            .def_property_readonly( "stream_record_n_bytes",
                &monarch3::M3Stream::GetStreamRecordNBytes,
                "Size in bytes of the full stream record data buffer" )
            .def_property_readonly( "is_interleaved",
                &monarch3::M3Stream::GetIsInterleaved,
                "True if multi-channel data is stored interleaved in the file" )

            // ---- Record access: reading ----
            .def( "read_record",
                &monarch3::M3Stream::ReadRecord,
                pybind11::arg( "offset" ) = 0,
                pybind11::arg( "if_new_acq_start_at_first_rec" ) = true,
                "Read a record from the file.\n\n"
                "If the last record read was [J], reads record [J+1+offset].\n"
                "offset=0 (default): next record; offset=-1: reread current;\n"
                "offset<-1: step backward; offset>0: skip forward.\n"
                "Returns True on success, False when past end/start of file.\n"
                "Raises Monarch3Exception on error.",
                MONARCH3_BIND_CALL_GUARD_STREAMS_AND_GIL )
            .def( "get_stream_record",
                ( monarch3::M3Record* ( monarch3::M3Stream::* )() )
                &monarch3::M3Stream::GetStreamRecord,
                pybind11::return_value_policy::reference_internal,
                "Return the stream-level record object (all channels interleaved)" )
            .def( "get_channel_record",
                ( monarch3::M3Record* ( monarch3::M3Stream::* )( unsigned ) )
                &monarch3::M3Stream::GetChannelRecord,
                pybind11::arg( "channel" ),
                pybind11::return_value_policy::reference_internal,
                "Return the record object for the given channel index" )

            // ---- Convenience: typed numpy data access ----
            .def( "get_stream_data",
                []( monarch3::M3Stream& s ) {
                    monarch3::M3Record* r = s.GetStreamRecord();
                    unsigned nbytes = s.GetStreamRecordNBytes();
                    return pybind11::array_t< monarch3::byte_type >(
                        { static_cast< pybind11::ssize_t >( nbytes ) },
                        { sizeof( monarch3::byte_type ) },
                        r->GetData(),
                        pybind11::cast( s )
                    );
                },
                pybind11::return_value_policy::reference_internal,
                "Return a writable numpy uint8 view of the full (interleaved) stream record.\n"
                "Call after read_record(). Use .view(dtype) to reinterpret elements." )
            .def( "get_channel_data",
                []( monarch3::M3Stream& s, unsigned channel ) {
                    monarch3::M3Record* r = s.GetChannelRecord( channel );
                    if( r == nullptr )
                    {
                        throw monarch3::M3Exception() << "Channel " << channel << " does not exist in this stream";
                    }
                    unsigned nbytes = s.GetChannelRecordNBytes();
                    return pybind11::array_t< monarch3::byte_type >(
                        { static_cast< pybind11::ssize_t >( nbytes ) },
                        { sizeof( monarch3::byte_type ) },
                        r->GetData(),
                        pybind11::cast( s )
                    );
                },
                pybind11::arg( "channel" ),
                pybind11::return_value_policy::reference_internal,
                "Return a writable numpy uint8 view of the data buffer for the given channel.\n"
                "Call after read_record(). Use .view(dtype) to reinterpret elements." )

            // ---- Record access: writing ----
            .def( "write_record",
                &monarch3::M3Stream::WriteRecord,
                pybind11::arg( "is_new_acquisition" ),
                "Write the current record contents to the file.\n\n"
                "is_new_acquisition=True starts a new acquisition group;\n"
                "is_new_acquisition=False continues the current acquisition.\n"
                "Returns True on success. Raises Monarch3Exception on error.",
                MONARCH3_BIND_CALL_GUARD_STREAMS_AND_GIL )
        ;

        return all_items;
    }

} /* namespace monarch3_pybind */

#endif /* M3STREAM_PYBIND_HH_ */
