/*
 * MRecord.cc
 *
 *  Created on: Dec 26, 2014
 *      Author: nsoblath
 */

#include "MRecord.hh"

#include <iostream>

namespace monarch
{
    MRecord::MRecord( unsigned aNBytes ) :
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

    MRecord::MRecord( byte_type* aDataPtr ) :
            fRecordId( 0 ),
            fTime( 0 ),
            fOwnsData( false ),
            fData( aDataPtr )
    {
    }

    MRecord::~MRecord()
    {
        std::cout << "deleting record at " << this << std::endl;
        ClearData();
    }

    void MRecord::SetData()
    {
        ClearData();
        fOwnsData = true;
        return;
    }

    void MRecord::SetData( unsigned aNBytes )
    {
        ClearData();
        fOwnsData = true;
        fData = new byte_type[ aNBytes ];
        return;
    }

    void MRecord::SetData( byte_type* aDataPtr )
    {
        ClearData();
        fOwnsData = false;
        fData = aDataPtr;
        return;
    }

    void MRecord::ClearData()
    {
        if( fOwnsData && fData != NULL ) delete [] fData;
        fData = NULL;
        return;
    }

}
