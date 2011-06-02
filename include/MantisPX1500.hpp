#ifndef MANTISPX1500SOURCE_HPP_
#define MANTISPX1500SOURCE_HPP_

#include "MantisCallable.hpp"

#include "MantisCondition.hpp"
#include <cstddef> // px1500.h refers to NULL without including cstddef
#include "px1500.h"
#include <sys/time.h>
#include "MantisStatus.hpp"
#include "MantisBuffer.hpp"
#include "MantisEnv.hpp"

class MantisPX1500 :
    public MantisCallable
{
    public:
        static MantisPX1500* digFromEnv(safeEnvPtr& env, 
				  MantisStatus* sts,
				  MantisBuffer* buf);
        virtual ~MantisPX1500();
        
        void SetStatus( MantisStatus* aStatus );
        void SetBuffer( MantisBuffer* aBuffer );
        void SetDigitizationRate( const double& aRate );
        
        void Initialize();
        void Execute();
        void Finalize();
        
    private:
        MantisPX1500();
        
        MantisCondition fCondition;
        HPX4 fHandle;
        
        unsigned long fAcquisitionCount;
        unsigned long fRecordCount;
        unsigned long long fLiveMicroseconds;
        unsigned long long fDeadMicroseconds;
                
        MantisStatus* fStatus;
        MantisBuffer* fBuffer;
        double fDigitizationRate;
};

#endif
