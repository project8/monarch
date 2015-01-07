#ifndef M2_IO_HH_
#define M2_IO_HH_

#include "M2Types.hh"

#include <string>
using std::string;

#include <iostream>
using std::cout;
using std::endl;

#include <cstdio>

namespace monarch2
{

    class M2IO
    {
            FILE *fFile;
            AccessModeType fMode;
        public:
            // Constructors and Destructors
            M2IO( AccessModeType aMode );
            ~M2IO();

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

    inline bool M2IO::Write( byte_type* anArray, size_t aCount )
    {
        size_t written = fwrite( anArray, sizeof(byte_type), aCount, fFile );
        return (written == sizeof(byte_type) * aCount);
    }
    template< class XType >
    inline bool M2IO::Write( XType* aDatum )
    {
            size_t written = fwrite( aDatum, sizeof(XType), 1, fFile );
            return (written == 1);
    }
    template< class XType >
    inline bool M2IO::Write( XType* aDatum, size_t aCount )
    {
            size_t written = fwrite( aDatum, sizeof(XType), aCount, fFile );
            return (written == aCount);
    }

    inline bool M2IO::Seek( long int aCount )
    {
        size_t success = fseek( fFile, aCount, SEEK_CUR );
        return( success == 0 );
    }

    inline bool M2IO::Read( byte_type* anArray, size_t aCount )
    {
        size_t read = fread( anArray, sizeof(byte_type), aCount, fFile );
        return (read == sizeof(byte_type) * aCount);
    }
    template< class XType >
    inline bool M2IO::Read( XType* aDatum )
    {
            size_t read = fread( aDatum, sizeof(XType), 1, fFile );
            return (read == 1);
    }
    template< class XType >
    inline bool M2IO::Read( XType* aDatum, size_t aCount )
    {
            size_t read = fread( aDatum, sizeof(XType), aCount, fFile );
            return (read == aCount);
    }

}

#endif
