#ifndef MANTISRUN_HPP_
#define MANTISRUN_HPP_

#include "MantisCallable.hpp"
#include "MantisEnv.hpp"
#include "MantisStatus.hpp"

class MantisRun :
    public MantisCallable
{
    public:
  static MantisRun* runFromEnv(safeEnvPtr& env, MantisStatus* sts);
        virtual ~MantisRun();
        
        void SetStatus( MantisStatus* aStatus );
        void SetDuration( const unsigned int& aDuration );
        
        void Initialize();
        void Execute();
        void Finalize();
    
    private:
        MantisRun();
        MantisStatus* fStatus;
        unsigned int fDuration;
};

#endif
