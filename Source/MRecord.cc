/*
 * MRecord.cc
 *
 *  Created on: Dec 26, 2014
 *      Author: nsoblath
 */

#include "MRecord.hh"

namespace monarch
{
    MRecordBytes::MRecordBytes( unsigned aNBytes ) :
            fRecordId( 0 ),
            fTime( 0 ),
            fOwnsData( true ),
            fData( NULL )
    {
        fData = new byte_type[ aNBytes ];
    }

    MRecordBytes::MRecordBytes( byte_type* aDataPtr ) :
            fRecordId( 0 ),
            fTime( 0 ),
            fOwnsData( false ),
            fData( aDataPtr )
    {
    }

    MRecordBytes::~MRecordBytes()
    {
        if( fOwnsData ) delete [] fData;
    }

}
