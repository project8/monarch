/*
 * M4Record.cc
 *
 *  Created on: Apr 12, 2023
 *      Author: N.S. Oblath
 */
#define M4_API_EXPORTS

#include <iostream>
#include "M4Record.hh"

namespace monarch4
{
    /*************************************************************************
    * @brief Construct a new default M4Record::M4Record object
    * 
    * @param aNBytes 
    *************************************************************************/
    M4Record::M4Record( unsigned aNBytes ) :
            fOwnsData( true ),
            fRecordId( nullptr ),
            fTime( nullptr ),
            fData( nullptr )
    {
std::cout << "M4Record::M4Record(size)\n";
        if( aNBytes != 0 )
        {
            // Note: see type definitions in M4Types.h
            fRecordId = new RecordIdType();
            fTime = new TimeType();
            fData = new byte_type[ aNBytes ];
        }
std::cout << "M4Record::M4Record(): void\n";
    }

    /*************************************************************************
    * @brief Construct a new M4Record::M4Record object with specifications
    * 
    * @param aRecPtr Pointer to type of record identifier
    * @param aTimePtr Pointer to record start time
    * @param aDataPtr Pointer to data buffer
    *************************************************************************/
    M4Record::M4Record( RecordIdType* aRecPtr, TimeType* aTimePtr, byte_type* aDataPtr ) :
            fOwnsData( false ),
            fRecordId( aRecPtr ),
            fTime( aTimePtr ),
            fData( aDataPtr )
    {
std::cout << "M4Record::M4Record(template)\n";
    }

    /*************************************************************************
    * @brief Destroy the M4Record::M4Record object
    * @note Data is cleared
    *************************************************************************/
    M4Record::~M4Record()
    {
std::cout << "M4Record::~M4Record()\n";
        ClearData();
    }

    /*************************************************************************
    * @brief Initialize record
    * @note Data is cleared
    *************************************************************************/
    void M4Record::Initialize()
    {
std::cout << "M4Record::Initialize(): clear\n";
        ClearData();
    }

    /*************************************************************************
    * @brief Initialize an M4Record with internaly managed buffer
    * 
    * @param aNBytes Size of record in bytes
    *************************************************************************/
    void M4Record::Initialize( unsigned aNBytes )
    {
std::cout << "M4Record::Initialize(): " << aNBytes << " bytes\n";;
        ClearData();
        fRecordId = new RecordIdType();

// fOwnsData = true;        signal internal data buffer??  see M4Record::ClearData()
        fTime = new TimeType();
        SetRecordId( 0 );
        SetTime( 0 );

///@todo smart-pointer instead?
        fData = new byte_type[ aNBytes ];
std::cout << "M4Record::Initialize(): void\n";
    }

    /*************************************************************************
    * @brief Initialize an M4Record with an external buffer
    * 
    * @param[in] aRecPtr Pointer to type of record identifier
    * @param[in] aTimePtr Pointer to record start time
    * @param[in] aDataPtr Pointer to external data buffer
    *************************************************************************/
    void M4Record::Initialize( RecordIdType* aRecPtr, TimeType* aTimePtr, byte_type* aDataPtr )
    {
std::cout << "M4Record::Initialize()\n";
        ClearData();

        fOwnsData = false;          // signal data buffer owned externally
        
        fRecordId = aRecPtr;
        fTime = aTimePtr;
        fData = aDataPtr;
std::cout << "M4Record::Initialize(): void\n";
    }

    /*************************************************************************
    * @brief Release record data
    * 
    *************************************************************************/
    void M4Record::ClearData()
    {
std::cout << "M4Record::ClearData()\n";
        if( fOwnsData )
        {
            // avoid releasing unallocated memory: segfault
            if (fRecordId != nullptr)
            { 
std::cout << "release fRecordId\n";
                delete fRecordId;
            }

            // avoid releasing unallocated memory: segfault
            if (fTime != nullptr)
            { 
std::cout << "release fData\n";
                delete fTime;
            }

            // avoid releasing unallocated memory: segfault
            if (fData != nullptr)
            { 
std::cout << "release fTime\n";
                delete [] fData;
            }
        }

        fRecordId = nullptr;
        fTime = nullptr;
        fData = nullptr;
        fOwnsData = true;       // <<<-------- in M4Record::Initialize( unsigned aNBytes ) instead?
std::cout << "M4Record::ClearData(): void\n";
    }

    /*************************************************************************
    * @brief Change the read-only M4Record pointer 
    * 
    * @param[in] aDataPtr Pointer to external data buffer
    *************************************************************************/
    void M4Record::UpdateDataPtr( const byte_type* aDataPtr ) const
    {
        const_cast< M4Record* >(this)->UpdateDataPtr( const_cast< byte_type* >( aDataPtr ) );
    }

    /*************************************************************************
    * @brief Change the read/write M4Record pointer 
    * 
    * @param aDataPtr Pointer to data buffer, external onlly
    *************************************************************************/
    void M4Record::UpdateDataPtr( byte_type* aDataPtr )
    {
        if( fOwnsData )
        {
            throw M4Exception() << "Cannot update data pointer when the record owns the data";
        }
        fData = aDataPtr;
    }

}
