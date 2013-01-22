#include "MonarchIO.hpp"

MonarchIO::MonarchIO( AccessModeType aMode ) :
    fFile( NULL ),
    fMode( aMode )
{

}
MonarchIO::~MonarchIO()
{
    if( fFile )
    {
        fclose( fFile );
    }
}

bool MonarchIO::Open( const string& aFilename )
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
bool MonarchIO::Done()
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
bool MonarchIO::Close()
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
