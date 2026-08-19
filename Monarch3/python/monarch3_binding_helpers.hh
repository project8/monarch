#ifndef MONARCH3_PYBIND_BINDING_HELPERS_HH_
#define MONARCH3_PYBIND_BINDING_HELPERS_HH_

#include "pybind11/iostream.h"

#define MONARCH3_BIND_CALL_GUARD_STREAMS \
    pybind11::call_guard< pybind11::scoped_ostream_redirect, pybind11::scoped_estream_redirect >()

#define MONARCH3_BIND_CALL_GUARD_GIL \
    pybind11::call_guard< pybind11::gil_scoped_release >()

#define MONARCH3_BIND_CALL_GUARD_STREAMS_AND_GIL \
    pybind11::call_guard< pybind11::scoped_ostream_redirect, pybind11::scoped_estream_redirect, pybind11::gil_scoped_release >()

#endif /* MONARCH3_PYBIND_BINDING_HELPERS_HH_ */
