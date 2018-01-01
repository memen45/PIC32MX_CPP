#pragma once

/*=============================================================================
 Voltage Reference (CVref, DAC)
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"

struct Cvref {

    //vref select
    enum REFSEL { NONE = 0, CVREF, AVDD };

    //public functions
    void    on      (bool);
    void    out     (bool);
    void    refsel  (REFSEL);
    void    dacdat  (uint8_t);

    private:

    static Reg r;

    enum { DAC1CON = 0xBF802380, ON = 1<<15, DACOE = 1<<8, REFSELCLR = 3 };

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
    r.clr(DAC1CON, REFSELCLR);
    r.setbit(DAC1CON, e & REFSELCLR);
}
//dac level 0-31
//(only DACDAT in upper 16bits, so can just write dat) which
void Cvref::dacdat(uint8_t n){
    r.val(DAC1CON+2, n);
}