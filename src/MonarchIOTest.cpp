#include "MonarchIO.hpp"

#include <iostream>
using std::cout;
using std::endl;

#include <cstring>

int main()
{
    size_t tNumbers[2] = { 448, 1894 };
    char tPhrase[19];
    strcpy( tPhrase, "prease to be write" );

    MonarchIO* tWrite = new MonarchIO( sWriteMode );
    if( tWrite->Open( "MonarchIOTest.dat" ) == false )
    {
        cout << "could not open file for writing" << endl;
        return -1;
    }
    if( tWrite->Write( &tPhrase[0], 18 ) == false )
    {
        cout << "could not write string" << endl;
        return -1;
    }
    if( tWrite->Write( &tNumbers[0], 2 ) == false )
    {
        cout << "could not write numbers" << endl;
        return -1;
    }
    tWrite->Close();

    tNumbers[0] = 0;
    tNumbers[1] = 0;
    strcpy( tPhrase, "                  " );

    MonarchIO* tRead = new MonarchIO( sReadMode );
    if( tRead->Open( "MonarchIOTest.dat" ) == false )
    {
        cout << "could not open file for writing" << endl;
        return -1;
    }
    if( tRead->Read( &tPhrase[0], 18 ) == false )
    {
        cout << "could not read string" << endl;
        return -1;
    }
    if( tRead->Read( &tNumbers[0], 2 ) == false )
    {
        cout << "could not read numbers" << endl;
        return -1;
    }
    tRead->Close();

    cout << "phrase <" << tPhrase << ">" << endl;
    cout << "numbers <" << tNumbers[0] << "," <<  tNumbers[1] << ">" << endl;

    return 0;
}




