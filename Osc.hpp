#pragma once

/*=============================================================================
 Oscillator
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"

struct Osc {

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
