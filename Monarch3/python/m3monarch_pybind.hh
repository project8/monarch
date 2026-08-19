#ifndef M3MONARCH_PYBIND_HH_
#define M3MONARCH_PYBIND_HH_

#include "M3Monarch.hh"

#include "monarch3_binding_helpers.hh"

#include "pybind11/pybind11.h"

#include <memory>

namespace monarch3_pybind
{

    // Custom deleter: call FinishReading/FinishWriting before deleting so HDF5 file is
    // closed properly even when Python's garbage collector destroys the object.
    struct Monarch3Deleter
    {
        void operator()( monarch3::Monarch3* m ) const
        {
            if( m == nullptr ) return;
            auto state = m->GetState();
            if( state == monarch3::Monarch3::eReadyToRead || state == monarch3::Monarch3::eOpenToRead )
            {
                try { m->FinishReading(); } catch(...) {}
            }
            else if( state == monarch3::Monarch3::eReadyToWrite || state == monarch3::Monarch3::eOpenToWrite )
            {
                try { m->FinishWriting(); } catch(...) {}
            }
            delete m;
        }
    };

    // Convenience alias
    using Monarch3Ptr = std::unique_ptr< monarch3::Monarch3, Monarch3Deleter >;


    std::list< std::string > export_monarch( pybind11::module& mod )
    {
        std::list< std::string > all_items;

        all_items.push_back( "Monarch3" );
        pybind11::class_< monarch3::Monarch3, Monarch3Ptr >( mod, "Monarch3",
                "Top-level egg v3 file handle.\n\n"
                "Use open_for_reading() or open_for_writing() to obtain an instance.\n"
                "Supports the context manager protocol ('with' statement)." )

            // ---- Factory methods ----
            .def_static( "open_for_reading",
                []( const std::string& filename ) -> Monarch3Ptr {
                    return Monarch3Ptr( const_cast<monarch3::Monarch3*>(monarch3::Monarch3::OpenForReading( filename )) );
                },
                pybind11::arg( "filename" ),
                "Open an existing egg file for reading.\n"
                "Returns a Monarch3 instance in the eOpenToRead state.",
                MONARCH3_BIND_CALL_GUARD_STREAMS )
            .def_static( "open_for_writing",
                []( const std::string& filename ) -> Monarch3Ptr {
                    return Monarch3Ptr( monarch3::Monarch3::OpenForWriting( filename ) );
                },
                pybind11::arg( "filename" ),
                "Create or overwrite an egg file for writing.\n"
                "Returns a Monarch3 instance in the eOpenToWrite state.",
                MONARCH3_BIND_CALL_GUARD_STREAMS )

            // ---- Context manager ----
            .def( "__enter__",
                []( monarch3::Monarch3* self ) { return self; },
                pybind11::return_value_policy::reference,
                "Enter context manager; returns self." )
            .def( "__exit__",
                []( monarch3::Monarch3* self,
                    pybind11::object /*exc_type*/,
                    pybind11::object /*exc_val*/,
                    pybind11::object /*exc_tb*/ )
                {
                    auto state = self->GetState();
                    if( state == monarch3::Monarch3::eReadyToRead || state == monarch3::Monarch3::eOpenToRead )
                    {
                        self->FinishReading();
                    }
                    else if( state == monarch3::Monarch3::eReadyToWrite || state == monarch3::Monarch3::eOpenToWrite )
                    {
                        self->FinishWriting();
                    }
                    return false;  // do not suppress exceptions
                },
                "Exit context manager; calls finish_reading() or finish_writing() as appropriate." )

            // ---- State ----
            .def_property_readonly( "state",
                &monarch3::Monarch3::GetState,
                "Current state of the file handle" )

            // ---- Reading ----
            .def( "read_header",
                &monarch3::Monarch3::ReadHeader,
                "Read header information from the file.\n"
                "Must be called after open_for_reading() before accessing data.",
                MONARCH3_BIND_CALL_GUARD_STREAMS )
            .def( "get_header",
                ( const monarch3::M3Header* ( monarch3::Monarch3::* )() const )
                &monarch3::Monarch3::GetHeader,
                pybind11::return_value_policy::reference_internal,
                "Return the file header (read access)" )
            .def( "get_stream",
                ( const monarch3::M3Stream* ( monarch3::Monarch3::* )( unsigned ) const )
                &monarch3::Monarch3::GetStream,
                pybind11::arg( "stream" ),
                pybind11::return_value_policy::reference_internal,
                "Return the stream object for the given stream index (read access)" )
            .def( "finish_reading",
                &monarch3::Monarch3::FinishReading,
                "Close the file after reading.",
                MONARCH3_BIND_CALL_GUARD_STREAMS )

            // ---- Writing ----
            .def( "write_header",
                &monarch3::Monarch3::WriteHeader,
                "Write the header to file and prepare streams for writing.\n"
                "Must be called after configuring the header and adding streams.",
                MONARCH3_BIND_CALL_GUARD_STREAMS )
            .def( "get_header",
                ( monarch3::M3Header* ( monarch3::Monarch3::* )() )
                &monarch3::Monarch3::GetHeader,
                pybind11::return_value_policy::reference_internal,
                "Return the file header (write access)" )
            .def( "get_stream",
                ( monarch3::M3Stream* ( monarch3::Monarch3::* )( unsigned ) )
                &monarch3::Monarch3::GetStream,
                pybind11::arg( "stream" ),
                pybind11::return_value_policy::reference_internal,
                "Return the stream object for the given stream index (write access)" )
            .def( "finish_writing",
                &monarch3::Monarch3::FinishWriting,
                "Flush and close the file after writing.",
                MONARCH3_BIND_CALL_GUARD_STREAMS )
        ;

        return all_items;
    }

} /* namespace monarch3_pybind */

#endif /* M3MONARCH_PYBIND_HH_ */
