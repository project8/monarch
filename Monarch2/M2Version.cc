/*
 * M2Version.cpp
 *
 *  Created on: Mar 21, 2014
 *      Author: nsoblath
 */

#include "M2Version.hh"

#include "logger.hh"

#include <sstream>

namespace monarch2
{
    LOGGER( mlog, "Version" );

    char M2Version::sDelimeter = '.';

    M2Version::M2Version() :
            fMajorVer( 0 ),
            fMinorVer( 0 ),
            fRevision( 0 ),
            fVersion()
    {
    }

    M2Version::M2Version( const std::string& aVer ) :
                    fMajorVer( 0 ),
                    fMinorVer( 0 ),
                    fRevision( 0 ),
                    fVersion()
    {
        Parse( aVer );
    }

    M2Version::~M2Version()
    {
    }

    unsigned M2Version::MajorVersion() const
    {
        return fMajorVer;
    }
    unsigned M2Version::MinorVersion() const
    {
        return fMinorVer;
    }
    unsigned M2Version::Revision() const
    {
        return fRevision;
    }

    const std::string& M2Version::VersionStr() const
    {
        return fVersion;
    }

    bool M2Version::Parse( const std::string& aVer )
    {
        if( aVer == "unknown" )
        {
            fMajorVer = 0;
            fMinorVer = 0;
            fRevision = 0;
            fVersion = aVer;
            return true;
        }

        size_t tDelimPos_1 = aVer.find( sDelimeter, 0 );
        if( tDelimPos_1 == std::string::npos )
        {
            ERROR( mlog, "version string <" << aVer << "> is not in the right format (did not find first delimeter)" );
            return false;
        }
        std::stringstream tMajVerStr;
        tMajVerStr << aVer.substr( 0, tDelimPos_1 );

        size_t tDelimPos_2 = aVer.find( sDelimeter, tDelimPos_1 + 1 );
        if( tDelimPos_2 == std::string::npos )
        {
            ERROR( mlog, "version string <" << aVer << "> is not in the right format (did not find second delimeter)" );
            return false;
        }
        std::stringstream tMinVerStr;
        tMinVerStr << aVer.substr(tDelimPos_1 + 1, tDelimPos_2 );

        std::stringstream tRevStr;
        tRevStr << aVer.substr( tDelimPos_2 + 1 );

        tMajVerStr >> fMajorVer;
        tMinVerStr >> fMinorVer;
        tRevStr >> fRevision;
        fVersion = aVer;

        return true;
    }

    bool M2Version::Combine( unsigned aMajVer, unsigned aMinVer, unsigned aRev )
    {
        std::stringstream tVerStr;
        tVerStr << aMajVer << sDelimeter << aMinVer << sDelimeter << aRev;
        fVersion = tVerStr.str();
        return true;
    }

} /* MT_VERSION_HH_ */


