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
            fOwnsData( true ),
            fRecordId( NULL ),
            fTime( NULL ),
            fData( NULL )
    {
        if( aNBytes != 0 )
        {
            fRecordId = new RecordIdType();
            fTime = new TimeType();
            fData = new byte_type[ aNBytes ];
        }
    }

    M3Record::M3Record( RecordIdType* aRecPtr, TimeType* aTimePtr, byte_type* aDataPtr ) :
            fOwnsData( false ),
            fRecordId( aRecPtr ),
            fTime( aTimePtr ),
            fData( aDataPtr )
    {
    }

    M3Record::~M3Record()
    {
        ClearData();
    }

    void M3Record::Initialize()
    {
        ClearData();
        return;
    }

    void M3Record::Initialize( unsigned aNBytes )
    {
        ClearData();
        fRecordId = new RecordIdType();
        fTime = new TimeType();
        SetRecordId( 0 );
        SetTime( 0 );
        fData = new byte_type[ aNBytes ];
        return;
    }

    void M3Record::Initialize( RecordIdType* aRecPtr, TimeType* aTimePtr, byte_type* aDataPtr )
    {
        ClearData();
        fOwnsData = false;
        fRecordId = aRecPtr;
        fTime = aTimePtr;
        fData = aDataPtr;
        return;
    }

    void M3Record::ClearData()
    {
        if( fOwnsData )
        {
            delete fRecordId;
            delete fTime;
            delete [] fData;
        }
        fRecordId = NULL;
        fTime = NULL;
        fData = NULL;
        fOwnsData = true;
        return;
    }

    void M3Record::UpdateDataPtr( const byte_type* aDataPtr ) const
    {
        const_cast< M3Record* >(this)->UpdateDataPtr( const_cast< byte_type* >( aDataPtr ) );
        return;
    }

    void M3Record::UpdateDataPtr( byte_type* aDataPtr )
    {
        if( fOwnsData )
        {
            throw M3Exception() << "Cannot update data pointer when the record owns the data";
        }
        fData = aDataPtr;
        return;
    }

}
