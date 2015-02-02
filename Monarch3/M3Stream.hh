/*
 * M3Stream.hh
 *
 *  Created on: Dec 26, 2014
 *      Author: nsoblath
 */

#ifndef M3STREAM_HH_
#define M3STREAM_HH_

#include "M3Header.hh"
#include "M3MemberVariable.hh"
#include "M3Record.hh"
#include "M3Types.hh"

#include "H5Cpp.h"

#ifdef _WIN32
M3_EXPIMP_TEMPLATE template class M3_API std::vector< std::pair< unsigned, unsigned > >;
#endif

namespace monarch3
{
    /*!
    @class M3Stream
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
    care of by the Monarch3 object.
    */    class M3_API M3Stream
    {
        public:
            enum Mode
            {
                kRead,
                kWrite
            };

        private:
            typedef void (M3Stream::*DoReadRecordFunc)() const;
            typedef void (M3Stream::*DoWriteRecordFunc)();

        public:
            M3Stream( const M3StreamHeader& aHeader, H5::CommonFG* aH5StreamParentLoc, MultiChannelFormatType aAccessFormat = sSeparate );
            virtual ~M3Stream();

            M3MEMBERVARIABLE( Mode, Mode );

            /// Setup to read/write data (called in constructor; only call this if read/write parameters change during file reading)
            void Initialize() const;

            //********************************
            // methods for reading (all const)
            //********************************

        public:
            /// Get the pointer to the stream record
            const M3Record* GetStreamRecord() const;
            /// Get the pointer to a particular channel record
            const M3Record* GetChannelRecord( unsigned aChannel ) const;

            /// Read the next record from the file
            /**
             * Assuming the last record read was [J], ReadRecord will access the [J+1+offset] record.
             * This means that the offset parameter has the following meanings:
             *  - if offset == 0 (default), the next record will be accessed;
             *  - if offset == -1, the current record will be reread;
             *  - offsets < -1 will go backwards in the file;
             *  - offsets > 0 will skip forward in the file.
            **/
            bool ReadRecord( int anOffset = 0 ) const;

            /// Close the file
            void Close() const;


            //*********************************
            // methods for writing (none const)
            //*********************************

        public:
            /// Get the pointer to the stream record
            M3Record* GetStreamRecord();
            /// Get the pointer to a particular channel record
            M3Record* GetChannelRecord( unsigned aChannel );

            /// Write the record contents to the file
            bool WriteRecord( bool aIsNewAcquisition );

            /// Close the file
            void Close();


        public:
            bool GetIsInitialized() const              { return fIsInitialized;   }
            bool GetRecordsAccessed() const            { return fRecordsAccessed; }
            unsigned GetDataTypeSize() const           { return fDataTypeSize;    }
            unsigned GetSampleSize() const             { return fSampleSize;      }
            unsigned GetStreamRecordNBytes() const     { return fStrRecNBytes;    }
            unsigned GetSreamRecordSize() const        { return fStrRecSize;      }
            unsigned GetChannelRecordNBytes() const    { return fChanRecNBytes;   }
            unsigned GetChannelRecordSize() const      { return fChanRecSize;     }
            unsigned GetNChannels() const              { return fNChannels;       }
            unsigned GetNAcquisitions() const          { return fNAcquisitions;   }
            AcquisitionIdType GetAcquisitionId() const { return fAcquisitionId;   }
            unsigned GetRecordCountInAcq() const       { return fRecordCountInAcq; }
            unsigned GetNRecordsInAcquisition() const  { return fNRecordsInAcq;   }
            unsigned GetRecordCountInFile() const      { return fRecordCountInFile; }
            unsigned GetNRecordsInFile() const         { return fNRecordsInFile;  }
            bool GetIsInterleaved() const              { return fDataInterleaved; }

            /// Access format can be changed during read or write; must call Initialize() after this
            void SetAccessFormat( MultiChannelFormatType aFormat ) const;
            MultiChannelFormatType GetAccessFormat() const { return fAccessFormat; }

        private:
            void ReadRecordInterleavedToSeparate() const;
            void ReadRecordAsIs() const;
            mutable DoReadRecordFunc fDoReadRecord;

            void WriteRecordSeparateToInterleaved();
            void WriteRecordAsIs();
            mutable DoWriteRecordFunc fDoWriteRecord;

            mutable bool fIsInitialized;
            mutable bool fRecordsAccessed;

            mutable unsigned fDataTypeSize;
            mutable unsigned fSampleSize;

            mutable unsigned fStrRecNBytes;
            mutable unsigned fStrRecSize;

            mutable unsigned fChanRecNBytes;
            mutable unsigned fChanRecSize;

            mutable M3Record fStreamRecord;

            mutable unsigned fNChannels;
            mutable M3Record* fChannelRecords;

            mutable unsigned fNAcquisitions;
            mutable AcquisitionIdType fAcquisitionId;

            mutable unsigned fRecordCountInAcq;
            mutable unsigned fNRecordsInAcq;

            mutable bool fDataInterleaved;
            mutable MultiChannelFormatType fAccessFormat;

            mutable std::vector< std::pair< unsigned, unsigned > > fRecordIndex;
            mutable unsigned fRecordCountInFile;
            mutable unsigned fNRecordsInFile;

        private:
            void BuildIndex() const; // for reading

            void FinalizeCurrentAcq(); // for writing
            void FinalizeStream(); // for writing

            mutable char fAcqNameBuffer[ 10 ];

            mutable H5::Group* fH5StreamParentLoc;
            mutable H5::Group* fH5AcqLoc;
            mutable H5::DataSet* fH5CurrentAcqDataSet;

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

    };

} /* namespace monarch */

#endif /* MSTREAM_HH_ */
