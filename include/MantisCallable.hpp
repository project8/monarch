#ifndef MANTISCALLABLE_HH_
#define MANTISCALLABLE_HH_

class MantisCallable
{
    public:
        MantisCallable();
        virtual ~MantisCallable();
        
        virtual void Execute() = 0;
};

#endif
