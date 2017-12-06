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

    //dac level 0-31, not using Reg::set/clear so there is no glitch
    //when changing level, new value written in one write
    void dac( uint8_t dat )
    {
        dat &= 31; dat <<= 16;
        uint32_t r = Reg::val( BASE_ADDR );
        r &= ~(31<<16); r |= dat;
        Reg::val( BASE_ADDR, r );
    }

}

#endif //CVREF_H
