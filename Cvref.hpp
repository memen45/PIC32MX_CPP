#ifndef _CVREF_H
#define _CVREF_H

#include <cstdint>
#include "Reg.hpp"

namespace Cvref {

    enum { BASE_ADDR = 0xBF802380 };
    enum REFSEL { AVDD = 3, CVREF = 1, NONE = 0 };

    void on( void ){ Reg::set( BASE_ADDR, 1<<15 );
    void off( void ){ Reg::clr( BASE_ADDR, 1<<15 );

    void pin_on( void ){ Reg::set( BASE_ADDR, 1<<8 );
    void pin_off( void ){ Reg::clr( BASE_ADDR, 1<<8 );

    void refsel( REFSEL rs )
    {
        Reg::clr( BASE_ADDR, 3 );
        Reg::set( BASE_ADDR, rs & 3 );
    }

    //dac level 0-31, using Reg::val16 to change value
    //(only DACDAT in upper 16bits, so can just write dat which
    //will also mask off invalid bits, if dat >31)
    void dac( uint8_t dat )
    {
        Reg::val16( BASE_ADDR+2, dat );
    }

}

#endif //CVREF_H
