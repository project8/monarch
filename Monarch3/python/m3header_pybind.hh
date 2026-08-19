#ifndef M3HEADER_PYBIND_HH_
#define M3HEADER_PYBIND_HH_

#include "M3Header.hh"

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include <sstream>

namespace monarch3_pybind
{

    std::list< std::string > export_header( pybind11::module& mod )
    {
        std::list< std::string > all_items;

        // M3StreamHeader
        all_items.push_back( "M3StreamHeader" );
        pybind11::class_< monarch3::M3StreamHeader >( mod, "M3StreamHeader",
                "Header information for a single data stream" )
            .def_property_readonly( "number",          &monarch3::M3StreamHeader::GetNumber )
            .def_property_readonly( "source",          []( const monarch3::M3StreamHeader& h ) { return h.Source(); } )
            .def_property_readonly( "n_channels",      &monarch3::M3StreamHeader::GetNChannels )
            .def_property_readonly( "channels",        []( const monarch3::M3StreamHeader& h ) { return h.Channels(); } )
            .def_property_readonly( "channel_format",  &monarch3::M3StreamHeader::GetChannelFormat )
            .def_property_readonly( "acquisition_rate",&monarch3::M3StreamHeader::GetAcquisitionRate )
            .def_property_readonly( "record_size",     &monarch3::M3StreamHeader::GetRecordSize )
            .def_property_readonly( "sample_size",     &monarch3::M3StreamHeader::GetSampleSize )
            .def_property_readonly( "data_type_size",  &monarch3::M3StreamHeader::GetDataTypeSize )
            .def_property_readonly( "data_format",     &monarch3::M3StreamHeader::GetDataFormat )
            .def_property_readonly( "bit_depth",       &monarch3::M3StreamHeader::GetBitDepth )
            .def_property_readonly( "bit_alignment",   &monarch3::M3StreamHeader::GetBitAlignment )
            .def_property_readonly( "n_acquisitions",  &monarch3::M3StreamHeader::GetNAcquisitions )
            .def_property_readonly( "n_records",       &monarch3::M3StreamHeader::GetNRecords )
            .def( "__repr__", []( const monarch3::M3StreamHeader& h ) {
                std::ostringstream out;
                out << h;
                return out.str();
            } )
        ;

        // M3ChannelHeader
        all_items.push_back( "M3ChannelHeader" );
        pybind11::class_< monarch3::M3ChannelHeader >( mod, "M3ChannelHeader",
                "Header information for a single data channel" )
            .def_property_readonly( "number",          &monarch3::M3ChannelHeader::GetNumber )
            .def_property_readonly( "source",          []( const monarch3::M3ChannelHeader& h ) { return h.Source(); } )
            .def_property_readonly( "acquisition_rate",&monarch3::M3ChannelHeader::GetAcquisitionRate )
            .def_property_readonly( "record_size",     &monarch3::M3ChannelHeader::GetRecordSize )
            .def_property_readonly( "sample_size",     &monarch3::M3ChannelHeader::GetSampleSize )
            .def_property_readonly( "data_type_size",  &monarch3::M3ChannelHeader::GetDataTypeSize )
            .def_property_readonly( "data_format",     &monarch3::M3ChannelHeader::GetDataFormat )
            .def_property_readonly( "bit_depth",       &monarch3::M3ChannelHeader::GetBitDepth )
            .def_property_readonly( "bit_alignment",   &monarch3::M3ChannelHeader::GetBitAlignment )
            .def_property_readonly( "voltage_offset",  &monarch3::M3ChannelHeader::GetVoltageOffset )
            .def_property_readonly( "voltage_range",   &monarch3::M3ChannelHeader::GetVoltageRange )
            .def_property_readonly( "dac_gain",        &monarch3::M3ChannelHeader::GetDACGain )
            .def_property_readonly( "frequency_min",   &monarch3::M3ChannelHeader::GetFrequencyMin )
            .def_property_readonly( "frequency_range", &monarch3::M3ChannelHeader::GetFrequencyRange )
            .def( "__repr__", []( const monarch3::M3ChannelHeader& h ) {
                std::ostringstream out;
                out << h;
                return out.str();
            } )
        ;

        // M3Header
        all_items.push_back( "M3Header" );
        pybind11::class_< monarch3::M3Header >( mod, "M3Header",
                "Egg file header: run metadata and stream/channel configuration" )
            // Read-only properties
            .def_property_readonly( "egg_version",   []( const monarch3::M3Header& h ) { return h.EggVersion(); } )
            .def_property_readonly( "n_channels",    &monarch3::M3Header::GetNChannels )
            .def_property_readonly( "n_streams",     &monarch3::M3Header::GetNStreams )
            .def_property_readonly( "stream_headers",
                []( const monarch3::M3Header& h ) { return h.StreamHeaders(); } )
            .def_property_readonly( "channel_headers",
                []( const monarch3::M3Header& h ) { return h.ChannelHeaders(); } )
            // Read-write properties (ref-accessor pattern)
            .def_property( "filename",
                []( const monarch3::M3Header& h ) { return h.Filename(); },
                []( monarch3::M3Header& h, const std::string& v ) { h.Filename() = v; } )
            .def_property( "run_duration",
                &monarch3::M3Header::GetRunDuration,
                &monarch3::M3Header::SetRunDuration )
            .def_property( "timestamp",
                []( const monarch3::M3Header& h ) { return h.Timestamp(); },
                []( monarch3::M3Header& h, const std::string& v ) { h.Timestamp() = v; } )
            .def_property( "description",
                []( const monarch3::M3Header& h ) { return h.Description(); },
                []( monarch3::M3Header& h, const std::string& v ) { h.Description() = v; } )
            // AddStream overloads — lambda wrappers are needed because pybind11 cannot
            // implicitly convert Python None to a raw std::vector<unsigned>* pointer.
            .def( "add_stream",
                []( monarch3::M3Header& h,
                    const std::string& source,
                    uint32_t acq_rate, uint32_t rec_size, uint32_t sample_size,
                    uint32_t data_type_size, uint32_t data_format,
                    uint32_t bit_depth, uint32_t bit_alignment,
                    pybind11::object chan_vec_py ) -> unsigned
                {
                    std::vector< unsigned > buf;
                    std::vector< unsigned >* ptr = nullptr;
                    if( ! chan_vec_py.is_none() )
                    {
                        buf = chan_vec_py.cast< std::vector< unsigned > >();
                        ptr = &buf;
                    }
                    return h.AddStream( source, acq_rate, rec_size, sample_size,
                                        data_type_size, data_format, bit_depth, bit_alignment, ptr );
                },
                pybind11::arg( "source" ),
                pybind11::arg( "acq_rate" ),
                pybind11::arg( "rec_size" ),
                pybind11::arg( "sample_size" ),
                pybind11::arg( "data_type_size" ),
                pybind11::arg( "data_format" ),
                pybind11::arg( "bit_depth" ),
                pybind11::arg( "bit_alignment" ),
                pybind11::arg( "chan_vec" ) = pybind11::none(),
                "Add a single-channel stream; returns the stream number" )
            .def( "add_stream",
                []( monarch3::M3Header& h,
                    const std::string& source,
                    uint32_t n_channels, uint32_t channel_format,
                    uint32_t acq_rate, uint32_t rec_size, uint32_t sample_size,
                    uint32_t data_type_size, uint32_t data_format,
                    uint32_t bit_depth, uint32_t bit_alignment,
                    pybind11::object chan_vec_py ) -> unsigned
                {
                    std::vector< unsigned > buf;
                    std::vector< unsigned >* ptr = nullptr;
                    if( ! chan_vec_py.is_none() )
                    {
                        buf = chan_vec_py.cast< std::vector< unsigned > >();
                        ptr = &buf;
                    }
                    return h.AddStream( source, n_channels, channel_format, acq_rate, rec_size,
                                        sample_size, data_type_size, data_format, bit_depth, bit_alignment, ptr );
                },
                pybind11::arg( "source" ),
                pybind11::arg( "n_channels" ),
                pybind11::arg( "channel_format" ),
                pybind11::arg( "acq_rate" ),
                pybind11::arg( "rec_size" ),
                pybind11::arg( "sample_size" ),
                pybind11::arg( "data_type_size" ),
                pybind11::arg( "data_format" ),
                pybind11::arg( "bit_depth" ),
                pybind11::arg( "bit_alignment" ),
                pybind11::arg( "chan_vec" ) = pybind11::none(),
                "Add a multi-channel stream; returns the stream number" )
            .def( "__repr__", []( const monarch3::M3Header& h ) {
                std::ostringstream out;
                out << h;
                return out.str();
            } )
        ;

        return all_items;
    }

} /* namespace monarch3_pybind */

#endif /* M3HEADER_PYBIND_HH_ */
