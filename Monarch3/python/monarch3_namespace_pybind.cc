/*
 * monarch3_namespace_pybind.cc
 *
 * Pybind11 module entry point for the monarch3 Python binding.
 */

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/numpy.h"

#include "M3Exception.hh"

#include "m3constants_pybind.hh"
#include "m3header_pybind.hh"
#include "m3record_pybind.hh"
#include "m3stream_pybind.hh"
#include "m3monarch_pybind.hh"

PYBIND11_MODULE( monarch3, mod )
{
    mod.doc() = "Python bindings for the Monarch3 egg v3 file library";

    std::list< std::string > all_members;

    // Exception
    pybind11::register_exception< monarch3::M3Exception >( mod, "Monarch3Exception" );
    all_members.push_back( "Monarch3Exception" );

    // Constants
    all_members.splice( all_members.end(), monarch3_pybind::export_constants( mod ) );

    // Header classes
    all_members.splice( all_members.end(), monarch3_pybind::export_header( mod ) );

    // Record class
    all_members.splice( all_members.end(), monarch3_pybind::export_record( mod ) );

    // Stream class
    all_members.splice( all_members.end(), monarch3_pybind::export_stream( mod ) );

    // Top-level Monarch3 class
    all_members.splice( all_members.end(), monarch3_pybind::export_monarch( mod ) );

    mod.attr( "__all__" ) = all_members;
}
