/*
 * M2Version.hpp
 *
 *  Generated by cmake from M2Version.hpp.in
 *
 *  Created on: Mar 20, 2013
 *      Author: nsoblath
 */

#ifndef M2_VERSION_HH_
#define M2_VERSION_HH_

#include "scarab_version.hh"

namespace monarch2
{
    class M2Version : public scarab::version_semantic
    {
        public:
            M2Version();
            ~M2Version();
    };
}

#endif /* MONARCH_VERSION_HPP_ */
