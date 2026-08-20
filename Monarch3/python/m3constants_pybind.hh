#ifndef M3CONSTANTS_PYBIND_HH_
#define M3CONSTANTS_PYBIND_HH_

#include "M3Constants.hh"

#include "pybind11/pybind11.h"

namespace monarch3_pybind
{

    std::list< std::string > export_constants( pybind11::module& mod )
    {
        std::list< std::string > all_items;

        // Data format constants
        all_items.push_back( "sDigitizedUS" );
        mod.attr( "sDigitizedUS" ) = monarch3::sDigitizedUS;

        all_items.push_back( "sDigitizedS" );
        mod.attr( "sDigitizedS" ) = monarch3::sDigitizedS;

        all_items.push_back( "sAnalog" );
        mod.attr( "sAnalog" ) = monarch3::sAnalog;

        // Bit alignment constants
        all_items.push_back( "sBitsAlignedLeft" );
        mod.attr( "sBitsAlignedLeft" ) = monarch3::sBitsAlignedLeft;

        all_items.push_back( "sBitsAlignedRight" );
        mod.attr( "sBitsAlignedRight" ) = monarch3::sBitsAlignedRight;

        // Channel format constants
        all_items.push_back( "sInterleaved" );
        mod.attr( "sInterleaved" ) = monarch3::sInterleaved;

        all_items.push_back( "sSeparate" );
        mod.attr( "sSeparate" ) = monarch3::sSeparate;

        return all_items;
    }

} /* namespace monarch3_pybind */

#endif /* M3CONSTANTS_PYBIND_HH_ */
