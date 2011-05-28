#include "MantisState.hpp"

MantisState::MantisState() :
    fValue( eRead )
{
}
MantisState::MantisState( const MantisState& aCopy ) :
    fValue( aCopy.fValue )
{
}
MantisState::~MantisState()
{
}

bool MantisState::IsWriting() const
{
    return (fValue == eWriting);	
}
void MantisState::SetWriting()
{
	fValue = eWriting;
	return;
}
        
bool MantisState::IsWritten() const
{
	return (fValue == eWritten);
}
void MantisState::SetWritten()
{
	fValue = eWritten;
	return;
}
        
bool MantisState::IsReading() const
{
	return (fValue == eReading);
}
void MantisState::SetReading()
{
	fValue = eReading;
	return;
}
        
bool MantisState::IsRead() const
{
	return (fValue == eRead);
}
void MantisState::SetRead()
{
	fValue = eRead;
	return;
}
