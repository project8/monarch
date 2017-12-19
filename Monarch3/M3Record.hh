/*
 * M3Record.hh
 *
 *  Created on: Dec 26, 2014
 *      Author: nsoblath
 */

#ifndef M3RECORD_HH_
#define M3RECORD_HH_

#include "M3Exception.hh"
#include "M3MemberVariable.hh"
#include "M3Types.hh"

namespace monarch3
{
    /*!
     @class M3Record
     @author N. S. Oblath

     @brief Contains the information that makes up a record.

     @details
     Includes the record ID, timestamp (ns since the start of the run), and data array.

     The array can either own its own data, or point to data elsewhere.
     This is useful for having streams with multiple non-interleaved channels
     The channel record objects can point to the appropriate location in the stream record object.

     If the M3Record( unsigned ) constructor or SetData( unsigned ) function is used, the record will allocate its own data.
     If the M3Record( byte_type* ) constructor or SetData( byte_type* ) function is used, the record will point to data elsewhere that it does not own.

     If the record does not own the data, then the data pointer can be updatd with UpdateDataPtr().
    */
    class M3_API M3Record
    {
        public:
            M3Record( unsigned aNBytes = 0 );
            M3Record( RecordIdType* aRecPtr, TimeType* aTimePtr, byte_type* aDataPtr );
            ~M3Record();

            /// Allocate no memory for the record; data pointer is to NULL
            void Initialize();
            /// Allocate aNBytes of memory for the record
            void Initialize( unsigned aNBytes );
            /// Do not allocate memory for the record; instead point to someone else's memory.
            /// Warning: if the memory pointed to is de-allocated, use of this record should be discontinued
            void Initialize( RecordIdType* aRecPtr, TimeType* aTimePtr, byte_type* aDataPtr );

            void ClearData();

            RecordIdType GetRecordId() const;
            RecordIdType* GetRecordIdPtr() const;
            void SetRecordId( RecordIdType aId );

            TimeType GetTime() const;
            TimeType* GetTimePtr() const;
            void SetTime( TimeType aTime );

            const byte_type* GetData() const;
            byte_type* GetData();

            void UpdateDataPtr( byte_type* aDataPtr );

            M3MEMBERVARIABLE_NOSET( bool, OwnsData );

        private:
            RecordIdType* fRecordId;
            TimeType* fTime;

            byte_type* fData;

    };

    inline RecordIdType M3Record::GetRecordId() const
    {
        return *fRecordId;
    }

    inline RecordIdType* M3Record::GetRecordIdPtr() const
    {
        return fRecordId;
    }

    inline void M3Record::SetRecordId( RecordIdType aId )
    {
        (*fRecordId) = aId;
        return;
    }

    inline TimeType M3Record::GetTime() const
    {
        return *fTime;
    }

    inline TimeType* M3Record::GetTimePtr() const
    {
        return fTime;
    }

    inline void M3Record::SetTime( TimeType aTime )
    {
        (*fTime) = aTime;
        return;
    }

    inline const byte_type* M3Record::GetData() const
    {
        return fData;
    }

    inline byte_type* M3Record::GetData()
    {
        return fData;
    }

}

#endif /* M3RECORD_HH_ */
