#ifndef MANTISFILEWRITER_HH_
#define MANTISFILEWRITER_HH_

#include "MantisCallable.hpp"

#include "MantisCondition.hpp"
#include <cstdio>

#include "MantisEnv.hpp"
#include "MantisStatus.hpp"
#include "MantisBuffer.hpp"

#include <string>
using std::string;

class MantisFileWriter :
    public MantisCallable
{
    public:
  static MantisFileWriter* writerFromEnv(safeEnvPtr& env,
					 MantisStatus* sts,
					 MantisBuffer* buf);
        virtual ~MantisFileWriter();

        void SetStatus( MantisStatus* aStatus );        
        void SetBuffer( MantisBuffer* aBuffer );
        void SetFileName( const string& aName );
        
        void Initialize();
        void Execute();
        void Finalize();
        
    private:
        MantisFileWriter();
        MantisCondition fCondition;
        FILE* fFile;
        unsigned long fRecordCount;
        
        MantisStatus* fStatus;
        MantisBuffer* fBuffer;
        string fFileName;
};

#endif
