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
    if( fMode == sWriteMode )
    {
        fFile = fopen( aFilename.c_str(), "wb" );
    }
    else if( fMode == sReadMode )
    {
        fFile = fopen( aFilename.c_str(), "rb" );
    }

    if( fFile == NULL )
    {
        return false;
    }
    return true;
}
bool MonarchIO::Close()
{
    if( this->fFile )
    {
        fclose( this->fFile );
        fFile = NULL;
        return true;
    }
    return false;
}
