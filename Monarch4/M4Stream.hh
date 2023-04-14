/*
 * M4Stream.hh
 *
 *  Created on: Apr 12, 2023
 *      Author: N.S. Oblath
 */

#ifndef M4STREAM_HH_
#define M4STREAM_HH_

#include "M4Header.hh"
#include "M4MemberVariable.hh"
#include "M4Record.hh"
#include "M4Types.hh"

#include "H5Cpp.h"

namespace monarch4
{
    /*!
    @class M4Stream
    @author N. S. Oblath

    @brief Read/write access for a data stream.

    @details
    Data can be read/written either for the stream as a whole, or for individual channels within the stream by
    accessing the relevant record objects.

    The function ReadRecord extracts a record from disk.  The offset allows the file to move forward or back within the file.
    After that call, the new record is available via the record objects.

    The function WriteRecord moves the data in memory to disk.  Before calling WriteRecord, the appropriate data should be copied
    to the record object(s).

    When a stream object is created, most of the information needed to define the stream characteristics is extracted from the
    stream header object that is passed to the constructor.  Most users won't need to worry about this, as it's already taken
    care of by the Monarch4 object.
    */

    class M4_API M4Stream
    {
        public:
            enum Mode
            {
                kRead,
                kWrite
            };

        private:
            typedef void (M4Stream::*DoReadRecordFunc)( bool ) const;
            typedef void (M4Stream::*DoWriteRecordFunc)( bool );

        public:
            M4Stream( const M4StreamHeader& aHeader, HAS_GRP_IFC* aH5StreamParentLoc, uint32_t aAccessFormat = sSeparate );
            virtual ~M4Stream();

            M4Stream( const M4Stream& ) = delete;
            M4Stream& operator=( const M4Stream& ) = delete;

            M4MEMBERVARIABLE( Mode, Mode );

        public:
            /// Setup to read/write data (called in constructor; only call this if read/write parameters change during file reading)
            void Initialize() const;

            void SetMutex( const mutex_ptr& aMutexPtr );

            //********************************
            // methods for reading (all const)
            //********************************

        public:
            /// Get the pointer to the stream record
            const M4Record* GetStreamRecord() const;
            /// Get the pointer to a particular channel record
            const M4Record* GetChannelRecord( unsigned aChannel ) const;

            /// Read the next record from the file
            /**
             * Assuming the last record read was [J], ReadRecord will access the [J+1+offset] record.
             * This means that the offset parameter has the following meanings:
             *  - if offset == 0 (default), the next record will be accessed;
             *  - if offset == -1, the current record will be reread;
             *  - offsets < -1 will go backwards in the file;
             *  - offsets > 0 will skip forward in the file.
             * The flag aIfNewAcqStartAtFirstRec determines the behavior if the read operation moves to a new acquisition.
             * If true, the record read will be the first in the acquisition.
             * If false, the record read will be whatever was stepped to given the offset.
             * Starting at the first record in a new acquisition is behavior that was added (and deemed more useful, and therefore is the default), and this flag provides backwards compatibility.
            **/
            bool ReadRecord( int anOffset = 0, bool aIfNewAcqStartAtFirstRec = true ) const;

            /// Close the file
            void Close() const;


            //*********************************
            // methods for writing (none const)
            //*********************************

        public:
            /// Get the pointer to the stream record
            M4Record* GetStreamRecord();
            /// Get the pointer to a particular channel record
            M4Record* GetChannelRecord( unsigned aChannel );

            /// Write the record contents to the file
            bool WriteRecord( bool aIsNewAcquisition );

            /// Close the stream
            void Close();


        public:
            bool GetIsInitialized() const              { return fIsInitialized;   }
            bool GetRecordsAccessed() const            { return fRecordsAccessed; }
            unsigned GetDataTypeSize() const           { return fDataTypeSize;    }
            unsigned GetSampleSize() const             { return fSampleSize;      }
            unsigned GetStreamRecordNBytes() const     { return fStrRecNBytes;    }
            unsigned GetStreamRecordSize() const       { return fStrRecSize;      }
            unsigned GetChannelRecordNBytes() const    { return fChanRecNBytes;   }
            unsigned GetChannelRecordSize() const      { return fChanRecSize;     }
            unsigned GetNChannels() const              { return fNChannels;       }
            unsigned GetNAcquisitions() const          { return fNAcquisitions;   }
            AcquisitionIdType GetAcquisitionId() const { return fAcquisitionId;   }
            unsigned GetRecordCountInAcq() const       { return fRecordCountInAcq; }
            unsigned GetNRecordsInAcquisition() const  { return fNRecordsInAcq;   }
            TimeType GetAcqFirstRecordTime() const     { return fAcqFirstRecTime; }
            RecordIdType GetAcqFirstRecordId() const   { return fAcqFirstRecId;   }
            TimeType* GetAcqFirstRecTimes() const      { return fAcqFirstRecTimes; }
            RecordIdType* GetAcqFirstRecordIds() const { return fAcqFirstRecIds;  }
            unsigned GetRecordCountInFile() const      { return fRecordCountInFile; }
            unsigned GetNRecordsInFile() const         { return fNRecordsInFile;  }
            unsigned GetFirstRecordInFile() const      { return fFirstRecordInFile; }
            bool GetIsInterleaved() const              { return fDataInterleaved; }

            /// Access format can be changed during read or write; must call Initialize() after this
            void SetAccessFormat( uint32_t aFormat ) const;
            uint32_t GetAccessFormat() const { return fAccessFormat; }

        private:
            void ReadRecordInterleavedToSeparate( bool aIsNewAcquisition ) const;
            void ReadRecordAsIs( bool aIsNewAcquisition ) const;
            mutable DoReadRecordFunc fDoReadRecord;

            void WriteRecordSeparateToInterleaved( bool aIsNewAcquisition );
            void WriteRecordAsIs( bool aIsNewAcquisition );
            mutable DoWriteRecordFunc fDoWriteRecord;

            mutable bool fIsInitialized;
            mutable bool fRecordsAccessed;

            mutable unsigned fDataTypeSize;
            mutable unsigned fSampleSize;

            mutable unsigned fStrRecNBytes;
            mutable unsigned fStrRecSize;

            mutable unsigned fChanRecNBytes;
            mutable unsigned fChanRecSize;
            mutable uint64_t fChanRecLength; // ns

            mutable M4Record fStreamRecord;

            mutable unsigned fNChannels;
            mutable M4Record* fChannelRecords;

            mutable unsigned fNAcquisitions;
            mutable AcquisitionIdType fAcquisitionId;

            mutable unsigned fRecordCountInAcq;
            mutable unsigned fNRecordsInAcq;
            mutable TimeType fAcqFirstRecTime; // used when reading with the stream record
            mutable RecordIdType fAcqFirstRecId; // used when reading with the stream record
            mutable TimeType* fAcqFirstRecTimes; // used when reading with the channel records
            mutable RecordIdType* fAcqFirstRecIds; // used when reading with the channel records

            mutable bool fDataInterleaved;
            mutable uint32_t fAccessFormat;

            mutable std::vector< std::pair< unsigned, unsigned > > fRecordIndex;  // has an entry for every record: (acquisition ID, record ID)
            mutable unsigned fRecordCountInFile;
            mutable unsigned fNRecordsInFile;
            mutable unsigned fFirstRecordInFile;

        private:
            void BuildIndex() const; // for reading

            void FinalizeCurrentAcq(); // for writing
            void FinalizeStream(); // for writing

            mutable char fAcqNameBuffer[ 10 ];

            mutable z5::filesystem::handle::Group* fZ5StreamParentLoc;
            mutable z5::filesystem::handle::Group* fZ5AcqLoc;
            mutable z5::filesystem::handle::Dataset* fZ5CurrentAcqDataSet;

            mutable H5::DataSpace* fH5DataSpaceUser;

            mutable H5::DataType fDataTypeInFile;
            mutable H5::DataType fDataTypeUser;

            enum { N_DATA_DIMS = 2 };
            mutable hsize_t fStrDataDims[ N_DATA_DIMS ];
            mutable hsize_t fStrMaxDataDims[ N_DATA_DIMS ];
            mutable hsize_t fStrDataChunkDims[ N_DATA_DIMS ];
            mutable hsize_t fDataDims1Rec[ N_DATA_DIMS ];
            mutable hsize_t fDataOffset[ N_DATA_DIMS ];
            mutable hsize_t fDataStride[ N_DATA_DIMS ];
            mutable hsize_t fDataBlock[ N_DATA_DIMS ];

            mutable mutex_ptr fMutexPtr;
    };

    inline void M4Stream::SetMutex( const mutex_ptr& aMutexPtr )
    {
        fMutexPtr = aMutexPtr;
        return;
    }

    inline M4Record* M4Stream::GetStreamRecord()
    {
        return &fStreamRecord;
    }

} /* namespace monarch */

#endif /* MSTREAM_HH_ */
