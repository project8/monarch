#ifndef MANTISFILEWRITER_HH_
#define MANTISFILEWRITER_HH_

#include "MantisCallable.hpp"

#include "MantisCondition.hpp"
#include "MantisEnv.hpp"
#include "MantisEgg.hpp"
#include <sys/time.h>
#include "MantisStatus.hpp"
#include "MantisBuffer.hpp"

class MantisFileWriter :
    public MantisCallable
{
    public:
  static MantisFileWriter* writerFromEnv(safeEnvPtr& env,
					 MantisEgg* egg,
					 MantisStatus* sts,
					 MantisBuffer* buf);
        virtual ~MantisFileWriter();

        void SetStatus( MantisStatus* aStatus );        
        void SetBuffer( MantisBuffer* aBuffer );
  void SetOutputEgg(MantisEgg* anEgg);
        void Initialize();
        void Execute();
        void Finalize();
        
    private:
        MantisFileWriter();
        MantisCondition fCondition;

        unsigned long fRecordCount;
        unsigned long long fLiveMicroseconds;
        
        MantisEgg* fEgg;
        MantisStatus* fStatus;
        MantisBuffer* fBuffer;
};

#endif
