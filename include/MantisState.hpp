#ifndef MANTISSTATE_HPP_
#define MANTISSTATE_HPP_

class MantisState
{
    public:
        MantisState();
        MantisState( const MantisState& aCopy );
        ~MantisState();
        
        bool IsWriting() const;
        void SetWriting();
        
        bool IsWritten() const;
        void SetWritten();
        
        bool IsReading() const;
        void SetReading();
        
        bool IsRead() const;
        void SetRead();

    private:
        enum { eWriting, eWritten, eReading, eRead } fValue;
};

#endif
