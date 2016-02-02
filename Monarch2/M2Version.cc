/*
 * M2Version.cpp
 *
 *  Created on: Mar 21, 2014
 *      Author: nsoblath
 */

#include "M2Version.hh"

#include "logger.hh"

namespace monarch2
{
    M2Version::M2Version() :
            scarab::version_semantic()
    {
        f_major_ver = @Monarch_VERSION_MAJOR@;
        f_minor_ver = @Monarch_VERSION_MINOR@;
        f_patch_ver = @Monarch_VERSION_PATCH@;
        f_version = TOSTRING(@Monarch_VERSION@);
        f_package = TOSTRING(@Monarch_PACKAGE_NAME@);
        f_commit = TOSTRING(@Monarch_GIT_COMMIT@);
    }

    M2Version::~M2Version()
    {
    }

} /* namespace monarch2 */


