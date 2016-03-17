#include "M2IO.hh"

namespace monarch2
{

    M2IO::M2IO( AccessModeType aMode ) :
            fFile( NULL ),
            fMode( aMode )
    {

    }
    M2IO::~M2IO()
    {
        if( fFile )
        {
            fclose( fFile );
        }
    }

    bool M2IO::Open( const std::string& aFilename )
    {
        if( fMode == sAccessRead )
        {
            fFile = fopen( aFilename.c_str(), "rb" );
        }
        else if( fMode == sAccessWrite )
        {
            fFile = fopen( aFilename.c_str(), "wb" );
        }

        if( fFile == NULL )
        {
            return false;
        }
        return true;
    }
    bool M2IO::Done()
    {
        if( fFile != NULL )
        {
            if( feof( fFile ) == 0 )
            {
                return false;
            }
            return true;
        }
        return false;
    }
    bool M2IO::Close()
    {
        if( fFile )
        {
            if( fclose( fFile ) != 0 )
            {
                fFile = NULL;
                return false;
            }
            fFile = NULL;
            return true;
        }
        return false;
    }

}
