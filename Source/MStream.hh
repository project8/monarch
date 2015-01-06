/*
 * MStream.hh
 *
 *  Created on: Dec 26, 2014
 *      Author: nsoblath
 */

#ifndef MSTREAM_HH_
#define MSTREAM_HH_

#include "MHeader.hh"
#include "MMemberVariable.hh"
#include "MRecord.hh"
#include "MTypes.hh"

#include "H5Cpp.h"


namespace monarch
{

    class MStream
    {
        public:
            enum Mode
            {
                kRead,
                kWrite
            };

        private:
            typedef void (MStream::*DoReadRecordFunc)() const;
            typedef void (MStream::*DoWriteRecordFunc)();

        public:
            MStream( const MStreamHeader& aHeader, H5::CommonFG* aH5StreamParentLoc, MultiChannelFormatType aAccessFormat = sSeparate );
            virtual ~MStream();

            MEMBERVARIABLE( Mode, Mode );

            /// Setup to read/write data (called in constructor; only call this if read/write parameters change during file reading)
            void Initialize() const;

            //********************************
            // methods for reading (all const)
            //********************************

        public:
            /// Get the pointer to the stream record
            const MRecord* GetStreamRecord() const;
            /// Get the pointer to a particular channel record
            const MRecord* GetChannelRecord( unsigned aChannel ) const;

            /// Read the next record from the file
            bool ReadRecord( int anOffset = 0 ) const;

            /// Close the file
            void Close() const;


            //*********************************
            // methods for writing (none const)
            //*********************************

        public:
            /// Get the pointer to the stream record
            MRecord* GetStreamRecord();
            /// Get the pointer to a particular channel record
            MRecord* GetChannelRecord( unsigned aChannel );

            /// Write the record contents to the file
            bool WriteRecord( bool aIsNewAcquisition );

            /// Close the file
            void Close();


        public:
            bool GetIsInitialized() const              { return fIsInitialized;   }
            bool GetRecordsAccessed() const            { return fRecordsAccessed; }
            unsigned GetDataTypeSize() const           { return fDataTypeSize;    }
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

            mutable unsigned fStrRecNBytes;
            mutable unsigned fStrRecSize;

            mutable unsigned fChanRecNBytes;
            mutable unsigned fChanRecSize;

            mutable MRecord fStreamRecord;

            mutable unsigned fNChannels;
            mutable MRecord* fChannelRecords;

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
