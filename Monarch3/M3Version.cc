/*
 * M3Version.cpp
 *
 *  Created on: Mar 21, 2014
 *      Author: nsoblath
 */

#define M3_API_EXPORTS

#include "M3Version.hh"

#include "logger.hh"

#include <sstream>

namespace monarch3
{
    LOGGER( mlog, "Version" );

    M3_API char M3Version::sDelimeter = '.';

    M3Version::M3Version() :
            fMajorVer( Monarch_VERSION_MAJOR ),
            fMinorVer( Monarch_VERSION_MINOR ),
            fPatchVer( Monarch_VERSION_PATCH ),
            fVersion(),
            fEggVersion( TOSTRING(Egg_VERSION) )
    {
        Combine( fMajorVer, fMinorVer, fPatchVer );
    }

    M3Version::M3Version( const std::string& aVer ) :
                    fMajorVer( 0 ),
                    fMinorVer( 0 ),
                    fPatchVer( 0 ),
                    fVersion(),
                    fEggVersion( TOSTRING(Egg_VERSION) )
    {
        Parse( aVer );
    }

    M3Version::~M3Version()
    {
    }

    unsigned M3Version::MajorVersion() const
    {
        return fMajorVer;
    }
    unsigned M3Version::MinorVersion() const
    {
        return fMinorVer;
    }
    unsigned M3Version::PatchVersion() const
    {
        return fPatchVer;
    }

    const std::string& M3Version::VersionStr() const
    {
        return fVersion;
    }

    const std::string& M3Version::EggVersion() const
    {
        return fEggVersion;
    }

    bool M3Version::Parse( const std::string& aVer )
    {
        if( aVer == "unknown" )
        {
            fMajorVer = 0;
            fMinorVer = 0;
            fPatchVer = 0;
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

        std::stringstream tPatchVerStr;
        tPatchVerStr << aVer.substr( tDelimPos_2 + 1 );

        tMajVerStr >> fMajorVer;
        tMinVerStr >> fMinorVer;
        tPatchVerStr >> fPatchVer;
        fVersion = aVer;

        return true;
    }

    bool M3Version::Combine( unsigned aMajVer, unsigned aMinVer, unsigned aPatchVer )
    {
        std::stringstream tVerStr;
        tVerStr << aMajVer << sDelimeter << aMinVer << sDelimeter << aPatchVer;
        fVersion = tVerStr.str();
        return true;
    }

} /* MT_VERSION_HH_ */


