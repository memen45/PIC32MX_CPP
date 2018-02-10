#pragma once

//Voltage Reference (CVref, DAC)

#include <cstdint>
#include "Reg.hpp"

struct Cvref {

    //vref select
    enum REFSEL { NONE = 0, CVREF, AVDD };

    static void     on              (bool);
    static void     out             (bool);
    static void     refsel          (REFSEL);
    static void     dacdat          (uint8_t);

    private:

    static Reg r;

    enum {
        DAC1CON = 0xBF802380,
            ON = 1<<15,
            DACOE = 1<<8,
            REFSEL_SHIFT = 0, REFSEL_CLR = 3
    };

};
