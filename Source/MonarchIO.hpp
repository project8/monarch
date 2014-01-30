#ifndef __MONARCH_IO_HPP
#define __MONARCH_IO_HPP

#include "MonarchTypes.hpp"

#include <string>
using std::string;

#include <iostream>
using std::cout;
using std::endl;

#include <cstdio>

namespace monarch
{

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
            bool Write( byte_type* anArray, size_t aCount );
            template< class XType >
            bool Write( XType* aDatum );
            template< class XType >
            bool Write( XType* aDatum, size_t aCount );

            // Seek by offset aCount bytes
            bool Seek( long int aCount );

            // Read aCout bytes of data from the file pointer and store
            // the result in the byte array anArray.
            bool Read( byte_type* anArray, size_t aCount );
            template< class XType >
            bool Read( XType* aDatum );
            template< class XType >
            bool Read( XType* aDatum, size_t aCount );

            // File is at end
            bool Done();

            // Close the file handle owned by this IO object
            bool Close();
    };

    inline bool MonarchIO::Write( byte_type* anArray, size_t aCount )
    {
        size_t written = fwrite( anArray, sizeof(byte_type), aCount, fFile );
        return (written == sizeof(byte_type) * aCount);
    }
    template< class XType >
    inline bool MonarchIO::Write( XType* aDatum )
    {
            size_t written = fwrite( aDatum, sizeof(XType), 1, fFile );
            return (written == 1);
    }
    template< class XType >
    inline bool MonarchIO::Write( XType* aDatum, size_t aCount )
    {
            size_t written = fwrite( aDatum, sizeof(XType), aCount, fFile );
            return (written == aCount);
    }

    inline bool MonarchIO::Seek( long int aCount )
    {
        size_t success = fseek( fFile, aCount, SEEK_CUR );
        return( success == 0 );
    }

    inline bool MonarchIO::Read( byte_type* anArray, size_t aCount )
    {
        size_t read = fread( anArray, sizeof(byte_type), aCount, fFile );
        return (read == sizeof(byte_type) * aCount);
    }
    template< class XType >
    inline bool MonarchIO::Read( XType* aDatum )
    {
            size_t read = fread( aDatum, sizeof(XType), 1, fFile );
            return (read == 1);
    }
    template< class XType >
    inline bool MonarchIO::Read( XType* aDatum, size_t aCount )
    {
            size_t read = fread( aDatum, sizeof(XType), aCount, fFile );
            return (read == aCount);
    }

}

#endif
