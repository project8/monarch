#ifndef __MONARCH_IO_HPP
#define __MONARCH_IO_HPP

#include "MonarchTypes.hpp"
#include "MonarchRecord.hpp"
#include "MonarchSerializer.hpp"
#include "MonarchExceptions.hpp"

#include <string>
using std::string;

#include <cstdio>

class MonarchIO
{
        FILE *fFile;
        AccessModeType fMode;
    public:
        // Constructors and Destructors
        MonarchIO( AccessModeType aMode );
        ~MonarchIO();

        // Open the file in whatever mode was given the constructor
        bool Open( const string& aFilename );

        // Write nbytes of data from the byte array wbuf to the
        // current position of the file pointer.
        bool Write( char* anArray, size_t aCount );
        template< class XType >
        bool Write( XType* aDatum );
        template< class XType >
        bool Write( XType* aDatum, size_t aCount );

        // Read nbytes of data from the file pointer and store
        // the result in the byte array rbuf.
        bool Read( char* anArray, size_t aCount );
        template< class XType >
        bool Read( XType* aDatum );
        template< class XType >
        bool Read( XType* aDatum, size_t aCount );

        // Close the file handle owned by this IO object
        bool Close();
};

inline bool MonarchIO::Write( char* anArray, size_t aCount )
{
    size_t written = fwrite( anArray, sizeof(char), aCount, fFile );
    return (written == sizeof(char) * aCount);
}
template< class XType >
inline bool MonarchIO::Write( XType* aDatum )
{
    size_t written = fwrite( aDatum, sizeof(XType), 1, fFile );
    return (written == sizeof(XType));
}
template< class XType >
inline bool MonarchIO::Write( XType* aDatum, size_t aCount )
{
    size_t written = fwrite( aDatum, sizeof(XType), aCount, fFile );
    return (written == sizeof(XType) * aCount);
}

inline bool MonarchIO::Read( char* anArray, size_t aCount )
{
    size_t read = fread( anArray, sizeof(char), aCount, fFile );
    return (read == sizeof(char) * aCount);
}
template< class XType >
inline bool MonarchIO::Read( XType* aDatum )
{
    size_t read = fread( aDatum, sizeof(XType), 1, fFile );
    return (read == sizeof(XType));
}
template< class XType >
inline bool MonarchIO::Read( XType* aDatum, size_t aCount )
{
    size_t read = fread( aDatum, sizeof(XType), aCount, fFile );
    return (read == sizeof(XType) * aCount);
}

#endif
