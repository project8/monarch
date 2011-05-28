#ifndef MANTISPX1500SOURCE_HPP_
#define MANTISPX1500SOURCE_HPP_

#include "MantisCallable.hpp"

#include "MantisStatus.hpp"
#include "MantisBuffer.hpp"

#include <cstddef> // px1500.h refers to NULL without including cstddef
#include "px1500.h"

class MantisPX1500 :
    public MantisCallable
{
    public:
        MantisPX1500();
        virtual ~MantisPX1500();
        
        void SetStatus( MantisStatus* aStatus );
        void SetBuffer( MantisBuffer* aBuffer );
        void SetDigitizationRate( const double& aRate );
        
        void Initialize();
        void Execute();
        void Finalize();
        
    private:
        HPX4 fHandle;
        unsigned long fAcquisitionCount;
        unsigned long fRecordCount;
        unsigned long fDeadTickCount;
        
        MantisStatus* fStatus;
        MantisBuffer* fBuffer;
        double fDigitizationRate;
};

#endif
