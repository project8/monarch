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

        public:
            MStream( const MStreamHeader& aHeader, H5::CommonFG* aH5StreamParentLoc );
            virtual ~MStream();

            MEMBERVARIABLE( Mode, Mode );


            //********************************
            // methods for reading (all const)
            //********************************

        public:
            // get the pointer to the stream record
            const MRecord* GetStreamRecord() const;
            // get the pointer to a particular channel record
            const MRecord* GetChannelRecord( unsigned aChannel ) const;

            // read the next record from the file
            bool ReadRecord() const;

            void Close() const;


            //*********************************
            // methods for writing (none const)
            //*********************************

        public:
            // get the pointer to the stream record
            MRecord* GetStreamRecord();
            // get the pointer to a particular channel record
            MRecord* GetChannelRecord( unsigned aChannel );

            // write the record contents to the file
            bool WriteRecord( bool aIsNewAcquisition );

            void Close();


        public:
            unsigned GetDataTypeSize() const           { return fDataTypeSize;  }
            unsigned GetStreamRecordNBytes() const     { return fStrRecNBytes;  }
            unsigned GetSreamRecordSize() const        { return fStrRecSize;    }
            unsigned GetChannelRecordNBytes() const    { return fChanRecNBytes; }
            unsigned GetChannelRecordSize() const      { return fChanRecSize;   }
            unsigned GetNChannels() const              { return fNChannels;     }
            unsigned GetNAcquisitions() const          { return fNAcquisitions; }
            AcquisitionIdType GetAcquisitionId() const { return fAcquisitionId; }
            unsigned GetRecordCount() const            { return fRecordCount;   }
            unsigned GetNRecordsInAcquisition() const  { return fNRecordsInAcq; }
            bool GetIsInterleaved() const              { return fInterleaved;   }

        private:
            void ZipChannels(); // for writing
            void UnzipChannels() const; // for reading

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

            mutable unsigned fRecordCount;
            mutable unsigned fNRecordsInAcq;

            mutable bool fInterleaved;

        private:
            void FinalizeCurrentAcq(); // for writing
            void FinalizeStream(); // for writing

            mutable H5::CommonFG* fH5StreamParentLoc;
            mutable H5::Group* fH5AcqLoc;
            mutable H5::DataSet* fH5CurrentAcqDataSet;

            mutable char fAcqNameBuffer[ 10 ];

            mutable H5::DataType fDataTypeInFile;
            mutable H5::DataType fDataTypeUser;

            enum { N_DATA_DIMS = 2 };
            mutable hsize_t fDataDims[ N_DATA_DIMS ];
            mutable hsize_t fDataDims1Rec[ N_DATA_DIMS ];
            mutable hsize_t fMaxDataDims[ N_DATA_DIMS ];
            mutable hsize_t fDataChunkDims[ N_DATA_DIMS ];
            mutable hsize_t fDataOffset[ N_DATA_DIMS ];

    };

} /* namespace monarch */

#endif /* MSTREAM_HH_ */
