#pragma once

/*=============================================================================
 Voltage Reference (CVref, DAC)
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"

struct Cvref {

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    //vref select
    enum REFSEL { NONE = 0, CVREF, AVDD };

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    static void    on      (bool);
    static void    out     (bool);
    static void    refsel  (REFSEL);
    static void    dacdat  (uint8_t);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    private:

    static Reg r;

    enum {
        DAC1CON = 0xBF802380,
            ON = 1<<15,
            DACOE = 1<<8,
            REFSEL_SHIFT = 0, REFSEL_CLR = 3
    };

};

/*=============================================================================
 all functions inline
=============================================================================*/

void Cvref::on(bool tf){
    r.setbit(DAC1CON, ON, tf);
}
void Cvref::out(bool tf){
    r.setbit(DAC1CON, DACOE, tf);
}
void Cvref::refsel(REFSEL e){
    r.clrbit(DAC1CON, REFSEL_CLR<<REFSEL_SHIFT);
    r.setbit(DAC1CON, e<<REFSEL_SHIFT);
}
//dac level 0-31
//(only DACDAT in upper 16bits, so can just write dat) which
void Cvref::dacdat(uint8_t v){
    r.val(DAC1CON+2, v);
}