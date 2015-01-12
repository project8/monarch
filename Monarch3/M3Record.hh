#ifndef M3RECORD_HH_
#define M3RECORD_HH_

#include "M3Exception.hh"
#include "M3MemberVariable.hh"
#include "M3Types.hh"

namespace monarch3
{
    class M3Record
    {
        public:
            M3Record( unsigned aNBytes = 0 );
            M3Record( byte_type* aDataPtr );
            ~M3Record();

            M3MEMBERVARIABLE( RecordIdType, RecordId );
            M3MEMBERVARIABLE( TimeType, Time );

            M3MEMBERVARIABLE_NOSET( bool, OwnsData );

        public:
            const byte_type* GetData() const;
            byte_type* GetData();

            /// Allocate no memory for the record; data pointer is to NULL
            void SetData();
            /// Allocate aNBytes of memory for the record
            void SetData( unsigned aNBytes );
            /// Do not allocate memory for the record; instead point to someone else's memory.
            /// Warning: if the memory pointed to is de-allocated, use of this record should be discontinued
            void SetData( byte_type* aDataPtr );

            void ClearData();

        private:
            byte_type* fData;

    };

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
