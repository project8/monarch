#ifndef M3RECORD_PYBIND_HH_
#define M3RECORD_PYBIND_HH_

#include "M3Record.hh"

#include "pybind11/pybind11.h"
#include "pybind11/numpy.h"

namespace monarch3_pybind
{

    std::list< std::string > export_record( pybind11::module& mod )
    {
        std::list< std::string > all_items;

        all_items.push_back( "M3Record" );
        pybind11::class_< monarch3::M3Record >( mod, "M3Record",
                "A single data record: ID, timestamp (ns), and raw data bytes" )
            .def_property_readonly( "record_id",
                &monarch3::M3Record::GetRecordId,
                "Record ID (uint64)" )
            .def_property_readonly( "time",
                &monarch3::M3Record::GetTime,
                "Timestamp in nanoseconds since the start of the run (uint64)" )
            .def( "get_data",
                []( const monarch3::M3Record& r, unsigned nbytes ) {
                    // Zero-copy view: the numpy array does not own the data.
                    // The caller must ensure the record (and its parent stream) stays alive.
                    return pybind11::array_t< monarch3::byte_type >(
                        { static_cast< pybind11::ssize_t >( nbytes ) },
                        { sizeof( monarch3::byte_type ) },
                        r.GetData(),
                        pybind11::cast( r )  // keep-alive base object
                    );
                },
                pybind11::arg( "nbytes" ),
                "Return a writable numpy uint8 view of the data buffer.\n"
                "nbytes should equal stream.channel_record_n_bytes (or stream_record_n_bytes).\n"
                "Use numpy.ndarray.view(dtype) to reinterpret as a different element type." )
        ;

        return all_items;
    }

} /* namespace monarch3_pybind */

#endif /* M3RECORD_PYBIND_HH_ */
