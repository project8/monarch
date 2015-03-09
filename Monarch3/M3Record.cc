/*
 * M3Record.cc
 *
 *  Created on: Dec 26, 2014
 *      Author: nsoblath
 */
#define M3_API_EXPORTS

#include "M3Record.hh"

namespace monarch3
{
    M3Record::M3Record( unsigned aNBytes ) :
            fRecordId( 0 ),
            fTime( 0 ),
            fOwnsData( true )
    {
        if( aNBytes == 0 )
        {
            fData = NULL;
            fOwnsData = false;
        }
        else
        {
            fData = new byte_type[ aNBytes ];
        }
    }

    M3Record::M3Record( byte_type* aDataPtr ) :
            fRecordId( 0 ),
            fTime( 0 ),
            fOwnsData( false ),
            fData( aDataPtr )
    {
    }

    M3Record::~M3Record()
    {
        ClearData();
    }

    void M3Record::SetData()
    {
        ClearData();
        fOwnsData = true;
        return;
    }

    void M3Record::SetData( unsigned aNBytes )
    {
        ClearData();
        fOwnsData = true;
        fData = new byte_type[ aNBytes ];
        return;
    }

    void M3Record::SetData( byte_type* aDataPtr )
    {
        ClearData();
        fOwnsData = false;
        fData = aDataPtr;
        return;
    }

    void M3Record::ClearData()
    {
        if( fOwnsData && fData != NULL ) delete [] fData;
        fData = NULL;
        return;
    }

}
