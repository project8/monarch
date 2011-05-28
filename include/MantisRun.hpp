#ifndef MANTISRUN_HPP_
#define MANTISRUN_HPP_

#include "MantisCallable.hpp"

#include "MantisStatus.hpp"

class MantisRun :
    public MantisCallable
{
    public:
        MantisRun();
        virtual ~MantisRun();
        
        void SetStatus( MantisStatus* aStatus );
        void SetDuration( const unsigned int& aDuration );
        
        void Initialize();
        void Execute();
        void Finalize();
    
    private:
        MantisStatus* fStatus;
        unsigned int fDuration;
};

#endif
