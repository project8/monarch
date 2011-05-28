#ifndef MANTISCALLABLE_HPP_
#define MANTISCALLABLE_HPP_

class MantisCallable
{
    public:
        MantisCallable();
        virtual ~MantisCallable();
        
        virtual void Execute() = 0;
};

#endif
