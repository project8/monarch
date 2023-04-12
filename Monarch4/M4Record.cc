/*
 * M4Record.cc
 *
 *  Created on: Apr 12, 2023
 *      Author: N.S. Oblath
 */
#define M4_API_EXPORTS

#include "M4Record.hh"

namespace monarch4
{
    M4Record::M4Record( unsigned aNBytes ) :
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

    M4Record::M4Record( RecordIdType* aRecPtr, TimeType* aTimePtr, byte_type* aDataPtr ) :
            fOwnsData( false ),
            fRecordId( aRecPtr ),
            fTime( aTimePtr ),
            fData( aDataPtr )
    {
    }

    M4Record::~M4Record()
    {
        ClearData();
    }

    void M4Record::Initialize()
    {
        ClearData();
        return;
    }

    void M4Record::Initialize( unsigned aNBytes )
    {
        ClearData();
        fRecordId = new RecordIdType();
        fTime = new TimeType();
        SetRecordId( 0 );
        SetTime( 0 );
        fData = new byte_type[ aNBytes ];
        return;
    }

    void M4Record::Initialize( RecordIdType* aRecPtr, TimeType* aTimePtr, byte_type* aDataPtr )
    {
        ClearData();
        fOwnsData = false;
        fRecordId = aRecPtr;
        fTime = aTimePtr;
        fData = aDataPtr;
        return;
    }

    void M4Record::ClearData()
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

    void M4Record::UpdateDataPtr( const byte_type* aDataPtr ) const
    {
        const_cast< M4Record* >(this)->UpdateDataPtr( const_cast< byte_type* >( aDataPtr ) );
        return;
    }

    void M4Record::UpdateDataPtr( byte_type* aDataPtr )
    {
        if( fOwnsData )
        {
            throw M4Exception() << "Cannot update data pointer when the record owns the data";
        }
        fData = aDataPtr;
        return;
    }

}
