#pragma once

/*=============================================================================
 Voltage Reference (CVref, DAC)
=============================================================================*/

#include <cstdint>

class Cvref {

    public:
        enum REFSEL { AVDD = 3, CVREF = 1, NONE = 0 };

        void on( bool );
        void out( bool );
        void refsel( REFSEL );
        void dacdat( uint8_t );

    private:

        enum { DAC1CON = 0xBF802380, ON = 1<<15, DACOE = 1<<8, REFSELCLR = 3 };

};

/*=============================================================================
 all functions inline
=============================================================================*/

void Cvref::on( bool tf ){             Reg::set( DAC1CON, ON, tf );
void Cvref::out( bool tf ){            Reg::set( DAC1CON, DACOE, tf );
void Cvref::refsel( REFSEL e )
{
    Reg::clr( DAC1CON, REFSELCLR );
    Reg::set( DAC1CON, e & REFSELCLR );
}
//dac level 0-31, using Reg::val16 to change value
//(only DACDAT in upper 16bits, so can just write dat which
//will also mask off invalid bits, if dat >31)
void Cvref::dacdat( uint8_t n ){   Reg::val16( DAC1CON+2, n ); }