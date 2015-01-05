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
        if( fOwnsData && fData != NULL ) delete [] fData;
    }

    void MRecord::SetData( unsigned aNBytes )
    {
        if( fOwnsData ) delete [] fData;
        fOwnsData = true;
        fData = new byte_type[ aNBytes ];
        return;
    }

    void MRecord::SetData( byte_type* aDataPtr )
    {
        if( fOwnsData ) delete [] fData;
        fOwnsData = false;
        fData = aDataPtr;
        return;
    }

}
