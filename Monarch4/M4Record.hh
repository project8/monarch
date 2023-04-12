/*
 * M4Record.hh
 *
 *  Created on: Apr 12, 2023
 *      Author: N.S. Oblath
 */

#ifndef M4RECORD_HH_
#define M4RECORD_HH_

#include "M4Exception.hh"
#include "M4MemberVariable.hh"
#include "M4Types.hh"

namespace monarch4
{
    /*!
     @class M4Record
     @author N. S. Oblath

     @brief Contains the information that makes up a record.

     @details
     Includes the record ID, timestamp (ns since the start of the run), and data array.

     The array can either own its own data, or point to data elsewhere.
     This is useful for having streams with multiple non-interleaved channels
     The channel record objects can point to the appropriate location in the stream record object.

     If the M4Record( unsigned ) constructor or SetData( unsigned ) function is used, the record will allocate its own data.
     If the M4Record( byte_type* ) constructor or SetData( byte_type* ) function is used, the record will point to data elsewhere that it does not own.

     If the record does not own the data, then the data pointer can be updatd with UpdateDataPtr().
    */
    class M4_API M4Record
    {
        public:
            M4Record( unsigned aNBytes = 0 );
            M4Record( RecordIdType* aRecPtr, TimeType* aTimePtr, byte_type* aDataPtr );
            ~M4Record();

            M4Record( const M4Record& ) = delete;
            M4Record& operator=( const M4Record& ) = delete;

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

            void UpdateDataPtr( const byte_type* aDataPtr ) const;
            void UpdateDataPtr( byte_type* aDataPtr );

            M4MEMBERVARIABLE_NOSET( bool, OwnsData );

        private:
            RecordIdType* fRecordId;
            TimeType* fTime;

            mutable byte_type* fData;

    };

    inline RecordIdType M4Record::GetRecordId() const
    {
        return *fRecordId;
    }

    inline RecordIdType* M4Record::GetRecordIdPtr() const
    {
        return fRecordId;
    }

    inline void M4Record::SetRecordId( RecordIdType aId )
    {
        (*fRecordId) = aId;
        return;
    }

    inline TimeType M4Record::GetTime() const
    {
        return *fTime;
    }

    inline TimeType* M4Record::GetTimePtr() const
    {
        return fTime;
    }

    inline void M4Record::SetTime( TimeType aTime )
    {
        (*fTime) = aTime;
        return;
    }

    inline const byte_type* M4Record::GetData() const
    {
        return fData;
    }

    inline byte_type* M4Record::GetData()
    {
        return fData;
    }

}

#endif /* M4RECORD_HH_ */
