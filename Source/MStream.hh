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
            MStream( const MStreamHeader& aHeader, H5::CommonFG* aH5StreamParentLoc );
            virtual ~MStream();

            //********************************
            // methods for reading (all const)
            //********************************

        public:
            // get the pointer to the stream record
            const MRecordBytes* GetStreamRecord() const;
            // get the pointer to a particular channel record
            const MRecordBytes* GetChannelRecord( unsigned aChannel ) const;

            // read the next record from the file
            void ReadRecord() const;


            //*********************************
            // methods for writing (none const)
            //*********************************

        public:
            // get the pointer to the stream record
            MRecordBytes* GetStreamRecord();
            // get the pointer to a particular channel record
            MRecordBytes* GetChannelRecord( unsigned aChannel );

            // write the record contents to the file
            void WriteRecord( bool aIsNewAcquisition );






            MEMBERVARIABLE( AcquisitionIdType, AcquisitionID );

        private:
            mutable MRecordBytes fStreamRecord;

            unsigned fNChannels;
            mutable MRecordBytes* fChannelRecords;


        private:
            H5::CommonFG* fH5StreamParentLoc;
            H5::CommonFG* fH5CurrentAcqLoc;
    };

} /* namespace monarch */

#endif /* MSTREAM_HH_ */
